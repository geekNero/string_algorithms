#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>

using namespace std;

#define test                                                                   \
  int T;                                                                       \
  cin >> T;                                                                    \
  while (T--)
#define int long long
#define pb push_back
#define bits(x) __builtin_popcount(x)
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
const int N = 1e5 + 7;

inline int gcd(int a, int b) {
  if (b == 0)
    return a;
  return gcd(b, a % b);
}

int max2(int x, int y) {
  if (x < y)
    return y;
  else
    return x;
}

int bs_sqrt(int x) {
  int left = 0, right = 2000000123;
  while (right > left) {
    int mid = (left + right) / 2;
    if (mid * mid > x)
      right = mid;
    else
      left = mid + 1;
  }
  return left - 1;
}

int modFact(int n, int p) {
  if (n >= p)
    return 0;
  int result = 1;
  for (int i = 1; i <= n; i++)
    result = (result * i) % p;
  return result;
}

void init_code() {
#ifndef ONLINE_JUDGE
  freopen("inputf.txt", "r", stdin);
  freopen("outputf.txt", "w", stdout);
#endif // ONLINE_JUDGE
}

int getClosest(int val1, int val2, int target) {
  if (target - val1 >= val2 - target)
    return val2;
  else
    return val1;
}

int findClosest(int arr[], int n, int target) {
  if (target <= arr[0])
    return arr[0];
  if (target >= arr[n - 1])
    return arr[n - 1];

  int i = 0, j = n, mid = 0;
  while (i < j) {
    mid = (i + j) / 2;
    if (arr[mid] == target)
      return arr[mid];
    if (target < arr[mid]) {
      if (mid > 0 && target > arr[mid - 1])
        return getClosest(arr[mid - 1], arr[mid], target);
      j = mid;
    } else {
      if (mid < n - 1 && target < arr[mid + 1])
        return getClosest(arr[mid], arr[mid + 1], target);
      i = mid + 1;
    }
  }
  return arr[mid];
}

bool isPowerOfTwo(int n) {
  if (n == 0)
    return false;
  return (ceil(log2(n)) == floor(log2(n)));
}

int fpow(int x, int y, int p = MOD) {
  x = x % p;
  int sum = 1;
  while (y) {
    if (y & 1)
      sum = sum * x;
    sum %= p;
    y = y >> 1;
    x = x * x;
    x %= p;
  }
  return sum;
}

int lcm(int a, int b) { return (a * b) / gcd(a, b); }

int add(int x, int y) { return ((x + y) % MOD + MOD) % MOD; }

int mul(int x, int y) { return x * 1ll * y % MOD; }

int binpow(int x, int y) {
  int z = 1;
  while (y) {
    if (y % 2 == 1)
      z = mul(z, x);
    x = mul(x, x);
    y /= 2;
  }
  return z;
}

int inv(int x) { return binpow(x, MOD - 2); }

int divide(int x, int y) { return mul(x, inv(y)); }

int mod_sub(int a, int b, int m) {
  a = a % m;
  b = b % m;
  return (((a - b) % m) + m) % m;
}

int nCrModPDp(int n, int r, int p) {
  int C[r + 1];
  memset(C, 0, sizeof(C));
  C[0] = 1;
  for (int i = 1; i <= n; i++) {
    for (int j = min(i, r); j > 0; j--) {
      C[j] = (C[j] + C[j - 1]) % p;
    }
  }
  return C[r];
}

vector<int> primes(N + 1, 1);

void sieve_of_eratothenes() {
  primes[0] = 0;
  primes[1] = 0;
  for (int i = 2; i <= sqrt(N); i++) {
    if (primes[i] == 1) {
      for (int j = 2; i * j <= N; j++) {
        primes[i * j] = 0;
      }
    }
  }
}

void binary_exponentiation(int base, int power) {
  long long ans = 1;
  while (power > 0) {
    if ((power % 2) == 0) {
      base = base * base;
      power /= 2;
    } else {
      ans *= base;
      power -= 1;
    }
  }
  cout << ans << endl;
}

int factorial(int n) {
  int factorial = 1;
  for (int i = 2; i <= n; i++)
    factorial = factorial * i;
  return factorial;
}

int nCr(int n, int r) {
  int C[r + 1];
  memset(C, 0, sizeof(C));
  C[0] = 1;
  for (int i = 1; i <= n; i++) {
    for (int j = min(i, r); j > 0; j--) {
      C[j] = (C[j] + C[j - 1]);
    }
  }
  return C[r];
}

void prime_factorization(int n) {
  for (int i = 2; i <= sqrt(n); i++) {
    if ((n % i) == 0) {
      int cnt = 0;
      while ((n % i) == 0) {
        n = n / i;
        cnt++;
      }
      cout << i << " raised to " << cnt;
    }
  }
  if ((n != 1)) {
    cout << n << " " << 1 << endl;
  }
}

vector<int> Divisors(int n) {
  vi divisors;
  for (int i = 1; i <= sqrt(n); i++) {
    if (n % i == 0) {
      if (n / i == i)
        divisors.pb(i);
      else {
        divisors.pb(i);
        divisors.pb(n / i);
      }
    }
  }
  return divisors;
}

int countDivisors(int n) {
  int ans = 0;
  for (int i = 1; i <= sqrt(n); i++) {
    if (n % i == 0) {
      if (n / i == i)
        ans++;
      else
        ans += 2;
    }
  }
  return ans;
}

int min2(int a, int b) { return a < b ? a : b; }

int adjacentElements(vi &arr, int element) {
  int top = arr.size() - 1;
  int bottom = 0;
  int mid = (top + bottom) / 2;
  while (top > bottom) {
    if (element > arr[mid])
      bottom = mid + 1;
    else
      top = mid - 1;
    mid = (top + bottom) / 2;
  }
  return mid;
}

void returnParents(vector<vi> &tree, int parent[], int cur, int par) {
  rep(i, 0, tree[cur].size()) {
    if (tree[cur][i] != par) {
      parent[tree[cur][i]] = cur;
      returnParents(tree, parent, tree[cur][i], cur);
    }
  }
}

int diagonal(int k, const string &s1, const string &s2) {
  int ans = 0;
  // upper half
  int n = s1.size();
  int m = s2.size();

  for (int i = 0; i < n; i++) {
    int l = 0;
    int r = -1;
    int mismatches = 0;

    // cout<< ((r+i)< n && r< m)<<"\n";
    while ((r + i) < n && r < m) {

      while (mismatches <= k && r + i + 1 < n && r + 1 < m) {
        r++;
        if (s1[r + i] != s2[r]) {
          mismatches++;
        }
        if (mismatches > k) {
          mismatches--;
          r--;
          break;
        }
      }
      if (mismatches <= k) {
        ans = max(ans, r - l + 1);
      }

      r++;
      if (!(r + i < n && r < m)) {
        break;
      }
      if (s1[i + r] != s2[r]) {
        mismatches++;
      }
      if (s1[i + l] != s2[l]) {
        mismatches--;
      }
      l++;
    }
  }
  return ans;
}

void compute() {
  int k;
  string s1, s2;
  cin >> k;
  cin >> s1 >> s2;

  cout << max(diagonal(k, s1, s2), diagonal(k, s2, s1));
}

signed main() {
  ios_base::sync_with_stdio(false);
  cin.tie(0);
  cout.tie(0);
  compute();
  return 0;
}
