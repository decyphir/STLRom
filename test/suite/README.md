# Test suite

The tests for stlrom's Python API. They are run with [pytest](https://docs.pytest.org) and are separate from the older tests in `test/` (the ones `ctest` runs).

## Running it

One-time setup, from the repository root:

```bash
uv pip install --python .venv/bin/python -e ".[dev]"   # installs stlrom, pytest and pytest-xdist
```

Then, from the repository root, either of:

```bash
.venv/bin/python -m pytest                      # the whole suite (about 5 seconds)
cmake --build build --target test-suite         # the same, through CMake
```

`ctest` does not run this suite, and the CMake target does not run the old tests.

**After changing C++ code, reinstall first.** The tests use the module installed in `.venv`, which is not rebuilt automatically:

```bash
uv pip install --python .venv/bin/python -e .
```

If you forget, the first lines of the pytest output show a warning naming the source file that is newer than the module.

Useful variations:

| Command | What it does |
|---|---|
| `pytest test/suite/test_temporal.py` | one file |
| `pytest -k until` | only tests whose name contains `until` |
| `pytest -x` | stop at the first failure |
| `pytest -v` | one line per test, with its name |
| `pytest -rx` | also list the known bugs (expected failures) and their reasons |
| `pytest -n 0` | run in one process (easier to debug; a crash stops the run) |
| `pytest --runxfail` | run the known-bug tests as normal tests, to see how they fail |

## Reading the output

Each test prints one character: `.` passed, `F` failed, `x` expected failure (a known bug, see below). The last line sums them up, e.g. `764 passed, 30 xfailed`. A failure prints the test's name, the line that failed, and the values on both sides of the comparison.

Tests run in parallel, one worker process per CPU. If a test crashes the process (a segfault in the C++ code), it is reported as failed with "worker crashed", and the other tests still run.

## Known bugs

`known_bugs.py` lists the bugs the suite knows about, each with a short explanation. Tests that hit one of them are marked as expected failures:

- while the bug is there, the test fails as expected, shows as `x`, and the run stays green;
- once the bug is fixed, the test passes, and pytest reports it as a **failure** (`XPASS(strict)`). This reminds you to remove the bug from `known_bugs.py` and its mark from the test.

`test_known_bugs.py` has the smallest example found for each one.

## How the tests are built

There are three kinds of check:

1. **Worked examples.** Small traces where the expected value is worked out by hand, with the working in a comment next to it. Most files are like this. They are meant to be read and checked by a person.
2. **A brute-force reference.** `reference.py` recomputes robustness straight from the definitions: it evaluates every formula on a fine time grid (step 0.005) and takes min and max over the grid points of each window. It does not use stlrom. `test_reference.py` checks it against hand-computed values. `test_against_reference.py` then compares stlrom with it on about 30 formulas, on four traces (`traces.py`):
   - smooth data;
   - data starting late, with x and y covering different times;
   - step signals (with jumps);
   - uneven time steps.
3. **Properties that always hold.** These compare stlrom with itself, for example `alw p = not ev not p`, "moving the data later moves the result later", "a past operator is the future operator on the reversed data", and "online bounds contain the final robustness".

Comparisons with the reference allow an error of 0.05. When a window contains a corner of the signal between two grid points, the grid can miss the true extreme by up to about 0.005 times the slope, so a small tolerance is needed. Instants where the result jumps are skipped, because the value at the exact instant of a jump is a matter of convention.

## Files

[TESTS.md](TESTS.md) describes every test, one by one.

| File | What it tests |
|---|---|
| `test_parser.py` | the formula language: what parses, aliases, precedence, parameters, functions, reuse |
| `test_signal.py` | the `Signal` class: building, `value_at`, `resize`, `shift`, `reverse`, operations, files, copies, BigM/Eps |
| `test_data.py` | `add_sample`, `set_signals`, CSV files, signal names |
| `test_atoms_and_expressions.py` | `+ - *`, `abs`, shifts, `t`, `dt` in products, atoms (`> < >= <= ==`), `not and or =>` |
| `test_temporal.py` | `ev alw until once hist since`, nested, with jumps, data not starting at 0, times like `i*0.01` |
| `test_coverage.py` | where a formula is defined: refused times, cut ranges, empty data |
| `test_online.py` | online robustness and its bounds |
| `test_semantics.py` | `BOOLEAN`, `LEFT_TIME`, `RIGHT_TIME`, and `sat/ltr/rtr/rho` |
| `test_driver_monitor.py` | monitors, copies, parameters per monitor, robustness maps |
| `test_helpers.py` | signal generators, plotting |
| `test_against_reference.py` | stlrom vs the brute-force reference, offline |
| `test_properties.py` | properties stlrom must satisfy whatever the data |
| `test_reference.py` | the reference itself, against hand-computed values |
| `test_known_bugs.py` | one minimal example per known bug |
| `conftest.py` | shared helpers (loaded by pytest automatically) |
| `reference.py`, `traces.py`, `known_bugs.py` | the reference, the test traces, the list of known bugs |

## Adding a test

A test is a function whose name starts with `test_`, in a file whose name starts with `test_`. Use `assert`, and `pytest.approx` for floats:

```python
from conftest import make_driver

def test_ev_takes_the_maximum():
    # x: 1, 3, 0 at t = 0, 1, 2.  max of x on [0, 1] is 3, so 3 - 2 = 1
    d = make_driver("signal x\nphi := ev_[0,1] x[t] > 2", [(0, 1), (1, 3), (2, 0)])
    assert d.get_rob("phi", 0) == pytest.approx(1)
```

Two things to keep in mind:
- A robustness of exactly 0 is reported as `+-Eps` (1e-10), so prefer examples whose answer is not 0.
- To compare a new formula with the reference, add it to `FORMULAS` in `test_against_reference.py`. The `x`, `y`, `t` and operator names there come from `reference.py`. For example, `Ev(0, 1, x > 0.5)` is `ev_[0,1] (x[t] > 0.5)`.
