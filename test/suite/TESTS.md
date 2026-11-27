# What each test checks

A catalogue of the tests in `test/suite/`, file by file, for review. For how to run them and how they are built, see [README.md](README.md).

The suite has **184 test functions**, which run as **834 tests**: a test with a table of cases (`@pytest.mark.parametrize`) runs once per row. The **Source of the expected value** column says where each expected value comes from:

- **hand**: worked out by hand. The working is in a comment or in the `how` column of the test's table.
- **reference**: computed by the brute-force reference in `reference.py`.
- **self**: stlrom compared with itself. Two computations that must agree, or a property that must hold.
- **docs**: the behaviour that `docs/` describes. These tests check that the code does what the documentation says.

Unless a test says otherwise, `x` and `y` are the declared signals, data points are joined by straight lines, and a robustness of exactly 0 is reported as ±Eps (1e-10).

| File | Functions | Tests | Area |
|---|---|---|---|
| [test_parser.py](#test_parserpy) | 23 | 69 | the formula language |
| [test_signal.py](#test_signalpy) | 32 | 35 | the `Signal` class |
| [test_data.py](#test_datapy) | 17 | 17 | getting data in and out |
| [test_atoms_and_expressions.py](#test_atoms_and_expressionspy) | 15 | 44 | expressions, atoms, `not and or =>` |
| [test_temporal.py](#test_temporalpy) | 12 | 75 | `ev alw until once hist since` |
| [test_coverage.py](#test_coveragepy) | 13 | 61 | where a formula is defined |
| [test_online.py](#test_onlinepy) | 15 | 141 | online robustness and bounds |
| [test_semantics.py](#test_semanticspy) | 17 | 72 | BOOLEAN, LEFT_TIME, RIGHT_TIME, `sat/ltr/rtr/rho` |
| [test_driver_monitor.py](#test_driver_monitorpy) | 16 | 20 | drivers, monitors, copies, maps |
| [test_helpers.py](#test_helperspy) | 10 | 10 | signal generators, plotting |
| [test_against_reference.py](#test_against_referencepy) | 3 | 160 | stlrom vs the reference |
| [test_properties.py](#test_propertiespy) | 4 | 103 | properties that always hold |
| [test_reference.py](#test_referencepy) | 7 | 27 | the reference itself |
| [test_known_bugs.py](#test_known_bugspy) | 0 | 0 | minimal examples of open bugs (none right now) |

Supporting files, which hold no tests: `conftest.py` (shared helpers, resets BigM/Eps/semantics after each test, warns when the compiled module is older than the sources), `reference.py` (the brute-force reference), `traces.py` (the four test traces), `known_bugs.py` (the list of open bugs, empty right now).

---

## test_parser.py

The formula language, as described in `docs/stl_language.md`. Most tests use x through (0, 1), (1, 3), (2, 0).

| Test | What it checks | Source of the expected value |
|---|---|---|
| `test_accepted` (20 cases) | Specs that must parse: several signals, comments (also before `signal`), a formula over several lines, `param` declarations, `.5` and `3.`, `[0 5]` with a space, `ev [0,1]` without `_`, shifts `x[t+1]`/`x[t-0.5]`, functions, reuse of a formula and of its robustness, `abs`, unary minus, parentheses, `==`, `t`, a parameter in an interval, `sat/ltr/rtr/rho` | docs |
| `test_refused` (15 cases) | Specs that must not parse, each with the reason: `;`, `1e-3`, `/`, `0 < x < 1`, two `signal` lines, `param` before `signal`, an undeclared signal, a parameter used before it is declared, `inf` bound, negative bound, `ev` without interval, `x[t+1+2]`, and the reserved names `F`, `U`, `t` | docs |
| `test_unknown_formula_name_raises_and_lists_the_defined_ones` | `get_rob("nope")` raises `ValueError`, and the message names the defined formulas | docs |
| `test_parse_file` | `parse_file` reads a spec from a file; `x > 2` with x = 5 gives 3 | hand |
| `test_parse_file_missing` | `parse_file` on a missing file returns `False` | docs |
| `test_parse_string_twice_adds_definitions` | A second `parse_string` adds formulas and keeps the earlier ones | hand |
| `test_aliases` (14 cases) | Each alias gives the same robustness as the main keyword at two times: `F` and `<>` = `ev`, `G` and `[]` = `alw`, `U` = `until`, `O` = `once`, `H` = `hist`, `S` = `since`, `!` = `not`, `&` = `and`, `\|` = `or`, `->` = `=>`, `[0 1]` = `[0,1]`, `ev [..]` = `ev_[..]` | self |
| `test_and_or_have_the_same_precedence_left_to_right` | `a or b and c` is `(a or b) and c`: −1, not 1 | hand |
| `test_not_binds_tighter_than_or` | `not p or q` is `(not p) or q`: 4, not −4 | hand |
| `test_temporal_operator_binds_tighter_than_and` | `ev_[0,1] p and q` is `(ev p) and q`: −0.5, not 0.5 | hand |
| `test_times_binds_tighter_than_plus` | `1 + 2*x` with x = 3 is 7 | hand |
| `test_parameter_value_and_set_param` | `get_param` returns the declared value, and `set_param` changes the result | hand |
| `test_parameter_declared_again_changes_its_value` | `param a = 1` then `param a = 2`: a = 2 is used | hand |
| `test_negative_parameter` | `param a = -2`: `x > a` with x = 3 is 5 | hand |
| `test_parameter_in_interval` | `alw_[0,T]` follows `set_param("T", ...)` | hand |
| `test_parameter_in_shift` | `x[t+c]` with `param c = 1` reads x(t+1) | hand |
| `test_set_param_changes_a_shift` | After `set_param("c", 2)`, `x[t+c]` and `x[t-c]` use the new c, and the valid interval moves with it | hand |
| `test_set_param_on_a_monitor_changes_its_shift` | A monitor's `set_param` changes its shift, and the driver keeps the old value | hand |
| `test_shift_parameter_that_starts_at_zero` | With `c = 0` at parse time, a later `set_param("c", 2)` still shifts | hand |
| `test_function_definition` | `f := 2*x[t] + 1` used as `f[t]` and `f[t+1]` | hand |
| `test_formula_reused_inside_another` | `phi := not mu or mu` inserts copies of `mu` | hand |
| `test_formula_robustness_used_as_a_value` | `mu[t] > 0.5` uses `mu`'s robustness as a signal | hand |
| `test_a_name_cannot_be_defined_twice` | Defining `mu` again after another definition is refused | docs |

## test_signal.py

The `Signal` class and the global settings.

| Test | What it checks | Source of the expected value |
|---|---|---|
| `test_append_linear_sample_sets_the_slopes` | Points (0,1) (1,3) (2,0) give slopes 2, −3, 0; begin and end times 0 and 2 | hand |
| `test_append_constant_sample_holds_values` | Constant samples have slope 0 | hand |
| `test_append_sample_with_and_without_slope` | `append_sample(t, v)` has slope 0; `append_sample(t, v, slope)` keeps the slope | docs |
| `test_append_sample_with_interp_replaces_the_previous_slope` | `interp=True` changes the previous sample's slope to join the two points | hand |
| `test_sample_at_or_before_the_last_time_is_ignored` (2 cases) | A sample at the last time or earlier is ignored | docs |
| `test_empty` | `empty()` is true before the first sample and false after it | docs |
| `test_value_at_inside_pieces` | Values on the lines between the samples, and at a sample | hand |
| `test_value_at_steps_between_jumps` | A held value between two jumps | hand |
| `test_value_at_a_jump_is_the_new_value` | At a sample's time, `value_at` returns that sample's value (3, not the old 1) | docs |
| `test_value_at_before_the_first_sample_holds_the_first_value` | Before the data: the first value, plus a printed warning | docs |
| `test_value_at_after_the_end_holds_the_last_value` | After the data: the last value, plus a printed warning | docs |
| `test_value_at_between_the_last_sample_and_end_time_follows_its_slope` | Between the last sample and `end_time`, the last slope applies, with no warning | hand |
| `test_value_at_inside_does_not_warn` | No warning inside the signal, including at its two ends | docs |
| `test_sample_value_at_and_attributes` | `Sample` and `Point` attributes; `Sample.value_at` extends its line | hand |
| `test_resize_inside_keeps_the_values` | Cutting x = t to [1, 3] keeps the values | hand |
| `test_resize_outside_holds_the_end_values` | Extending to [0, 4] holds the first and last values | docs |
| `test_shift` | `shift(2)` moves the times by 2 | hand |
| `test_reverse_linear` | The value at t moves to begin + end − t | hand |
| `test_reverse_steps` | Reversing a step signal puts the jump in the mirrored place | hand |
| `test_reverse_twice_gives_the_original_values` | Two reversals give back the original values | self |
| `test_compute_not` | Negation | hand |
| `test_compute_and_or` | min and max of a rising and a falling line that cross at t = 1 | hand |
| `test_compute_boolean` | 1 where positive, −Eps where not (on a step signal, as these functions need a sample at each sign change) | docs |
| `test_compute_time_robustness` | Left and right time robustness on a step signal, true on [1, 3) | hand |
| `test_write_then_read_gives_the_same_signal` | CSV written and read back gives the same samples | self |
| `test_copies_are_independent` (3 cases) | `copy()`, `copy.copy` and `copy.deepcopy` give independent signals | self |
| `test_bigm_set_and_get` | Default BigM is 10000, and `set_BigM` changes it | docs |
| `test_values_must_stay_below_bigm` | With BigM raised to 1e6, `alw_[0,1] (t > 0)` at 10001 is 10001 | hand |
| `test_get_bigm_rounds_down` | `set_BigM(10.7)` then `get_BigM()` gives 10 | docs |
| `test_eps_set_and_get` | Default Eps is 1e-10, and `set_Eps` changes it | docs |
| `test_semantics_set_and_get` | Global semantics defaults to `SPACE` and can be set | docs |
| `test_nan_is_not_used_for_empty_signals` | `value_at` on an empty signal returns 0, not NaN | docs |

## test_data.py

Getting data into a driver and back out.

| Test | What it checks | Source of the expected value |
|---|---|---|
| `test_add_sample_fills_every_signal_in_declaration_order` | `add_sample([t, x, y])` fills x and y in declaration order | docs |
| `test_add_sample_without_interp_holds_the_previous_value` | `interp=False` holds each value until the next sample | docs |
| `test_add_sample_with_the_wrong_length_raises` | Too few or too many values raise `ValueError` | docs |
| `test_add_sample_at_an_earlier_time_is_ignored` | A sample at or before the last time is ignored | docs |
| `test_set_signals` | `set_signals` with two signals; `x > y` gives 2 | hand |
| `test_set_signals_with_the_wrong_count_raises` | The wrong number of signals raises `ValueError` | docs |
| `test_set_signals_replaces_the_data` | `set_signals` replaces earlier data | hand |
| `test_signals_can_cover_different_times` | x on [0, 2] and y on [1, 3]: `x > y` is defined on [1, 2] | hand |
| `test_names_and_indices` | `get_signals_names`, `get_signal_idx`, `get_signame_from_idx` | docs |
| `test_get_signal_by_name_or_index` | `get_signal("y")` and `get_signal(1)` are the same | self |
| `test_write_csv_then_load_csv` | `write_csv` then `load_csv` gives the same samples | self |
| `test_load_csv_with_two_columns` | Rows `t,value` have slope 0: each value is held until the next row | docs |
| `test_load_csv_with_slopes` | Rows `t,value,slope` keep the slopes | hand |
| `test_load_csv_missing_file_raises` | A missing file raises `ValueError` | docs |
| `test_load_csv_bad_row_raises` | A row with 4 columns raises `ValueError` | docs |
| `test_stldata_add_signal_sample_to_one_signal` | `data.add_signal_sample` adds to one signal; an unknown name is ignored | docs |
| `test_stldata_signal_map` | `data.signal_map` is `{"x": 0, "y": 1}` | docs |

## test_atoms_and_expressions.py

Expressions, atoms and boolean operators. Data: x = 1, 3, 0 and y = 0, 1, 1 at t = 0, 1, 2.

| Test | What it checks | Source of the expected value |
|---|---|---|
| `test_expression_value` (16 cases) | `e > 0` has robustness e, so each case reads one expression: `x+y`, `x-y`, `2*x`, `x*y` at a data point, `-x`, `-(x-4)`, `abs` (including 0), `x[t+0.5]`, `x[t-0.5]`, `t`, constants, `x + t*2`, `(x+1)*2` | hand |
| `test_abs_crossing_zero_inside_a_piece` | x from −1 to 1: `abs(x) > 0.25` is 0.75, 0.25, −0.25, 0.25, 0.75 at t = 0, 0.5, 1, 1.5, 2 | hand |
| `test_shift_changes_where_the_data_is_valid` | `x[t+0.5]` is defined on [−0.5, 1.5], `x[t-0.5]` on [0.5, 2.5] | hand |
| `test_shifted_signal_on_a_range` | `x[t-1]` and `x[t+1]` over ranges that end before the data does keep their last piece (a fixed bug) | hand |
| `test_constants_and_time_need_no_data` | `2 > 1` and `t > 1` compute with no samples at all | hand |
| `test_product_without_dt_uses_the_data_points_only` | Without `dt`, x·y follows its tangent at the left data point between data points: 0 at t = 1 for x = y = t, exact (4) at t = 2 | hand |
| `test_product_error_shrinks_like_dt_squared` | With dt = 0.1 the error is about 0.01; halving dt divides it by 4 | hand |
| `test_dt_must_be_positive` (2 cases) | dt = 0 or −1 raises `ValueError` in `get_rob`, `get_rob_signal`, `get_online_rob` | docs |
| `test_dt_does_not_change_linear_formulas` | `dt` changes nothing without products | self |
| `test_atom_robustness` (7 cases) | `> < >= <=` with constants and between signals | hand |
| `test_equality_atom` | `x == 2`: +Eps where x = 2, −\|x − 2\| elsewhere | docs |
| `test_atom_sign_change_inside_the_last_piece` | A sign change between the last two samples is found (a past bug) | hand |
| `test_atom_at_exactly_zero_is_eps_sized` | A robustness of exactly 0 comes back as ±Eps, never 0 | docs |
| `test_boolean` (8 cases) | `not`, `not not`, `and`, `or`, `=>` both ways, and De Morgan | hand |
| `test_and_or_over_a_range_cross_correctly` | `and`/`or` over a range follow min/max at 6 times, including where the two lines cross | hand |

## test_temporal.py

The temporal operators, on small traces worked out by hand. The file's docstring gives the traces:
- **W:** x through (0,0) (1,2) (2,−1) (3,1) (4,1), straight lines;
- **S:** steps 1, 3, 0, 2, held on [0,1) [1,2) [2,3) [3,4];
- **W_LATE:** W moved to start at t = 1.

| Test | What it checks | Source of the expected value |
|---|---|---|
| `test_single_time` (25 cases) | `get_rob` at one time for `ev`, `alw`, `once`, `hist`, each with windows `[0,b]` and `[a,b]`, plus `until` and `since`. They include cases where p limits the result and a case showing that p must hold from t, not from t+a. Each row has its working in its `how` column | hand |
| `test_range_gives_the_same` (25 cases) | The same 25 cases, read from one range request | hand |
| `test_nested_ev_alw` | `ev_[0,1] alw_[0,0.5] x > 0.25` at t = 0 is 1.15 (the max of min(2s, 3.5−3s) is at s = 0.7) | hand |
| `test_nested_past_in_future` | `alw_[0,1] once_[0,1] x > 0.25` at t = 1 is 1.75 | hand |
| `test_with_jumps` (11 cases) | All operators on the step trace S, at times away from the jumps, as single time and as range | hand |
| `test_data_starting_later` (6 cases) | The main operators on W_LATE (data starting at t = 1), as single time and as range | hand |
| `test_default_time_0_is_refused_when_data_starts_later` | `get_rob("phi")` (t = 0) raises when the data starts at 1 | docs |
| `test_times_on_a_fine_grid` | At times i·0.01 (not exact in floating point), single-time and range requests agree | self |
| `test_since_single_time_rounding` | A `since` formula at t = 2.8, 2.89, 2.9 and 2.91 gives −0.5. It used to crash at 2.89 and 2.9 (rounding in `plateauMax`) | reference |
| `test_until_single_time_rounding` | `until_[0.4,1.2]` at t = 0.15 gives 0.1786. It used to crash | reference |
| `test_until_on_ranges_of_different_lengths` | `until_[0.4,1.2]` at t = 4.45 gives 0.0417 for ranges starting at 3.7, 4.0, 4.2 and 4.3 (a fixed bug) | hand |
| `test_until_on_a_range_with_data_ending_at_0` | `(1000 > 0) until_[0,1] (y > 0.5)` on data ending at 0 gives 0.4333 at t = −1.2, not 0.5 (a fixed bug) | hand |

## test_coverage.py

Where a formula has a value. Data: x on [1, 4], y on [2, 5]. Expected intervals come from the rules in the file's docstring. For example, `alw_[a,b]` over a child valid on [s, e] is valid on [s−a, e−b].

| Test | What it checks | Source of the expected value |
|---|---|---|
| `test_range_request_is_cut_to_the_valid_interval` (17 cases) | `get_rob_signal(-10, 10)` starts and ends at the expected valid interval. The 17 formulas cover each row of the rules: signals, `and`, `not`, `abs`, products, shifts, `ev`, `alw`, `once`, `hist`, `until`, `since`, nesting, `or` | hand |
| `test_single_time_accepted_at_the_edges` (17 cases) | `get_rob` is accepted at both edges of the interval and in its middle | hand |
| `test_single_time_refused_outside` (17 cases) | `get_rob` 0.01 outside each edge raises `ValueError` | hand |
| `test_refusal_message_names_the_formula_and_interval` | The error names the formula, the interval and t | docs |
| `test_edges_on_a_time_grid` | Edges hit as i·0.01 (rounding) are still accepted | hand |
| `test_formulas_without_signals_are_valid_everywhere` | `t > 0 and 1 > 0` at t = −1000 and 1000 | hand |
| `test_range_outside_the_data_is_empty` | A range fully outside the data gives an empty signal | docs |
| `test_range_partly_outside_is_cut` | A range partly outside is cut to the data | docs |
| `test_window_longer_than_the_data` | `alw_[0,10]` on 3 time units of data: empty result, `get_rob` raises | docs |
| `test_no_samples_get_rob_raises` | No samples: `get_rob` raises | docs |
| `test_no_samples_get_rob_signal_is_empty` | No samples: `get_rob_signal` is empty | docs |
| `test_no_samples_constant_formula_still_computes` | No samples: `1 > 0` still gives 1 | docs |
| `test_one_signal_empty_the_other_not` | `x > 0 and y > 0` with y empty: raises / empty | docs |

## test_online.py

Online robustness: `[rob, lower, upper]`, where the bounds must hold whatever data comes next. Worked examples use x through (0,0) (1,2) (2,−1).

| Test | What it checks | Source of the expected value |
|---|---|---|
| `test_no_data` | No data: `[nan, -BigM, +BigM]` | docs |
| `test_no_data_signal_form` | No data, signal form: empty `z`, bounds ±BigM | docs |
| `test_atom_inside_the_data_is_exact` | Inside the data, rob = lower = upper | hand |
| `test_atom_after_the_data_is_unknown` | After the data, the bounds are ±BigM | docs |
| `test_partial_window` (4 cases) | `ev`, `alw`, `hist`, `once` when the window is partly unknown. The seen part fixes one bound and the other stays ±BigM, including before the data starts | hand |
| `test_window_inside_the_data_equals_offline` (6 cases) | When the window is fully known, all three values equal offline `get_rob` | self |
| `test_online_signal_trims_only_rob` | `get_online_rob_signal`: `z` is cut to the valid interval, while the bounds cover the whole request | docs |
| `test_monitor_keeps_the_last_values` | `rob`, `lower_rob`, `upper_rob` and the getters after an online request | hand |
| `test_bigm_setting_is_used_for_unknown_bounds` | With `set_BigM(500)`, the unknown bounds are ±500 | docs |
| `test_until_before_the_data` | `until_[0.4,1.2]` at t = −1: upper bound BigM for single time and range (single time used to give 1.0) | hand |
| `test_since_bounds_when_the_window_starts_before_the_data` | `since_[0,1]` at t = 0.5 with x = 1: bounds [1, 1] (they used to be ±BigM) | hand |
| `test_until_sample_by_sample_does_not_crash` | `until_[3,5]` with 81 samples added one by one: no crash, lower ≤ upper (an old crash) | self |
| `test_bounds_tighten_as_data_arrives` | As samples arrive, lower never goes down and upper never goes up; they meet once the window is known | self |
| `test_bounds_match_the_reference` (60 cases) | 15 formulas × 4 traces, data cut at t ≈ 3: lower and upper bounds equal the reference's interval propagation, over a range and at single times | reference |
| `test_bounds_contain_the_final_robustness` (60 cases) | Same cases: the robustness of the *full* trace lies inside the bounds computed from the cut data, and rob lies inside its own bounds | reference |

## test_semantics.py

What an atom measures. Worked example: x through (0,−1) (1,1) (3,1) (4,−1) (6,−1), so `x > 0` holds on (0.5, 3.5).

| Test | What it checks | Source of the expected value |
|---|---|---|
| `test_time_semantics_worked_example` (4 cases) | Left and right time robustness at 4 times, by `set_semantics` and by `ltr(...)`/`rtr(...)`. For example, at t = 1: true since 0.5 → 0.5, true until 3.5 → 2.5. Durations stop at the edges of the data | hand |
| `test_time_semantics_on_a_range` | The same values read from a range request | hand |
| `test_time_semantics_then_temporal_operator` | `ev_[0,1] ltr(...)` and `alw_[0,1] rtr(...)` at t = 1 give 1.5 | hand |
| `test_time_semantics_no_sign_change_is_empty` (6 cases) | An atom that never changes sign (`x > -2` always true, `x > 2` always false), by `ltr`/`rtr` or by `set_semantics`: rob is NaN, the range signal is empty | docs |
| `test_time_semantics_no_sign_change_propagates` (5 cases) | The empty result goes through `not`, `ev`, `alw`; in `and`/`or`, one atom without a sign change empties the whole result | docs |
| `test_boolean_semantics` | 1 where true, −Eps where false | docs |
| `test_boolean_not_gives_the_right_sign` | `not` of false is positive (why false is −Eps and not 0) | docs |
| `test_boolean_with_temporal_operators` | `ev` and `alw` of booleans | hand |
| `test_sat_operator` | `sat(...)` gives boolean values inside a SPACE driver | docs |
| `test_rho_gives_space_inside_a_time_semantics_driver` | `rho(...)` gives space values inside a LEFT_TIME driver | docs |
| `test_innermost_semantics_wins` | `ltr(rho(..))` is space, `rho(ltr(..))` is time | docs |
| `test_local_semantics_mixed_in_one_formula` | Space and time parts in one formula: min(1, 1.5) = 1 | hand |
| `test_set_semantics_get_semantics` | The four names can be set and read back | docs |
| `test_invalid_semantics_name` | An invalid name raises `ValueError` on drivers and monitors | docs |
| `test_monitor_semantics_is_its_own` | A monitor copies the driver's semantics, then changing it leaves the driver alone | docs |
| `test_driver_semantics_does_not_leak_to_another_driver` | BOOLEAN on one driver does not affect another | self |
| `test_matches_reference` (44 cases) | 11 formulas × 4 traces with `ltr`, `rtr`, `sat`, nested under temporal operators, against the reference's Donzé–Maler time robustness. Instants where an atom switches are skipped | reference |

## test_driver_monitor.py

Drivers, monitors and their copies. Spec: `phi := x > a`, `psi := ev_[0,1] x > a`, `param a = 1`, x = 1, 3, 0.

| Test | What it checks | Source of the expected value |
|---|---|---|
| `test_monitor_gives_the_same_results_as_the_driver` | Monitor and driver agree on `get_rob`, `get_online_rob`, `get_rob_signal` | self |
| `test_monitor_data_is_a_copy` | Samples added to the driver do not reach the monitor, and the reverse | docs |
| `test_monitor_parameters_are_its_own` | `set_param` on a monitor leaves the driver alone | docs |
| `test_monitor_copies_parameters_set_before` | A monitor made after `set_param` has the new value | docs |
| `test_get_monitor_unknown_name` | Unknown name raises `ValueError` | docs |
| `test_empty_monitor_raises_runtime_error` | `STLMonitor()` has no formula: `RuntimeError` | docs |
| `test_set_eval_time_sets_the_default_range` | `set_eval_time` sets the range of `get_rob_signal()` with no arguments | docs |
| `test_monitor_signal_names_and_signals` | Names, indices and signals on a monitor | docs |
| `test_driver_copies_are_independent` (3 cases) | `copy()`, `copy.copy`, `copy.deepcopy` of a driver are independent (data and parameters) | self |
| `test_monitor_copies_are_independent` (3 cases) | The same for monitors | self |
| `test_many_monitors_from_one_driver` | 20 monitors from one driver, each with its own data | hand |
| `test_driver_survives_repeated_requests` | The same request 200 times gives the same answer | self |
| `test_robustness_map_structure` | The map has depths 0, 1, 2 with the formula, the atom and the signal, and the root's value | hand |
| `test_online_robustness_map_has_bounds` | The online map's `z_low`/`z_up` hold the bounds | hand |
| `test_robustness_map_unknown_name` | Unknown name raises `ValueError` | docs |
| `test_disp_and_str_mention_the_formulas` | `str(driver)` names the formulas | docs |

## test_helpers.py

Pure-Python helpers. Plots are only checked to run (with the Agg backend, nothing is drawn on screen).

| Test | What it checks | Source of the expected value |
|---|---|---|
| `test_pwl_generator` | `PWLSignalGen` values through its points | hand |
| `test_pwc_generator` | `PWCSignalGen` holds its values | hand |
| `test_oscillating_generator` | `OscillSignalGen` matches 3·sin(πt) + 1 | hand |
| `test_random_generator_is_repeatable` | Same seed gives the same signal; another seed gives a different one | self |
| `test_random_generator_respects_its_limits` | Values within [v_min, v_max], steps at most dt_max | docs |
| `test_generated_signal_works_in_a_driver` | A generated signal used in a driver gives 1 for `ev_[0,2] x > 1` | hand |
| `test_time_array` | `get_time_array(0, 1, 0.25)` | docs |
| `test_signal_plot` | `Signal.plot` returns an Axes | docs |
| `test_driver_plots` | `plot_signal` and `plot_signals` return Axes | docs |
| `test_plot_robustness_map` | `plot_rob_map` for robustness, lower and upper | docs |

## test_against_reference.py

stlrom against the brute-force reference. The four traces in `traces.py`:
- smooth data starting at 0;
- data starting late, with x and y covering different times;
- step signals with jumps;
- uneven time steps.

Values are compared at every time i·0.01, within 0.05 (the reference's grid error), skipping instants where the reference jumps.

| Test | What it checks | Source of the expected value |
|---|---|---|
| `test_matches_reference` (132 cases) | 33 formulas × 4 traces. The formulas: atoms and expressions (including shifts, `abs`, `t`), boolean operators, every temporal operator with `[0,b]` and `[a,b]` windows, and nested past/future formulas. For each one: `get_rob_signal` over a range wider than the data starts and ends where the reference becomes defined, matches it at every time, and `get_rob` at sampled single times matches too, or raises where the reference is undefined | reference |
| `test_product_with_dt` (4 cases) | `ev_[0,0.5] (x*y > 0.3)` with dt = 0.01 matches the true product on each trace | reference |
| `test_ranges_of_different_lengths_agree_with_single_times` (24 cases) | 6 formulas × 4 traces, each requested over 7 ranges with different starts and ends inside its valid interval. Every range agrees with single-time requests. This catches bugs that only appear for some ranges, like the `until` bug | self, reference (which times to skip) |

## test_properties.py

Properties that must hold for any data. They compare stlrom with itself, on the four traces.

| Test | What it checks | Source of the expected value |
|---|---|---|
| `test_duality` (52 cases) | 13 identities × 4 traces: `not not p = p`, `alw = not ev not`, `hist = not once not`, `=>` as `or`, De Morgan, `x > y = y < x`, `x > c = x − c > 0`, `ev` distributes over `or` and `alw` over `and`, splitting a window (`ev_[0,1] = ev_[0,0.5] ev_[0,0.5]`, same for `alw`), and `ev = (1000 > 0) until`, `once = (1000 > 0) since` | self |
| `test_moving_the_data_moves_the_result` (32 cases) | 8 formulas × 4 traces: moving all data later by 1.37 moves the result later by 1.37. Up to 2% of times may differ, where a jump falls on one side of a grid time | self |
| `test_past_is_future_backwards` (15 cases) | 5 pairs × 3 traces (not the step trace): a past operator on the data equals the future operator on the time-reversed data (`once`/`ev`, `hist`/`alw`, nested, `since`/`until`) | self |
| `test_scaling_data_and_thresholds_scales_robustness` (4 cases) | Multiplying data and thresholds by 3 multiplies the robustness by 3 | self |

## test_reference.py

The reference itself, checked against hand-computed values. If one of these fails, the reference is wrong, not stlrom. Trace: x through (0,0) (1,2) (2,−1) (3,1) (4,1).

| Test | What it checks | Source of the expected value |
|---|---|---|
| `test_reference_value` (21 cases) | Atoms, `abs`, shifts, `t`, `ev`, `alw`, `once`, `hist`, `until`, `since`, `=>` at chosen times, each with its working | hand |
| `test_reference_is_nan_outside_the_data` | `ev_[0,1]` is defined on [0, 3] and NaN outside | hand |
| `test_reference_time_robustness` | Left and right time robustness on the semantics worked example, within one grid step | hand |
| `test_reference_local_semantics_innermost_wins` | `rho(ltr(...))` uses LEFT_TIME | hand |
| `test_reference_online_bounds` | Interval propagation on cut data: the seen part fixes one bound, the other is ±BigM | hand |
| `test_trace_until_cuts_the_data` | `Trace.until(T)` cuts the data at T | hand |
| `test_formula_text` | The formula text the reference produces matches the language | hand |

## test_known_bugs.py

The smallest example of each open bug in `known_bugs.py`. There are no open bugs right now, so this file holds no tests. Fixed bugs kept their examples as ordinary tests in the file of their area. They are the tests above whose description says "a fixed bug" or "it used to".
