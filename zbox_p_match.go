package main

import "fmt"

func isParameter(r rune) bool {
	return r >= 'a' && r <= 'z'
}

func pMatch(z int, i int, s string, occ []int) int {
	for z+i < len(s) {
		if isParameter(rune(s[z])) {
			if !isParameter(rune(s[z+i])) {
				break
			}
			boundedOccurrence := occ[z+i]
			if z-boundedOccurrence < 0 {
				boundedOccurrence = 0
			}

			if boundedOccurrence != occ[z] {
				break
			}
		} else if s[z] != s[z+i] {
			break
		}
		z++
	}
	return z
}

func ZBoxPMatch(s string) {

	occ := make([]int, len(s))
	prevOccurrence := map[rune]int{}

	for i, r := range s {
		if isParameter(r) {
			if prevOccurrence[r] > 0 {
				occ[i] = i + 1 - prevOccurrence[r]
			} else {
				occ[i] = 0
			}

			prevOccurrence[r] = i + 1
		} else {
			prevOccurrence[r] = -1
		}
	}

	l, r := 0, 0
	z := []int{-1}

	for i := 1; i < len(s); i++ {
		if r < i {
			zVal := pMatch(0, i, s, occ)
			z = append(z, zVal)

			if zVal > 0 {
				l = i
				r = zVal + i - 1
			}
		} else {

			alpha := z[i-l]
			if alpha < r-i+1 {
				z = append(z, alpha)
			} else {
				zVal := pMatch(r-i+1, i, s, occ)
				z = append(z, zVal)
				r = i + zVal - 1
				l = i
			}

		}
	}

	fmt.Println(z)

}
