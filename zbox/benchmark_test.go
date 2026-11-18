package zbox_test

import (
	"testing"

	"string_algorithms/zbox"
)

var benchmarkPositions []int
var benchmarkError error

func BenchmarkFindAll(b *testing.B) {
	benchmarkCases(b, loadExactCases(b), zbox.FindAll)
}

func BenchmarkFindAllParameterized(b *testing.B) {
	benchmarkCases(b, parameterizedCases, zbox.FindAllParameterized)
}

func benchmarkCases(b *testing.B, cases []matchCase, find func(string, string) ([]int, error)) {
	for _, tc := range cases {
		b.Run(tc.Name, func(b *testing.B) {
			// Verify expected output before timing the complete search.
			checkMatch(b, tc, find)
			b.ReportAllocs()
			for b.Loop() {
				benchmarkPositions, benchmarkError = find(tc.Text, tc.Pattern)
			}
			if benchmarkError != nil {
				b.Fatal(benchmarkError)
			}
		})
	}
}
