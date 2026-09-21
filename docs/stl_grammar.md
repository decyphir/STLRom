# STL Grammar Reference

This document describes the grammar of the Signal Temporal Logic (STL) language
accepted by STLRom's parser (`src/parser.y`) and scanner (`src/scanner.l`).

---

## Overview

An STL specification in STLRom is a sequence of **declarations** and
**definitions**:

```
start : assignement
      | param_assignements
      | signal_decl
      | start assignement
      | start param_assignements
      | start END
      | END
```

Each top-level statement is terminated by the **end of file** (`END`) or by a
period (`.`).

---

## Lexical Tokens (Scanner)

The scanner (`scanner.l`) recognizes the following tokens.

### Keywords & Operators

| Token          | Lexemes             | Description                          |
|----------------|---------------------|--------------------------------------|
| `BOX`          | `[]`, `[]_`, `G`, `G_`, `alw`, `alw_` | "Always" temporal operator    |
| `DIAMOND`      | `<>`, `<>_`, `F`, `F_`, `ev`, `ev_`   | "Eventually" temporal operator |
| `UNTIL`        | `U`, `U_`, `until`, `until_` | "Until" temporal operator  |
| `NOT`          | `not`, `!`          | Logical negation                     |
| `AND`          | `and`, `&`          | Logical conjunction                  |
| `OR`           | `or`, `\|`          | Logical disjunction                  |
| `IMPLIES`      | `=>`, `->`          | Logical implication                  |
| `LT`           | `<`                 | Less-than                            |
| `GT`           | `>`                 | Greater-than                         |
| `LE`           | `<=`                | Less-than or equal                   |
| `GE`           | `>=`                | Greater-than or equal                |
| `PARAM_EQ`     | `=`                 | Equality / parameter assignment      |
| `PLUS`         | `+`                 | Addition / unary plus                |
| `MINUS`        | `-`                 | Subtraction / unary minus            |
| `MULT`         | `*`                 | Multiplication                       |
| `ASSIGN`       | `:=`                | Formula/definition assignment        |
| `LINT`         | `[`                 | Left interval bracket                |
| `RINT`         | `]`                 | Right interval bracket               |
| `LPAREN`       | `(`                 | Left parenthesis                     |
| `RPAREN`       | `)`                 | Right parenthesis                    |
| `COMMA`        | `,`                 | Comma separator                      |
| `SEMICOLON`    | `;`                 | Semicolon                            |
| `ABS`          | `abs`               | Absolute value function              |
| `TIME`         | `t`                 | Time variable in signals             |
| `PARAM_DECL`   | `param`             | Parameter declaration keyword        |
| `SIGNAL_DECL`  | `signal`            | Signal declaration keyword           |
| `TEST`         | `test`              | Test keyword (reserved)              |

### Values & Identifiers

| Token        | Pattern                          | Description                              |
|--------------|----------------------------------|------------------------------------------|
| `NUMBER`     | (declared, currently unused)     | Integer literal                          |
| `DOUBLE`     | (declared, currently unused)     | Floating-point literal                   |
| `CONSTANT`   | `[0-9]+`, `[0-9]*\.[0-9]+`, `[0-9]*\.` | Numeric literal                  |
| `STRING`     | `\"(...)\"`                      | Double-quoted string                     |
| `BOOL_TRUE`  | `true`                           | Boolean true                             |
| `BOOL_FALSE` | `false`                          | Boolean false                            |
| Identifier   | `[a-zA-Z][A-Za-z0-9_]*`          | Classified at runtime (see below)        |

### Identifier Classification

An identifier is classified by the scanner at parse time based on existing
context:

- **`PHI_ID`** — identifier already defined as an STL formula.
- **`PARAM_ID`** — identifier already defined as a parameter.
- **`SIGNAL_ID`** — identifier already defined as a signal.
- **`NEW_ID`** — identifier not yet defined (new formula, new parameter, or new signal name).

### Comments & Whitespace

- **Comments**: `#` starts a comment that runs to the end of the line.
- **Whitespace**: spaces, tabs, and carriage returns are skipped.

---

## Grammar

### 1. Signal Declarations

Declare one or more signal names. Once declared, a signal can be used in
expressions.

```
signal_decl : SIGNAL_DECL signal_new_list
signal_new_list : signal_new
                | signal_new_list COMMA signal_new
signal_new : NEW_ID
           | SIGNAL_ID
```

**Example:**

```
signal x, y, z
```

### 2. Parameter Declarations

Declare or assign numerical parameters. Parameters may be assigned positive or
negative constants.

```
param_assignements : PARAM_DECL param_assignement_list
param_assignement_list : param_assignement
                       | param_assignement_list COMMA param_assignement
param_assignement : PARAM_ID PARAM_EQ CONSTANT
                  | PARAM_ID PARAM_EQ MINUS CONSTANT
                  | NEW_ID PARAM_EQ CONSTANT
                  | NEW_ID PARAM_EQ MINUS CONSTANT
```

**Example:**

```
param a=1, b=2, p = -3
```

### 3. Formula Assignments

Define a named STL formula that can be referenced later.

```
assignement : NEW_ID ASSIGN stl_formula
```

**Example:**

```
phi1 := alw_[0,5] mux
```

### 4. Signal Expressions

Signal expressions are arithmetic combinations of signal atoms.

```
signal_expr : signal_addexpr

signal_addexpr : signal_multexpr
               | signal_addexpr PLUS signal_multexpr
               | signal_addexpr MINUS signal_multexpr

signal_multexpr : signal_unaryexpr
                | signal_multexpr MULT signal_unaryexpr

signal_unaryexpr : signal_atom
                 | ABS LPAREN signal_expr RPAREN
                 | MINUS signal_unaryexpr
                 | PLUS signal_unaryexpr

signal_atom : signal
            | constant_signal
            | LPAREN signal_expr RPAREN

signal : SIGNAL_ID LINT TIME RINT        e.g.,  x[t]

constant_signal : CONSTANT               e.g.,  3.14
                | PARAM_ID               e.g.,  a
```

**Example expressions:**

```
x[t]
3.14
a * x[t] + b
abs(x[t])
-(x[t] + y[t])
```

### 5. Intervals

Temporal intervals are bounded by square brackets `[ ... ]` and contain two
constants, separated by either a comma or whitespace.

```
interval : LINT constant COMMA constant RINT     e.g.,  [0, 5]
         | LINT constant constant RINT           e.g.,  [0 5]
```

**Example:**

```
[0, 5]
[3 10]
```

### 6. Atomic Predicates

An atomic predicate compares two signal expressions using a relational operator.

```
stl_atom : signal_expr op signal_expr
op       : LT      ( < )
         | GT      ( > )
         | PARAM_EQ PARAM_EQ   ( = )
```

**Example:**

```
x[t] > 0
a*y[t] + b > p
x[t] = 1
```

### 7. STL Formulas

An STL formula is built recursively from atomic predicates, Boolean operators,
and temporal operators.

```
stl_formula : signal_expr op_eq signal_expr        (negated atom, see note)
            | stl_atom
            | NOT stl_formula
            | stl_formula AND stl_formula
            | stl_formula OR stl_formula
            | stl_formula IMPLIES stl_formula
            | DIAMOND interval stl_formula          (eventually)
            | BOX interval stl_formula              (always)
            | stl_formula UNTIL interval stl_formula
            | LPAREN stl_formula RPAREN
            | PHI_ID

op_eq : LE     ( <= )
      | GE     ( >= )
```

**Note on `op_eq`:** The `<=` and `>=` comparisons are internally rewritten as
negated strict inequalities:
- `a <= b` is equivalent to `!(a > b)`
- `a >= b` is equivalent to `!(a < b)`

**Examples:**

```
x[t] > 0                      atomic predicate
not (x[t] > 0)                negation
phi1 and phi2                 conjunction
phi1 or phi2                  disjunction
phi1 => phi2                  implication
alw_[0, 5] mux                always over interval [0,5]
ev_[3, 4] (mux)               eventually over interval [3,4]
phi1 until_[0, 1] phi2        until over interval [0,1]
(x[t]>0) and (y[t]<2)         parenthesized subformula
```

### 8. Precedence & Associativity

From lowest to highest precedence:

| Precedence          | Operators                        | Associativity |
|---------------------|----------------------------------|---------------|
| 1 (lowest)          | `and`                            | left          |
| 2                   | `or`                             | left          |
| 3                   | `not`                            | left          |
| 4                   | `=>`, `->` (implication)         | left          |
| 5                   | `<`, `>`, `<=`, `>=`             | left          |
| 6                   | `alw`, `ev`, `until`, `:=`       | left          |
| 7                   | `+`, `-`                         | left          |
| 8                   | `*`                              | left          |
| 9 (highest)         | unary `-`, `+`, `abs`            | —             |
| —                   | `(`, `)`                         | non-assoc     |

### 9. Reserved & Unused Tokens

The following tokens exist in the grammar/scanner but are not used in the
current STL language constructs:

- `TEST` — reserved word `test`
- `SEMICOLON` — `;` (currently unused in production rules)
- `NUMBER`, `DOUBLE` — numeric token declarations (currently unused)
- `BOOL_TRUE` / `BOOL_FALSE` — `true` / `false` literals (currently unused)
- `STRING` — quoted strings (reserved for `test` traces)

---

## Full Specification Example

```
# Declare signals
signal x, y

# Declare parameters
param a=1, b=2, p = -3

# Define atomic predicates and formulas
mux := x[t] > 0
muy := a*y[t] + b > p
phi1 := alw_[0, 5] mux
phi2 := ev_[3, 4] (mux) or phi1
phi_until := phi1 until_[0, 1] phi2
```

---

## Notes

- Comments begin with `#` and extend to the end of the line.
- Formulas defined with `:=` become available as `PHI_ID` tokens for later use
  in other formula definitions.
- Both `<>`/`<>_` (or `F`/`F_`/`ev`/`ev_`) spell the *eventually* operator, and
  `[]`/`[]_` (or `G`/`G_`/`alw`/`alw_`) spell the *always* operator.
