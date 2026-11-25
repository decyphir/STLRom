"""
Data coverage: a formula only has a value where the data covers all its windows.

Data: x on [1, 4], y on [2, 5] (straight lines; the values do not matter here).

For each formula the expected valid interval comes from its definition:
- alw/ev/x[t+k] over [a, b] at t need the child on [t+a, t+b]  -> [s-a, e-b]
- hist/once over [a, b] at t need the child on [t-b, t-a]        -> [s+b, e+a]
- p until_[a,b] q needs p on [t, t+b] and q on [t+a, t+b]
- p since_[a,b] q needs p on [t-b, t] and q on [t-b, t-a]
where [s, e] is the child's own valid interval.
"""

import math

import pytest

import stlrom
from conftest import make_driver, signal_from

X = [(1, 0.5), (2.5, 2), (4, 1)]
Y = [(2, 1), (3, -1), (5, 0.5)]


def driver(formula):
    d = make_driver(f"signal x, y\nphi := {formula}")
    d.set_signals([signal_from(X), signal_from(Y)])
    return d


TABLE = [
    # formula, valid interval, why
    ("x[t] > 0", (1, 4), "x's data"),
    ("y[t] > 0", (2, 5), "y's data"),
    ("x[t] > 0 and y[t] > 0", (2, 4), "intersection"),
    ("x[t] > y[t]", (2, 4), "intersection"),
    ("not x[t] > 0", (1, 4), "same as the child"),
    ("abs(x[t]) > 0", (1, 4), "same as the child"),
    ("x[t] * y[t] > 0", (2, 4), "intersection"),
    ("x[t+0.5] > 0", (0.5, 3.5), "[1-0.5, 4-0.5]"),
    ("x[t-0.5] > 0", (1.5, 4.5), "[1+0.5, 4+0.5]"),
    ("ev_[0.5,1] x[t] > 0", (0.5, 3), "[1-0.5, 4-1]"),
    ("alw_[0.5,1] x[t] > 0", (0.5, 3), "[1-0.5, 4-1]"),
    ("once_[0.5,1] x[t] > 0", (2, 4.5), "[1+1, 4+0.5]"),
    ("hist_[0.5,1] x[t] > 0", (2, 4.5), "[1+1, 4+0.5]"),
    ("x[t] > 0 until_[0.5,1] y[t] > 0", (1.5, 3), "[max(1, 2-0.5), min(4-1, 5-1)]"),
    ("x[t] > 0 since_[0.5,1] y[t] > 0", (3, 4), "[max(1+1, 2+1), min(4, 5+0.5)]"),
    ("ev_[0,1] once_[0,1] x[t] > 0", (2, 3), "once: [2, 4], then ev: [2, 3]"),
    ("ev_[0,1] x[t] > 0 or y[t] > 0", (2, 3), "[0, 3] and [2, 5]"),
]


@pytest.mark.parametrize("formula, interval, why", TABLE, ids=[c[0] for c in TABLE])
def test_range_request_is_cut_to_the_valid_interval(formula, interval, why):
    z = driver(formula).get_rob_signal("phi", -10, 10)
    assert (z.begin_time, z.end_time) == pytest.approx(interval), why


@pytest.mark.parametrize("formula, interval, why", TABLE, ids=[c[0] for c in TABLE])
def test_single_time_accepted_at_the_edges(formula, interval, why):
    d = driver(formula)
    s, e = interval
    for t in (s, e, (s + e) / 2):
        assert not math.isnan(d.get_rob("phi", t))


@pytest.mark.parametrize("formula, interval, why", TABLE, ids=[c[0] for c in TABLE])
def test_single_time_refused_outside(formula, interval, why):
    d = driver(formula)
    s, e = interval
    for t in (s - 0.01, e + 0.01):
        with pytest.raises(ValueError):
            d.get_rob("phi", t)


def test_refusal_message_names_the_formula_and_interval():
    d = driver("ev_[0.5,1] x[t] > 0")
    with pytest.raises(ValueError) as e:
        d.get_rob("phi", 3.5)
    msg = str(e.value)
    assert "ev_[0.5,1]" in msg and "[0.5, 3]" in msg and "3.5" in msg


def test_edges_on_a_time_grid():
    # data from 1.3: times like i*0.01 are not exact, but the edges must still be accepted
    d = make_driver("signal x\nphi := ev_[0.2,0.7] x[t] > 0")
    d.set_signals([signal_from([(1.3, 1), (2.9, 2)])])
    d.get_rob("phi", 110 * 0.01)  # 1.1 = 1.3 - 0.2
    d.get_rob("phi", 220 * 0.01)  # 2.2 = 2.9 - 0.7


def test_formulas_without_signals_are_valid_everywhere():
    d = driver("t > 0 and 1 > 0")
    assert d.get_rob("phi", -1000) == pytest.approx(-1000)
    assert d.get_rob("phi", 1000) == pytest.approx(1)


def test_range_outside_the_data_is_empty():
    z = driver("x[t] > 0").get_rob_signal("phi", 6, 8)
    assert z.empty()


def test_range_partly_outside_is_cut():
    z = driver("x[t] > 0").get_rob_signal("phi", 3, 8)
    assert (z.begin_time, z.end_time) == pytest.approx((3, 4))


def test_window_longer_than_the_data():
    d = driver("alw_[0,10] x[t] > 0")
    assert d.get_rob_signal("phi", -10, 10).empty()
    with pytest.raises(ValueError):
        d.get_rob("phi", 1)


# ---------------------------------------------------------------------------
# No data
# ---------------------------------------------------------------------------

def test_no_samples_get_rob_raises():
    d = make_driver("signal x\nphi := x[t] > 0")
    with pytest.raises(ValueError):
        d.get_rob("phi", 0)


def test_no_samples_get_rob_signal_is_empty():
    d = make_driver("signal x\nphi := x[t] > 0")
    assert d.get_rob_signal("phi", 0, 1).empty()


def test_no_samples_constant_formula_still_computes():
    d = make_driver("signal x\nphi := 1 > 0")
    assert d.get_rob("phi", 0) == pytest.approx(1)


def test_one_signal_empty_the_other_not():
    d = make_driver("signal x, y\nphi := x[t] > 0 and y[t] > 0")
    d.set_signals([signal_from(X), stlrom.Signal()])
    with pytest.raises(ValueError):
        d.get_rob("phi", 2)
    assert d.get_rob_signal("phi", 0, 5).empty()
