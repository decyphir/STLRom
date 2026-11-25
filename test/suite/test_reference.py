"""
Checks of the brute-force reference itself (reference.py), on values worked
out by hand. If these fail, the reference is wrong, not stlrom.

The trace used below, x through (0, 0), (1, 2), (2, -1), (3, 1), (4, 1):

    t    0    0.5   1    1.5   2    2.5   3    4
    x    0    1     2    0.5   -1   0     1    1
"""

import math

import numpy as np
import pytest

from reference import (Abs, Alw, Ev, Hist, Implies, Local, Once, Since, Trace, Until, Var,
                       evaluate, evaluate_bounds, x, t)

W = {"x": Trace([(0, 0), (1, 2), (2, -1), (3, 1), (4, 1)])}

# (formula, time, expected robustness, how it is computed)
CASES = [
    (x > 0.25, 0.5, 0.75, "x(0.5) - 0.25"),
    (x < 0.25, 2.0, 1.25, "0.25 - x(2)"),
    (Abs(x) > 0.5, 2.0, 0.5, "|x(2)| - 0.5"),
    (Var("x", 0.5) > 0, 0.5, 2.0, "x(1)"),
    (Var("x", -0.5) > 0, 2.5, -1.0, "x(2)"),
    (t > 1, 3.0, 2.0, "3 - 1"),
    (Ev(0, 1, x > 0.25), 0.0, 1.75, "max of x on [0,1] is 2"),
    (Ev(0, 1, x > 0.25), 1.5, 0.25, "max of x on [1.5,2.5] is 0.5"),
    (Ev(0.5, 1, x > 0.25), 1.0, 0.25, "max of x on [1.5,2] is 0.5"),
    (Alw(0, 1, x > 0.25), 0.0, -0.25, "min of x on [0,1] is 0"),
    (Alw(0, 1, x > 0.25), 1.0, -1.25, "min of x on [1,2] is -1"),
    (Alw(0.5, 1.5, x > 0.25), 0.0, 0.25, "min of x on [0.5,1.5] is 0.5"),
    (Once(0, 1, x > 0.25), 2.5, 0.25, "max of x on [1.5,2.5] is 0.5"),
    (Hist(0, 1, x > 0.25), 2.0, -1.25, "min of x on [1,2] is -1"),
    (Hist(0.5, 1, x > 0.25), 3.0, -1.25, "min of x on [2,2.5] is -1"),
    (Until(x > -1, 0, 1, x > 1.5), 0.0, 0.5, "q is best at t'=1: x-1.5 = 0.5; p >= 1 on [0,1]"),
    (Until(x > -0.25, 0, 1, x > 0.5), 2.5, 0.25, "q reaches 0.5 but p = x+0.25 is 0.25 at t=2.5"),
    (Until(x > 0.25, 0.5, 1, x > 1.5), 0.0, -0.25, "p must hold from t=0, where it is -0.25"),
    (Since(x > -1, 0, 1, x > 1.5), 1.0, 0.5, "q is best at t'=1: 0.5"),
    (Since(x > -1.25, 0, 1, x > 1.5), 2.0, 0.25, "q best at t'=1 (0.5), p on [1,2] is x(2)+1.25 = 0.25"),
    (Implies(x > 1, x > 0.5), 2.0, 2.0, "x(2) = -1: max(-(x-1), x-0.5) = max(2, -1.5)"),
]


@pytest.mark.parametrize("formula, time, expected, why", CASES,
                         ids=[f"{c[0].text()} @ {c[1]}" for c in CASES])
def test_reference_value(formula, time, expected, why):
    values, _ = evaluate(formula, W, [time])
    assert values[0] == pytest.approx(expected, abs=1e-9), why


def test_reference_is_nan_outside_the_data():
    # ev_[0,1] needs data up to t+1, so it is defined on [0, 3] only
    values, _ = evaluate(Ev(0, 1, x > 0), W, [-0.5, 0.0, 3.0, 3.5])
    assert math.isnan(values[0])
    assert not math.isnan(values[1])
    assert not math.isnan(values[2])
    assert math.isnan(values[3])


def test_reference_time_robustness():
    # x > 0 holds on (0.5, 3.5); data on [0, 6]
    tr = {"x": Trace([(0, -1), (1, 1), (3, 1), (4, -1), (6, -1)])}
    left, _ = evaluate(x > 0, tr, [0.25, 1.0, 3.9], sem="LEFT_TIME")
    right, _ = evaluate(x > 0, tr, [0.25, 1.0, 3.9], sem="RIGHT_TIME")
    # the grid puts the switch up to one step (0.005) late
    np.testing.assert_allclose(left, [-0.25, 0.5, -0.4], atol=0.011)
    np.testing.assert_allclose(right, [-0.25, 2.5, -2.1], atol=0.011)


def test_reference_local_semantics_innermost_wins():
    tr = {"x": Trace([(0, -1), (1, 1), (3, 1), (4, -1), (6, -1)])}
    # ltr inside rho: the atom uses LEFT_TIME
    v, _ = evaluate(Local("SPACE", Local("LEFT_TIME", x > 0)), tr, [1.0])
    assert v[0] == pytest.approx(0.5, abs=0.011)


def test_reference_online_bounds():
    # data up to 1.5 only; ev_[0,1] at t=1 looks at [1, 2]
    partial = {"x": W["x"].until(1.5)}
    lo, up, _ = evaluate_bounds(Ev(0, 1, x > 0.25), partial, [1.0])
    assert lo[0] == pytest.approx(1.75)  # x(1) = 2 is already seen
    assert up[0] == 10000.0  # the rest of the window could be anything
    lo, up, _ = evaluate_bounds(Alw(0, 1, x > 0.25), partial, [1.0])
    assert lo[0] == -10000.0
    assert up[0] == pytest.approx(0.25)  # x(1.5) = 0.5 is already seen


def test_trace_until_cuts_the_data():
    cut = W["x"].until(1.5)
    assert cut.end == 1.5
    assert cut(np.array([1.5]))[0] == pytest.approx(0.5)
    assert math.isnan(cut(np.array([1.6]))[0])


def test_formula_text():
    assert (x > 0.25).text() == "x[t] > 0.25"
    assert Ev(0, 1, x > 0).text() == "ev_[0,1] (x[t] > 0)"
    assert Until(x > 0, 0.5, 1, x < 1).text() == "(x[t] > 0) until_[0.5,1] (x[t] < 1)"
    assert Var("x", -0.3).text() == "x[t-0.3]"
