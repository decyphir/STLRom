"""
Getting data into a driver: add_sample, set_signals, CSV files, and reading it back.
"""

import pytest

import stlrom
from conftest import make_driver, points, signal_from


def test_add_sample_fills_every_signal_in_declaration_order():
    d = make_driver("signal x, y", [(0, 1, 10), (1, 2, 20)])
    assert points(d.get_signal("x")) == [(0, 1, 1), (1, 2, 0)]
    assert points(d.get_signal("y")) == [(0, 10, 10), (1, 20, 0)]


def test_add_sample_without_interp_holds_the_previous_value():
    d = make_driver("signal x\nphi := x[t] > 0")
    d.add_sample([0, 1], False)
    d.add_sample([1, 3], False)
    d.add_sample([2, 3], False)
    assert d.get_rob("phi", 0.5) == pytest.approx(1)  # still 1, not 2
    assert d.get_rob("phi", 1.5) == pytest.approx(3)


def test_add_sample_with_the_wrong_length_raises():
    d = make_driver("signal x, y")
    with pytest.raises(ValueError):
        d.add_sample([0, 1])
    with pytest.raises(ValueError):
        d.add_sample([0, 1, 2, 3])


def test_add_sample_at_an_earlier_time_is_ignored():
    d = make_driver("signal x", [(0, 1), (1, 2)])
    d.add_sample([1, 100])
    d.add_sample([0.5, 100])
    assert points(d.get_signal("x")) == [(0, 1, 1), (1, 2, 0)]


def test_set_signals():
    d = make_driver("signal x, y\nphi := x[t] > y[t]")
    d.set_signals([signal_from([(0, 1), (2, 3)]), signal_from([(0, 0), (2, 0)])])
    assert d.get_rob("phi", 1) == pytest.approx(2)


def test_set_signals_with_the_wrong_count_raises():
    d = make_driver("signal x, y")
    with pytest.raises(ValueError):
        d.set_signals([signal_from([(0, 1), (1, 1)])])


def test_set_signals_replaces_the_data():
    d = make_driver("signal x\nphi := x[t] > 0", [(0, 5), (1, 5)])
    d.set_signals([signal_from([(0, 1), (1, 1)])])
    assert d.get_rob("phi", 0) == pytest.approx(1)


def test_signals_can_cover_different_times():
    # x on [0, 2], y on [1, 3]: x > y is only defined on [1, 2]
    d = make_driver("signal x, y\nphi := x[t] > y[t]")
    d.set_signals([signal_from([(0, 0), (2, 2)]), signal_from([(1, 0), (3, 0)])])
    z = d.get_rob_signal("phi", 0, 3)
    assert (z.begin_time, z.end_time) == pytest.approx((1, 2))


def test_names_and_indices():
    d = make_driver("signal x, y, speed")
    assert d.get_signals_names() == "x y speed"
    assert d.get_signal_idx("speed") == 2
    assert d.get_signame_from_idx(1) == "y"


def test_get_signal_by_name_or_index():
    d = make_driver("signal x, y", [(0, 1, 2), (1, 1, 2)])
    assert points(d.get_signal("y")) == points(d.get_signal(1))


def test_write_csv_then_load_csv(tmp_path):
    d = make_driver("signal x, y", [(0, 1, -1), (0.5, 2, 0.25), (2, 0, 3)])
    d.write_csv(str(tmp_path))
    files = [str(tmp_path / "x.csv"), str(tmp_path / "y.csv")]
    e = make_driver("signal x, y")
    e.load_csv(files)
    for name in ("x", "y"):
        for got, want in zip(points(e.get_signal(name)), points(d.get_signal(name))):
            assert got == pytest.approx(want)


def test_load_csv_with_two_columns(tmp_path):
    f = tmp_path / "x.csv"
    f.write_text("0,1\n1,3\n2,0\n")
    d = make_driver("signal x\nphi := x[t] > 0")
    d.load_csv([str(f)])
    # without a slope column the slope is 0: each value is held until the next row
    assert d.get_rob("phi", 0.5) == pytest.approx(1)
    assert d.get_rob("phi", 1.5) == pytest.approx(3)


def test_load_csv_with_slopes(tmp_path):
    f = tmp_path / "x.csv"
    f.write_text("0,1,2\n1,3,-3\n2,0,0\n")  # the slopes join the rows with lines
    d = make_driver("signal x\nphi := x[t] > 0")
    d.load_csv([str(f)])
    assert d.get_rob("phi", 0.5) == pytest.approx(2)
    assert d.get_rob("phi", 1.5) == pytest.approx(1.5)


def test_load_csv_missing_file_raises(tmp_path):
    d = make_driver("signal x")
    with pytest.raises(ValueError):
        d.load_csv([str(tmp_path / "nope.csv")])


def test_load_csv_bad_row_raises(tmp_path):
    f = tmp_path / "x.csv"
    f.write_text("0,1\n1,2,3,4\n")
    d = make_driver("signal x")
    with pytest.raises(ValueError):
        d.load_csv([str(f)])


def test_stldata_add_signal_sample_to_one_signal():
    d = make_driver("signal x, y")
    d.data.add_signal_sample("y", 0, 5)
    d.data.add_signal_sample("nope", 0, 5)  # unknown names are ignored
    assert d.get_signal("x").empty()
    assert points(d.get_signal("y")) == [(0, 5, 0)]


def test_stldata_signal_map():
    d = make_driver("signal x, y")
    assert dict(d.data.signal_map) == {"x": 0, "y": 1}
