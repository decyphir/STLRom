"""
Pure-Python helpers: signal generators and plotting.

Plotting is only checked to run and return an Axes (with the Agg backend,
which draws nothing on screen).
"""

import math

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt  # noqa: E402
import pytest  # noqa: E402

import stlrom  # noqa: E402
from conftest import make_driver, points  # noqa: E402


@pytest.fixture(autouse=True)
def close_figures():
    yield
    plt.close("all")


# ---------------------------------------------------------------------------
# Signal generators
# ---------------------------------------------------------------------------

def test_pwl_generator():
    s = stlrom.PWLSignalGen(times=[0, 1, 3], values=[0, 2, 1]).get_signal(0, 3)
    for t, v in [(0, 0), (0.5, 1), (1, 2), (2, 1.5), (3, 1)]:
        assert s.value_at(t) == pytest.approx(v), t


def test_pwc_generator():
    s = stlrom.PWCSignalGen(times=[0, 1, 3], values=[0, 2, 1]).get_signal(0, 4)
    for t, v in [(0.5, 0), (1.5, 2), (2.5, 2), (3.5, 1)]:
        assert s.value_at(t) == pytest.approx(v), t


def test_oscillating_generator():
    s = stlrom.OscillSignalGen(period=2, amplitude=3, base=1).get_signal(0, 4, dt=0.01)
    for t in (0.5, 1.5, 2.25):
        assert s.value_at(t) == pytest.approx(3 * math.sin(math.pi * t) + 1, abs=1e-3), t


def test_random_generator_is_repeatable():
    a = stlrom.RandSignalGen(seed=7).get_signal(0, 5)
    b = stlrom.RandSignalGen(seed=7).get_signal(0, 5)
    c = stlrom.RandSignalGen(seed=8).get_signal(0, 5)
    assert points(a) == points(b)
    assert points(a) != points(c)


def test_random_generator_respects_its_limits():
    g = stlrom.RandSignalGen(seed=3, v_min=-2, v_max=2, dt_min=0.1, dt_max=0.5)
    s = g.get_signal(0, 20)
    samples = points(s)
    assert all(-2 <= v <= 2 for _, v, _ in samples)
    gaps = [b[0] - a[0] for a, b in zip(samples, samples[1:])]
    assert all(g <= 0.5 + 1e-9 for g in gaps)


def test_generated_signal_works_in_a_driver():
    s = stlrom.PWLSignalGen(times=[0, 1, 3], values=[0, 2, 1]).get_signal(0, 3)
    d = make_driver("signal x\nphi := ev_[0,2] x[t] > 1")
    d.set_signals([s])
    assert d.get_rob("phi", 0) == pytest.approx(1)


def test_time_array():
    a = stlrom.get_time_array(0, 1, 0.25)
    assert list(a) == pytest.approx([0, 0.25, 0.5, 0.75, 1])


# ---------------------------------------------------------------------------
# Plotting
# ---------------------------------------------------------------------------

def test_signal_plot():
    s = stlrom.PWLSignalGen(times=[0, 1], values=[0, 2]).get_signal(0, 1)
    ax = s.plot(label="x", draw_samples=True)
    assert ax is not None


def test_driver_plots():
    d = make_driver("signal x, y\nphi := alw_[0,1] x[t] > y[t]", [(0, 1, 0), (1, 2, 1), (2, 0, 1)])
    assert d.plot_signal("x") is not None
    assert d.plot_signals() is not None


def test_plot_robustness_map():
    d = make_driver("signal x\nphi := alw_[0,1] x[t] > 0", [(0, 1), (1, 2), (2, 0)])
    d.get_online_rob_signal("phi", 0, 2)
    rmap = d.get_online_robustness_map("phi")
    for what in ("robustness", "lower", "upper"):
        assert stlrom.plot_rob_map(rmap, to_plot=what) is not None
