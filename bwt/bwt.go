package bwt

import (
	"errors"
	"slices"
	"strings"
)

// ErrReservedTerminator indicates that the input contains the internal terminator.
var ErrReservedTerminator = errors.New("'$' is reserved as the BWT terminator")

// ErrEmptyPattern is returned when a search is given an empty pattern.
var ErrEmptyPattern = errors.New("pattern must not be empty")

// BWTByte contains a last-column byte and its link to a first-column row.
type BWTByte struct {
	Char  byte
	LFRow int // LF link: row corresponding to the preceding text position.
}

// Transform keeps the columns and LF links for one text together.
// Construct it with BurrowsWheelerTransform; its accessors expose no mutable
// references to the stored columns or links.
type Transform struct {
	firstColumn []byte
	lastColumn  []BWTByte
}

// BurrowsWheelerTransform constructs a transform for text.
// It preserves case and whitespace and processes bytes, not Unicode code points.
// A unique '$' terminator is appended internally; input containing '$' returns
// ErrReservedTerminator. Empty input is valid and produces a single '$' row.
func BurrowsWheelerTransform(text string) (*Transform, error) {
	if strings.IndexByte(text, '$') >= 0 {
		return nil, ErrReservedTerminator
	}
	text += "$"
	n := len(text)

	suffixArray := make([]int, n)
	for i := range n {
		suffixArray[i] = i
	}

	// Sort suffixes directly to keep this learning implementation straightforward.
	slices.SortFunc(suffixArray, func(a int, b int) int {
		return strings.Compare(text[a:], text[b:])
	})

	lastColumn := make([]BWTByte, n)
	firstColumn := make([]byte, n)

	// Map each text position to the row where its suffix appears.
	suffixRank := make([]int, n)

	for i, index := range suffixArray {
		suffixRank[index] = i
		firstColumn[i] = text[index]
	}

	for i, index := range suffixArray {
		// The preceding byte forms the last column; its suffix row is the LF link.
		lastColumn[i] = BWTByte{Char: text[(index+n-1)%n], LFRow: suffixRank[(index+n-1)%n]}
	}

	// The first column could be derived by sorting the last; retain it for clarity.
	return &Transform{firstColumn: firstColumn, lastColumn: lastColumn}, nil
}

// FirstColumn returns the sorted first column, including the terminator.
func (t *Transform) FirstColumn() string {
	return string(t.firstColumn)
}

// LastColumn returns the BWT byte sequence, including the terminator.
func (t *Transform) LastColumn() string {
	column := make([]byte, len(t.lastColumn))
	for row, entry := range t.lastColumn {
		column[row] = entry.Char
	}
	return string(column)
}

// LFMapping returns a copy of the row links. Entry i identifies the row for the
// text position preceding the suffix at row i.
func (t *Transform) LFMapping() []int {
	links := make([]int, len(t.lastColumn))
	for row, entry := range t.lastColumn {
		links[row] = entry.LFRow
	}
	return links
}

// Contains tells whether searchString occurs in the original text.
// Matching is case-sensitive and byte-based. An empty query returns
// ErrEmptyPattern; a query containing '$' returns ErrReservedTerminator.
func (t *Transform) Contains(searchString string) (bool, error) {
	if len(searchString) == 0 {
		return false, ErrEmptyPattern
	}
	if strings.IndexByte(searchString, '$') >= 0 {
		return false, ErrReservedTerminator
	}
	candidateRows := []int{}

	patternIndex := len(searchString) - 1

	for index, char := range t.firstColumn {
		if searchString[patternIndex] == char {
			candidateRows = append(candidateRows, index)
		}
	}

	if len(candidateRows) == 0 {
		return false, nil
	}

	patternIndex--

	nextCandidateRows := []int{}
	for patternIndex >= 0 {
		// Candidates represent the already-matched suffix of the query.
		if len(candidateRows) == 0 {
			return false, nil
		}
		for _, index := range candidateRows {
			if t.lastColumn[index].Char == searchString[patternIndex] {
				nextCandidateRows = append(nextCandidateRows, t.lastColumn[index].LFRow)
			}
		}
		// Reuse the old input buffer only after its rows have all been read.
		candidateRows, nextCandidateRows = nextCandidateRows, candidateRows[:0]
		patternIndex--
	}
	// The final comparison may have removed the last remaining candidate.
	return len(candidateRows) > 0, nil
}
