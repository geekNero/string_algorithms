#include <algorithm>
#include <climits>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <random>
#include <stdexcept>
#include <unordered_map>
#include <vector>

using namespace std;

#define test                                                                   \
  int T;                                                                       \
  cin >> T;                                                                    \
  while (T--)
#define pb push_back
#define all(x) (x).begin(), (x).end()
#define pii pair<int, int>
#define vi vector<int>
#define vb vector<bool>
#define vpii vector<pair<int, int>>
#define rep(i, a, b) for (int i = a; i < b; i++)
#define re(i, a, b) for (int i = a; i >= b; i--)
#define pVec(v)                                                                \
  for (auto e : v)                                                             \
    cout << e << " ";                                                          \
  cout << "\n"
#define aut(e, v) for (auto e : v)

const u_int64_t MOD64 = (1ULL << 63) + 29ULL;

u_int64_t binary_exponentiation(u_int64_t x, int n) {
  unsigned __int128 base = x % MOD64; // Assumes x >= 0.
  unsigned __int128 result = 1;

  while (n > 0) {
    if (n & 1) {
      result *= base;
    }
    base *= base;
    base %= MOD64;
    result %= MOD64;
    n >>= 1;
  }
  return result;
}

int compute_bits_per_char(uint base) {
  if (base < 2)
    throw std::invalid_argument("base must be at least 2");
  int bits = 32 - __builtin_clz(base - 1);
  if (bits >= 64)
    throw std::invalid_argument("base is too large");
  return bits;
}

u_int64_t
nondeterminstically_find_fingerprint_offset(vector<u_int64_t> &blocks,
                                            const u_int64_t multiplier) {
  u_int64_t prefix_hash = 0;
  u_int64_t offset = 0;
  u_int64_t MSB = 1ULL << 63;

  mt19937_64 rng(random_device{}());
  uniform_int_distribution<u_int64_t> distribution(0, MOD64 - 1);

  while (true) {
    prefix_hash = 0;
    bool valid = true;
    for (int i = 0; i < blocks.size(); i++) {

      prefix_hash = (static_cast<unsigned __int128>(prefix_hash) * multiplier +
                     blocks[i]) %
                    MOD64;
      // check if hash with offset has it's MSB set.
      u_int64_t prefix_hash_with_offset =
          (static_cast<unsigned __int128>(prefix_hash) + offset) % MOD64;

      if (prefix_hash_with_offset & MSB) {
        valid = false;
        break;
      }
    }
    if (valid)
      return offset;

    offset = distribution(rng);
  }
}
u_int64_t deterministicaly_find_fingerprint_offset(vector<u_int64_t> &blocks,
                                                   const u_int64_t multiplier) {
  u_int64_t prefix_hash = 0;
  vector<u_int64_t> distance_to_msb;

  u_int64_t offset = 0;
  u_int64_t MSB = 1ULL << 63;

  for (int i = 0; i < blocks.size(); i++) {

    prefix_hash =
        (static_cast<unsigned __int128>(prefix_hash) * multiplier + blocks[i]) %
        MOD64;

    if (prefix_hash & MSB) {
      offset = max(offset, MOD64 - prefix_hash);
    } else {
      distance_to_msb.push_back(MSB - prefix_hash);
    }
  }

  sort(distance_to_msb.begin(), distance_to_msb.end());

  auto it = upper_bound(distance_to_msb.begin(), distance_to_msb.end(), offset);
  if (it != distance_to_msb.begin()) {
    --it;

    for (; it != distance_to_msb.end(); ++it) {
      if (*it > offset) {
        break;
      }
      if (*it + 29 > offset) {
        offset = *it + 29;
      }
    }
  }

  return offset;
}
// An attempt at implementing Prezza's inplace fingerprinting. Depending on
// the base, we might not be able to evenly distribute characters in a 64 bit
// word. To deal with this, we can leave some spare bits and use a smaller
// prime based on the actual number of bits being used. For now, I'm sticking
// to a base of 256.
u_int64_t inplace_fingerprint(vector<u_int64_t> &blocks) {
  const int base = 256;
  const int bits_per_char = compute_bits_per_char(base); // 8
  const int tau = 64 / bits_per_char;                    // 8

  u_int64_t prefix_hash = 0;
  const u_int64_t multiplier = binary_exponentiation(base, tau);
  // const u_int64_t offset =
  //     deterministicaly_find_fingerprint_offset(blocks, multiplier);
  const u_int64_t offset =
      nondeterminstically_find_fingerprint_offset(blocks, multiplier);

  for (int i = 0; i < blocks.size(); i++) {
    bool quotient_bit = blocks[i] >= MOD64;

    prefix_hash =
        (static_cast<unsigned __int128>(prefix_hash) * multiplier + blocks[i]) %
        MOD64;

    blocks[i] = (static_cast<unsigned __int128>(prefix_hash) + offset) % MOD64;
    if (quotient_bit) {
      blocks[i] |= 1ULL << 63;
    }
  }

  return offset;
}

// This is an attempt at implementing the version of karp-rabin mentioned in
// the Practical Performance of Space Efficient Data Structures for Longest
// Common Extensions
vector<uint> karp_rabin(string &text, int sigma, unordered_map<char, int> &rank,
                        uint &mx_rank) {

  //   // Since we don't have a way of determining the size of the grammar as
  //   of now,
  //   // we'll take is as an input for now.
  vector<uint> prefix_hash(text.size());

  // since we don't care for the lexiographical ordering in karp-rabin, we can
  // assign ranks to characters as we see them, with minimum rank being 1.

  uint64_t current_hash = 0;

  rep(i, 0, text.size()) {

    current_hash *= sigma;
    if (rank[text[i]] == 0) {
      rank[text[i]] = ++mx_rank;
    }
    current_hash += rank[text[i]];
    current_hash %= MOD64;

    prefix_hash[i] = current_hash;
  }

  return prefix_hash;
}

uint query_hash(vector<uint> &prefix_hash, int sigma, int y, int x) {
  if (x <= 0) {
    return prefix_hash[y];
  }
  // The order of operations was done this way to avoid any uint32
  // wraparounds.
  uint64_t removed = uint64_t(prefix_hash[x - 1]) *
                     binary_exponentiation(sigma, y - x + 1) % MOD64;
  return (uint64_t(prefix_hash[y]) + MOD64 - removed) % MOD64;
}

vector<u_int64_t> encode_string(string &text, int base,
                                unordered_map<char, int> &rank) {

  const int bits_per_char = compute_bits_per_char(base);

  vector<uint64_t> packed_text;
  // the only way I can think of packing each word is by filing each int64
  // individually.
  uint64_t packet = 0;
  int packet_size = 64;
  for (auto ch : text) {
    const int use_bits = min(packet_size, bits_per_char);
    const uint64_t code = rank.at(ch);
    if (code < 0 || code >= base)
      throw invalid_argument("rank outside alphabet");
    uint64_t value = code >> (bits_per_char - use_bits);
    packet <<= use_bits;
    packet += value;
    packet_size -= use_bits;
    if (packet_size == 0) {
      packed_text.push_back(packet);
      packet = 0;
      packet_size = 64;

      int const remaining_bits = bits_per_char - use_bits;
      if (remaining_bits) {

        uint64_t const mask = (1ULL << remaining_bits) - 1;
        value = code & mask;

        packet = value;
        packet_size -= remaining_bits;
      }
    }
  }
  if (packet_size != 64) {
    // shift the used bits to the left by padding it with trailing zeroes
    packed_text.push_back(packet << packet_size);
  }
  return packed_text;
}

string decode_string(const vector<u_int64_t> &packed_text, int base,
                     const vector<char> &rank, int n) {
  if (n < 0)
    throw invalid_argument("character count must be nonnegative");

  const int bits_per_char = compute_bits_per_char(base);

  string s = "";
  s.reserve(n);

  // extracting back a string is a tad bit difficult.
  // The lower bits are accessible but it's the higher bits that we need.
  // To do so, for each character we consume, we will track the remaining
  // consumable bits left in the current block we are using and use left shift
  // and right shift operations to extract the code. We also need to replenish
  // the block we consume completely, and if we end early, some data was
  // missing.

  int block_index = 0;
  int start_bit = 64;

  while (n > 0) {
    if (block_index >= packed_text.size())
      throw invalid_argument("packed text is too short");

    int end_bit = max(start_bit - bits_per_char, 0);
    const int remaining = bits_per_char - (start_bit - end_bit);
    const int ls = 64 - start_bit;

    uint64_t value = (packed_text[block_index] << ls) >> (end_bit + ls);

    start_bit = end_bit;

    if (!start_bit) {
      block_index++;
      start_bit = 64;

      if (remaining) {
        if (block_index >= packed_text.size())
          throw invalid_argument("packed text is too short");

        end_bit = 64 - remaining;
        value <<= remaining;
        value += (packed_text[block_index]) >> end_bit;
        start_bit = end_bit;
      }
    }

    if (value >= base || value >= rank.size()) {
      throw invalid_argument("decoded rank is invalid");
    }

    s += rank[value];
    n--;
  }

  return s;
}

// void compute() {

//   int base = 128;

//   string s = "lalalalala";
//   unordered_map<char, int> ranks;
//   uint mx_rank = 0;
//   auto pref_hash = karp_rabin(s, base, ranks, mx_rank);

//   cout << query_hash(pref_hash, base, 3, 0) << "\n";
//   cout << query_hash(pref_hash, base, 5, 2) << "\n";
//   cout << query_hash(pref_hash, base, 7, 4) << "\n";
// }

void compute2() {

  string s = "abbadabbabajabbaaakallaaa";
  unordered_map<char, int> rank;
  vector<char> code = {' '};
  int mx_rank = 0;
  rep(i, 0, s.size()) {
    if (rank[s[i]] == 0) {
      rank[s[i]] = ++mx_rank;
      code.push_back(s[i]);
    }
  }

  auto encoded = encode_string(s, 128, rank);
  pVec(encoded);
  // auto decoded = decode_string(encoded, 128, code, s.size());
  // cout << decoded << " " << (s == decoded);
  auto offset = inplace_fingerprint(encoded);
  pVec(encoded);
  cout << "offset: " << offset;
}

signed main() {
  ios_base::sync_with_stdio(false);
  cin.tie(0);
  cout.tie(0);
  // compute();
  compute2();

  return 0;
}
