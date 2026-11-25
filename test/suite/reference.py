"""
Brute-force reference for STL robustness, used to check stlrom.

This file does not import stlrom. It recomputes robustness straight from the
definitions, as slowly and plainly as possible, so that a person can check it
by reading it.

How it works
------------
Everything is evaluated on one uniform time grid with step H. Each formula is
a small tree of Python objects (Atom, Alw, Until, ...). Calling
`formula.rob(env)` returns a numpy array: the robustness at every grid time.

- A signal is a Trace: a list of points, joined by straight lines
  (steps=False) or held constant until the next point (steps=True).
- Outside the data a trace is NaN ("unknown"). NaN spreads through min, max
  and arithmetic, so a formula is NaN exactly where the data does not cover
  its windows. The valid interval is not coded anywhere: it falls out of this.
- Temporal operators take the min or max over the grid points of their window.
  When a window holds a kink of the signal between two grid points, the
  result can be off by about H times the slope, so comparisons use a
  tolerance (see `compare`).

Online bounds (`bounds(env)`) use the same operators on two arrays, lower and
upper. Where the data is unknown, an atom's lower bound is -BIGM and its upper
bound is +BIGM. `not` swaps and negates them. Every other operator is
monotone, so it is applied to the lower bounds and to the upper bounds
separately.
"""

import math
import numpy as np
from numpy.lib.stride_tricks import sliding_window_view

H = 0.005  # grid step
BIGM = 10000.0  # stlrom's default "infinity" (Signal.get_BigM())
EPS = 1e-10  # stlrom's default Eps (Signal.get_Eps())


def steps_of(x):
    """Number of grid steps in x. x must be a multiple of H."""
    k = round(x / H)
    assert abs(k * H - x) < 1e-9, f"{x} is not a multiple of the grid step {H}"
    return k


def fmt(v):
    """A number as the formula language accepts it: no exponent, no trailing zeros."""
    s = f"{v:.6f}".rstrip("0").rstrip(".")
    return s if s not in ("-0", "") else "0"


# ---------------------------------------------------------------------------
# Signals
# ---------------------------------------------------------------------------

class Trace:
    """A signal given by points (t, v).

    steps=False: straight lines between the points.
    steps=True: each value is held until the next point (the signal jumps there,
    and at the jump time it already has the new value).
    """

    def __init__(self, points, steps=False):
        self.times = np.array([p[0] for p in points], dtype=float)
        self.values = np.array([p[1] for p in points], dtype=float)
        assert np.all(np.diff(self.times) > 0), "times must increase"
        self.steps = steps
        self.start = self.times[0]
        self.end = self.times[-1]

    def __call__(self, t):
        t = np.asarray(t, dtype=float)
        if self.steps:
            i = np.searchsorted(self.times, t + 1e-12, side="right") - 1
            v = self.values[np.clip(i, 0, len(self.values) - 1)]
        else:
            v = np.interp(t, self.times, self.values)
        v = np.array(v, dtype=float)
        v[(t < self.start - 1e-9) | (t > self.end + 1e-9)] = np.nan
        return v

    def until(self, T):
        """The same trace with only the data up to time T (for online checks)."""
        kept = [(t, v) for t, v in zip(self.times, self.values) if t < T - 1e-9]
        kept.append((T, float(self(np.array([T]))[0])))
        return Trace(kept, self.steps)

    def __repr__(self):
        kind = "steps" if self.steps else "linear"
        pts = ", ".join(f"({fmt(t)}, {fmt(v)})" for t, v in zip(self.times, self.values))
        return f"Trace({kind}: {pts})"


class Env:
    """The traces and the time grid that formulas are evaluated on."""

    def __init__(self, traces, t_min, t_max):
        self.traces = traces
        self.k0 = steps_of(t_min)
        self.n = steps_of(t_max) - self.k0 + 1
        self.T = (self.k0 + np.arange(self.n)) * H

    def index(self, t):
        """Position of time t in the grid."""
        return steps_of(t) - self.k0


# ---------------------------------------------------------------------------
# Window helpers: out[k] = op(A[k + lo .. k + hi]), padded outside the grid
# ---------------------------------------------------------------------------

def window(A, lo, hi, op, pad):
    n = len(A)
    left, right = max(0, -lo), max(0, hi)
    P = np.concatenate([np.full(left, pad), A, np.full(right, pad)])
    W = sliding_window_view(P, hi - lo + 1)
    start = lo + left
    return op(W[start:start + n], axis=1)


def until_arrays(p, q, a, b, pad):
    """max over j in [a, b] of min(q[k+j], min(p[k .. k+j]))  (a, b in grid steps)."""
    n = len(p)
    Pp = np.concatenate([p, np.full(b, pad)])
    Pq = np.concatenate([q, np.full(b, pad)])
    out = np.empty(n)
    for k in range(n):
        p_run = np.minimum.accumulate(Pp[k:k + b + 1])  # p_run[j] = min p on [t, t+j]
        out[k] = np.max(np.minimum(Pq[k + a:k + b + 1], p_run[a:b + 1]))
    return out


def since_arrays(p, q, a, b, pad):
    """max over j in [a, b] of min(q[k-j], min(p[k-j .. k]))  (a, b in grid steps)."""
    n = len(p)
    Pp = np.concatenate([np.full(b, pad), p])
    Pq = np.concatenate([np.full(b, pad), q])
    out = np.empty(n)
    for k in range(n):
        p_back = Pp[k:k + b + 1][::-1]  # p_back[j] = p at t - j
        q_back = Pq[k:k + b + 1][::-1]
        p_run = np.minimum.accumulate(p_back)  # p_run[j] = min p on [t-j, t]
        out[k] = np.max(np.minimum(q_back[a:b + 1], p_run[a:b + 1]))
    return out


# ---------------------------------------------------------------------------
# Expressions: value(env) returns an array over the grid
# ---------------------------------------------------------------------------

class Expr:
    def __add__(self, o): return Add(self, wrap(o))
    def __sub__(self, o): return Sub(self, wrap(o))
    def __mul__(self, o): return Mul(self, wrap(o))
    def __neg__(self): return Neg(self)
    def __gt__(self, o): return Atom(self, ">", wrap(o))
    def __lt__(self, o): return Atom(self, "<", wrap(o))
    def __ge__(self, o): return Atom(self, ">=", wrap(o))
    def __le__(self, o): return Atom(self, "<=", wrap(o))


def wrap(v):
    return v if isinstance(v, Expr) else Const(v)


class Var(Expr):
    """x[t], or x[t+c] with shift=c."""

    def __init__(self, name, shift=0.0):
        self.name, self.shift = name, shift

    def text(self):
        if self.shift == 0:
            return f"{self.name}[t]"
        sign = "+" if self.shift > 0 else "-"
        return f"{self.name}[t{sign}{fmt(abs(self.shift))}]"

    def value(self, env):
        return env.traces[self.name](env.T + self.shift)


class Const(Expr):
    def __init__(self, v): self.v = v
    def text(self): return fmt(self.v)
    def value(self, env): return np.full(env.n, float(self.v))


class Time(Expr):
    def text(self): return "t"
    def value(self, env): return env.T.copy()


class Add(Expr):
    def __init__(self, a, b): self.a, self.b = a, b
    def text(self): return f"({self.a.text()} + {self.b.text()})"
    def value(self, env): return self.a.value(env) + self.b.value(env)


class Sub(Expr):
    def __init__(self, a, b): self.a, self.b = a, b
    def text(self): return f"({self.a.text()} - {self.b.text()})"
    def value(self, env): return self.a.value(env) - self.b.value(env)


class Mul(Expr):
    def __init__(self, a, b): self.a, self.b = a, b
    def text(self): return f"({self.a.text()} * {self.b.text()})"
    def value(self, env): return self.a.value(env) * self.b.value(env)


class Neg(Expr):
    def __init__(self, a): self.a = a
    def text(self): return f"-({self.a.text()})"
    def value(self, env): return -self.a.value(env)


class Abs(Expr):
    def __init__(self, a): self.a = a
    def text(self): return f"abs({self.a.text()})"
    def value(self, env): return np.abs(self.a.value(env))


x, y, t = Var("x"), Var("y"), Time()


# ---------------------------------------------------------------------------
# Formulas: rob(env, sem) and bounds(env) return arrays over the grid
# ---------------------------------------------------------------------------

def time_robustness(space, side):
    """Donze-Maler time robustness of an atom, from its space robustness.

    chi = +1 where the atom holds (space > 0), -1 elsewhere.
    side="left":  chi * (how long chi has kept its value, looking back)
    side="right": chi * (how long chi will keep its value, looking forward)
    Durations stop at the edges of the data (where space is NaN).
    """
    n = len(space)
    out = np.full(n, np.nan)
    known = ~np.isnan(space)
    chi = np.where(space > 0, 1.0, -1.0)
    k = 0
    while k < n:
        if not known[k]:
            k += 1
            continue
        # a run: indices s..e with the same chi and known data
        s = k
        e = k
        while e + 1 < n and known[e + 1] and chi[e + 1] == chi[s]:
            e += 1
        data_ends_here = e + 1 >= n or not known[e + 1]
        for i in range(s, e + 1):
            if side == "left":
                out[i] = chi[s] * (i - s) * H
            else:
                # the switch happens between e and e+1; at the end of the data it is at e
                end = e if data_ends_here else e + 1
                out[i] = chi[s] * (end - i) * H
        k = e + 1
    return out


class Formula:
    def __and__(self, o): return And(self, o)
    def __or__(self, o): return Or(self, o)
    def __invert__(self): return Not(self)


class Atom(Formula):
    """e1 > e2, e1 < e2, e1 >= e2, e1 <= e2."""

    def __init__(self, a, op, b):
        self.a, self.op, self.b = a, op, b

    def text(self):
        return f"{self.a.text()} {self.op} {self.b.text()}"

    def space(self, env):
        if self.op in (">", ">="):
            return self.a.value(env) - self.b.value(env)
        return self.b.value(env) - self.a.value(env)

    def rob(self, env, sem="SPACE"):
        r = self.space(env)
        if sem == "SPACE":
            return r
        if sem == "BOOLEAN":
            return np.where(np.isnan(r), np.nan, np.where(r > 0, 1.0, -EPS))
        if sem == "LEFT_TIME":
            return time_robustness(r, "left")
        if sem == "RIGHT_TIME":
            return time_robustness(r, "right")
        raise ValueError(sem)

    def bounds(self, env):
        r = self.space(env)
        return np.where(np.isnan(r), -BIGM, r), np.where(np.isnan(r), BIGM, r)


class Not(Formula):
    def __init__(self, p): self.p = p
    def text(self): return f"not ({self.p.text()})"
    def rob(self, env, sem="SPACE"): return -self.p.rob(env, sem)

    def bounds(self, env):
        lo, up = self.p.bounds(env)
        return -up, -lo


class _Binary(Formula):
    word = None
    op = None

    def __init__(self, p, q): self.p, self.q = p, q
    def text(self): return f"({self.p.text()}) {self.word} ({self.q.text()})"
    def rob(self, env, sem="SPACE"): return self.op(self.p.rob(env, sem), self.q.rob(env, sem))

    def bounds(self, env):
        (pl, pu), (ql, qu) = self.p.bounds(env), self.q.bounds(env)
        return self.op(pl, ql), self.op(pu, qu)


class And(_Binary):
    word = "and"
    op = staticmethod(np.minimum)


class Or(_Binary):
    word = "or"
    op = staticmethod(np.maximum)


class Implies(Formula):
    """p => q, which is (not p) or q."""

    def __init__(self, p, q): self.p, self.q = p, q
    def text(self): return f"({self.p.text()}) => ({self.q.text()})"
    def rob(self, env, sem="SPACE"): return Or(Not(self.p), self.q).rob(env, sem)
    def bounds(self, env): return Or(Not(self.p), self.q).bounds(env)


class _Window(Formula):
    """ev, alw, once, hist over [a, b]."""
    word = None
    past = False
    op = None

    def __init__(self, a, b, p):
        self.a, self.b, self.p = a, b, p

    def text(self):
        return f"{self.word}_[{fmt(self.a)},{fmt(self.b)}] ({self.p.text()})"

    def _apply(self, A, pad):
        a, b = steps_of(self.a), steps_of(self.b)
        lo, hi = (-b, -a) if self.past else (a, b)
        return window(A, lo, hi, self.op, pad)

    def rob(self, env, sem="SPACE"):
        return self._apply(self.p.rob(env, sem), np.nan)

    def bounds(self, env):
        lo, up = self.p.bounds(env)
        return self._apply(lo, -BIGM), self._apply(up, BIGM)


class Ev(_Window):
    word, past, op = "ev", False, staticmethod(np.max)


class Alw(_Window):
    word, past, op = "alw", False, staticmethod(np.min)


class Once(_Window):
    word, past, op = "once", True, staticmethod(np.max)


class Hist(_Window):
    word, past, op = "hist", True, staticmethod(np.min)


class Until(Formula):
    """p until_[a,b] q: max over t' in [t+a, t+b] of min(q(t'), min of p on [t, t'])."""

    def __init__(self, p, a, b, q):
        self.p, self.a, self.b, self.q = p, a, b, q

    def text(self):
        return f"({self.p.text()}) until_[{fmt(self.a)},{fmt(self.b)}] ({self.q.text()})"

    def rob(self, env, sem="SPACE"):
        return until_arrays(self.p.rob(env, sem), self.q.rob(env, sem),
                            steps_of(self.a), steps_of(self.b), np.nan)

    def bounds(self, env):
        (pl, pu), (ql, qu) = self.p.bounds(env), self.q.bounds(env)
        a, b = steps_of(self.a), steps_of(self.b)
        return until_arrays(pl, ql, a, b, -BIGM), until_arrays(pu, qu, a, b, BIGM)


class Since(Formula):
    """p since_[a,b] q: max over t' in [t-b, t-a] of min(q(t'), min of p on [t', t])."""

    def __init__(self, p, a, b, q):
        self.p, self.a, self.b, self.q = p, a, b, q

    def text(self):
        return f"({self.p.text()}) since_[{fmt(self.a)},{fmt(self.b)}] ({self.q.text()})"

    def rob(self, env, sem="SPACE"):
        return since_arrays(self.p.rob(env, sem), self.q.rob(env, sem),
                            steps_of(self.a), steps_of(self.b), np.nan)

    def bounds(self, env):
        (pl, pu), (ql, qu) = self.p.bounds(env), self.q.bounds(env)
        a, b = steps_of(self.a), steps_of(self.b)
        return since_arrays(pl, ql, a, b, -BIGM), since_arrays(pu, qu, a, b, BIGM)


class Local(Formula):
    """sat(p), ltr(p), rtr(p), rho(p): evaluate p's atoms with a fixed semantics."""
    WORDS = {"BOOLEAN": "sat", "LEFT_TIME": "ltr", "RIGHT_TIME": "rtr", "SPACE": "rho"}

    def __init__(self, sem, p):
        self.sem, self.p = sem, p

    def text(self):
        return f"{self.WORDS[self.sem]}({self.p.text()})"

    def rob(self, env, sem="SPACE"):
        return self.p.rob(env, self.sem)  # the innermost semantics wins


def horizon(f):
    """How far a formula looks (past, future) from t. Used to size the grid."""
    if isinstance(f, Atom):
        shifts = [e.shift for e in _vars(f.a) + _vars(f.b)]
        return max([0.0] + [-s for s in shifts]), max([0.0] + shifts)
    if isinstance(f, (Not, Local)):
        return horizon(f.p)
    if isinstance(f, (_Binary, Implies)):
        (p1, f1), (p2, f2) = horizon(f.p), horizon(f.q)
        return max(p1, p2), max(f1, f2)
    if isinstance(f, _Window):
        p, fu = horizon(f.p)
        return (p + f.b, fu) if f.past else (p, fu + f.b)
    if isinstance(f, (Until, Since)):
        (p1, f1), (p2, f2) = horizon(f.p), horizon(f.q)
        p, fu = max(p1, p2), max(f1, f2)
        return (p + f.b, fu) if isinstance(f, Since) else (p, fu + f.b)
    raise TypeError(f)


def _vars(e):
    if isinstance(e, Var):
        return [e]
    if isinstance(e, (Add, Sub, Mul)):
        return _vars(e.a) + _vars(e.b)
    if isinstance(e, (Neg, Abs)):
        return _vars(e.a)
    return []


def make_env(formula, traces, t_min, t_max):
    """A grid wide enough to evaluate `formula` at every time in [t_min, t_max].

    It always covers all the data too: time robustness looks at the whole
    data, however far from t.
    """
    past, future = horizon(formula)
    data_lo = min(tr.start for tr in traces.values())
    data_hi = max(tr.end for tr in traces.values())
    lo = math.floor(min(t_min - past, data_lo) / H) * H - H
    hi = math.ceil(max(t_max + future, data_hi) / H) * H + H
    return Env(traces, round(lo, 9), round(hi, 9))


def evaluate(formula, traces, times, sem="SPACE", jump=0.15):
    """Robustness of `formula` at each of `times`.

    Returns (values, near_jump):
    - values: the robustness, NaN where the data does not cover the formula.
    - near_jump: True where the result jumps by more than `jump` within two grid
      steps. At a jump the value at the exact instant is a convention, and grid
      sampling can put the jump one step off, so comparisons skip those times.
    """
    env = make_env(formula, traces, min(times) - 2 * H, max(times) + 2 * H)
    r = formula.rob(env, sem)
    values, near = [], []
    for t in times:
        k = env.index(_on_grid(t))
        values.append(r[k])
        near.append(bool(np.any(np.abs(np.diff(r[k - 2:k + 3])) > jump)))
    return np.array(values), np.array(near)


def evaluate_bounds(formula, traces, times, jump=0.15):
    """Online bounds of `formula` at each of `times`, from the data in `traces`.

    Returns (lower, upper, near_jump), with near_jump as in `evaluate` (True
    where either bound jumps, for example from a value to +-BIGM).
    """
    env = make_env(formula, traces, min(times) - 2 * H, max(times) + 2 * H)
    lo, up = formula.bounds(env)
    lows, ups, near = [], [], []
    for t in times:
        k = env.index(_on_grid(t))
        lows.append(lo[k])
        ups.append(up[k])
        near.append(bool(np.any(np.abs(np.diff(lo[k - 2:k + 3])) > jump)
                         or np.any(np.abs(np.diff(up[k - 2:k + 3])) > jump)))
    return np.array(lows), np.array(ups), np.array(near)


def _on_grid(t):
    return round(round(t / H) * H, 9)
