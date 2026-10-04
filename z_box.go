package main

import (
	"fmt"
)

func ZBox(s string) {
	l, r := 0, 0
	z := make([]int, len(s))

	for i := 1; i < len(s); i++ {
		if i > r {
			z[i] = bruteMatch(i, s)

			if z[i] > 0 {
				l = i
				r = i + z[i] - 1
			}
		} else {
			pos := i - l
			alpha := r - i + 1

			if z[pos] < alpha {
				z[i] = z[pos]
			} else {
				z[i] = alpha

				for i+z[i] < len(s) &&
					s[z[i]] == s[i+z[i]] {
					z[i]++
				}

				l = i
				r = i + z[i] - 1
			}
		}
	}

	fmt.Println(z)
}

func bruteMatch(index int, s string) int {
	z := 0

	for index+z < len(s) && s[z] == s[index+z] {
		z++
	}

	return z
}

func isParameter(r rune) bool {
	return r >= 'a' && r <= 'z'
}

func ZBoxPMatch(s string) {
	occurrence := make([]int, len(s))

	prevOccurrence := map[rune]int{}
	for i, r := range s {
		if isParameter(r) {
			if prevOccurrence[r] == 0 {
				occurrence[i] = 0
			} else {
				occurrence[i] = i + 1 - prevOccurrence[r]
			}
			prevOccurrence[r] = i + 1
		} else {
			s += string(r)
			occurrence[i] = -1
		}
	}

	print(occurrence)

	l := 0
	r := 0
	// First value of z is never supposed to be used
	z := []int{-1}

	for i := 1; i < len(s); i++ {
		if i > r {

			zVal := 0
			index := i
			for j, char := range s {
				if isParameter(char) {
					if occurrence[index] != -1 && (index-occurrence[index]) >= i {
						if occurrence[j] == occurrence[index] {
							index++
							zVal++
						} else {
							break
						}
					} else if occurrence[index] != -1 {
						if occurrence[j] == 0 {
							index++
							zVal++
						} else {
							break
						}
					} else {
						break
					}
				} else {
					if char == rune(s[index]) {
						index++
						zVal++
					} else {
						break
					}
				}
			}

			z = append(z, zVal)
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
				index := r + 1
				j := alpha + 1
				for index < len(s) {
					if isParameter(rune(s[j])) {
						if occurrence[index] != -1 && (index-occurrence[index]) >= i {
							if occurrence[j] == occurrence[index] {
								index++
							} else {
								break
							}
						} else if occurrence[index] != -1 {
							if occurrence[j] == 0 {
								index++
							} else {
								break
							}
						} else {
							break
						}
					} else {
						if s[j] == s[index] {
							index++
						} else {
							break
						}
					}
					j++
				}
				z = append(z, index-i)
			} else {
				z = append(z, z[pos])
			}
		}
	}

	fmt.Println(z)
}
