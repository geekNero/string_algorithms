package bwt_test

import (
	"encoding/json"
	"errors"
	"math/rand"
	"os"
	"slices"
	"strings"
	"testing"

	"string_algorithms/bwt"
)

type transformCase struct {
	Name        string `json:"name"`
	Text        string `json:"text"`
	FirstColumn string `json:"first_column"`
	LastColumn  string `json:"last_column"`
	LFMapping   []int  `json:"lf_mapping"`
}

type searchCase struct {
	Name     string `json:"name"`
	Text     string `json:"text"`
	Pattern  string `json:"pattern"`
	Expected []int  `json:"expected"`
}

func readCases(tb testing.TB, filename string, cases any) {
	tb.Helper()
	data, err := os.ReadFile("../testdata/" + filename)
	if err != nil {
		tb.Fatal(err)
	}
	if err := json.Unmarshal(data, cases); err != nil {
		tb.Fatal(err)
	}
}

func loadTransformCases(tb testing.TB) []transformCase {
	tb.Helper()
	var cases []transformCase
	readCases(tb, "bwt.json", &cases)
	if len(cases) == 0 {
		tb.Fatal("BWT fixtures must contain at least one case")
	}
	return cases
}

func loadSearchCases(tb testing.TB) []searchCase {
	tb.Helper()
	var cases []searchCase
	readCases(tb, "exact_matching.json", &cases)
	if len(cases) == 0 {
		tb.Fatal("search fixtures must contain at least one case")
	}
	return cases
}

// A seeded generator avoids adding a large text file to this learning project.
func comparisonInput() (string, []string) {
	const textSize = 256 * 1024
	const alphabet = "abcdefghijklmnopqrstuvwxyz "
	rng := rand.New(rand.NewSource(20261120))
	randomText := func(length int) string {
		data := make([]byte, length)
		for i := range data {
			data[i] = alphabet[rng.Intn(len(alphabet))]
		}
		return string(data)
	}
	text := randomText(textSize)
	patterns := make([]string, 0, 12)
	// Present patterns are sampled from the beginning, middle, and end.
	for i, length := range []int{1, 4, 8, 16, 32, 64} {
		start := i * (len(text) - length) / 5
		patterns = append(patterns, text[start:start+length])
	}
	// Absent patterns use the same alphabet, rather than a special missing byte.
	for _, length := range []int{8, 12, 16, 24, 32, 64} {
		for {
			pattern := randomText(length)
			if !strings.Contains(text, pattern) {
				patterns = append(patterns, pattern)
				break
			}
		}
	}
	return text, patterns
}

func TestTransform(t *testing.T) {
	for _, tc := range loadTransformCases(t) {
		t.Run(tc.Name, func(t *testing.T) {
			checkTransform(t, tc)
		})
	}
}

func TestContainsComparison(t *testing.T) {
	text, patterns := comparisonInput()
	t.Helper()
	transform, err := bwt.BurrowsWheelerTransform(text)
	if err != nil {
		t.Fatal(err)
	}
	for _, pattern := range patterns {
		checkContains(t, transform, pattern, strings.Contains(text, pattern))
	}
}

func checkTransform(tb testing.TB, tc transformCase) {
	tb.Helper()
	transform, err := bwt.BurrowsWheelerTransform(tc.Text)
	if err != nil {
		tb.Fatal(err)
	}
	if got := transform.FirstColumn(); got != tc.FirstColumn {
		tb.Fatalf("first column: got %q, want %q", got, tc.FirstColumn)
	}
	if got := transform.LastColumn(); got != tc.LastColumn {
		tb.Fatalf("last column: got %q, want %q", got, tc.LastColumn)
	}
	if got := transform.LFMapping(); !slices.Equal(got, tc.LFMapping) {
		tb.Fatalf("LF mapping: got %v, want %v", got, tc.LFMapping)
	}
}

func TestContains(t *testing.T) {
	for _, tc := range loadSearchCases(t) {
		t.Run(tc.Name, func(t *testing.T) {
			if strings.Contains(tc.Text, "$") || strings.Contains(tc.Pattern, "$") {
				t.Skip("BWT reserves '$'; rejection is checked in TestInputErrors")
			}
			transform, err := bwt.BurrowsWheelerTransform(tc.Text)
			if err != nil {
				t.Fatal(err)
			}
			checkContains(t, transform, tc.Pattern, len(tc.Expected) > 0)
		})
	}
}

func TestContainsBoundaryCases(t *testing.T) {
	cases := []struct {
		name, text, pattern string
		want                bool
	}{
		{"final_mismatch", "banana", "xana", false},
		{"two_byte_mismatch", "a", "ba", false},
		{"past_text_end", "a", "aa", false},
		{"single_byte_match", "a", "a", true},
		{"preserve_whitespace", " A ", " A ", true},
		{"preserve_case", " A ", "a", false},
	}
	for _, tc := range cases {
		t.Run(tc.name, func(t *testing.T) {
			transform, err := bwt.BurrowsWheelerTransform(tc.text)
			if err != nil {
				t.Fatal(err)
			}
			checkContains(t, transform, tc.pattern, tc.want)
		})
	}
}

func checkContains(tb testing.TB, transform *bwt.Transform, pattern string, want bool) {
	tb.Helper()
	got, err := transform.Contains(pattern)
	if err != nil {
		tb.Fatal(err)
	}
	if got != want {
		tb.Fatalf("pattern %q: got %t, want %t", pattern, got, want)
	}
}

func TestInputErrors(t *testing.T) {
	for _, text := range []string{"$", "a$b"} {
		transform, err := bwt.BurrowsWheelerTransform(text)
		if transform != nil || !errors.Is(err, bwt.ErrReservedTerminator) {
			t.Fatalf("text %q: expected nil transform and ErrReservedTerminator, got %v, %v", text, transform, err)
		}
	}
	transform, err := bwt.BurrowsWheelerTransform("banana")
	if err != nil {
		t.Fatal(err)
	}
	for _, pattern := range []string{"", "$", "a$"} {
		want := bwt.ErrReservedTerminator
		if pattern == "" {
			want = bwt.ErrEmptyPattern
		}
		found, err := transform.Contains(pattern)
		if found || !errors.Is(err, want) {
			t.Fatalf("pattern %q: got %t, %v; want false, %v", pattern, found, err, want)
		}
	}
}
