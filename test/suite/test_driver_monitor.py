"""
STLDriver and STLMonitor: copies, independence, and the extra monitor methods.

A monitor is made by driver.get_monitor(name) and has its OWN copy of the data,
the parameters and the semantics.
"""

import copy

import pytest

import stlrom
from conftest import make_driver, points

SPEC = "signal x\nparam a = 1\nphi := x[t] > a\npsi := ev_[0,1] x[t] > a"
DATA = [(0, 1), (1, 3), (2, 0)]  # x: 1 -> 3 -> 0


def test_monitor_gives_the_same_results_as_the_driver():
    d = make_driver(SPEC, DATA)
    m = d.get_monitor("psi")
    assert m.get_rob(0.5) == pytest.approx(d.get_rob("psi", 0.5))
    assert m.get_online_rob(1.5) == pytest.approx(d.get_online_rob("psi", 1.5))
    z1, z2 = m.get_rob_signal(0, 1), d.get_rob_signal("psi", 0, 1)
    for t in (0, 0.3, 0.7, 1):
        assert z1.value_at(t) == pytest.approx(z2.value_at(t))


def test_monitor_data_is_a_copy():
    d = make_driver(SPEC, DATA)
    m = d.get_monitor("phi")
    d.add_sample([3, 10])  # driver only
    m.add_sample([3, -10])  # monitor only
    assert d.get_rob("phi", 3) == pytest.approx(9)
    assert m.get_rob(3) == pytest.approx(-11)


def test_monitor_parameters_are_its_own():
    d = make_driver(SPEC, DATA)
    m = d.get_monitor("phi")
    m.set_param("a", 2)
    assert m.get_param("a") == 2
    assert d.get_param("a") == 1
    assert m.get_rob(1) == pytest.approx(1)  # 3 - 2
    assert d.get_rob("phi", 1) == pytest.approx(2)  # 3 - 1


def test_monitor_copies_parameters_set_before():
    d = make_driver(SPEC, DATA)
    d.set_param("a", 2.5)
    m = d.get_monitor("phi")
    assert m.get_rob(1) == pytest.approx(0.5)


def test_get_monitor_unknown_name():
    with pytest.raises(ValueError):
        make_driver(SPEC, DATA).get_monitor("nope")


def test_empty_monitor_raises_runtime_error():
    m = stlrom.STLMonitor()
    with pytest.raises(RuntimeError):
        m.get_rob(0)
    with pytest.raises(RuntimeError):
        m.get_online_rob(0)


def test_set_eval_time_sets_the_default_range():
    m = make_driver(SPEC, DATA).get_monitor("phi")
    m.set_eval_time(0.5, 1.5)
    z = m.get_rob_signal()
    assert (z.begin_time, z.end_time) == pytest.approx((0.5, 1.5))
    assert m.rob == pytest.approx(1)  # x(0.5) - 1 = 2 - 1


def test_monitor_signal_names_and_signals():
    m = make_driver("signal x, y\nphi := x[t] > y[t]", [(0, 1, 2), (1, 1, 2)]).get_monitor("phi")
    assert m.get_signals_names() == "x y"
    assert m.get_signal_idx("y") == 1
    assert points(m.get_signal("y")) == [(0, 2, 0), (1, 2, 0)]


@pytest.mark.parametrize("make_copy", [lambda d: d.copy(), copy.copy, copy.deepcopy],
                         ids=["copy()", "copy.copy", "copy.deepcopy"])
def test_driver_copies_are_independent(make_copy):
    d = make_driver(SPEC, DATA)
    c = make_copy(d)
    c.add_sample([3, 10])
    c.set_param("a", 0)
    with pytest.raises(ValueError):
        d.get_rob("phi", 3)  # the original has no data at 3
    assert c.get_rob("phi", 3) == pytest.approx(10)
    assert d.get_param("a") == 1


@pytest.mark.parametrize("make_copy", [lambda m: m.copy(), copy.copy, copy.deepcopy],
                         ids=["copy()", "copy.copy", "copy.deepcopy"])
def test_monitor_copies_are_independent(make_copy):
    m = make_driver(SPEC, DATA).get_monitor("phi")
    c = make_copy(m)
    c.add_sample([3, 10])
    with pytest.raises(ValueError):
        m.get_rob(3)
    assert c.get_rob(3) == pytest.approx(9)


def test_many_monitors_from_one_driver():
    d = make_driver(SPEC, DATA)
    monitors = [d.get_monitor("phi") for _ in range(20)]
    for i, m in enumerate(monitors):
        m.add_sample([3 + i, i + 0.5])
    for i, m in enumerate(monitors):
        assert m.get_rob(3 + i) == pytest.approx(i - 0.5)  # (i + 0.5) - a


def test_driver_survives_repeated_requests():
    # the same request many times gives the same answer
    d = make_driver(SPEC, DATA)
    first = d.get_rob("psi", 0.37)
    for _ in range(200):
        assert d.get_rob("psi", 0.37) == first


# ---------------------------------------------------------------------------
# Robustness maps
# ---------------------------------------------------------------------------

def test_robustness_map_structure():
    d = make_driver(SPEC, DATA)
    d.get_rob_signal("psi", 0, 1)
    rmap = d.get_robustness_map("psi")
    assert set(rmap) == {0, 1, 2}  # formula, atom, signal
    assert list(rmap[0]) == ["ev_[0,1] (x[t] > a)"]
    assert list(rmap[1]) == ["x[t] > a"]
    assert list(rmap[2]) == ["x"]
    root = rmap[0]["ev_[0,1] (x[t] > a)"]["z"]
    assert root.value_at(0) == pytest.approx(2)  # max of x on [0,1] is 3


def test_online_robustness_map_has_bounds():
    d = make_driver(SPEC, DATA)
    d.get_online_rob_signal("psi", 0, 2)
    entry = d.get_online_robustness_map("psi")[0]["ev_[0,1] (x[t] > a)"]
    assert entry["z_up"].value_at(1.5) == 10000  # window [1.5, 2.5] reaches past the data
    assert entry["z_low"].value_at(1.5) == pytest.approx(0.5)  # x(1.5) - 1


def test_robustness_map_unknown_name():
    with pytest.raises(ValueError):
        make_driver(SPEC, DATA).get_robustness_map("nope")


def test_disp_and_str_mention_the_formulas():
    d = make_driver(SPEC, DATA)
    text = str(d)
    assert "phi" in text and "psi" in text
