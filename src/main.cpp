#include <iostream>

#include "list.hpp"

int main()
{
  List<int> list{};

  for (int i{0}; i < 10; ++i) { list.push_front(i); }

  for (int i : list) { std::cout << i << ' '; }

  std::cout << '\n';
}
