// Keep headers out of clangd 21's preamble: merging that PCH with the
// imported modules otherwise produces spurious libc++ type mismatches.
#include <random>
static_assert(true);

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <cstddef>
#include <format>
#include <iostream>
#include <stdexcept>
#include <string>

import oracle;
import fingerprint;

using namespace std;

string to_hex(const string &s) {
  string out;
  out.reserve(s.size() * 3);
  for (unsigned char c : s) {
    out += format("{:02x} ", c);
  }

  return out;
}

TEST_CASE("In-place fingerprinting basic tests") {
  SUBCASE("empty string") {
    auto res = inplace_fingerprint_wrapper("");
    CHECK(res.character_count == 0);
    CHECK(res.fingerprint.has_value());
    CHECK_THROWS_AS(lce(res, 0, 0), invalid_argument);
  }
  SUBCASE("single character") {
    auto res = inplace_fingerprint_wrapper("a");
    INFO("Hash block at index 0: " << res.words[0] << "\n");
    CHECK(res.character_count == 1);
    REQUIRE(lce(res, 0, 0) == 1);
  }
  SUBCASE("all identical characters") {
    auto input = string(100, 'z');
    auto res = inplace_fingerprint_wrapper(input);
    CHECK_EQ(lce(res, 9, 19), oracle_lce(input, 9, 19));
  }
}

void check_oracle(const string &s, size_t i, size_t j, size_t expected) {
  INFO("seed=handwritten n=" << s.size() << " i=" << i << " j=" << j
                             << " input_hex=" << to_hex(s));
  const auto actual = oracle_lce(s, i, j);
  INFO("expected=" << expected << " actual=" << actual);
  CHECK_EQ(expected, actual);
}

TEST_CASE("Verify Oracle") {
  const string s = "banana";
  check_oracle(s, 1, 3, 3);
  check_oracle(s, 0, 2, 0);
  check_oracle(s, 3, 5, 1);
  check_oracle(s, 0, 0, 6);
  check_oracle(s, 1, 5, 1);
  check_oracle(s, 0, 1, 0);
  check_oracle("a", 0, 0, 1);
  check_oracle("aaaaa", 0, 2, 3);
}

TEST_CASE("Binary input including null and 0xff") {
  // proves strings are not handled based on null ending characters
  const string s = {'A',        char(0x00), 'B',        char(0xff), 'A',
                    char(0x00), 'B',        char(0xff), 'X'};
  INFO("seed=handwritten n=" << s.size() << " input_hex=" << to_hex(s));
  auto packing = inplace_fingerprint_wrapper(s);
  REQUIRE(packing.character_count == 9);
  struct BinaryQuery {
    size_t i;
    size_t j;
    size_t expected;
  };

  const BinaryQuery cases[] = {
      {0, 4, 4}, // match through 0x00 and 0xff, stop at 'A' vs 'X'
      {4, 0, 4}, // symmetry (i > j)
      {1, 5, 3}, // start directly on 0x00
      {3, 7, 1}, // start directly on 0xff (end of first word)
      {1, 3, 0}, // direct mismatch: 0x00 vs 0xff
      {1, 1, 8}, // self-query starting on 0x00 through end of string
  };

  size_t queries_tested = 0;
  for (const auto &q : cases) {

    const auto oracle = oracle_lce(s, q.i, q.j);
    const auto actual = lce(packing, q.i, q.j);
    INFO("i=" << q.i << " j=" << q.j << " expected=" << q.expected
              << " oracle=" << oracle << " actual=" << actual);
    // ensuring the oracle
    REQUIRE_EQ(oracle, q.expected);
    REQUIRE_EQ(actual, q.expected);
    ++queries_tested;
  }

  CHECK_THROWS_AS(lce(packing, s.size(), 0), invalid_argument);
  CHECK_THROWS_AS(lce(packing, 0, s.size()), invalid_argument);

  cout << "Binary phase: strings=1 targeted_queries=" << queries_tested << "\n";
}

TEST_CASE("Random Inputs") {
  size_t strings_tested = 0;
  size_t queries_tested = 0;
  int seed = 55;
  mt19937 rng(seed);
  string alphabet = "!\"#$%&'()*+,-./"
                    "0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`"
                    "abcdefghijklmnopqrstuvwxyz{|}~";
  for (int test = 0; test < 1000; test++) {
    int len = 1 + (rng() % 2000);
    string s = "";
    for (int i = 0; i < len; i++) {
      s += alphabet[rng() % alphabet.size()];
    }

    auto inplace = inplace_fingerprint_wrapper(s);
    ++strings_tested;
    INFO("Seed: " << seed << " expected length= " << len << " actual length= "
                  << inplace.character_count << "\nstring: " << to_hex(s));
    REQUIRE(inplace.character_count == len);

    for (int query = 0; query < 100; query++) {
      int i = rng() % len;
      int j = rng() % len;
      auto actual = lce(inplace, i, j);
      auto expected = oracle_lce(s, i, j);
      INFO("i=" << i << " j=" << j << "\nexpected=" << expected
                << " actual=" << actual);
      REQUIRE_EQ(expected, actual);
      ++queries_tested;
    }
  }

  cout << "Random phase: seed=" << seed << " strings=" << strings_tested
       << " queries=" << queries_tested << "\n";
}

TEST_CASE("Binary Alphabet - extensive queries") {
  unsigned int limit = (1 << 9);
  size_t queries_tested = 0;
  size_t strings_tested = 0;
  for (unsigned int test = 0; test < limit; test++) {
    string s = format("{:0{}b}", test, 9);
    INFO("seed=handwritten n=" << s.size() << " input_hex=" << to_hex(s));
    auto packing = inplace_fingerprint_wrapper(s);
    REQUIRE(packing.character_count == 9);
    strings_tested++;
    for (size_t i = 0; i < s.size(); i++) {
      for (size_t j = i; j < s.size(); j++) {
        const auto oracle = oracle_lce(s, i, j);
        const auto actual = lce(packing, i, j);
        INFO("i=" << i << " j=" << j << " expected=" << oracle
                  << " actual=" << actual);
        // ensuring the oracle
        REQUIRE_EQ(actual, oracle);
        ++queries_tested;
      }
    }
  }
  cout << "Binary Alphabet: strings=" << strings_tested
       << " targeted_queries=" << queries_tested << "\n";
}

TEST_CASE("Longer matching LCE") {
  string s = "";
  int lengths[] = {1,  2,  7,  8,  9,  15,  16,  17, 31,
                   32, 33, 63, 64, 65, 127, 128, 129};

  char cur = 'a';
  struct query {
    int i;
    int j;
  };

  vector<query> queries;

  for (int i : lengths) {
    int l = s.size();
    s += string(i, cur) + 'X';
    int r = s.size();
    s += string(i, cur) + 'Y';
    queries.push_back({l, r});
    cur++;
  }
  INFO("seed=handwritten n=" << s.size() << " input_hex=" << to_hex(s));

  auto packing = inplace_fingerprint_wrapper(s);

  CHECK(packing.character_count == s.size());
  for (const auto &q : queries) {
    auto actual = lce(packing, q.i, q.j);
    auto expected = oracle_lce(s, q.i, q.j);

    INFO("i=" << q.i << " j=" << q.j << " expected=" << expected
              << " actual=" << actual << "\n");
    REQUIRE_EQ(actual, expected);
  }

  cout << "Longer matching LCE: strings=1"
       << " targeted_queries=" << queries.size() << "\n";
}
