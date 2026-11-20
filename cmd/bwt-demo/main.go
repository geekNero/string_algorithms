package main

import (
	"fmt"

	"string_algorithms/bwt"
)

func main() {
	const text = "banana"
	transform, err := bwt.BurrowsWheelerTransform(text)
	if err != nil {
		fmt.Println("Transform error:", err)
		return
	}

	first, last := transform.FirstColumn(), transform.LastColumn()
	links := transform.LFMapping()
	fmt.Printf("Text: %q (the transform appends '$')\n", text)
	fmt.Printf("First column: %q\nLast column (BWT): %q\n", first, last)
	fmt.Println("Row  First  Last  LF row")
	for row := 0; row < len(last); row++ {
		fmt.Printf("%3d  %5q  %4q  %6d\n", row, first[row], last[row], links[row])
	}
	fmt.Println("Each LF link moves to the row for the preceding text position.")
	fmt.Println("Search reads the query from right to left through these links.")

	for _, pattern := range []string{"ana", "ANA", "apple", "", "$"} {
		found, err := transform.Contains(pattern)
		if err != nil {
			fmt.Printf("Contains %q: error: %v\n", pattern, err)
			continue
		}
		fmt.Printf("Contains %q: %t\n", pattern, found)
	}
}
