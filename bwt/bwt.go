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
	Index int // LF link: row corresponding to the preceding text position.
}

// BurrowsWheelerTransform returns the first and last columns of the transform.
// It preserves case and whitespace and processes bytes, not Unicode code points.
// A unique '$' terminator is appended internally; input containing '$' returns
// ErrReservedTerminator. Empty input is valid and produces a single '$' row.
func BurrowsWheelerTransform(textData []byte) ([]byte, []BWTByte, error) {
	text := string(textData)
	if strings.IndexByte(text, '$') >= 0 {
		return nil, nil, ErrReservedTerminator
	}
	text += "$"
	n := len(text)

	substringIndices := []int{}
	for i := range n {
		substringIndices = append(substringIndices, i)
	}

	slices.SortFunc(substringIndices, func(a int, b int) int {
		if text[a:] < text[b:] {
			return -1
		} else if text[a:] > text[b:] {
			return 1
		} else {
			return 0
		}
	})

	lastColumn := []BWTByte{}
	firstColumn := []byte{}
	lastColumnIndex := map[int]int{}

	for index, i := range substringIndices {
		lastColumnIndex[i] = index
		firstColumn = append(firstColumn, text[i])
	}

	for _, i := range substringIndices {
		lastColumn = append(lastColumn, BWTByte{Char: text[(i+n-1)%n], Index: lastColumnIndex[(i+n-1)%n]})
	}

	return firstColumn, lastColumn, nil
}

// BWTSearch reports whether searchString occurs in the original text.
// Matching is case-sensitive and byte-based. An empty query returns
// ErrEmptyPattern; a query containing '$' returns ErrReservedTerminator.
// Pass the unmodified columns returned together by BurrowsWheelerTransform.
func BWTSearch(firstColumn []byte, lastColumn []BWTByte, searchString string) (bool, error) {
	if len(searchString) == 0 {
		return false, ErrEmptyPattern
	}
	if strings.IndexByte(searchString, '$') >= 0 {
		return false, ErrReservedTerminator
	}
	cutset := []int{}

	target := searchString[len(searchString)-1]

	for index, char := range firstColumn {
		if char == target {
			cutset = append(cutset, index)
		}
	}

	if len(cutset) == 0 {
		return false, nil
	}

	return search(searchString[:len(searchString)-1], firstColumn, lastColumn, cutset), nil
}

func search(searchString string, firstColumn []byte, lastColumn []BWTByte, cutset []int) bool {
	if len(searchString) == 0 {
		return true
	}

	newCutset := []int{}
	target := searchString[len(searchString)-1]
	for _, index := range cutset {
		if lastColumn[index].Char == target {
			newCutset = append(newCutset, lastColumn[index].Index)
		}
	}

	if len(newCutset) == 0 {
		return false
	}

	return search(searchString[:len(searchString)-1], firstColumn, lastColumn, newCutset)
}
