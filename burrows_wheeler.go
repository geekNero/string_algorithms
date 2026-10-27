package main

import (
	"fmt"
	"os"
	"slices"
	"strings"
)

type BWTRune struct {
	char  rune
	index int
}

func main() {

	if len(os.Args) < 3 {
		fmt.Println("expected args: filename searchQuery")
		os.Exit(1)
	}

	filePath := os.Args[1]

	searchQuery := os.Args[2]

	textData, err := os.ReadFile(filePath)
	if err != nil {
		fmt.Println("failed to read file data, error: ", err.Error())
		os.Exit(1)
	}

	firstCol, lastCol := BurrowsWheelerTransform(textData)

	fmt.Println(BWTSearch(firstCol, lastCol, searchQuery))

}

func BurrowsWheelerTransform(textData []byte) ([]rune, []BWTRune) {

	text := string(textData)
	text = strings.ToLower(text)
	text = strings.TrimSpace(text) + "$"
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

	lastColumn := []BWTRune{}
	firstColumn := []rune{}
	lastColumnIndex := map[int]int{}

	for index, i := range substringIndices {
		lastColumnIndex[i] = index
		firstColumn = append(firstColumn, rune(text[i]))
	}

	for _, i := range substringIndices {
		lastColumn = append(lastColumn, BWTRune{char: rune(text[(i+n-1)%n]), index: lastColumnIndex[(i+n-1)%n]})
	}

	return firstColumn, lastColumn

}

func BWTSearch(firstColumn []rune, lastColumn []BWTRune, searchString string) bool {
	cutset := []int{}

	target := rune(searchString[len(searchString)-1])

	for index, char := range firstColumn {
		if char == target {
			cutset = append(cutset, index)
		}
	}

	if len(cutset) == 0 {
		return false
	}

	return search(searchString[:len(searchString)-1], firstColumn, lastColumn, cutset)

}

func search(searchString string, firstColumn []rune, lastColumn []BWTRune, cutset []int) bool {

	if len(searchString) == 0 {
		return true
	}

	newCutset := []int{}
	target := rune(searchString[len(searchString)-1])
	for _, index := range cutset {
		if lastColumn[index].char == target {
			newCutset = append(newCutset, lastColumn[index].index)
		}
	}

	if len(newCutset) == 0 {
		return false
	}

	if len(searchString) == 1 {
		return true
	} else {
		return search(searchString[:len(searchString)-1], firstColumn, lastColumn, newCutset)
	}

}
