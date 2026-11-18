# String matching examples

`exact_matching.json` contains inputs and expected outputs for exact matching.
It is shared by the Go correctness tests and benchmarks, and can be reused by
implementations in other languages.

Each case has this format:

```json
{
  "name": "overlapping",
  "text": "banana",
  "pattern": "ana",
  "expected": [1, 3]
}
```

- Strings are UTF-8. Positions are **zero-based byte offsets**, not character
  indices. For example, `"éé"` with pattern `"é"` gives `[0, 2]`.
- Matching is case-sensitive and includes overlapping occurrences.
- Positions are ascending. An empty array means no match.
- Patterns in this file are nonempty. Empty patterns are errors, checked
  separately by `TestEmptyPattern` for both matching functions.

The 13 cases cover overlaps, repetition, periodic text, natural language,
irregular lowercase text, missing matches, long common prefixes, whole-text
matches, patterns longer than the text, empty text, literal `$`, case
sensitivity, and UTF-8 byte offsets. Text lengths range from 0 to 1,024 bytes.

To add an example, append an object with a unique name, the exact input strings,
and the expected positions. Both the test and exact-match benchmark will pick
it up automatically. Keep expected positions independently calculated.

## Run from the repository root

Run the fixed demonstration:

```sh
go run ./cmd/strings-demo
```

Check the examples:

```sh
go test ./zbox -v
```

Benchmark exact matching against every JSON case:

```sh
go test ./zbox -run '^$' -bench '^BenchmarkFindAll$' -benchmem -benchtime=100ms
```

Benchmark parameterized matching against its small example table:

```sh
go test ./zbox -run '^$' -bench '^BenchmarkFindAllParameterized$' -benchmem -benchtime=100ms
```

Focus on one exact-match case:

```sh
go test ./zbox -run '^$' -bench '^BenchmarkFindAll$/^periodic$' -benchmem -benchtime=100ms
```

`-run '^$'` skips ordinary tests. Each benchmark still checks its expected
positions once before timing. JSON loading and correctness checks are excluded
from timing; each measured operation is a complete search, including Z-array
construction and collecting match positions.

## Read the benchmark output

| Column | Meaning |
|---|---|
| Benchmark name | Matching function and example name; the numeric suffix is the Go CPU setting. |
| Iteration count | How many searches Go ran to measure that example; this is not the match count. |
| `ns/op` | Average nanoseconds per complete search; lower is faster. |
| `B/op` | Bytes allocated per search, not peak memory usage or input size. |
| `allocs/op` | Number of allocations per search. |

The 100 ms duration makes a quick learning-project run. To compare a code change,
use the same named cases and machine before and after; add `-count=3` and use
`-benchtime=1s` for less noisy measurements. Short examples expose function and
allocation overhead; they do not establish large-input scaling. Larger explicit
examples can be added to the JSON when that becomes the question of interest.
