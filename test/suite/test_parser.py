"""
The formula language (docs/stl_language.md): what parses, what does not, and
what the parsed formulas mean.
"""

import pytest

import stlrom
from conftest import make_driver

# x goes 1 -> 3 -> 0 at t = 0, 1, 2 (straight lines)
X3 = [(0, 1), (1, 3), (2, 0)]


def parses(spec):
    return stlrom.STLDriver().parse_string(spec)


# ---------------------------------------------------------------------------
# What is accepted and what is refused
# ---------------------------------------------------------------------------

@pytest.mark.parametrize("spec", [
    "signal x\nphi := x[t] > 0",
    "signal x, y, speed\nphi := x[t] > y[t] and speed[t] < 120",
    "signal x\nphi := x[t] > 0   # a comment",
    "# a comment first\nsignal x\nphi := x[t] > 0",
    "signal x\nphi := alw_[0, 5]\n   (x[t] > 0)",  # a formula over several lines
    "signal x\nparam a = 1, b = -2.5\nphi := x[t] > a and x[t] > b",
    "signal x\nphi := x[t] > .5 and x[t] < 3.",  # numbers without a leading or trailing digit
    "signal x\nphi := ev_[0 5] x[t] > 0",  # interval with a space
    "signal x\nphi := ev [0,1] x[t] > 0",  # no underscore
    "signal x\nphi := x[t+1] > x[t-0.5]",
    "signal x\nf := 2*x[t] + 1\nphi := f[t] > 3",
    "signal x\nmu := x[t] > 0\nphi := not mu and alw_[0,1] mu",
    "signal x\nmu := x[t] > 0\nphi := mu[t] > 1",
    "signal x\nphi := abs(x[t]) > 1",
    "signal x\nphi := -x[t] > 1",
    "signal x\nphi := (x[t] + 1) * 2 > 3",
    "signal x\nphi := x[t] == 1",
    "signal x\nphi := t > 1",
    "signal x\nparam T = 2\nphi := alw_[0, T] x[t] > 0",
    "signal x\nphi := sat(x[t] > 0) and ltr(x[t] > 0) and rtr(x[t] > 0) and rho(x[t] > 0)",
])
def test_accepted(spec):
    assert parses(spec)


@pytest.mark.parametrize("spec, why", [
    ("signal x\nphi := x[t] > 0;", "; is not a separator"),
    ("signal x\nphi := x[t] > 1e-3", "no exponent notation"),
    ("signal x\nphi := x[t] / 2 > 0", "no division"),
    ("signal x\nphi := 0 < x[t] < 1", "no chained comparisons"),
    ("signal x\nsignal y\nphi := x[t] > 0", "only one signal declaration"),
    ("param a = 1\nsignal x\nphi := x[t] > a", "signal must come first"),
    ("signal x\nphi := z[t] > 0", "z is not declared"),
    ("signal x\nphi := x[t] > a\nparam a = 1", "a parameter must be declared before use"),
    ("signal x\nphi := ev_[0, inf] x[t] > 0", "no inf bound"),
    ("signal x\nphi := ev_[-1, 1] x[t] > 0", "no negative bound"),
    ("signal x\nphi := ev x[t] > 0", "temporal operators need an interval"),
    ("signal x\nphi := x[t+1+2] > 0", "only one shift term"),
    ("signal F\nphi := F[t] > 0", "F is reserved"),
    ("signal x\nU := x[t] > 0", "U is reserved"),
    ("signal t\nphi := t[t] > 0", "t is reserved"),
])
def test_refused(spec, why):
    assert not parses(spec), why


def test_unknown_formula_name_raises_and_lists_the_defined_ones():
    d = make_driver("signal x\nphi := x[t] > 0\npsi := x[t] < 0", X3)
    with pytest.raises(ValueError) as e:
        d.get_rob("nope", 0)
    assert "phi" in str(e.value) and "psi" in str(e.value)


def test_parse_file(tmp_path):
    f = tmp_path / "spec.stl"
    f.write_text("signal x\nphi := x[t] > 2\n")
    d = stlrom.STLDriver()
    assert d.parse_file(str(f))
    d.add_sample([0, 5])
    assert d.get_rob("phi", 0) == pytest.approx(3)


def test_parse_file_missing():
    assert not stlrom.STLDriver().parse_file("/no/such/file.stl")


def test_parse_string_twice_adds_definitions():
    d = make_driver("signal x\nphi := x[t] > 0", X3)
    assert d.parse_string("psi := x[t] > 2")
    assert d.get_rob("phi", 1) == pytest.approx(3)
    assert d.get_rob("psi", 1) == pytest.approx(1)


# ---------------------------------------------------------------------------
# Aliases mean the same thing
# ---------------------------------------------------------------------------

@pytest.mark.parametrize("a, b", [
    ("ev_[0,1] x[t] > 2", "F_[0,1] x[t] > 2"),
    ("ev_[0,1] x[t] > 2", "<>_[0,1] x[t] > 2"),
    ("alw_[0,1] x[t] > 2", "G_[0,1] x[t] > 2"),
    ("alw_[0,1] x[t] > 2", "[]_[0,1] x[t] > 2"),
    ("x[t] > 0 until_[0,1] x[t] > 2", "x[t] > 0 U_[0,1] x[t] > 2"),
    ("once_[0,1] x[t] > 2", "O_[0,1] x[t] > 2"),
    ("hist_[0,1] x[t] > 2", "H_[0,1] x[t] > 2"),
    ("x[t] > 0 since_[0,1] x[t] > 2", "x[t] > 0 S_[0,1] x[t] > 2"),
    ("not x[t] > 2", "!x[t] > 2"),
    ("x[t] > 0 and x[t] < 2", "x[t] > 0 & x[t] < 2"),
    ("x[t] > 0 or x[t] < 2", "x[t] > 0 | x[t] < 2"),
    ("x[t] > 2 => x[t] > 1", "x[t] > 2 -> x[t] > 1"),
    ("ev_[0,1] x[t] > 2", "ev_[0 1] x[t] > 2"),
    ("ev_[0,1] x[t] > 2", "ev [0,1] x[t] > 2"),
])
def test_aliases(a, b):
    d = make_driver(f"signal x\nA := {a}\nB := {b}", [(0, 1), (1, 3), (2, 0), (3, 2)])
    for t in (1.0, 1.5):
        assert d.get_rob("A", t) == pytest.approx(d.get_rob("B", t))


# ---------------------------------------------------------------------------
# Precedence
# ---------------------------------------------------------------------------

def test_and_or_have_the_same_precedence_left_to_right():
    # with x = 1:  a = x>0 -> 1,  b = x>5 -> -4,  c = x<0 -> -1
    # (a or b) and c = min(max(1, -4), -1) = -1
    # a or (b and c) = max(1, min(-4, -1)) = 1
    d = make_driver("signal x\nphi := x[t] > 0 or x[t] > 5 and x[t] < 0", [(0, 1), (1, 1)])
    assert d.get_rob("phi", 0) == pytest.approx(-1)


def test_not_binds_tighter_than_or():
    # x = 1:  (not x>0) or x<5 = max(-1, 4) = 4;  not (x>0 or x<5) would be -4
    d = make_driver("signal x\nphi := not x[t] > 0 or x[t] < 5", [(0, 1), (1, 1)])
    assert d.get_rob("phi", 0) == pytest.approx(4)


def test_temporal_operator_binds_tighter_than_and():
    # x goes 0 -> 2 on [0, 2].  At t = 0:
    # (ev_[0,1] x>0) and x>0.5 = min(max of x on [0,1], x(0) - 0.5) = min(1, -0.5) = -0.5
    # ev_[0,1] (x>0 and x>0.5) would be max of x-0.5 on [0,1] = 0.5
    d = make_driver("signal x\nphi := ev_[0,1] x[t] > 0 and x[t] > 0.5", [(0, 0), (2, 2)])
    assert d.get_rob("phi", 0) == pytest.approx(-0.5)


def test_times_binds_tighter_than_plus():
    # 1 + 2*x with x = 3 is 7 (not 9)
    d = make_driver("signal x\nphi := 1 + 2 * x[t] > 0", [(0, 3), (1, 3)])
    assert d.get_rob("phi", 0) == pytest.approx(7)


# ---------------------------------------------------------------------------
# Parameters, functions and reuse
# ---------------------------------------------------------------------------

def test_parameter_value_and_set_param():
    d = make_driver("signal x\nparam a = 1\nphi := x[t] > a", X3)
    assert d.get_param("a") == 1
    assert d.get_rob("phi", 1) == pytest.approx(2)  # 3 - 1
    d.set_param("a", 2.5)
    assert d.get_rob("phi", 1) == pytest.approx(0.5)


def test_parameter_declared_again_changes_its_value():
    d = make_driver("signal x\nparam a = 1\nparam a = 2\nphi := x[t] > a", X3)
    assert d.get_rob("phi", 1) == pytest.approx(1)


def test_negative_parameter():
    d = make_driver("signal x\nparam a = -2\nphi := x[t] > a", X3)
    assert d.get_rob("phi", 1) == pytest.approx(5)


def test_parameter_in_interval():
    # x = t on [0, 4].  alw_[0,T] x>0.5 at t=1: min of x on [1, 1+T] is 1
    d = make_driver("signal x\nparam T = 2\nphi := alw_[0, T] x[t] > 0.5", [(0, 0), (4, 4)])
    assert d.get_rob("phi", 1) == pytest.approx(0.5)
    d.set_param("T", 1)
    assert d.get_rob("phi", 2) == pytest.approx(1.5)  # min of x on [2, 3] is 2


def test_parameter_in_shift():
    d = make_driver("signal x\nparam c = 1\npsi := x[t+c] > 0", [(0, 0), (4, 4)])
    assert d.get_rob("psi", 1) == pytest.approx(2)  # x(2)


def test_set_param_changes_a_shift():
    d = make_driver("signal x\nparam c = 1\npsi := x[t+c] > 0\nneg := x[t-c] > 0", [(0, 0), (4, 4)])
    d.set_param("c", 2)
    assert d.get_rob("psi", 1) == pytest.approx(3)  # x(1+2)
    assert d.get_rob("neg", 3) == pytest.approx(1)  # x(3-2)
    z = d.get_rob_signal("psi", -10, 10)
    assert (z.begin_time, z.end_time) == pytest.approx((-2, 2))  # x on [0, 4], moved by 2


def test_set_param_on_a_monitor_changes_its_shift():
    d = make_driver("signal x\nparam c = 1\npsi := x[t+c] > 0", [(0, 0), (4, 4)])
    m = d.get_monitor("psi")
    m.set_param("c", 2)
    assert m.get_rob(1) == pytest.approx(3)
    assert d.get_rob("psi", 1) == pytest.approx(2)  # the driver keeps c = 1


def test_shift_parameter_that_starts_at_zero():
    # with c = 0 at parse time there is no shift yet; set_param must still create one
    d = make_driver("signal x\nparam c = 0\npsi := x[t+c] > 0", [(0, 0), (4, 4)])
    assert d.get_rob("psi", 1) == pytest.approx(1)
    d.set_param("c", 2)
    assert d.get_rob("psi", 1) == pytest.approx(3)


def test_function_definition():
    d = make_driver("signal x\nf := 2*x[t] + 1\nphi := f[t] > 2.5\npsi := f[t+1] > 2.5", X3)
    assert d.get_rob("phi", 0) == pytest.approx(0.5)  # 2*1 + 1 - 2.5
    assert d.get_rob("psi", 0) == pytest.approx(4.5)  # 2*3 + 1 - 2.5


def test_formula_reused_inside_another():
    # mu = x > 2: at t=1 it is 1.  phi = not mu or mu = max(-1, 1) = 1
    d = make_driver("signal x\nmu := x[t] > 2\nphi := not mu or mu", X3)
    assert d.get_rob("phi", 1) == pytest.approx(1)


def test_formula_robustness_used_as_a_value():
    # mu = x - 2;  psi = mu[t] > 0.5 = x - 2.5
    d = make_driver("signal x\nmu := x[t] > 2\npsi := mu[t] > 0.5", X3)
    assert d.get_rob("psi", 1) == pytest.approx(0.5)


def test_a_name_cannot_be_defined_twice():
    # documented: a name can be defined only once
    assert not parses("signal x\nmu := x[t] > 2\nphi := x[t] > 1\nmu := x[t] > 0")
