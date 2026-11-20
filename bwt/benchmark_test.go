package bwt_test

import (
	"strings"
	"testing"

	"string_algorithms/bwt"
)

var (
	benchmarkTransform *bwt.Transform
	benchmarkFound     bool
	benchmarkError     error
)

func BenchmarkTransform(b *testing.B) {
	for _, tc := range loadTransformCases(b) {
		b.Run(tc.Name, func(b *testing.B) {
			checkTransform(b, tc)
			b.ReportAllocs()
			// Measure construction, including sorting and building the columns.
			for b.Loop() {
				benchmarkTransform, benchmarkError = bwt.BurrowsWheelerTransform(tc.Text)
			}
			if benchmarkError != nil {
				b.Fatal(benchmarkError)
			}
		})
	}
}

func BenchmarkContains(b *testing.B) {
	for _, tc := range loadSearchCases(b) {
		b.Run(tc.Name, func(b *testing.B) {
			if strings.Contains(tc.Text, "$") || strings.Contains(tc.Pattern, "$") {
				b.Skip("BWT reserves '$'")
			}
			transform, err := bwt.BurrowsWheelerTransform(tc.Text)
			if err != nil {
				b.Fatal(err)
			}
			checkContains(b, transform, tc.Pattern, len(tc.Expected) > 0)
			b.ReportAllocs()
			// b.Loop excludes the setup above. Only search on the built transform
			// is timed; fixture loading and output validation are also excluded.
			for b.Loop() {
				benchmarkFound, benchmarkError = transform.Contains(tc.Pattern)
			}
			if benchmarkError != nil {
				b.Fatal(benchmarkError)
			}
		})
	}
}
