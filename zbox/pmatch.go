package zbox

func isParameter(b byte) bool {
	return b >= 'a' && b <= 'z'
}

// extendParameterized extends a known parameterized match at start and
// returns its total length. prevDistance is the encoding of the entire s.
func extendParameterized(s string, prevDistance []int, start, matched int) int {
	for matched+start < len(s) {
		if isParameter(s[matched]) {
			if !isParameter(s[matched+start]) {
				break
			}
			distance := prevDistance[matched+start]
			// An occurrence before start is outside the candidate substring,
			// so this parameter is locally unseen and its distance becomes zero.
			if distance > matched {
				distance = 0
			}

			// Equal previous-occurrence distances enforce a consistent bijection.
			if distance != prevDistance[matched] {
				break
			}
		} else if s[matched] != s[matched+start] {
			break
		}
		matched++
	}
	return matched
}

// previousDistances encodes each parameter's distance to its previous
// occurrence. First occurrences are zero. Fixed bytes also retain zero,
// but are compared literally rather than through this encoding.
func previousDistances(s string) []int {
	prevDistance := make([]int, len(s))
	var lastSeen [26]int // Previous byte position + 1; zero means unseen.

	for i := 0; i < len(s); i++ {
		b := s[i]
		if !isParameter(b) {
			continue
		}

		slot := b - 'a'
		if previous := lastSeen[slot]; previous != 0 {
			prevDistance[i] = i + 1 - previous
		}
		lastSeen[slot] = i + 1
	}
	return prevDistance
}

// ZBoxPMatch returns the byte-based parameterized Z-array of s.
// Lowercase ASCII letters a-z match under a consistent one-to-one renaming;
// all other bytes must match literally. Each z[i] is the longest matching
// prefix length at byte i. By convention, z[0] is zero.
// Empty input returns an empty slice.
func ZBoxPMatch(s string) []int {
	prevDistance := previousDistances(s)
	// [l, r] matches the prefix under a bijection of parameters; r never decreases.
	l, r := 0, 0
	z := make([]int, len(s))

	for i := 1; i < len(s); i++ {
		if i <= r {
			pos := i - l
			remaining := r - i + 1
			// A match ending inside the box can be reused without extending it.
			if z[pos] < remaining {
				z[i] = z[pos]
				continue
			}
			// The box guarantees this prefix; only the part beyond it is unknown.
			z[i] = remaining
		}
		z[i] = extendParameterized(s, prevDistance, i, z[i])
		if z[i] > 0 {
			l = i
			r = i + z[i] - 1
		}
	}

	return z
}
