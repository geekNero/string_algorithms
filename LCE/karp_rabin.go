package main

/*
	Code in this file would be written with the intent of better understanding Karp-Rabin.
*/

/*
	Karp Rabin as explained by Introduction to Algorithm CLRS
*/

// exercise 32.2-1
func KarpRabinCLRS(pattern string, text string) {
	n, m := len(text), len(pattern)
	if m == 0 || m > n {
		return
	}

	const (
		q                 = 11
		sigma             = 10
		smallestCharacter = int('0')
	)

	var hashP, hashT int
	for i := range m {
		hashP = (hashP*sigma + int(pattern[i]) - smallestCharacter) % q
		hashT = (hashT*sigma + int(text[i]) - smallestCharacter) % q
	}
	// fmt.Println("Initial hashes:", hashP, hashT)

	h := ModuloPow(sigma, m-1, q)

	for i := 0; i <= n-m; i++ {
		if hashP == hashT {
			if text[i:i+m] == pattern {
				// fmt.Printf("Found a match at index %d: %s\n", i, text[i:i+m])
			} else {
				// fmt.Printf("Spurious hit at index %d: %s\n", i, text[i:i+m])
			}
		}

		// Roll the hash to the next window
		if i < n-m {
			outgoing := int(text[i]) - smallestCharacter
			incoming := int(text[i+m]) - smallestCharacter

			hashT = ((hashT-outgoing*h)*sigma + incoming) % q
			if hashT < 0 {
				hashT += q
			}
		}
	}
}
