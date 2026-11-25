"""
The Signal class: building signals, reading values, and the operations on them.

A Signal is a list of samples (time, value, slope). Between two samples the
value follows the first sample's slope.
"""

import copy
import math

import pytest

import stlrom
from conftest import EPS, make_driver, points, signal_from


# ---------------------------------------------------------------------------
# Building
# ---------------------------------------------------------------------------

def test_append_linear_sample_sets_the_slopes():
    s = signal_from([(0, 1), (1, 3), (2, 0)])
    # slopes: (3-1)/1 = 2, then (0-3)/1 = -3, and 0 on the last sample
    assert points(s) == [(0, 1, 2), (1, 3, -3), (2, 0, 0)]
    assert (s.begin_time, s.end_time) == (0, 2)


def test_append_constant_sample_holds_values():
    s = signal_from([(0, 1), (1, 3), (2, 0)], steps=True)
    assert points(s) == [(0, 1, 0), (1, 3, 0), (2, 0, 0)]


def test_append_sample_with_and_without_slope():
    s = stlrom.Signal()
    s.append_sample(0, 1)  # slope 0
    s.append_sample(1, 2, 0.5)  # explicit slope
    assert points(s) == [(0, 1, 0), (1, 2, 0.5)]


def test_append_sample_with_interp_replaces_the_previous_slope():
    s = stlrom.Signal()
    s.append_sample(0, 1, 5.0)
    s.append_sample(2, 3, 0.0, True)  # joins (0,1) to (2,3): previous slope becomes 1
    assert points(s)[0] == (0, 1, 1)


@pytest.mark.parametrize("t_bad", [2, 1.5])
def test_sample_at_or_before_the_last_time_is_ignored(t_bad):
    s = signal_from([(0, 1), (2, 3)])
    s.append_linear_sample(t_bad, 100)
    assert points(s) == [(0, 1, 1), (2, 3, 0)]


def test_empty():
    s = stlrom.Signal()
    assert s.empty()
    s.append_linear_sample(0, 1)
    assert not s.empty()


# ---------------------------------------------------------------------------
# Reading values
# ---------------------------------------------------------------------------

def test_value_at_inside_pieces():
    s = signal_from([(0, 1), (1, 3), (2, 0)])
    assert s.value_at(0.5) == pytest.approx(2)
    assert s.value_at(1.5) == pytest.approx(1.5)
    assert s.value_at(1) == pytest.approx(3)


def test_value_at_steps_between_jumps():
    s = signal_from([(0, 1), (1, 3), (2, 0)], steps=True)
    assert s.value_at(0.5) == 1
    assert s.value_at(1.5) == 3


def test_value_at_a_jump_is_the_new_value():
    # 1 on [0,1), 3 on [1,2), 0 at 2: at t = 1 the sample says 3
    s = signal_from([(0, 1), (1, 3), (2, 0)], steps=True)
    assert s.value_at(1) == 3
    assert s.value_at(2) == 0


def test_value_at_before_the_first_sample_holds_the_first_value(capfd):
    # as resize does, and it prints a warning
    s = signal_from([(0, 1), (1, 3)])
    assert s.value_at(-1) == pytest.approx(1)
    assert "Warning" in capfd.readouterr().out


def test_value_at_after_the_end_holds_the_last_value(capfd):
    s = signal_from([(0, 1), (1, 3)])
    assert s.value_at(5) == pytest.approx(3)
    assert "Warning" in capfd.readouterr().out


def test_value_at_between_the_last_sample_and_end_time_follows_its_slope(capfd):
    # end_time can be later than the last sample: that piece follows the last slope
    s = stlrom.Signal()
    s.append_sample(0, 1, 2.0)
    s.end_time = 1
    assert s.value_at(1) == pytest.approx(3)
    assert "Warning" not in capfd.readouterr().out


def test_value_at_inside_does_not_warn(capfd):
    s = signal_from([(0, 1), (1, 3)])
    s.value_at(0)
    s.value_at(0.5)
    s.value_at(1)
    assert "Warning" not in capfd.readouterr().out


def test_sample_value_at_and_attributes():
    smp = stlrom.Sample(1, 2, 3)
    assert (smp.time, smp.value, smp.derivative) == (1, 2, 3)
    assert smp.value_at(2) == pytest.approx(5)
    p = stlrom.Point(1, 2)
    assert (p.time, p.value) == (1, 2)


# ---------------------------------------------------------------------------
# resize, shift, reverse
# ---------------------------------------------------------------------------

def test_resize_inside_keeps_the_values():
    s = signal_from([(0, 0), (4, 4)])
    s.resize(1, 3)
    assert (s.begin_time, s.end_time) == (1, 3)
    for t in (1, 2, 3):
        assert s.value_at(t) == pytest.approx(t)


def test_resize_outside_holds_the_end_values():
    s = signal_from([(1, 1), (2, 3)])
    s.resize(0, 4)
    assert (s.begin_time, s.end_time) == (0, 4)
    assert s.value_at(0.5) == pytest.approx(1)  # first value held before the data
    assert s.value_at(3.5) == pytest.approx(3)  # last value held after the data


def test_shift():
    s = signal_from([(0, 1), (1, 3)])
    s.shift(2)
    assert (s.begin_time, s.end_time) == (2, 3)
    assert s.value_at(2.5) == pytest.approx(2)


def test_reverse_linear():
    # value at t moves to begin + end - t
    s = signal_from([(0, 0), (1, 2), (3, 0)])
    s.reverse()
    assert (s.begin_time, s.end_time) == (0, 3)
    for t, v in [(0, 0), (1, 1), (2, 2), (2.5, 1), (3, 0)]:
        assert s.value_at(t) == pytest.approx(v), t


def test_reverse_steps():
    # 1 on [0,1), 3 on [1,3]  ->  3 on [0,2), 1 on (2,3]
    s = signal_from([(0, 1), (1, 3), (3, 3)], steps=True)
    s.reverse()
    assert s.value_at(0.5) == pytest.approx(3)
    assert s.value_at(1.5) == pytest.approx(3)
    assert s.value_at(2.5) == pytest.approx(1)


def test_reverse_twice_gives_the_original_values():
    s = signal_from([(0, 1), (0.7, -2), (1.3, 0.5), (2, 2)])
    r = s.copy()
    r.reverse()
    r.reverse()
    for t in (0.1, 0.5, 1, 1.5, 1.9):
        assert r.value_at(t) == pytest.approx(s.value_at(t))


# ---------------------------------------------------------------------------
# Operations
# ---------------------------------------------------------------------------

def test_compute_not():
    s = signal_from([(0, 1), (1, -2)])
    r = stlrom.Signal()
    r.compute_not(s)
    assert r.value_at(0) == pytest.approx(-1)
    assert r.value_at(0.5) == pytest.approx(0.5)


def test_compute_and_or():
    a = signal_from([(0, 0), (2, 2)])  # rising
    b = signal_from([(0, 2), (2, 0)])  # falling, they cross at t = 1
    lo, hi = stlrom.Signal(), stlrom.Signal()
    lo.compute_and(a, b)
    hi.compute_or(a, b)
    for t, mn, mx in [(0, 0, 2), (0.5, 0.5, 1.5), (1, 1, 1), (1.5, 0.5, 1.5), (2, 0, 2)]:
        assert lo.value_at(t) == pytest.approx(mn), t
        assert hi.value_at(t) == pytest.approx(mx), t


# compute_boolean and the time robustness read the sign of each sample: they do
# not look for a sign change inside a piece (in a formula, the atom adds samples
# at the crossings first). So these tests use step signals.

def test_compute_boolean():
    s = signal_from([(0, -1), (1, 1), (2, 1)], steps=True)
    b = stlrom.Signal()
    b.compute_boolean(s)
    assert b.value_at(0.5) == pytest.approx(-EPS, abs=1e-15)
    assert b.value_at(1.5) == 1


def test_compute_time_robustness():
    # holds on [1, 3): -1 on [0,1), 1 on [1,3), -1 on [3,4]
    s = signal_from([(0, -1), (1, 1), (3, -1), (4, -1)], steps=True)
    left, right = stlrom.Signal(), stlrom.Signal()
    left.compute_left_time_rob(s)
    right.compute_right_time_rob(s)
    assert left.value_at(0.5) == pytest.approx(-0.5)  # false since 0
    assert left.value_at(2) == pytest.approx(1)  # true since 1
    assert left.value_at(3.5) == pytest.approx(-0.5)  # false since 3
    assert right.value_at(0.5) == pytest.approx(-0.5)  # false until 1
    assert right.value_at(2) == pytest.approx(1)  # true until 3
    assert right.value_at(3.5) == pytest.approx(-0.5)  # false until the end, 4


# ---------------------------------------------------------------------------
# Files and copies
# ---------------------------------------------------------------------------

def test_write_then_read_gives_the_same_signal(tmp_path):
    s = signal_from([(0, 1), (0.5, -2.25), (1.75, 3)])
    f = str(tmp_path / "s.csv")
    s.write_to_file(f)
    r = stlrom.Signal()
    r.read_from_file(f)
    for got, want in zip(points(r), points(s)):
        assert got == pytest.approx(want)
    assert len(points(r)) == len(points(s))


@pytest.mark.parametrize("make_copy", [lambda s: s.copy(), copy.copy, copy.deepcopy])
def test_copies_are_independent(make_copy):
    s = signal_from([(0, 1), (1, 2)])
    c = make_copy(s)
    c.append_linear_sample(2, 5)
    c.shift(10)
    assert points(s) == [(0, 1, 1), (1, 2, 0)]


# ---------------------------------------------------------------------------
# Global settings
# ---------------------------------------------------------------------------

def test_bigm_set_and_get():
    assert stlrom.Signal.get_BigM() == 10000
    stlrom.Signal.set_BigM(500)
    assert stlrom.Signal.get_BigM() == 500


def test_values_must_stay_below_bigm():
    # BigM stands for +infinity: values must stay below it. Here t > 0 is t, and its
    # minimum over [10001, 10002] is 10001, which is above the default BigM (10000)
    d = make_driver("signal x\nphi := alw_[0,1] (t > 0)", [(0, 1), (1, 1)])
    stlrom.Signal.set_BigM(1e6)
    assert d.get_rob("phi", 10001) == pytest.approx(10001)


def test_get_bigm_rounds_down():
    stlrom.Signal.set_BigM(10.7)
    assert stlrom.Signal.get_BigM() == 10


def test_eps_set_and_get():
    assert stlrom.Signal.get_Eps() == EPS
    stlrom.Signal.set_Eps(1e-6)
    assert stlrom.Signal.get_Eps() == 1e-6


def test_semantics_set_and_get():
    assert stlrom.Signal.get_semantics() == "SPACE"
    stlrom.Signal.set_semantics("BOOLEAN")
    assert stlrom.Signal.get_semantics() == "BOOLEAN"


def test_nan_is_not_used_for_empty_signals():
    s = stlrom.Signal()
    assert not math.isnan(s.value_at(0))  # an empty signal returns 0 (and prints a message)
