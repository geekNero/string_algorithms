package zbox

import "errors"

// ErrEmptyPattern is returned when a search is given an empty pattern.
var ErrEmptyPattern = errors.New("pattern must not be empty")

// FindAll returns the starting byte offsets of all exact occurrences of pattern
// in text, in ascending order, including overlapping matches. An empty pattern
// returns ErrEmptyPattern. No matches returns an empty slice and no error.
// The search takes O(len(text) + len(pattern)) time and space.
func FindAll(text, pattern string) ([]int, error) {
	return findAll(text, pattern, ZBox)
}

// FindAllParameterized returns the starting byte offsets of all parameterized
// occurrences of pattern in text. Lowercase ASCII letters a-z match under a
// consistent one-to-one renaming within each occurrence; other bytes must match
// literally. Offsets are ascending and include overlapping matches. An empty
// pattern returns ErrEmptyPattern. No matches returns an empty slice and no error.
// The search takes O(len(text) + len(pattern)) time and space.
func FindAllParameterized(text, pattern string) ([]int, error) {
	return findAll(text, pattern, ZBoxPMatch)
}

func findAll(text, pattern string, buildZ func(string) []int) ([]int, error) {
	n, m := len(text), len(pattern)
	if m == 0 {
		return nil, ErrEmptyPattern
	}

	positions := make([]int, 0)
	if m > n {
		return positions, nil
	}

	z := buildZ(pattern + text)
	for start := 0; start <= n-m; start++ {
		// The pattern occupies the first m bytes, so text[start] is at m+start.
		// No separator is needed: a match may extend beyond the pattern, but
		// only its first m bytes matter. The loop excludes incomplete windows.
		if z[m+start] >= m {
			positions = append(positions, start)
		}
	}
	return positions, nil
}
