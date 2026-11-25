"""
Shared helpers for the test suite.

pytest loads this file automatically before the tests. It provides:
- `reset_globals`: runs around every test and restores the process-wide
  settings (BigM, Eps, semantics), so one test cannot change another.
- small helpers to build drivers and read signals, used by every test file.
- a warning at the top of the run if the compiled module is older than the
  C++ sources (the editable install does not rebuild by itself).
"""

import os
import sys
import time

import pytest

sys.path.insert(0, os.path.dirname(__file__))  # so tests can `import reference`

import stlrom  # noqa: E402

REPO = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
BIGM = 10000.0
EPS = 1e-10


# ---------------------------------------------------------------------------
# Global settings
# ---------------------------------------------------------------------------

@pytest.fixture(autouse=True)
def reset_globals():
    """BigM, Eps and the global semantics are shared by the whole process."""
    yield
    stlrom.Signal.set_BigM(BIGM)
    stlrom.Signal.set_Eps(EPS)
    stlrom.Signal.set_semantics("SPACE")


# ---------------------------------------------------------------------------
# Building drivers
# ---------------------------------------------------------------------------

def make_driver(spec, samples=(), interp=True):
    """Parse `spec` and add `samples`, each a tuple (t, x, y, ...).

    Fails the test right away if the spec does not parse.
    """
    d = stlrom.STLDriver()
    assert d.parse_string(spec), f"spec did not parse:\n{spec}"
    for s in samples:
        d.add_sample(list(s), interp)
    return d


def signal_from(points, steps=False):
    """A stlrom.Signal through `points` [(t, v), ...].

    steps=False joins the points with straight lines; steps=True holds each
    value until the next point.
    """
    s = stlrom.Signal()
    for t, v in points:
        if steps:
            s.append_constant_sample(t, v)
        else:
            s.append_linear_sample(t, v)
    return s


def signal_from_trace(trace):
    """The stlrom.Signal for a reference.Trace."""
    return signal_from(zip(trace.times, trace.values), trace.steps)


def driver_for(formulas, traces):
    """A driver with signals x and y and the given formulas {name: text}.

    `traces` maps "x" and "y" to reference.Trace objects (missing ones get no
    samples).
    """
    spec = "signal x, y\n" + "\n".join(f"{n} := {f}" for n, f in formulas.items())
    d = make_driver(spec)
    sigs = []
    for name in ("x", "y"):
        sigs.append(signal_from_trace(traces[name]) if name in traces else stlrom.Signal())
    d.set_signals(sigs)
    return d


def points(signal):
    """The samples of a Signal as a list of (time, value, slope)."""
    return [(s.time, s.value, s.derivative) for s in signal.get_samples_list()]


def grid(t0, t1, step=0.01):
    """Times t0, t0+step, ..., t1, computed as i*step (as a user would)."""
    n0, n1 = round(t0 / step), round(t1 / step)
    return [i * step for i in range(n0, n1 + 1)]


# ---------------------------------------------------------------------------
# Stale build warning
# ---------------------------------------------------------------------------

# written by bison/flex during a CMake build, which does not touch the Python module
GENERATED = {"parser.cpp", "scanner.cpp", "parser.hpp"}


def pytest_report_header(config):
    so = stlrom._stlrom.__file__
    built = os.path.getmtime(so)
    newest, newest_file = 0, None
    for folder in ("src", "include"):
        for root, _, files in os.walk(os.path.join(REPO, folder)):
            for f in files:
                if f in GENERATED:
                    continue
                if f.endswith((".cpp", ".h", ".y", ".l")):
                    p = os.path.join(root, f)
                    if os.path.getmtime(p) > newest:
                        newest, newest_file = os.path.getmtime(p), p
    lines = [f"stlrom module: {so} (built {time.ctime(built)})"]
    if newest > built:
        lines.append("!" * 78)
        lines.append(f"WARNING: {os.path.relpath(newest_file, REPO)} is newer than the compiled module.")
        lines.append("These tests run the OLD build. Rebuild first:")
        lines.append("    uv pip install --python .venv/bin/python -e .")
        lines.append("!" * 78)
    return lines
