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
  // Explicit bytes preserve the embedded nulls; this need not be valid UTF-8.
  const string s = {'A',        char(0x00), 'B',        char(0xff), 'A',
                    char(0x00), 'B',        char(0xff), 'X'};
  INFO("seed=handwritten n=" << s.size() << " input_hex=" << to_hex(s));
  auto packing = inplace_fingerprint_wrapper(s);
  REQUIRE(packing.character_count == 9);
  REQUIRE(oracle_lce(s, 0, 4) == 4);
  REQUIRE(lce(packing, 0, 4) == 4);
  size_t queries_tested = 0;
  for (size_t i = 0; i < s.size(); ++i) {
    for (size_t j = 0; j < s.size(); ++j) {
      const auto expected = oracle_lce(s, i, j);
      const auto actual = lce(packing, i, j);
      INFO("i=" << i << " j=" << j << " expected=" << expected
                << " actual=" << actual);
      REQUIRE_EQ(expected, actual);
      ++queries_tested;
    }
  }
  CHECK_THROWS_AS(lce(packing, s.size(), 0), invalid_argument);
  CHECK_THROWS_AS(lce(packing, 0, s.size()), invalid_argument);
  cout << "Binary phase: strings=1 all_pairs_queries=" << queries_tested
       << " handwritten_queries=1\n";
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
