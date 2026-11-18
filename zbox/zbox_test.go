package zbox_test

import (
	"slices"
	"testing"

	"string_algorithms/zbox"
)

func TestZArrays(t *testing.T) {
	tests := []struct {
		name, text           string
		exact, parameterized []int
	}{
		{"empty", "", []int{}, []int{}},
		{"single", "a", []int{0}, []int{0}},
		{"repeated", "aaaa", []int{0, 3, 2, 1}, []int{0, 3, 2, 1}},
		{"distinct", "abc", []int{0, 0, 0}, []int{0, 2, 1}},
		{"periodic", "abab", []int{0, 0, 2, 0}, []int{0, 3, 2, 1}},
		{"fixed_symbol", "a#a", []int{0, 0, 1}, []int{0, 0, 1}},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			if got := zbox.ZBox(tt.text); !slices.Equal(got, tt.exact) {
				t.Fatalf("exact: got %v, want %v", got, tt.exact)
			}
			if got := zbox.ZBoxPMatch(tt.text); !slices.Equal(got, tt.parameterized) {
				t.Fatalf("parameterized: got %v, want %v", got, tt.parameterized)
			}
		})
	}
}
