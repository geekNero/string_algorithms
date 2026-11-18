package zbox

// ZBox returns the byte-based Z-array of s. Each entry z[i] is the length
// of the longest prefix of s matching the suffix starting at byte i.
// By convention, z[0] is zero. Empty input returns an empty slice.
func ZBox(s string) []int {
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

	return z
}

func bruteMatch(index int, s string) int {
	z := 0

	for index+z < len(s) && s[z] == s[index+z] {
		z++
	}

	return z
}
