package zbox_test

import (
	"encoding/json"
	"os"
	"slices"
	"testing"

	"string_algorithms/zbox"
)

type matchCase struct {
	Name     string `json:"name"`
	Text     string `json:"text"`
	Pattern  string `json:"pattern"`
	Expected []int  `json:"expected"`
}

func loadExactCases(tb testing.TB) []matchCase {
	tb.Helper()
	data, err := os.ReadFile("../testdata/exact_matching.json")
	if err != nil {
		tb.Fatal(err)
	}
	var cases []matchCase
	if err := json.Unmarshal(data, &cases); err != nil {
		tb.Fatal(err)
	}
	if len(cases) == 0 {
		tb.Fatal("exact matching fixtures must contain at least one case")
	}
	return cases
}

// The same small set of examples is used in tests and benchmarks.
var parameterizedCases = []matchCase{
	{"overlapping", "xyxyx", "aba", []int{0, 1, 2}},
	{"repeated", "xxxxx", "aaa", []int{0, 1, 2}},
	{"distinct", "xyz", "abc", []int{0}},
	{"inconsistent_renaming", "xyz", "aba", []int{}},
	{"bijection_required", "xx", "ab", []int{}},
	{"fixed_symbols", "x#xy#y", "a#a", []int{0, 3}},
	{"fixed_case", "xAx", "aAa", []int{0}},
	{"fixed_cannot_be_parameter", "a", "A", []int{}},
	{"pattern_too_long", "x", "aa", []int{}},
}

func TestFindAll(t *testing.T) {
	for _, tc := range loadExactCases(t) {
		t.Run(tc.Name, func(t *testing.T) { checkMatch(t, tc, zbox.FindAll) })
	}
}

func TestFindAllParameterized(t *testing.T) {
	for _, tc := range parameterizedCases {
		t.Run(tc.Name, func(t *testing.T) { checkMatch(t, tc, zbox.FindAllParameterized) })
	}
}

func checkMatch(tb testing.TB, tc matchCase, find func(string, string) ([]int, error)) {
	tb.Helper()
	got, err := find(tc.Text, tc.Pattern)
	if err != nil {
		tb.Fatal(err)
	}
	if !slices.Equal(got, tc.Expected) {
		tb.Fatalf("got %v, want %v", got, tc.Expected)
	}
}
