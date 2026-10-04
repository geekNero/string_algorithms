package main

import "fmt"

func main() {
	fmt.Println("Input Text:")
	var text string
	fmt.Scan(&text)

	// fmt.Println("Input Pattern:")
	// var pattern string
	// fmt.Scan(&pattern)

	// s := pattern + "$" + text

	// ZBox(text)
	ZBoxPMatch(text)
}
