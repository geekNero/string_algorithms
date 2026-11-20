package main

import (
	"fmt"

	"string_algorithms/bwt"
)

func main() {
	const text = "banana"
	first, last, err := bwt.BurrowsWheelerTransform([]byte(text))
	if err != nil {
		fmt.Println("Transform error:", err)
		return
	}

	lastBytes := make([]byte, len(last))
	for i, entry := range last {
		lastBytes[i] = entry.Char
	}
	fmt.Printf("Text: %q (the transform appends '$')\n", text)
	fmt.Printf("First column: %q\nLast column (BWT): %q\n", first, lastBytes)
	fmt.Println("Row  First  Last  LF row")
	for row, entry := range last {
		fmt.Printf("%3d  %5q  %4q  %6d\n", row, first[row], entry.Char, entry.Index)
	}
	fmt.Println("Each LF link moves to the row for the preceding text position.")
	fmt.Println("Search reads the query from right to left through these links.")

	for _, pattern := range []string{"ana", "ANA", "apple", "", "$"} {
		found, err := bwt.BWTSearch(first, last, pattern)
		if err != nil {
			fmt.Printf("Contains %q: error: %v\n", pattern, err)
			continue
		}
		fmt.Printf("Contains %q: %t\n", pattern, found)
	}
}
