"""
Properties that must hold for any data. They compare stlrom with itself, so
they need no expected values:

- dualities: alw p = not ev not p, hist p = not once not p, ...
- moving all the data later by c moves the robustness later by c
- a past operator on the data equals the future operator on the data run backwards
- scaling the data and the thresholds scales the robustness
"""

import numpy as np
import pytest

from conftest import driver_for, grid
from reference import Trace
from traces import ALL, data_range

TOL = 1e-6


def rob_on_grid(formula, traces, times):
    d = driver_for({"phi": formula}, traces)
    z = d.get_rob_signal("phi", times[0], times[-1])
    return z, [z.value_at(t) if z.begin_time <= t <= z.end_time else None for t in times]


def same(f1, f2, traces, shift=0.0, times=None):
    """f1 on `traces` equals f2 on `traces`, at every time of a grid."""
    lo, hi = data_range(traces)
    times = times or grid(round(lo - 1, 1), round(hi + 1, 1))
    z1, v1 = rob_on_grid(f1, traces, times)
    z2, v2 = rob_on_grid(f2, traces, times)
    assert (z1.begin_time, z1.end_time) == pytest.approx((z2.begin_time, z2.end_time))
    wrong = [(round(t, 2), a, b) for t, a, b in zip(times, v1, v2)
             if a is not None and b is not None and abs(a - b) > TOL]
    assert not wrong, f"{len(wrong)} times differ (t, first, second): {wrong[:4]}"


DUALITIES = [
    ("not not x[t] > 0.5", "x[t] > 0.5"),
    ("alw_[0.2,0.8] x[t] > 0.5", "not ev_[0.2,0.8] not x[t] > 0.5"),
    ("hist_[0.2,0.8] x[t] > 0.5", "not once_[0.2,0.8] not x[t] > 0.5"),
    ("x[t] > 0 => y[t] > 0", "(not x[t] > 0) or y[t] > 0"),
    ("not (x[t] > 0 and y[t] > 0)", "(not x[t] > 0) or (not y[t] > 0)"),
    ("x[t] > y[t]", "y[t] < x[t]"),
    ("x[t] > 0.5", "x[t] - 0.5 > 0"),
    ("ev_[0.2,0.8] (x[t] > 0 or y[t] > 0)", "(ev_[0.2,0.8] x[t] > 0) or (ev_[0.2,0.8] y[t] > 0)"),
    ("alw_[0.2,0.8] (x[t] > 0 and y[t] > 0)", "(alw_[0.2,0.8] x[t] > 0) and (alw_[0.2,0.8] y[t] > 0)"),
    ("ev_[0,1] x[t] > 0.5", "ev_[0,0.5] ev_[0,0.5] x[t] > 0.5"),
    ("alw_[0.3,1] x[t] > 0.5", "alw_[0.3,0.3] alw_[0,0.7] x[t] > 0.5"),
    # "true" written as 1000 > 0: its robustness (1000) is larger than any value here
    ("ev_[0,1] x[t] > 0.5", "(1000 > 0) until_[0,1] x[t] > 0.5"),
    ("once_[0,1] x[t] > 0.5", "(1000 > 0) since_[0,1] x[t] > 0.5"),
]

DUALITY_CASES = [pytest.param(n, a, b, id=f"{n}: {a}  ==  {b}") for n in ALL for a, b in DUALITIES]


@pytest.mark.parametrize("trace_name, f1, f2", DUALITY_CASES)
def test_duality(trace_name, f1, f2):
    same(f1, f2, ALL[trace_name])


# ---------------------------------------------------------------------------
# Moving the data in time
# ---------------------------------------------------------------------------

FORMULAS = [
    "x[t] > 0.5",
    "ev_[0.3,0.8] x[t] > 0.5",
    "alw_[0,1] x[t] > y[t]",
    "once_[0.2,0.6] y[t] > 0",
    "hist_[0,1] x[t] > 0",
    "x[t] > -0.5 until_[0,1] y[t] > 0.5",
    "x[t] > -0.5 since_[0,1] y[t] > 0.5",
    "ev_[0,1] alw_[0,0.5] x[t] > 0",
]

SHIFT = 1.37  # not a round number on purpose


def moved(traces, c):
    return {k: Trace([(t + c, v) for t, v in zip(tr.times, tr.values)], tr.steps)
            for k, tr in traces.items()}


@pytest.mark.parametrize("trace_name, formula",
                         [pytest.param(n, f, id=f"{n}: {f}") for n in ALL for f in FORMULAS])
def test_moving_the_data_moves_the_result(trace_name, formula):
    traces = ALL[trace_name]
    lo, hi = data_range(traces)
    times = grid(round(lo - 1, 1), round(hi + 1, 1))
    z1, v1 = rob_on_grid(formula, traces, times)
    z2, v2 = rob_on_grid(formula, moved(traces, SHIFT), [t + SHIFT for t in times])
    assert z2.begin_time == pytest.approx(z1.begin_time + SHIFT)
    assert z2.end_time == pytest.approx(z1.end_time + SHIFT)
    wrong = [(round(t, 2), a, b) for t, a, b in zip(times, v1, v2)
             if a is not None and b is not None and abs(a - b) > 1e-6]
    # a jump of the result can fall on one side or the other of a grid time after
    # moving; allow a few such times, never more than 2% of them
    assert len(wrong) <= max(2, len(times) // 50), f"{len(wrong)} times differ: {wrong[:4]}"


# ---------------------------------------------------------------------------
# Past = future on the reversed data
# ---------------------------------------------------------------------------

MIRROR = [
    ("once_[0.2,0.8] x[t] > 0.5", "ev_[0.2,0.8] x[t] > 0.5"),
    ("hist_[0,1] x[t] > y[t]", "alw_[0,1] x[t] > y[t]"),
    ("once_[0,1] alw_[0,0.5] x[t] > 0", "ev_[0,1] hist_[0,0.5] x[t] > 0"),
    ("x[t] > -0.5 since_[0,1] y[t] > 0.5", "x[t] > -0.5 until_[0,1] y[t] > 0.5"),
    ("x[t] > -1 since_[0.3,1.2] y[t] > 0", "x[t] > -1 until_[0.3,1.2] y[t] > 0"),
]


def reversed_traces(traces):
    """x'(t) = x(-t).  Linear traces only (steps would change which side a jump belongs to)."""
    return {k: Trace([(-t, v) for t, v in zip(tr.times[::-1], tr.values[::-1])])
            for k, tr in traces.items()}


@pytest.mark.parametrize("trace_name, past, future",
                         [pytest.param(n, p, f, id=f"{n}: {p}") for n in ("smooth", "late", "uneven")
                          for p, f in MIRROR])
def test_past_is_future_backwards(trace_name, past, future):
    traces = ALL[trace_name]
    lo, hi = data_range(traces)
    times = grid(round(lo - 1, 1), round(hi + 1, 1))
    z1, v1 = rob_on_grid(past, traces, times)
    z2, v2 = rob_on_grid(future, reversed_traces(traces), [-t for t in times[::-1]])
    v2 = v2[::-1]
    assert z2.begin_time == pytest.approx(-z1.end_time)
    assert z2.end_time == pytest.approx(-z1.begin_time)
    wrong = [(round(t, 2), a, b) for t, a, b in zip(times, v1, v2)
             if a is not None and b is not None and abs(a - b) > 1e-6]
    assert not wrong, f"{len(wrong)} times differ (t, past, future backwards): {wrong[:4]}"


# ---------------------------------------------------------------------------
# Scaling
# ---------------------------------------------------------------------------

def scaled(traces, k):
    return {n: Trace([(t, k * v) for t, v in zip(tr.times, tr.values)], tr.steps)
            for n, tr in traces.items()}


@pytest.mark.parametrize("trace_name", ALL)
def test_scaling_data_and_thresholds_scales_robustness(trace_name):
    traces = ALL[trace_name]
    lo, hi = data_range(traces)
    times = grid(round(lo, 1), round(hi, 1))
    f1 = "alw_[0,0.5] (x[t] > 0.5 or ev_[0,0.4] y[t] < -0.25)"
    f2 = "alw_[0,0.5] (x[t] > 1.5 or ev_[0,0.4] y[t] < -0.75)"
    _, v1 = rob_on_grid(f1, traces, times)
    _, v2 = rob_on_grid(f2, scaled(traces, 3), times)
    for t, a, b in zip(times, v1, v2):
        if a is not None and b is not None:
            assert b == pytest.approx(3 * a, abs=1e-6), t
