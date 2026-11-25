"""
Online robustness: get_online_rob and get_online_rob_signal on partial data.

They return [rob, lower, upper]:
- rob: the robustness computed from the data so far;
- lower, upper: bounds that must hold whatever data comes next.
Where nothing is known, the bounds are -BigM and +BigM. Before the data
starts (past operators looking back), nothing is known either.
"""

import math

import numpy as np
import pytest

import known_bugs
import stlrom
from conftest import BIGM, driver_for, grid, make_driver
from reference import (Alw, And, Ev, Hist, Implies, Not, Once, Or, Since, Until, evaluate,
                       evaluate_bounds, x, y)
from traces import ALL, data_range

# x through (0,0) (1,2) (2,-1): the first half of the trace W in test_temporal.py
W2 = [(0, 0), (1, 2), (2, -1)]


def online(formula, t, samples=W2):
    d = make_driver(f"signal x\nphi := {formula}", samples)
    return d.get_online_rob("phi", t)


# ---------------------------------------------------------------------------
# Worked examples
# ---------------------------------------------------------------------------

def test_no_data():
    rob, lo, up = online("x[t] > 0", 0, samples=[])
    assert math.isnan(rob)
    assert (lo, up) == (-BIGM, BIGM)


def test_no_data_signal_form():
    d = make_driver("signal x\nphi := ev_[0,1] x[t] > 0")
    z, lo, up = d.get_online_rob_signal("phi", 0, 2)
    assert z.empty()
    assert lo.value_at(1) == -BIGM and up.value_at(1) == BIGM


def test_atom_inside_the_data_is_exact():
    assert online("x[t] > 0.25", 1) == pytest.approx([1.75, 1.75, 1.75])


def test_atom_after_the_data_is_unknown():
    _, lo, up = online("x[t] > 0.25", 3)
    assert (lo, up) == (-BIGM, BIGM)


@pytest.mark.parametrize("formula, t, rob, lo, up, how", [
    ("ev_[0,1] x[t] > 0.25", 1.5, 0.25, 0.25, BIGM,
     "window [1.5, 2.5]: seen x on [1.5, 2], max 0.5; the rest could be anything"),
    ("alw_[0,1] x[t] > 0.25", 1.5, -1.25, -BIGM, -1.25,
     "window [1.5, 2.5]: seen min -1, so it can only get lower"),
    ("hist_[0,1] x[t] > 0.25", 0.5, -0.25, -BIGM, -0.25,
     "window [-0.5, 0.5]: before 0 is unknown; seen min on [0, 0.5] is 0"),
    ("once_[0,1] x[t] > 0.25", 0.5, 0.75, 0.75, BIGM,
     "window [-0.5, 0.5]: seen max on [0, 0.5] is 1"),
])
def test_partial_window(formula, t, rob, lo, up, how):
    r = online(formula, t)
    assert r == pytest.approx([rob, lo, up]), how


@pytest.mark.parametrize("formula, t", [
    ("ev_[0,1] x[t] > 0.25", 0.5),
    ("alw_[0,1] x[t] > 0.25", 0.5),
    ("once_[0,1] x[t] > 0.25", 1.5),
    ("hist_[0,1] x[t] > 0.25", 1.5),
    ("x[t] > -2 until_[0,1] x[t] > 1.5", 0.5),
    ("x[t] > -2 since_[0,1] x[t] > 1.5", 1.5),
])
def test_window_inside_the_data_equals_offline(formula, t):
    d = make_driver(f"signal x\nphi := {formula}", W2)
    offline = d.get_rob("phi", t)
    assert d.get_online_rob("phi", t) == pytest.approx([offline] * 3)


def test_online_signal_trims_only_rob():
    # ev_[0,1] on data [0, 2]: rob is defined on [0, 1]; the bounds cover the request
    d = make_driver("signal x\nphi := ev_[0,1] x[t] > 0.25", W2)
    z, lo, up = d.get_online_rob_signal("phi", 0, 3)
    assert (z.begin_time, z.end_time) == pytest.approx((0, 1))
    assert (lo.begin_time, lo.end_time) == pytest.approx((0, 3))
    assert (up.begin_time, up.end_time) == pytest.approx((0, 3))
    assert up.value_at(2.5) == BIGM


def test_monitor_keeps_the_last_values():
    d = make_driver("signal x\nphi := ev_[0,1] x[t] > 0.25", W2)
    m = d.get_monitor("phi")
    m.get_online_rob_signal(1.5, 2)
    assert m.rob == pytest.approx(0.25)
    assert m.lower_rob == pytest.approx(0.25)
    assert m.upper_rob == BIGM
    assert (m.get_lower_rob(), m.get_upper_rob()) == (m.lower_rob, m.upper_rob)


def test_bigm_setting_is_used_for_unknown_bounds():
    stlrom.Signal.set_BigM(500)
    _, lo, up = online("x[t] > 0", 0, samples=[])
    assert (lo, up) == (-500, 500)


# ---------------------------------------------------------------------------
# Adding samples one by one
# ---------------------------------------------------------------------------

def test_until_before_the_data():
    # at t = -1 the window [-0.6, 0.2] is mostly before the data: the upper bound is
    # unknown. It used to be 1.0 for a single time (compute_timed_until compared the
    # window length with the signals' end time and returned alw p)
    d = make_driver("signal x, y\nphi := (x[t] > 0) until_[0.4,1.2] (y[t] > 0)",
                    [(0, 1, 1), (3, 1, 1)])
    _, lo, up = d.get_online_rob_signal("phi", -1, 1)
    assert up.value_at(-1) == BIGM
    assert d.get_online_rob("phi", -1)[2] == BIGM


def test_since_bounds_when_the_window_starts_before_the_data():
    # x = 1 on [0, 3].  At t = 0.5, since_[0,1] looks at t' in [-0.5, 0.5]; for t' in
    # [0, 0.5] both sides are known: min(1, 1) = 1, so the result is at least 1, and
    # every t' needs p = 1 on [0, 0.5], so it is at most 1. It used to be [-BigM, BigM]
    d = make_driver("signal x\nphi := (x[t] > 0) since_[0,1] (x[t] > 0)", [(0, 1), (3, 1)])
    _, lo, up = d.get_online_rob("phi", 0.5)
    assert (lo, up) == pytest.approx((1, 1))


def test_until_sample_by_sample_does_not_crash():
    # an old report: until_[3,5] with add_sample crashed at t = 3
    d = make_driver("signal x, y\nphi := x[t] > 0 until_[3,5] y[t] > 0")
    for i in range(0, 81):
        t = i * 0.1
        d.add_sample([t, math.sin(t), math.cos(t)])
        rob, lo, up = d.get_online_rob("phi", 0)
        assert lo <= up


def test_bounds_tighten_as_data_arrives():
    # lower never goes down and upper never goes up as samples are added
    spec = "signal x\nphi := alw_[0,2] (x[t] > 0 => ev_[0,1] x[t] < 0.5)"
    d = make_driver(spec)
    prev_lo, prev_up = -math.inf, math.inf
    for i in range(0, 51):
        t = i * 0.1
        d.add_sample([t, math.sin(3 * t)])
        _, lo, up = d.get_online_rob("phi", 0.5)
        assert lo >= prev_lo - 1e-9, f"lower went down at t = {t}"
        assert up <= prev_up + 1e-9, f"upper went up at t = {t}"
        prev_lo, prev_up = lo, up
    assert lo == pytest.approx(up)  # by t = 5 the whole window [0.5, 3.5] is known


# ---------------------------------------------------------------------------
# Against the reference: the bounds must be exactly the interval propagation
# (reference.bounds), and must contain the robustness of the full trace
# ---------------------------------------------------------------------------

FORMULAS = [
    x > 0.5,
    Ev(0, 1, x > 0.5),
    Ev(0.3, 0.8, x > 0.5),
    Alw(0, 1, x > 0.5),
    Once(0, 1, x > 0.5),
    Hist(0.2, 0.6, y > 0),
    Until(x > -0.5, 0, 1, y > 0.5),
    Until(x > -1, 0.4, 1.2, y > 0),
    Since(x > -0.5, 0, 1, y > 0.5),
    Not(Ev(0, 1, x > 0.5)),
    And(x > 0, Ev(0, 0.5, y > 0)),
    Or(Alw(0, 0.5, x > 0), y > 0),
    Implies(x > 1, Ev(0, 0.5, y > 0)),
    Ev(0, 1, Alw(0, 0.5, x > 0)),
    Alw(0, 1, Once(0.2, 0.5, y > 0)),
]

CUT = {"smooth": 3.0, "late": 3.5, "steps": 3.0, "uneven": 3.0}  # data known up to here

# (trace, formula text) -> known bug, for the exact-bounds comparison only
KNOWN = {
}


def case(n, f, known):
    marks = [known_bugs.xfail(known[(n, f.text())])] if (n, f.text()) in known else []
    return pytest.param(n, f, marks=marks, id=f"{n} cut at {CUT[n]}: {f.text()}")


CASES = [case(n, f, {}) for n in ALL for f in FORMULAS]
EXACT_CASES = [case(n, f, KNOWN) for n in ALL for f in FORMULAS]


def cut(traces, T):
    return {k: tr.until(T) for k, tr in traces.items()}


@pytest.mark.parametrize("trace_name, formula", EXACT_CASES)
def test_bounds_match_the_reference(trace_name, formula):
    full = ALL[trace_name]
    T = CUT[trace_name]
    partial = cut(full, T)
    lo_data, _ = data_range(full)
    times = grid(round(lo_data - 1, 1), T + 1)
    ref_lo, ref_up, near = evaluate_bounds(formula, partial, times)

    d = driver_for({"phi": formula.text()}, partial)
    z, lo, up = d.get_online_rob_signal("phi", times[0], times[-1])
    wrong = []
    for t, rl, ru, skip in zip(times, ref_lo, ref_up, near):
        if skip:
            continue
        if abs(lo.value_at(t) - rl) > 0.05 or abs(up.value_at(t) - ru) > 0.05:
            wrong.append((round(t, 2), lo.value_at(t), up.value_at(t), rl, ru))
    assert not wrong, (f"{len(wrong)} times differ, first ones "
                       f"(t, lower, upper, reference lower, reference upper): {wrong[:4]}")

    for t, rl, ru, skip in list(zip(times, ref_lo, ref_up, near))[::29]:
        if skip:
            continue
        _, l1, u1 = d.get_online_rob("phi", t)
        assert (l1, u1) == pytest.approx((rl, ru), abs=0.05), f"get_online_rob at t = {t}"


@pytest.mark.parametrize("trace_name, formula", CASES)
def test_bounds_contain_the_final_robustness(trace_name, formula):
    """Soundness: what the full trace finally gives must lie inside the earlier bounds."""
    full = ALL[trace_name]
    T = CUT[trace_name]
    lo_data, hi_data = data_range(full)
    times = grid(round(lo_data, 1), round(hi_data, 1))
    final, near = evaluate(formula, full, times)

    d = driver_for({"phi": formula.text()}, cut(full, T))
    z, lo, up = d.get_online_rob_signal("phi", times[0], times[-1])
    for t, f, skip in zip(times, final, near):
        if math.isnan(f) or skip:
            continue
        assert lo.value_at(t) <= f + 0.05 and f - 0.05 <= up.value_at(t), (
            f"t = {t}: final robustness {f} outside [{lo.value_at(t)}, {up.value_at(t)}]")
        if z.begin_time <= t <= z.end_time:
            assert lo.value_at(t) - 1e-9 <= z.value_at(t) <= up.value_at(t) + 1e-9, (
                f"t = {t}: rob {z.value_at(t)} outside its own bounds")
