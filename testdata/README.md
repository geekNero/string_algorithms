# Test data

Run commands from the repository root.

| Task | Command |
|---|---|
| All Z-box and BWT tests | `go test ./zbox ./bwt -v` |
| Z-box tests | `go test ./zbox -v` |
| BWT tests | `go test ./bwt -v` |
| Exact Z-box benchmark | `go test ./zbox -run '^$' -bench '^BenchmarkFindAll$' -benchmem -benchtime=100ms` |
| Parameterized Z-box benchmark | `go test ./zbox -run '^$' -bench '^BenchmarkFindAllParameterized$' -benchmem -benchtime=100ms` |
| BWT construction benchmark | `go test ./bwt -run '^$' -bench '^BenchmarkTransform$' -benchmem -benchtime=100ms` |
| BWT search benchmark | `go test ./bwt -run '^$' -bench '^BenchmarkContains$' -benchmem -benchtime=100ms` |

## Files and consumers

| File | Fields | Used by |
|---|---|---|
| [exact_matching.json](exact_matching.json) | `name`, `text`, `pattern`, `expected` | Exact Z-box search and BWT search tests/benchmarks |
| [bwt.json](bwt.json) | `name`, `text`, `first_column`, `last_column`, `lf_mapping` | BWT construction tests/benchmarks |

Parameterized Z-box matching uses its own example table in
`zbox/matches_test.go`. The JSON fixtures are language-independent.


## Fixture rules

- Strings are UTF-8; matching is case-sensitive and preserves whitespace.
- Exact-match patterns must be nonempty. `expected` lists all overlapping matches
  as ascending, zero-based **byte offsets**; `[]` means no match.
- BWT search expects `true` when `expected` is nonempty. Cases containing `$` in
  the text or pattern are skipped because BWT reserves that byte.
- Keep `bwt.json` examples ASCII so columns remain readable JSON strings. Input
  text must not contain `$`; expected columns include the appended terminator.
  `lf_mapping` contains zero-based row indices, not text positions.
- Invalid-input behavior, including empty-pattern errors, belongs in package tests.

## Benchmarks

`-run '^$'` skips ordinary tests. Each benchmark checks expected output before
measurement; fixture loading and validation are excluded. `100ms` is the target
measurement time **per case**. Add `-count=3` to repeat measurements.

- **Exact/parameterized Z-box:** complete search, including Z-array construction
  and collecting positions.
- **BWT construction:** suffix sorting and building columns and links.
- **BWT search:** query on an already-built transform; construction is excluded.

`ns/op` is time per operation, `B/op` is bytes allocated per operation, and
`allocs/op` is allocation count. The iteration count is how often Go repeated the
operation. Compare the same case and workload before/after a change; BWT's
query-only Boolean result and Z-box's complete search are different workloads.
