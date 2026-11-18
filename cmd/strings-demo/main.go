package main

import (
	"fmt"

	"string_algorithms/zbox"
)

func main() {
	fmt.Println("Input Text:")
	var text string
	fmt.Scan(&text)

	fmt.Println("Input Pattern:")
	var pattern string
	fmt.Scan(&pattern)

	s := pattern + "$" + text

	fmt.Println(zbox.ZBox(text))
	fmt.Println(zbox.ZBoxPMatch(s))
}
