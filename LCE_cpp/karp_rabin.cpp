#include <algorithm>
#include <climits>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
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

const int MOD = 1e9 + 7;

uint binary_exponentiation(int x, int n) {
  uint64_t base = x % MOD; // Assumes x >= 0.
  uint64_t result = 1;

  while (n > 0) {
    if (n & 1) {
      result *= base;
    }
    base *= base;
    base %= MOD;
    result %= MOD;
    n >>= 1;
  }
  return result;
}

// This is an attempt at implementing the version of karp-rabin mentioned in the
// Practical Performance of Space Efficient Data Structures for Longest Common
// Extensions
vector<uint> karp_rabin(string &text, int sigma, unordered_map<char, int> &rank,
                        uint &mx_rank) {

  // Since we don't have a way of determining the size of the grammar as of now,
  // we'll take is as an input for now.
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
    current_hash %= MOD;

    prefix_hash[i] = current_hash;
  }

  return prefix_hash;
}

uint query_hash(vector<uint> &prefix_hash, int sigma, int y, int x) {
  if (x <= 0) {
    return prefix_hash[y];
  }
  // The order of operations was done this way to avoid any uint32 wraparounds.
  uint64_t removed = uint64_t(prefix_hash[x - 1]) *
                     binary_exponentiation(sigma, y - x + 1) % MOD;
  return (uint64_t(prefix_hash[y]) + MOD - removed) % MOD;
}

void compute() {

  int base = 128;

  string s = "lalalalala";
  unordered_map<char, int> ranks;
  uint mx_rank = 0;
  auto pref_hash = karp_rabin(s, base, ranks, mx_rank);

  cout << query_hash(pref_hash, base, 3, 0) << "\n";
  cout << query_hash(pref_hash, base, 5, 2) << "\n";
  cout << query_hash(pref_hash, base, 7, 4) << "\n";
}

signed main() {
  ios_base::sync_with_stdio(false);
  cin.tie(0);
  cout.tie(0);
  compute();
  return 0;
}
