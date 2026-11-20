# The STLRom formula language

This page describes the text passed to `STLDriver.parse_string` or `parse_file`, and everything it accepts. The grammar itself is in [src/parser.y](../src/parser.y) and the keywords in [src/scanner.l](../src/scanner.l).

```
# signals first, once
signal x, y, speed

# parameters (can be changed later with set_param)
param a = 1, T = 2.5, low = -3

# formulas
mu    := x[t] > a
phi   := alw_[0, T] (mu and speed[t] < 120)
react := (x[t] > 0) => ev_[0, 1] (y[t] > x[t-0.5])

# functions: an expression without a comparison
energy := 0.5 * speed[t] * speed[t]
safe   := energy[t] < 7200
```

## Structure of a specification

A specification is a sequence of statements. Line breaks and spaces have no meaning, so a formula can span several lines. A `#` starts a comment that runs to the end of the line. There is no statement separator: **`;` is a syntax error**.

| Statement | Form | Rules |
|---|---|---|
| Signal declaration | `signal x, y, z` | **Must be the first statement, and there can be only one.** The order of the names is the order of the values in `add_sample([t, x, y, z])`. |
| Parameter declaration | `param a = 1, b = -2.5` | Values must be plain numbers, optionally negative. Can appear several times; declaring a parameter again changes its value. A parameter must be declared before a formula uses it. |
| Formula definition | `name := <formula>` | Defines a formula you can monitor by name and reuse in later formulas. Defining the same name again replaces it. |
| Function definition | `name := <expression>` | An expression with no comparison, for example `f := 2*x[t] + 1`. Use it in later formulas as `f[t]`. |

Names start with a letter and continue with letters, digits or `_`. Some single letters and words are reserved and cannot be used as names (see [Reserved words](#reserved-words)). Among them are `t`, `F`, `G`, `U`, `H`, `O` and `S`.

## Expressions

Expressions compute a signal from the declared signals.

| Syntax | Meaning |
|---|---|
| `x[t]` | the value of signal `x` at the current time |
| `x[t+c]`, `x[t-c]` | `x` shifted in time by `c`, a number or a parameter. Only one shift term is allowed (`x[t+1+2]` is an error). |
| `f[t]`, `f[t+c]` | a function defined earlier |
| `phi[t]`, `phi[t-c]` | the **robustness signal** of a formula defined earlier, used as a value |
| `t` | the current time |
| `3`, `2.5`, `.5`, `3.` | numbers. No exponent notation (`1e-3` is an error). |
| `a` | a parameter |
| `e1 + e2`, `e1 - e2`, `e1 * e2` | arithmetic. `*` binds tighter than `+` and `-`. There is no division. |
| `-e` | negation of a signal, number, parameter, `abs(...)` or parenthesised expression |
| `abs(e)` | absolute value |
| `( e )` | grouping |

A product of two signals is not linear between samples. By default it is evaluated at the data points only. See `dt` in the [README](README.md#concepts).

## Formulas

### Atoms

An atom compares two expressions. Its robustness is a signal over time:

| Atom | Robustness |
|---|---|
| `e1 > e2` | `e1 - e2` |
| `e1 < e2` | `e2 - e1` |
| `e1 >= e2` | the same as `not (e1 < e2)`, so `e1 - e2` |
| `e1 <= e2` | the same as `not (e1 > e2)`, so `e2 - e1` |
| `e1 == e2` | `+Eps` where they are equal, `-abs(e1 - e2)` elsewhere |

`Eps` is a small global margin, default `1e-10` (`stlrom.Signal.set_Eps`). Chained comparisons such as `0 < x[t] < 1` are not allowed. Write `0 < x[t] and x[t] < 1` instead.

### Boolean operators

| Operator | Aliases | Robustness |
|---|---|---|
| `not p` | `!p` | `-p` |
| `p and q` | `p & q` | `min(p, q)` |
| `p or q` | `p \| q` | `max(p, q)` |
| `p => q` | `p -> q` | `max(-p, q)` |

> **Precedence:** `and`, `or` and `=>` all have the **same precedence** and group **left to right**. So `a or b and c` means `(a or b) and c`, not the usual `a or (b and c)`. Use parentheses.

`not` and the temporal operators bind tighter than `and`, `or` and `=>`. So `ev_[0,1] p and q` means `(ev_[0,1] p) and q`.

### Temporal operators

Every temporal operator needs an interval `[a, b]`. There are no untimed versions. `a` and `b` are non-negative numbers or parameters, separated by a comma or a space: `[0, 5]` or `[0 5]`. There is no `inf`. For "until the end of the data", use a bound larger than your trace.

The `_` after the keyword is optional: `ev_[0,1]` and `ev [0,1]` are the same.

**Future operators** look at times after `t`:

| Operator | Aliases | Holds at `t` when… | Robustness at `t` |
|---|---|---|---|
| `ev_[a,b] p` | `F_[a,b] p`, `<>_[a,b] p` | `p` holds at some time in `[t+a, t+b]` | max of `p` over `[t+a, t+b]` |
| `alw_[a,b] p` | `G_[a,b] p`, `[]_[a,b] p` | `p` holds at every time in `[t+a, t+b]` | min of `p` over `[t+a, t+b]` |
| `p until_[a,b] q` | `p U_[a,b] q` | `q` holds at some `t'` in `[t+a, t+b]` and `p` holds on `[t, t']` | max over `t'` of `min(q(t'), min of p on [t, t'])` |

**Past operators** look at times before `t`:

| Operator | Aliases | Holds at `t` when… |
|---|---|---|
| `once_[a,b] p` | `O_[a,b] p` | `p` held at some time in `[t-b, t-a]` |
| `hist_[a,b] p` | `H_[a,b] p` | `p` held at every time in `[t-b, t-a]` |
| `p since_[a,b] q` | `p S_[a,b] q` | `q` held at some `t'` in `[t-b, t-a]` and `p` has held on `[t', t]` |

Their robustness mirrors the future operators: max for `once`, min for `hist`, and max-min for `since`.

### Data coverage

A formula only has a value at times where the data covers all of its windows:
- `alw_[0,2] p` at `t` needs data up to `t + 2`.
- `once_[0,2] p` at `t` needs data from `t - 2`.

Outside that range, `get_rob` raises `ValueError` with a message such as `... is only defined on [2, 4]; t = 0 is outside`. The online functions return bounds instead.

### Reusing definitions

- A formula name used inside another formula (`phi := not mu and alw_[0,1] mu`) inserts a copy of that formula.
- A formula name with `[t]` (`mu[t] > 1`) uses its robustness signal as a value.
- A function name always needs `[t]`.

## Reserved words

These cannot be used as signal, parameter or formula names:

```
signal  param  t  abs  inv  test  true  false
and  or  not
ev  F  alw  G  until  U
once  O  hist  H  since  S
```

The same words with a trailing `_` are reserved too (`ev_`, `F_`, …).

`inv` is an internal time-reversal operator used to build the past operators. `true`, `false` and `test` are reserved but have no meaning in the current grammar.

## Errors

A syntax error prints a message with the line and column to stderr, and `parse_string` or `parse_file` returns `False`:

```
Error: syntax error, unexpected semicolon, expecting end of file or param_decl or new_id
At line 1, column 9
```

Some causes are easy to miss:

| Message mentions | Likely cause |
|---|---|
| `unexpected signal_decl` | a second `signal` line, or `signal` after another statement |
| `unexpected new_id` | a signal or parameter that was not declared, or a parameter declared after the formula that uses it |
| `expecting constant or param_id` | `inf`, a negative bound or another expression inside an interval |
| `unexpected since` (or `S`, `F`, …) | a reserved single letter used as a name |
| `Scanner: unknown character [/]` | division, which is not supported |
