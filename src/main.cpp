#include <cstdint>

#include <algorithm>
#include <iostream>
#include <locale>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

#include "list.hpp"

std::unordered_set<void*> newed{};
std::unordered_set<void*> deleted{};

#ifdef _MSC_VER
#define NEVER_INLINE __declspec(noinline)
#define FUNCTION __FUNCSIG__
#else
#define NEVER_INLINE __attribute__((noinline))
#define FUNCTION __PRETTY_FUNCTION__
#endif

struct AssertionViolationException : std::logic_error {
  AssertionViolationException(
    std::int64_t       line,
    const std::string& function,
    const std::string& expression,
    const std::string& expected,
    const std::string& actual)
    : std::logic_error{
      formatErrorMsg(line, function, expression, expected, actual)}
  {
  }

  static std::string formatErrorMsg(
    std::int64_t       line,
    const std::string& function,
    const std::string& expression,
    const std::string& expected,
    const std::string& actual)
  {
    std::ostringstream oss{};
    oss.imbue(std::locale::classic());

    oss << "AssertionViolationException:\n"
        << '"' << expression << "\" failed!\n"
        << "Line     : " << line << '\n'
        << "Function : " << function << '\n'
        << "Expected : " << expected << '\n'
        << "Actual   : " << actual << "\n\n";
  }
};

template<typename Ty>
std::string toString(const Ty& any)
{
  std::ostringstream oss{};
  oss.imbue(std::locale::classic());
  oss << any;
  return oss.str();
}

#define MACRO_BEGIN do {
#define MACRO_END \
  }               \
  while (false)

#define RAISE_EXCEPTION(expected, actual, operator)                            \
  throw AssertionViolationException                                            \
  {                                                                            \
    __LINE__, FUNCTION, #expected " " operator" " #actual, toString(expected), \
      toString(actual)                                                         \
  }

#define ASSERT_EQ(expected, actual)                                           \
  MACRO_BEGIN                                                                 \
  if (!((expected) == (actual))) { RAISE_EXCEPTION(expected, actual, "=="); } \
  MACRO_END

#define ASSERT_NE(expected, actual)                                           \
  MACRO_BEGIN                                                                 \
  if (!((expected) != (actual))) { RAISE_EXCEPTION(expected, actual, "!="); } \
  MACRO_END

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
