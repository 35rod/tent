# Tent test suite

The Tent test suite is fixture-based and executed through CTest.

## Run tests

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Run only smoke tests:

```sh
ctest --test-dir build -L smoke --output-on-failure
```

## Fixture format

Each test case lives in:

`tests/cases/<case-name>/`

Required:

- `program.tent` — program to execute

Optional:

- `args.txt` — CLI args (one argument per line)
- `stdin.txt` — stdin passed to the program
- `expected.out` — exact expected stdout
- `expected.err` — exact expected stderr
- `expected.err.contains` — required stderr snippets (one per line)
- `expected.code` — expected process exit code (defaults to `0`)
- `smoke` — empty marker file to include the case in the smoke subset

## Adding new tests

1. Create `tests/cases/<new-case>/program.tent`.
2. Add expected output files (`expected.out`, `expected.code`, etc.).
3. Optionally add a `smoke` file.
4. Re-run CMake configure/build and execute CTest.

Cases are auto-discovered by `tests/CMakeLists.txt`; no manual registration is needed.
