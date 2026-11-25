# LCE API contract

## Input and ownership

- Symbol-based LCE requires ASCII or another single-byte encoding: one byte
  per symbol. Unicode-character LCE on UTF-8 text is not supported.
- `inplace_fingerprint_wrapper(const string &text)` accepts arbitrary bytes,
  including `0x00` and `0xFF`, and returns a caller-owned `WordRAMCompressed`.
  It does not validate the encoding or reject non-ASCII input.
- Construction leaves the input unchanged and retains no reference to it.
  The caller owns the input and returned structure independently.
- Query only a structure produced by the wrapper, without modifying its fields.
  `lce` reads it through a const reference and does not modify it.

## Queries

- Positions count bytes and use zero-based `size_t` indices: `0 <= i,j < n`.
- The intended answer is the number of matching consecutive bytes from `i`
  and `j`, stopping at the first mismatch or either suffix's end.
- Every result satisfies `0 <= result <= n - max(i,j)`; self-queries return `n-i`.
  Other answers are subject to the collision limitation below.
- Empty input can be constructed but has no valid queries.
- Invalid positions or missing fingerprint metadata cause `std::invalid_argument`.


## Correctness

Assuming correct construction and hash arithmetic, queries with a **true LCE
below eight bytes** are exact. Longer queries can overestimate because unequal
substrings can have equal hashes. The base and modulus are fixed; **no numerical
collision probability is claimed**.

Proof of the short-LCE guarantee: the search compares lengths `1, 2, 4, 8, ...`.
Values of up to four ranks are below `2^32 < q`, so those comparisons are exact.
Reaching length eight means the first four ranks already match. Their contribution
cancels when subtracting the eight-rank values, leaving a difference of magnitude
below `2^32`, which cannot be a nonzero multiple of `q`. If the true LCE is below
eight, the search therefore cannot accept length eight; remaining refinement
comparisons have lengths at most four and are exact too.

## Storage and limits

- Platform: 8-bit bytes, 64-bit words and `size_t`; base 256; modulus `2^63 + 29`.
- Packed payload: `8 * ceil(n/8)` bytes. Final-word padding does not extend the
  logical text. Metadata, container capacity, and allocation overhead are extra.
- The wrapper uses `O(n)` additional memory overall, including temporary
  construction storage; the retained power table uses `O(log(n+2))` space.
- No fixed maximum supported length has been established. Lengths must fit
  `size_t` and container limits, and required allocations must succeed.
  `max_size()` is a container limit, not an available-RAM measurement.
  Allocation/container exceptions propagate to the caller.
- Construction's offset invariant has no explicit failure check; correctness for
  every representable input length has not been established.
