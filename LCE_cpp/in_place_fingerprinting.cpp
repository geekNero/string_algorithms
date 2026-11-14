#include <algorithm>
#include <climits>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <optional>
#include <random>
#include <stdexcept>
#include <unordered_map>
#include <utility>
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

uint64_t subtract_mod(uint64_t a, uint64_t b) {
  return a >= b ? a - b : MOD64 - (b - a);
}

struct FingerprintMetadata {

  int tau;
  u_int64_t offset;
  u_int64_t multiplier;
};

struct WordRAMCompressed {
  int base = 256;
  int bits_per_char = 0;
  size_t character_count = 0;
  vector<u_int64_t> words;
  unordered_map<char, int> mapping;
  vector<char> rank;

  optional<FingerprintMetadata> fingerprint;
};

u_int64_t binary_exponentiation(u_int64_t x, size_t n) {
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

  for (size_t i = 0; i < blocks.size(); i++) {

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
void inplace_fingerprint(WordRAMCompressed &packing) {

  if (packing.fingerprint.has_value())
    throw invalid_argument("already fingerprinted");

  if (packing.base != 256)
    throw invalid_argument("only base 256 is currently supported");

  const int bits_per_char = compute_bits_per_char(packing.base); // 8
  const int tau = 64 / bits_per_char;                            // 8

  u_int64_t prefix_hash = 0;
  const u_int64_t multiplier = binary_exponentiation(packing.base, tau);
  // const u_int64_t offset =
  //     deterministicaly_find_fingerprint_offset(blocks, multiplier);
  const u_int64_t offset =
      nondeterminstically_find_fingerprint_offset(packing.words, multiplier);

  for (size_t i = 0; i < packing.words.size(); i++) {
    bool quotient_bit = packing.words[i] >= MOD64;

    prefix_hash = (static_cast<unsigned __int128>(prefix_hash) * multiplier +
                   packing.words[i]) %
                  MOD64;

    packing.words[i] =
        (static_cast<unsigned __int128>(prefix_hash) + offset) % MOD64;
    if (quotient_bit) {
      packing.words[i] |= 1ULL << 63;
    }
  }

  packing.fingerprint = {tau, offset, multiplier};

  return;
}

uint64_t clean_fingerprint_block(uint64_t block, uint64_t offset) {
  const uint64_t MASK = (1ULL << 63) - 1;
  uint64_t shifted = block & MASK;
  return subtract_mod(shifted, offset);
}

u_int64_t read_fingerprint_word(const WordRAMCompressed &packing,
                                size_t block_index) {
  if (block_index >= packing.words.size())
    throw invalid_argument("block_index exceeds block count");

  const auto &metadata = packing.fingerprint.value();

  uint64_t quotient_bit = packing.words[block_index] >> 63;

  // Bi mod q = (Hi - H(i-1) * M) mod q

  auto Hi =
      clean_fingerprint_block(packing.words[block_index], metadata.offset);
  auto Hi_1 = block_index == 0
                  ? 0
                  : clean_fingerprint_block(packing.words[block_index - 1],
                                            metadata.offset);
  Hi_1 = static_cast<uint64_t>(
      (static_cast<unsigned __int128>(Hi_1) * metadata.multiplier) % MOD64);

  u_int64_t block = subtract_mod(Hi, Hi_1);

  // Bi = (Bi mod q) + Di.q
  block += quotient_bit * MOD64;

  return block;
}

/*
  1. Find in which block the prefix ends at.
  2. Extract the block it ends in, and exctract the required characters.
  3. Extend the hash of the blocks before it.
*/
// [begin, end)
u_int64_t query_prefix_hash(const WordRAMCompressed &packing, size_t length) {

  if (!packing.fingerprint.has_value())
    throw invalid_argument("words aren't fingerprinted");

  auto const &metadata = packing.fingerprint.value();

  const size_t prefix_blocks = length / metadata.tau;
  const int tail_count = length % metadata.tau;

  if (length > packing.character_count)
    throw invalid_argument("length exceeds string size");

  u_int64_t prefix_hash = 0;
  if (prefix_blocks)
    prefix_hash = clean_fingerprint_block(packing.words[prefix_blocks - 1],
                                          metadata.offset);

  if (tail_count) {
    //  |a b c| d e f g h = right shift value (8 * (8 - 3))
    u_int64_t tail = read_fingerprint_word(packing, prefix_blocks) >>
                     (packing.bits_per_char * (metadata.tau - tail_count));

    prefix_hash = (static_cast<unsigned __int128>(prefix_hash) *
                       binary_exponentiation(packing.base, tail_count) +
                   tail) %
                  MOD64;
  }

  return prefix_hash;
}

/*
  Query hash relies on prefix hashe function.
  compute prefix hash end.
  compute prefix hash beginning.
  final hash = (end - (beginning-1)*(sigma^(end-beginning +1)))%q
*/
// [begin, end)
u_int64_t query_hash(const WordRAMCompressed &packing, size_t begin,
                     size_t end) {

  if (begin > end || end > packing.character_count)
    throw invalid_argument("invalid start or end");

  auto end_hash = query_prefix_hash(packing, end);
  auto start_hash = query_prefix_hash(packing, begin);

  u_int64_t contribution = (static_cast<unsigned __int128>(start_hash) *
                            binary_exponentiation(packing.base, end - begin)) %
                           MOD64;

  return subtract_mod(end_hash, contribution);
}

void pack_string(const string &text, WordRAMCompressed &encoding) {

  const int bits_per_char = compute_bits_per_char(encoding.base);

  vector<uint64_t> packed_text;
  // the only way I can think of packing each word is by filing each int64
  // individually.
  uint64_t packet = 0;
  int packet_size = 64;
  for (auto ch : text) {
    const int use_bits = min(packet_size, bits_per_char);
    const uint64_t code = encoding.mapping.at(ch);
    if (code >= encoding.base)
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
  encoding.words = std::move(packed_text);
  encoding.bits_per_char = bits_per_char;
  encoding.character_count = text.size();
  encoding.fingerprint.reset();
}

string unpack_string(const WordRAMCompressed &packing) {
  if (packing.fingerprint.has_value())
    throw invalid_argument("packing must be unhashed");

  if (packing.character_count < 0)
    throw invalid_argument("character count must be nonnegative");

  string s = "";
  s.reserve(packing.character_count);

  // extracting back a string is a tad bit difficult.
  // The lower bits are accessible but it's the higher bits that we need.
  // To do so, for each character we consume, we will track the remaining
  // consumable bits left in the current block we are using and use left shift
  // and right shift operations to extract the code. We also need to replenish
  // the block we consume completely, and if we end early, some data was
  // missing.

  size_t block_index = 0;
  int start_bit = 64;
  size_t n = packing.character_count;

  while (n > 0) {
    if (block_index >= packing.words.size())
      throw invalid_argument("packed text is too short");

    int end_bit = max(start_bit - packing.bits_per_char, 0);
    const int remaining = packing.bits_per_char - (start_bit - end_bit);
    const int ls = 64 - start_bit;

    uint64_t value = (packing.words[block_index] << ls) >> (end_bit + ls);

    start_bit = end_bit;

    if (!start_bit) {
      block_index++;
      start_bit = 64;

      if (remaining) {
        if (block_index >= packing.words.size())
          throw invalid_argument("packed text is too short");

        end_bit = 64 - remaining;
        value <<= remaining;
        value += (packing.words[block_index]) >> end_bit;
        start_bit = end_bit;
      }
    }

    if (value >= packing.base || value >= packing.rank.size()) {
      throw invalid_argument("decoded rank is invalid");
    }

    s += packing.rank[value];
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

  string s = "allabbabssbaabssssbaab";
  unordered_map<char, int> mapping;
  vector<char> rank;
  for (char ch : s) {
    if (mapping.find(ch) == mapping.end()) {
      mapping[ch] = rank.size();
      rank.push_back(ch);
    }
  }

  auto word_ram_packing = WordRAMCompressed{};
  word_ram_packing.base = 256;
  word_ram_packing.mapping = mapping;
  word_ram_packing.rank = rank;

  pack_string(s, word_ram_packing);
  pVec(word_ram_packing.words);
  auto decoded = unpack_string(word_ram_packing);
  cout << decoded << " " << (s == decoded);
  inplace_fingerprint(word_ram_packing);
  pVec(word_ram_packing.words);

  cout << "string at 10:14 " << decoded.substr(10, 4)
       << " hash: " << query_hash(word_ram_packing, 10, 14) << endl;
  cout << "string at 18:22 " << decoded.substr(18, 4)
       << " hash: " << query_hash(word_ram_packing, 18, 22) << endl;

  cout << "string at 8:10 " << decoded.substr(8, 2)
       << " hash: " << query_hash(word_ram_packing, 8, 10) << endl;
  cout << "string at 15:17 " << decoded.substr(15, 2)
       << " hash: " << query_hash(word_ram_packing, 15, 17) << endl;
}

signed main() {
  ios_base::sync_with_stdio(false);
  cin.tie(0);
  cout.tie(0);
  // compute();
  compute2();

  return 0;
}
