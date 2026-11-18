package main

import (
	"fmt"

	"string_algorithms/zbox"
)

func main() {
	const text, pattern = "banana", "ana"
	combined := pattern + text
	fmt.Printf("Text: %q\nPattern: %q\n", text, pattern)
	fmt.Printf("Z-arrays are computed on pattern + text: %q\n", combined)
	fmt.Println("Exact Z-array:", zbox.ZBox(combined))
	fmt.Println("Parameterized Z-array:", zbox.ZBoxPMatch(combined))
	fmt.Println("A complete match has z[len(pattern)+start] >= len(pattern).")

	exact, err := zbox.FindAll(text, pattern)
	if err != nil {
		fmt.Println("Search error:", err)
		return
	}
	printMatches("Exact matches", text, len(pattern), exact)

	parameterized, err := zbox.FindAllParameterized(text, pattern)
	if err != nil {
		fmt.Println("Search error:", err)
		return
	}
	printMatches("Parameterized matches", text, len(pattern), parameterized)
	fmt.Println(`"nan" matches "ana" by renaming a to n and n to a.`)
}

func printMatches(label, text string, patternLength int, positions []int) {
	fmt.Println(label + ":")
	if len(positions) == 0 {
		fmt.Println("  No matches")
		return
	}
	for _, start := range positions {
		fmt.Printf("  byte %d: %q\n", start, text[start:start+patternLength])
	}
}
