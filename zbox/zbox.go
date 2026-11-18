package zbox

// ZBox returns the byte-based Z-array of s. Each entry z[i] is the length
// of the longest prefix of s matching the suffix starting at byte i.
// By convention, z[0] is zero. Empty input returns an empty slice.
// It runs in O(len(s)) time, using constant auxiliary space besides the result.
func ZBox(s string) []int {
	// [l, r] is an inclusive interval matching the prefix; r never decreases.
	l, r := 0, 0
	z := make([]int, len(s))

	for i := 1; i < len(s); i++ {
		if i <= r {
			pos := i - l
			remaining := r - i + 1
			// A match ending inside the box has an already-known mismatch.
			if z[pos] < remaining {
				z[i] = z[pos]
				continue
			}
			// The box guarantees this prefix; compare only beyond its boundary.
			z[i] = remaining
		}

		z[i] = extendExact(s, i, z[i])
		if z[i] > 0 {
			l = i
			r = i + z[i] - 1
		}
	}
	return z
}

// extendExact extends a known matching prefix of length matched at start
// and returns the total match length.
func extendExact(s string, start, matched int) int {
	for start+matched < len(s) && s[matched] == s[start+matched] {
		matched++
	}
	return matched
}
