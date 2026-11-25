"""
Semantics: what an atom's robustness measures.

- SPACE (default): how far the values are from the threshold.
- BOOLEAN: 1 if the atom holds, -Eps if not (so `not` flips the sign).
- LEFT_TIME / RIGHT_TIME (Donze-Maler time robustness): +-(how long the atom
  has held / will hold its truth value), the sign telling which value. The
  durations stop at the edges of the data.
The semantics is set for the whole driver (set_semantics) or for a subformula
with sat(...), ltr(...), rtr(...), rho(...). The innermost one wins.

Worked example, trace T (straight lines):

    t   0    0.5   1    3    3.5   4    6
    x   -1   0     1    1    0     -1   -1

x > 0 holds on (0.5, 3.5): from 0 to 0.5 false, 0.5 to 3.5 true, 3.5 to 6 false.
"""

import math

import pytest

from conftest import EPS, driver_for, grid, make_driver
from reference import Alw, And, Ev, Local, Not, Once, Or, Until, evaluate, x, y
from traces import ALL, data_range

T = [(0, -1), (1, 1), (3, 1), (4, -1), (6, -1)]
SWITCH = 1e-9  # the atom switches where x crosses Eps, not exactly 0


def driver(formula, semantics="SPACE", samples=T):
    d = make_driver(f"signal x\nphi := {formula}", samples)
    d.set_semantics(semantics)
    return d


# ---------------------------------------------------------------------------
# Worked examples
# ---------------------------------------------------------------------------

@pytest.mark.parametrize("t, left, right, how", [
    (0.25, -0.25, -0.25, "false since 0 (data start), false until 0.5"),
    (1.0, 0.5, 2.5, "true since 0.5, true until 3.5"),
    (2.0, 1.5, 1.5, "true since 0.5, true until 3.5"),
    (3.9, -0.4, -2.1, "false since 3.5, false until 6 (data end)"),
])
def test_time_semantics_worked_example(t, left, right, how):
    assert driver("x[t] > 0", "LEFT_TIME").get_rob("phi", t) == pytest.approx(left, abs=1e-6), how
    assert driver("x[t] > 0", "RIGHT_TIME").get_rob("phi", t) == pytest.approx(right, abs=1e-6), how
    assert driver("ltr(x[t] > 0)").get_rob("phi", t) == pytest.approx(left, abs=1e-6), how
    assert driver("rtr(x[t] > 0)").get_rob("phi", t) == pytest.approx(right, abs=1e-6), how


def test_time_semantics_on_a_range():
    z = driver("x[t] > 0", "LEFT_TIME").get_rob_signal("phi", 0, 6)
    for t, v in [(0.25, -0.25), (1.0, 0.5), (2.0, 1.5), (3.9, -0.4), (5.0, -1.5)]:
        assert z.value_at(t) == pytest.approx(v, abs=1e-6), t
    z = driver("x[t] > 0", "RIGHT_TIME").get_rob_signal("phi", 0, 6)
    for t, v in [(0.25, -0.25), (1.0, 2.5), (2.0, 1.5), (3.9, -2.1), (5.0, -1.0)]:
        assert z.value_at(t) == pytest.approx(v, abs=1e-6), t


def test_time_semantics_then_temporal_operator():
    # ev_[0,1] of the left time robustness at t = 1: max over [1, 2] of (s - 0.5) = 1.5
    assert driver("ev_[0,1] ltr(x[t] > 0)").get_rob("phi", 1) == pytest.approx(1.5, abs=1e-6)
    # alw_[0,1] of the right time robustness at t = 1: min over [1, 2] of (3.5 - s) = 1.5
    assert driver("alw_[0,1] rtr(x[t] > 0)").get_rob("phi", 1) == pytest.approx(1.5, abs=1e-6)


def test_boolean_semantics():
    d = driver("x[t] > 0", "BOOLEAN")
    assert d.get_rob("phi", 2) == 1
    assert d.get_rob("phi", 0.25) == pytest.approx(-EPS, abs=1e-15)


def test_boolean_not_gives_the_right_sign():
    # not (false) must be true (positive), which is why false is -Eps and not 0
    d = driver("not x[t] > 0", "BOOLEAN")
    assert d.get_rob("phi", 0.25) > 0
    assert d.get_rob("phi", 2) < 0


def test_boolean_with_temporal_operators():
    d = driver("ev_[0,1] x[t] > 0", "BOOLEAN")
    assert d.get_rob("phi", 0) == 1  # true at some time in [0, 1]
    assert d.get_rob("phi", 4.5) < 0  # false on [4.5, 5.5]
    d = driver("alw_[0,1] x[t] > 0", "BOOLEAN")
    assert d.get_rob("phi", 1) == 1
    assert d.get_rob("phi", 0) < 0


def test_sat_operator():
    assert driver("sat(x[t] > 0)").get_rob("phi", 2) == 1
    assert driver("sat(ev_[0,1] x[t] > 0)").get_rob("phi", 0) == 1


def test_rho_gives_space_inside_a_time_semantics_driver():
    d = driver("rho(x[t] > 0)", "LEFT_TIME")
    assert d.get_rob("phi", 0.75) == pytest.approx(0.5)  # x(0.75) = 0.5


def test_innermost_semantics_wins():
    assert driver("ltr(rho(x[t] > 0))").get_rob("phi", 0.75) == pytest.approx(0.5)  # space
    assert driver("rho(ltr(x[t] > 0))").get_rob("phi", 2) == pytest.approx(1.5, abs=1e-6)  # time


def test_local_semantics_mixed_in_one_formula():
    # space part: x(2) - 0 = 1;  left time part: 1.5.  min = 1
    d = driver("x[t] > 0 and ltr(x[t] > 0)")
    assert d.get_rob("phi", 2) == pytest.approx(1, abs=1e-6)


def test_set_semantics_get_semantics():
    d = make_driver("signal x\nphi := x[t] > 0", T)
    assert d.get_semantics() == "SPACE"
    for name in ("LEFT_TIME", "RIGHT_TIME", "BOOLEAN", "SPACE"):
        d.set_semantics(name)
        assert d.get_semantics() == name


def test_invalid_semantics_name():
    d = make_driver("signal x\nphi := x[t] > 0", T)
    with pytest.raises(ValueError):
        d.set_semantics("TIME")
    with pytest.raises(ValueError):
        d.get_monitor("phi").set_semantics("nope")


def test_monitor_semantics_is_its_own():
    d = make_driver("signal x\nphi := x[t] > 0", T)
    d.set_semantics("BOOLEAN")
    m = d.get_monitor("phi")  # copies BOOLEAN
    assert m.get_semantics() == "BOOLEAN"
    m.set_semantics("SPACE")
    assert d.get_semantics() == "BOOLEAN"
    assert m.get_rob(2) == pytest.approx(1)  # x(2) = 1 in space
    assert d.get_rob("phi", 2) == 1  # still boolean


def test_driver_semantics_does_not_leak_to_another_driver():
    d1 = driver("x[t] > 0", "BOOLEAN")
    d2 = driver("x[t] > 0")
    d1.get_rob("phi", 2)
    assert d2.get_rob("phi", 0.75) == pytest.approx(0.5)


# ---------------------------------------------------------------------------
# Against the reference (time semantics: durations within one grid step)
# ---------------------------------------------------------------------------

REF_FORMULAS = [
    Local("LEFT_TIME", x > 0.5),
    Local("RIGHT_TIME", x > 0.5),
    Local("LEFT_TIME", x < y),
    Ev(0, 1, Local("LEFT_TIME", x > 0)),
    Alw(0, 0.5, Local("RIGHT_TIME", y > 0)),
    And(Local("LEFT_TIME", x > 0), Local("RIGHT_TIME", y > 0)),
    Not(Local("RIGHT_TIME", x > 0.5)),
    Once(0, 1, Local("RIGHT_TIME", x > 0)),
    Local("BOOLEAN", Ev(0, 1, x > 0.5)),
    Or(Local("BOOLEAN", x > 0.5), Local("BOOLEAN", y > 0)),
    Until(Local("BOOLEAN", x > -0.5), 0, 1, Local("BOOLEAN", y > 0.5)),
]


@pytest.mark.parametrize("trace_name, formula",
                         [pytest.param(n, f, id=f"{n}: {f.text()}") for n in ALL for f in REF_FORMULAS])
def test_matches_reference(trace_name, formula):
    traces = ALL[trace_name]
    lo, hi = data_range(traces)
    times = grid(round(lo - 0.5, 1), round(hi + 0.5, 1))
    # time robustness changes by at most 1 per time unit (0.01 per two grid steps),
    # so anything above 0.03 is a switch of an atom: skip those instants
    ref, near = evaluate(formula, traces, times, jump=0.03)
    d = driver_for({"phi": formula.text()}, traces)
    z = d.get_rob_signal("phi", times[0], times[-1])
    wrong = []
    for t, r, skip in zip(times, ref, near):
        if math.isnan(r) or skip or not (z.begin_time <= t <= z.end_time):
            continue
        if abs(z.value_at(t) - r) > 0.02:
            wrong.append((round(t, 2), z.value_at(t), r))
    assert not wrong, f"{len(wrong)} times differ (t, stlrom, reference): {wrong[:5]}"
