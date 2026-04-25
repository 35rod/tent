# TODO

## Bugs
- [x] `main.cpp` — no `return 1` after failed file open
- [x] `io.cpp` — `vec_to_string` returns wrong string (vector printing broken)
- [x] `io.cpp` — `input()` breaks on spaces (`cin >>` → `getline`)
- [x] `io.cpp` — no bounds check on file handle index
- [x] `evaluator.cpp` — `vec.push()` silently ignores non-int values
- [x] `evaluator.cpp` — null deref in `++`/`--` error path + typo "non-varaible"
- [x] `evaluator.cpp` — typo "n ot" in dictionary error message
- [x] `evaluator.cpp` — unsupported binary op continues execution with null instead of halting
- [x] `parser.cpp` — dangling pointer / UB on `load` statement
- [x] `parser.cpp` — unchecked `dynamic_cast` for for-loop variable
- [x] `stdtent.cpp` — `ord()` checks for `int` instead of `string`
- [ ] `lexer.cpp` — shadowed member variable `curChar` in identifier scan loop

## Design / Code Quality
- [ ] `evalExpr` — ~600 lines of `dynamic_cast` chains; consider visitor pattern
- [ ] No block scoping — variables leak out of `if`/`while`/`for` blocks
- [ ] REPL brace counting ignores string literals (e.g. `"{"` breaks multi-line detection)
- [ ] Compound-assign token arithmetic relies on fragile enum ordering; use a lookup table
- [ ] `contract` keyword is lexed but never parsed or evaluated (dead keyword)
- [ ] `stdtent.cpp` / `io.cpp` — duplicated print-formatting logic; extract `value_to_string()` utility
- [ ] Bytecode VM — tree-walking interpreter is slow; consider compiling to bytecode for better performance

## Testing
- [ ] Test coverage is nearly zero — add `.tent` test files for each language feature
