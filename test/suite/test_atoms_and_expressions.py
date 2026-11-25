"""
Expressions, atoms and boolean operators, on small traces worked out by hand.

Unless said otherwise, the data is:

    t    0     0.5   1     1.5   2
    x    1     2     3     1.5   0        (straight lines through t = 0, 1, 2)
    y    0     0.5   1     1     1
"""

import math

import pytest

from conftest import EPS, make_driver, points

XY = [(0, 1, 0), (1, 3, 1), (2, 0, 1)]


def rob(formula, t, samples=XY, signals="x, y"):
    d = make_driver(f"signal {signals}\nphi := {formula}", samples)
    return d.get_rob("phi", t)


# ---------------------------------------------------------------------------
# Expressions: e > 0 has robustness e, so it shows the expression's value
# ---------------------------------------------------------------------------

@pytest.mark.parametrize("expr, t, value", [
    ("x[t]", 0.5, 2),
    ("x[t] + y[t]", 1.5, 2.5),
    ("x[t] - y[t]", 1.5, 0.5),
    ("2 * x[t]", 1.5, 3),
    ("x[t] * y[t]", 1, 3),  # at a data point the product is exact
    ("-x[t]", 0.5, -2),
    ("-(x[t] - 4)", 0.5, 2),
    ("abs(x[t] - 2)", 1.5, 0.5),
    ("abs(x[t] - 2)", 0.5, 0),
    ("x[t+0.5]", 0.5, 3),  # x(1)
    ("x[t-0.5]", 1.5, 3),  # x(1)
    ("t", 1.5, 1.5),
    ("3", 1.5, 3),
    ("-3", 1.5, -3),
    ("x[t] + t * 2", 0.5, 3),
    ("(x[t] + 1) * 2", 0.5, 6),
])
def test_expression_value(expr, t, value):
    r = rob(f"{expr} > 0", t)
    if value == 0:
        assert abs(r) <= EPS * 1.0001  # an exact 0 is reported as +-Eps
    else:
        assert r == pytest.approx(value)


def test_abs_crossing_zero_inside_a_piece():
    # x goes -1 -> 1 on [0, 2], so |x| goes 1 -> 0 at t = 1 -> 1
    d = make_driver("signal x\nphi := abs(x[t]) > 0.25", [(0, -1), (2, 1)])
    z = d.get_rob_signal("phi", 0, 2)
    for t, v in [(0, 0.75), (0.5, 0.25), (1, -0.25), (1.5, 0.25), (2, 0.75)]:
        assert z.value_at(t) == pytest.approx(v, abs=1e-9), t


def test_shift_changes_where_the_data_is_valid():
    # x on [0, 2]; x[t+0.5] needs x at t+0.5, so it is defined on [-0.5, 1.5]
    d = make_driver("signal x, y\nphi := x[t+0.5] > 0\npsi := x[t-0.5] > 0", XY)
    z = d.get_rob_signal("phi", -5, 5)
    assert (z.begin_time, z.end_time) == pytest.approx((-0.5, 1.5))
    z = d.get_rob_signal("psi", -5, 5)
    assert (z.begin_time, z.end_time) == pytest.approx((0.5, 2.5))


def test_shifted_signal_on_a_range():
    # x: 0 until t=1, then rises to 2 at t=3.  x[t-1] at t=2.5 is x(1.5) = 0.5.
    # A range that stops before the data's end used to lose the last piece.
    d = make_driver("signal x\nphi := x[t-1] > 0\npsi := x[t+1] > 0", [(0, 0), (1, 0), (3, 2)])
    assert d.get_rob_signal("phi", 1, 3).value_at(2.5) == pytest.approx(0.5)
    assert d.get_rob_signal("phi", 1, 2.5).value_at(2.2) == pytest.approx(0.2)
    assert d.get_rob_signal("psi", 0, 1.5).value_at(1.4) == pytest.approx(1.4)  # x(2.4)


def test_constants_and_time_need_no_data():
    d = make_driver("signal x\nphi := 2 > 1\npsi := t > 1")  # no samples at all
    assert d.get_rob("phi", 0) == pytest.approx(1)
    assert d.get_rob("phi", -50) == pytest.approx(1)
    assert d.get_rob("psi", 3) == pytest.approx(2)


# ---------------------------------------------------------------------------
# Products and dt
# ---------------------------------------------------------------------------

def product_error(dt):
    """Largest error of x*y on [0, 2] for x = y = t (true product t^2)."""
    d = make_driver("signal x, y\nphi := x[t] * y[t] > 0", [(0, 0, 0), (2, 2, 2)])
    z = d.get_rob_signal("phi", 0, 2) if dt is None else d.get_rob_signal("phi", 0, 2, dt)
    return max(abs(z.value_at(i * 0.01) - (i * 0.01) ** 2) for i in range(201))


def test_product_without_dt_uses_the_data_points_only():
    # x = y = t, with data points at t = 0 and t = 2 only.  Between data points the
    # product follows its tangent at the left point: at t = 0, x*y = 0 with slope
    # x'y + xy' = 0, so it stays 0 until t = 2, where it is 4.
    d = make_driver("signal x, y\nphi := x[t] * y[t] > 0", [(0, 0, 0), (2, 2, 2)])
    z = d.get_rob_signal("phi", 0, 2)
    assert abs(z.value_at(1)) <= EPS * 1.0001  # true value 1
    assert d.get_rob("phi", 2) == pytest.approx(4)  # exact at the data point
    assert product_error(None) == pytest.approx(4, abs=0.05)


def test_product_error_shrinks_like_dt_squared():
    e1, e2 = product_error(0.1), product_error(0.05)
    assert e1 == pytest.approx(0.01, rel=0.05)  # about dt^2 (tangent pieces of length dt)
    assert e1 / e2 == pytest.approx(4, rel=0.1)  # half the step, a quarter of the error


@pytest.mark.parametrize("dt", [0, -1])
def test_dt_must_be_positive(dt):
    d = make_driver("signal x, y\nphi := x[t] * y[t] > 0", XY)
    with pytest.raises(ValueError):
        d.get_rob("phi", 0, dt)
    with pytest.raises(ValueError):
        d.get_rob_signal("phi", 0, 1, dt)
    with pytest.raises(ValueError):
        d.get_online_rob("phi", 0, dt)


def test_dt_does_not_change_linear_formulas():
    d = make_driver("signal x, y\nphi := ev_[0,1] x[t] - y[t] > 0", XY)
    assert d.get_rob("phi", 0.5, 0.01) == pytest.approx(d.get_rob("phi", 0.5))


# ---------------------------------------------------------------------------
# Atoms
# ---------------------------------------------------------------------------

@pytest.mark.parametrize("atom, value", [
    ("x[t] > 1", 1),  # x - 1 at t = 0.5 (x = 2)
    ("x[t] < 1", -1),  # 1 - x
    ("x[t] >= 1", 1),  # same as >
    ("x[t] <= 1", -1),  # same as <
    ("x[t] > y[t]", 1.5),  # 2 - 0.5
    ("y[t] < x[t]", 1.5),
    ("1 < x[t]", 1),
])
def test_atom_robustness(atom, value):
    assert rob(atom, 0.5) == pytest.approx(value)


def test_equality_atom():
    # x == 2: +Eps where x = 2, -|x - 2| elsewhere.  x = 2 at t = 0.5 and t = 4/3
    d = make_driver("signal x\nphi := x[t] == 2", [(0, 1), (1, 3), (2, 0)])
    assert d.get_rob("phi", 0.5) == pytest.approx(EPS, abs=1e-15)
    assert d.get_rob("phi", 0) == pytest.approx(-1)
    assert d.get_rob("phi", 1) == pytest.approx(-1)
    assert d.get_rob("phi", 2) == pytest.approx(-2)


def test_atom_sign_change_inside_the_last_piece():
    # one piece from 1 to -1: x > 0 changes sign at t = 1, before the last sample
    d = make_driver("signal x\nphi := x[t] > 0", [(0, 1), (2, -1)])
    z = d.get_rob_signal("phi", 0, 2)
    assert z.value_at(0.5) == pytest.approx(0.5)
    assert z.value_at(1.5) == pytest.approx(-0.5)
    assert z.value_at(2) == pytest.approx(-1)


def test_atom_at_exactly_zero_is_eps_sized():
    # robustness 0 is never returned: it is +-Eps
    r = rob("x[t] > 2", 0.5)
    assert r != 0 and abs(r) <= EPS * 1.0001


# ---------------------------------------------------------------------------
# Boolean operators
# ---------------------------------------------------------------------------

@pytest.mark.parametrize("formula, value", [
    # at t = 0.5: x = 2, y = 0.5.  p = x > 1 -> 1,  q = y > 1 -> -0.5
    ("not x[t] > 1", -1),
    ("not not x[t] > 1", 1),
    ("x[t] > 1 and y[t] > 1", -0.5),
    ("x[t] > 1 or y[t] > 1", 1),
    ("x[t] > 1 => y[t] > 1", -0.5),  # max(-1, -0.5)
    ("y[t] > 1 => x[t] > 1", 1),  # max(0.5, 1)
    ("not (x[t] > 1 and y[t] > 1)", 0.5),
    ("(not x[t] > 1) or (not y[t] > 1)", 0.5),  # De Morgan: same as above
])
def test_boolean(formula, value):
    assert rob(formula, 0.5) == pytest.approx(value)


def test_and_or_over_a_range_cross_correctly():
    # x - 1 goes 0 -> 2 -> -1, y goes 0 -> 1 -> 1.  min and max of them on a range
    d = make_driver("signal x, y\nconj := x[t] > 1 and y[t] > 0\ndisj := x[t] > 1 or y[t] > 0", XY)
    za, zo = d.get_rob_signal("conj", 0, 2), d.get_rob_signal("disj", 0, 2)
    for t in [0.25, 0.5, 1, 1.25, 1.5, 1.75]:
        p = (1 + 2 * t if t <= 1 else 3 - 3 * (t - 1)) - 1
        q = t if t <= 1 else 1
        assert za.value_at(t) == pytest.approx(min(p, q)), t
        assert zo.value_at(t) == pytest.approx(max(p, q)), t
