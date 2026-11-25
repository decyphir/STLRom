"""
Temporal operators on small traces, with the expected values worked out by hand.

Trace W (straight lines between the points):

    t    0    0.5   1    1.5   2    2.5   3    3.5   4
    x    0    1     2    0.5   -1   0     1    1     1

Trace S (steps; each value held until the next point):

    x = 1 on [0,1),  3 on [1,2),  0 on [2,3),  2 on [3,4]
"""

import pytest

import stlrom
from conftest import driver_for, make_driver
from traces import LATE, SMOOTH

W = [(0, 0), (1, 2), (2, -1), (3, 1), (4, 1)]
S = [(0, 1), (1, 3), (2, 0), (3, 2), (4, 2)]
# W moved 1 later: data on [1, 5]
W_LATE = [(t + 1, v) for t, v in W]


def rob(formula, t, samples=W, interp=True):
    d = make_driver(f"signal x\nphi := {formula}", samples, interp)
    return d.get_rob("phi", t)


def rob_range(formula, t, samples=W, interp=True, t0=-10, t1=10):
    """The same, read from a range request over [t0, t1]."""
    d = make_driver(f"signal x\nphi := {formula}", samples, interp)
    return d.get_rob_signal("phi", t0, t1).value_at(t)


CASES = [
    # formula, t, expected, how
    ("ev_[0,1] x[t] > 0.25", 0, 1.75, "max of x on [0,1] is 2"),
    ("ev_[0,1] x[t] > 0.25", 1, 1.75, "max of x on [1,2] is 2"),
    ("ev_[0,1] x[t] > 0.25", 1.5, 0.25, "max of x on [1.5,2.5] is 0.5"),
    ("ev_[0,1] x[t] > 0.25", 2, 0.75, "max of x on [2,3] is 1"),
    ("ev_[0.5,1] x[t] > 0.25", 0, 1.75, "max of x on [0.5,1] is 2"),
    ("ev_[0.5,1] x[t] > 0.25", 1, 0.25, "max of x on [1.5,2] is 0.5"),
    ("alw_[0,1] x[t] > 0.25", 0, -0.25, "min of x on [0,1] is 0"),
    ("alw_[0,1] x[t] > 0.25", 1, -1.25, "min of x on [1,2] is -1"),
    ("alw_[0,1] x[t] > 0.25", 3, 0.75, "min of x on [3,4] is 1"),
    ("alw_[0.5,1.5] x[t] > 0.25", 0, 0.25, "min of x on [0.5,1.5] is 0.5"),
    ("once_[0,1] x[t] > 0.25", 1, 1.75, "max of x on [0,1] is 2"),
    ("once_[0,1] x[t] > 0.25", 2.5, 0.25, "max of x on [1.5,2.5] is 0.5"),
    ("once_[0,1] x[t] > 0.25", 3, 0.75, "max of x on [2,3] is 1"),
    ("once_[0.5,1] x[t] > 0.25", 2, 1.75, "max of x on [1,1.5] is 2"),
    ("hist_[0,1] x[t] > 0.25", 1, -0.25, "min of x on [0,1] is 0"),
    ("hist_[0,1] x[t] > 0.25", 2, -1.25, "min of x on [1,2] is -1"),
    ("hist_[0,1] x[t] > 0.25", 4, 0.75, "min of x on [3,4] is 1"),
    ("hist_[0.5,1] x[t] > 0.25", 3, -1.25, "min of x on [2,2.5] is -1"),
    # until: max over t' in [t+a, t+b] of min(q(t'), min of p on [t, t'])
    ("x[t] > -1 until_[0,1] x[t] > 1.5", 0, 0.5,
     "q = x-1.5 is best at t'=1 (0.5); p = x+1 >= 1 on [0,1]"),
    ("x[t] > -0.25 until_[0,1] x[t] > 0.5", 2.5, 0.25,
     "q reaches 0.5 at t'=3, but p = x+0.25 is only 0.25 at t=2.5"),
    ("x[t] > 0.25 until_[0.5,1] x[t] > 1.5", 0, -0.25,
     "p must hold from t=0 on, not from t+a: p(0) = -0.25"),
    ("x[t] > -1 until_[0.5,1] x[t] > 1.5", 0, 0.5, "as the first until, t' in [0.5,1]"),
    # since: max over t' in [t-b, t-a] of min(q(t'), min of p on [t', t])
    ("x[t] > -1 since_[0,1] x[t] > 1.5", 1, 0.5, "q is best at t'=1 (0.5)"),
    ("x[t] > -1.25 since_[0,1] x[t] > 1.5", 2, 0.25,
     "q best at t'=1 (0.5), but p on [1,2] is x(2)+1.25 = 0.25"),
    ("x[t] > 0.75 since_[0,1] x[t] > 0.5", 4, 0.25, "q = 0.5 on [3,4], p = 0.25 on [3,4]"),
]


@pytest.mark.parametrize("formula, t, expected, how", CASES, ids=[f"{c[0]} @ {c[1]}" for c in CASES])
def test_single_time(formula, t, expected, how):
    assert rob(formula, t) == pytest.approx(expected, abs=1e-9), how


@pytest.mark.parametrize("formula, t, expected, how", CASES, ids=[f"{c[0]} @ {c[1]}" for c in CASES])
def test_range_gives_the_same(formula, t, expected, how):
    assert rob_range(formula, t) == pytest.approx(expected, abs=1e-9), how


def test_nested_ev_alw():
    # g(s) = alw_[0,0.5] x>0.25 at s = (min of x on [s, s+0.5]) - 0.25.
    # x = 2s up to s = 1, then 2 - 3(s-1).  For s in [0, 1] the min of x on
    # [s, s+0.5] is min(2s, 3.5 - 3s), largest at s = 0.7 where both are 1.4.
    # ev_[0,1] g at t=0 = max of g on [0, 1] = 1.4 - 0.25 = 1.15
    assert rob("ev_[0,1] alw_[0,0.5] x[t] > 0.25", 0) == pytest.approx(1.15)


def test_nested_past_in_future():
    # once_[0,1] x>0.25 at s = max of x on [s-1, s] - 0.25.  alw_[0,1] of it at t = 1:
    # min over s in [1,2] of (max of x on [s-1,s]) - 0.25.  For s in [1,2] the window
    # [s-1, s] contains t=1, where x=2, so it is 2 everywhere -> 1.75
    assert rob("alw_[0,1] once_[0,1] x[t] > 0.25", 1) == pytest.approx(1.75)


# ---------------------------------------------------------------------------
# Signals with jumps (trace S).  Times chosen away from the jumps.
# ---------------------------------------------------------------------------

JUMP_CASES = [
    ("ev_[0,0.5] x[t] > 0.5", 0.75, 2.5, "window [0.75,1.25] holds 1 and 3"),
    ("ev_[0,0.5] x[t] > 0.5", 0.25, 0.5, "window [0.25,0.75] only holds 1"),
    ("alw_[0,0.5] x[t] > 0.5", 1.75, -0.5, "window [1.75,2.25] holds 3 and 0"),
    ("alw_[0,0.5] x[t] > 0.5", 1.25, 2.5, "window [1.25,1.75] only holds 3"),
    ("once_[0,0.5] x[t] > 0.5", 2.25, 2.5, "window [1.75,2.25] holds 3 and 0"),
    ("once_[0,0.5] x[t] > 0.5", 2.75, -0.5, "window [2.25,2.75] only holds 0"),
    ("hist_[0,0.5] x[t] > 0.5", 2.25, -0.5, "window [1.75,2.25] holds 3 and 0"),
    ("hist_[0,0.5] x[t] > 0.5", 3.25, -0.5, "window [2.75,3.25] holds 0 and 2"),
    ("hist_[0,0.5] x[t] > 0.5", 3.75, 1.5, "window [3.25,3.75] only holds 2"),
    ("x[t] > 0.5 until_[0,1] x[t] > 2.5", 0.5, 0.5,
     "q = 0.5 from t'=1 on; p = 0.5 on [0.5, 1]"),
    ("x[t] > 0.5 since_[0,1] x[t] > 2.5", 1.5, 0.5, "q = 0.5 for t' in [1,1.5]; p = 2.5 on [t', 1.5]"),
]


@pytest.mark.parametrize("formula, t, expected, how", JUMP_CASES,
                         ids=[f"{c[0]} @ {c[1]}" for c in JUMP_CASES])
def test_with_jumps(formula, t, expected, how):
    assert rob(formula, t, S, interp=False) == pytest.approx(expected, abs=1e-9), how
    assert rob_range(formula, t, S, interp=False) == pytest.approx(expected, abs=1e-9), how


# ---------------------------------------------------------------------------
# Data that does not start at 0 (W moved to [1, 5])
# ---------------------------------------------------------------------------

LATE_CASES = [
    ("ev_[0,1] x[t] > 0.25", 1, 1.75),
    ("alw_[0,1] x[t] > 0.25", 2, -1.25),
    ("once_[0,1] x[t] > 0.25", 2, 1.75),
    ("hist_[0,1] x[t] > 0.25", 3, -1.25),
    ("x[t] > -1 until_[0,1] x[t] > 1.5", 1, 0.5),
    ("x[t] > -1 since_[0,1] x[t] > 1.5", 2, 0.5),
]


@pytest.mark.parametrize("formula, t, expected", LATE_CASES, ids=[f"{c[0]} @ {c[1]}" for c in LATE_CASES])
def test_data_starting_later(formula, t, expected):
    assert rob(formula, t, W_LATE) == pytest.approx(expected, abs=1e-9)
    assert rob_range(formula, t, W_LATE) == pytest.approx(expected, abs=1e-9)


def test_default_time_0_is_refused_when_data_starts_later():
    d = make_driver("signal x\nphi := x[t] > 0", W_LATE)
    with pytest.raises(ValueError):
        d.get_rob("phi")


# ---------------------------------------------------------------------------
# Rounding: times such as i * 0.01 are not exact in floating point
# ---------------------------------------------------------------------------

def test_times_on_a_fine_grid():
    # ev_[0,1] x>0.25 at t in [0, 3]: compare range and single-time requests at
    # t = i*0.01, a grid that produces times like 0.07 = 0.07000000000000001
    d = make_driver("signal x\nphi := ev_[0,1] x[t] > 0.25", W)
    z = d.get_rob_signal("phi", 0, 3)
    for i in range(0, 301, 7):
        t = i * 0.01
        assert d.get_rob("phi", t) == pytest.approx(z.value_at(t), abs=1e-9), t


# ---------------------------------------------------------------------------
# Regressions: until and since bugs fixed in robustness.cpp
# ---------------------------------------------------------------------------

def test_since_single_time_rounding():
    # crashed: plateauMax read past the end of its input when beginTime + a rounded
    # just past the last sample (2.3 + 0.6 = 2.8999999999999999 > 2.8999999999999995)
    d = make_driver("signal x, y\n"
                    "phi := (hist_[0,0.3] (x[t] > -0.5)) since_[0.1,0.7] (once_[0,0.4] (y[t] > 0))")
    x, y = stlrom.Signal(), stlrom.Signal()
    for t, v in [(1.3, 0.5), (2, 2), (2.7, -1), (3.5, 1), (4.6, -0.5), (5.7, 1)]:
        x.append_linear_sample(t, v)
    for t, v in [(0.8, -1), (1.9, 1), (3, 0.2), (4.1, 1.5), (5, -0.5), (6.2, 0.5)]:
        y.append_linear_sample(t, v)
    d.set_signals([x, y])
    for t in (2.8, 2.89, 2.9, 2.91):
        assert d.get_rob("phi", t) == pytest.approx(-0.5), t  # reference: -0.5


def test_until_single_time_rounding():
    # crashed the same way, through until
    d = driver_for({"phi": "(x[t] > -1) until_[0.4,1.2] (y[t] > 0)"}, SMOOTH)
    assert d.get_rob("phi", 0.15) == pytest.approx(0.1786, abs=1e-3)  # reference


def test_until_on_ranges_of_different_lengths():
    # at t = 4.45 the best t' is 5.65 (end of the window), where y = 0.0417, and
    # x + 1 >= 0.5 on [4.45, 5.65], so the robustness is 0.0417. The range [4.0, 4.5]
    # used to give -0.1667 (computePartialEventually took a crossing time from the
    # segment start instead of the sample's time)
    d = driver_for({"phi": "(x[t] > -1) until_[0.4,1.2] (y[t] > 0)"}, LATE)
    assert d.get_rob("phi", 4.45) == pytest.approx(0.0417, abs=1e-3)
    for t0 in (3.7, 4.0, 4.2, 4.3):
        z = d.get_rob_signal("phi", t0, 4.5)
        assert z.value_at(4.45) == pytest.approx(0.0417, abs=1e-3), t0


def test_until_on_a_range_with_data_ending_at_0():
    # y on [-1.5, 0] goes 0.5 -> 1, so q = y - 0.5 goes 0 -> 0.5. With p always true,
    # until_[0,1] q at t = -1.2 is the max of q on [-1.2, -0.2] = q(-0.2) = 0.4333.
    # The range used to give q(0) = 0.5 (window length compared with the end time)
    d = make_driver("signal y\nphi := (1000 > 0) until_[0,1] (y[t] > 0.5)")
    s = stlrom.Signal()
    s.append_linear_sample(-1.5, 0.5)
    s.append_linear_sample(0, 1)
    d.set_signals([s])
    assert d.get_rob("phi", -1.2) == pytest.approx(0.4333, abs=1e-3)
    assert d.get_rob_signal("phi", -1.5, -1).value_at(-1.2) == pytest.approx(0.4333, abs=1e-3)
