package main

import "fmt"

func ZBox(s string) {

	l := 0
	r := 0
	// First value of z is never supposed to be used
	z := []int{-1}

	for i := 1; i < len(s); i++ {
		if i > r {
			z = append(z, bruteMatch(i, &s))
			if z[i] > 0 {
				l = i
				r = l + z[i] - 1
			}
		} else {
			pos := i - l

			alpha := r - i + 1
			if z[pos] > alpha {
				z = append(z, alpha)
			} else if z[pos] == alpha {
				index := alpha
				for _, char := range s {
					if char == rune(s[index]) {
						index++
					} else {
						break
					}
				}
				z = append(z, index)
			} else {
				z = append(z, z[pos])
			}
		}
	}

	fmt.Println(z)
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
