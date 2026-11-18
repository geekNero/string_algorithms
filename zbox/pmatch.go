package zbox

func isParameter(b byte) bool {
	return b >= 'a' && b <= 'z'
}

func pMatch(z int, i int, s string, occ []int) int {
	for z+i < len(s) {
		if isParameter(s[z]) {
			if !isParameter(s[z+i]) {
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

// ZBoxPMatch returns the byte-based parameterized Z-array of s.
// Lowercase ASCII letters a-z match under a consistent one-to-one renaming;
// all other bytes must match literally. Each z[i] is the longest matching
// prefix length at byte i. By convention, z[0] is zero.
// Empty input returns an empty slice.
func ZBoxPMatch(s string) []int {
	occ := make([]int, len(s))
	prevOccurrence := map[byte]int{}

	for i := 0; i < len(s); i++ {
		b := s[i]
		if isParameter(b) {
			if prevOccurrence[b] > 0 {
				occ[i] = i + 1 - prevOccurrence[b]
			} else {
				occ[i] = 0
			}

			prevOccurrence[b] = i + 1
		}
	}

	l, r := 0, 0
	z := make([]int, len(s))

	for i := 1; i < len(s); i++ {
		if r < i {
			zVal := pMatch(0, i, s, occ)
			z[i] = zVal

			if zVal > 0 {
				l = i
				r = zVal + i - 1
			}
		} else {

			alpha := z[i-l]
			if alpha < r-i+1 {
				z[i] = alpha
			} else {
				zVal := pMatch(r-i+1, i, s, occ)
				z[i] = zVal
				r = i + zVal - 1
				l = i
			}

		}
	}

	return z
}
