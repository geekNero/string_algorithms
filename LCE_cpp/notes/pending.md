The current implementation works only with ascii, as utf-8 uses multiple bytes.
The mapping and rank isn't formed on a single byte, so they aren't one-to-one with the alphabet.

The lce would still return an output but it could also partially match a multi byte charcter, for ex:
🙂é🙂ê

In this case, lce(start, middle) would partially match both e characters.


To support utf-8, provide additional wrapper for utf-8 that uses updated word packing functions.
The mapping and rank should be formed on the symbols.
Since inplace fingerprinting only touches the word packed symbols, it can be oblivious of the underlying
representation of the string.
