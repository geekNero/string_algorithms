package main

import "fmt"

func ZBox() {
	fmt.Println("Input Text:")
	var text string
	fmt.Scan(&text)

	fmt.Println("Input Pattern:")
	var pattern string
	fmt.Scan(&pattern)

	s := pattern + "$" + text

	l := 0
	r := 0
	z := []int{}

	for i := 1; i < len(s); i++ {
		if len(z) == 0 {
			z = append(z, bruteMatch(i, &s))
			if z[i-1] > 0 {
				l = i
				r = l + z[i-1] + 1
			}
		}
		// if r>= x
	}
}

func bruteMatch(index int, s *string) int {
	z := 0

	for _, char := range *s {
		if char == rune((*s)[index]) {
			index++
			z++
		} else {
			return z
		}
	}
	return z
}
