#include <algorithm>
#include <iostream>
#include <unordered_set>
#include <vector>

#include "list.hpp"

std::unordered_set<void*> newed{};
std::unordered_set<void*> deleted{};

#ifdef _MSC_VER
#define NEVER_INLINE __declspec(noinline)
#else
#define NEVER_INLINE __attribute__((noinline))
#endif

NEVER_INLINE void testFunction()
{
  List<int> list{};

  for (int i{0}; i < 10; ++i) { list.push_front(i); }

  for (int i : list) { std::cout << i << ' '; }

  std::cout << '\n';
}

int main()
{
  testFunction();

  std::vector<void*> leaks{};

  for (void* p : newed) {
    auto it{deleted.find(p)};

    if (it == deleted.end()) { leaks.push_back(p); }
  }

  std::sort(leaks.begin(), leaks.end());

  if (leaks.empty()) { std::cout << "No memory leaks found\n"; }
  else {
    std::cerr << leaks.size() << " memory leaks found.\n";

    for (void* addr : leaks) { std::cerr << addr << '\n'; }
  }
}
