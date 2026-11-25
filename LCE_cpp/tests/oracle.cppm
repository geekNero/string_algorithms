module;
#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <string>

export module oracle;
using namespace std;

// expects 0 based indices.
export size_t oracle_lce(const string &text, size_t i, size_t j) {

  if (i >= text.size() || j >= text.size())
    throw invalid_argument("indexes out of range");

  size_t max_lce = min(text.size() - i, text.size() - j);

  size_t lce_length = 0;
  for (; lce_length < max_lce; lce_length++) {
    if (text[i + lce_length] != text[j + lce_length]) {
      return lce_length;
    }
  }

  return lce_length;
}
