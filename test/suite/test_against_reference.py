"""
stlrom against the brute-force reference (reference.py), offline.

For every formula below and every trace in traces.py:
1. `get_rob_signal` over a range wider than the data must:
   - start and end where the reference becomes defined / undefined, and
   - match the reference at every time i*0.01 in between.
2. `get_rob(t)` at a few single times must match too, or raise ValueError
   where the reference is undefined.

Times where the reference jumps are skipped (see reference.evaluate).
"""

import math

import numpy as np
import pytest

import known_bugs
from conftest import driver_for, grid
from reference import (Abs, Alw, And, Ev, Hist, Implies, Not, Once, Or, Since, Until, Var, Mul,
                       evaluate, horizon, x, y, t)
from traces import ALL, data_range

TOL = 0.05  # the reference's grid error is at most about H * slope = 0.005 * 7

FORMULAS = [
    # atoms and expressions
    x > 0.5,
    x < y,
    (x + y) > 1,
    (x - y * 2) < 0.5,
    Abs(x) > 0.8,
    Var("x", 0.3) > 0,
    Var("x", -0.4) > y,
    -x > -0.2,
    t > x + 2,
    # boolean
    Not(x > 0.5),
    And(x > 0, y > 0),
    Or(x > 0, y > 0),
    Implies(x > 1, y > 0),
    # future
    Ev(0, 1, x > 0.5),
    Ev(0.3, 0.8, x > 0.5),
    Alw(0, 1, x > 0.5),
    Alw(0.2, 0.6, y > 0),
    Until(x > -0.5, 0, 1, y > 0.5),
    Until(x > -1, 0.4, 1.2, y > 0),
    # past
    Once(0, 1, x > 0.5),
    Once(0.3, 0.8, x > 0.5),
    Hist(0, 1, x > 0.5),
    Hist(0.2, 0.6, y > 0),
    Since(x > -0.5, 0, 1, y > 0.5),
    Since(x > -1, 0.4, 1.2, y > 0),
    # nested
    Ev(0, 1, Alw(0, 0.5, x > 0)),
    Alw(0, 2, Implies(x > 1, Ev(0, 0.5, y > 0))),
    Hist(0, 1, Once(0, 0.5, x > 0)),
    Ev(0, 1, Hist(0, 0.5, x > 0)),
    Alw(0, 1, Once(0.2, 0.5, y > 0)),
    Until(Ev(0, 0.3, x > 0), 0, 1, Alw(0, 0.3, y > 0)),
    Since(Hist(0, 0.3, x > -0.5), 0.1, 0.7, Once(0, 0.4, y > 0)),
    Not(Ev(0, 1, Not(Var("x", -0.2) > y))),
]

# (trace, formula text) -> known bug (see known_bugs.py)
KNOWN = {}
# (trace, formula text) of cases that crash the process: they are not run
CRASHES = set()


def case(name, f):
    key = (name, f.text())
    marks = []
    if key in KNOWN:
        marks = [known_bugs.xfail(KNOWN[key])]
    if key in CRASHES:
        marks = [known_bugs.xfail("crashes the process (see known_bugs.py)", run=False)]
    return pytest.param(name, f, marks=marks, id=f"{name}: {f.text()}")


CASES = [case(name, f) for name in ALL for f in FORMULAS]


def compare(formula, traces, dt=None, tol=TOL):
    lo_data, hi_data = data_range(traces)
    past, future = horizon(formula)
    times = grid(round(lo_data - past - 0.5, 1), round(hi_data + 0.5, 1))
    ref, near_jump = evaluate(formula, traces, times)

    d = driver_for({"phi": formula.text()}, traces)
    if dt is None:
        z = d.get_rob_signal("phi", times[0], times[-1])
    else:
        z = d.get_rob_signal("phi", times[0], times[-1], dt)

    # 1. where it is defined
    defined = ~np.isnan(ref)
    assert defined.any(), "the reference is undefined everywhere: bad test data"
    first, last = times[np.argmax(defined)], times[len(times) - 1 - np.argmax(defined[::-1])]
    assert not z.empty(), f"empty result, the reference is defined on [{first}, {last}]"
    # the reference only knows the edges to within one grid step of 0.01
    assert z.begin_time == pytest.approx(first, abs=0.0100001)
    assert z.end_time == pytest.approx(last, abs=0.0100001)

    # 2. the values
    wrong = []
    for ti, r, skip in zip(times, ref, near_jump):
        if math.isnan(r) or skip or not (z.begin_time <= ti <= z.end_time):
            continue
        v = z.value_at(ti)
        if abs(v - r) > tol:
            wrong.append((round(ti, 3), v, r))
    assert not wrong, (f"{len(wrong)} times differ, first ones (t, stlrom, reference): "
                       f"{wrong[:5]}")

    # 3. single-time requests
    for ti, r, skip in list(zip(times, ref, near_jump))[::37]:
        if skip:
            continue
        if math.isnan(r):
            if ti < first - 0.0101 or ti > last + 0.0101:
                with pytest.raises(ValueError):
                    d.get_rob("phi", ti) if dt is None else d.get_rob("phi", ti, dt)
            continue
        v = d.get_rob("phi", ti) if dt is None else d.get_rob("phi", ti, dt)
        assert v == pytest.approx(r, abs=tol), f"get_rob at t = {ti}"


@pytest.mark.parametrize("trace_name, formula", CASES)
def test_matches_reference(trace_name, formula):
    compare(formula, ALL[trace_name])


@pytest.mark.parametrize("trace_name", ALL)
def test_product_with_dt(trace_name):
    """x*y is not linear between samples: with dt = 0.01 it must match closely."""
    compare(Ev(0, 0.5, Mul(x, y) > 0.3), ALL[trace_name], dt=0.01, tol=0.06)


# ---------------------------------------------------------------------------
# Ranges of different lengths
# ---------------------------------------------------------------------------
# Range requests cut the children's signals to the range plus the windows, so
# a bug can show for some ranges only. Here each formula is requested over
# ranges with different starts and ends inside its valid interval, and every
# range must agree with single-time requests (which the reference checks).

RANGE_FORMULAS = [
    Until(x > -1, 0.4, 1.2, y > 0),
    Until(x > -0.5, 0, 1, y > 0.5),
    Since(x > -1, 0.4, 1.2, y > 0),
    Ev(0, 1, Alw(0, 0.5, x > 0)),
    Once(0.3, 0.8, x > 0.5),
    Hist(0, 1, Once(0, 0.5, x > 0)),
]

@pytest.mark.parametrize("trace_name, formula", [case(n, f) for n in ALL for f in RANGE_FORMULAS])
def test_ranges_of_different_lengths_agree_with_single_times(trace_name, formula):
    traces = ALL[trace_name]
    d = driver_for({"phi": formula.text()}, traces)
    full = d.get_rob_signal("phi", -10, 20)
    s, e = full.begin_time, full.end_time
    times = [t for t in grid(round(s, 2), round(e, 2)) if s + 0.01 <= t <= e - 0.01]
    _, near = evaluate(formula, traces, times)
    single = {t: d.get_rob("phi", t) for t, skip in zip(times, near) if not skip}
    wrong = []
    for f0, f1 in [(0, 1), (0.25, 1), (0.5, 1), (0.75, 1), (0, 0.5), (0.25, 0.75), (0.6, 0.7)]:
        t0, t1 = s + f0 * (e - s), s + f1 * (e - s)
        z = d.get_rob_signal("phi", t0, t1)
        for t, v in single.items():
            if t0 + 0.01 <= t <= t1 - 0.01 and abs(z.value_at(t) - v) > 1e-6:
                wrong.append((round(t0, 2), round(t1, 2), round(t, 2), z.value_at(t), v))
                break
    assert not wrong, f"ranges that differ (start, end, t, range value, single value): {wrong}"
