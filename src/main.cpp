#include <cstdint>

#include <algorithm>
#include <iostream>
#include <iterator>
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
    oss << std::boolalpha;

    oss << "AssertionViolationException:\n"
        << '"' << expression << "\" failed!\n"
        << "Line     : " << line << '\n'
        << "Function : " << function << '\n'
        << "Expected : " << expected << '\n'
        << "Actual   : " << actual << "\n\n";

    return oss.str();
  }
};

template<typename Ty>
std::string toString(const Ty& any)
{
  std::ostringstream oss{};
  oss.imbue(std::locale::classic());
  oss << std::boolalpha;
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

using TestFunction = void (*)();
std::vector<TestFunction> testFunctions{};

#define TEST_CASE(testName)                                    \
  NEVER_INLINE void testName();                                \
  struct testName##Struct {                                    \
    testName##Struct() { testFunctions.push_back(&testName); } \
  } testName##StructInstance{};                                \
  NEVER_INLINE void testName()

NEVER_INLINE List<int> makeTestList()
{
  List<int> list{};

  for (int i{0}; i < 10; ++i) { list.push_back(i); }

  return list;
}

TEST_CASE(shouldBeAbleToConstructEmptyList)
{
  List<int> l{};
  ASSERT_EQ(true, l.empty());
  ASSERT_EQ(0, l.size());
}

TEST_CASE(shouldBeAbleToCopyConstructAList)
{
  const List<int> l1{makeTestList()};
  const List<int> l2{l1};

  ASSERT_EQ(l1.empty(), l2.empty());
  ASSERT_EQ(l1.size(), l2.size());

  for (std::size_t i{0}; i < l1.size(); ++i) { ASSERT_EQ(l1[i], l2[i]); }

  List<int>::const_iterator it1{l1.begin()};
  List<int>::const_iterator it2{l2.begin()};

  while (it1 != l1.end()) {
    ASSERT_EQ(*it1, *it2);
    ++it1;
    ++it2;
  }
}

TEST_CASE(shouldBeAbleToCopyAssign)
{
  const List<int> l1{makeTestList()};
  List<int>       l2{};
  l2.push_back(1);
  l2.push_back(2);
  l2.push_back(3);
  l2.push_front(0);

  l2 = l1;

  ASSERT_EQ(l1.empty(), l2.empty());
  ASSERT_EQ(l1.size(), l2.size());

  for (std::size_t i{0}; i < l1.size(); ++i) { ASSERT_EQ(l1[i], l2[i]); }

  List<int>::const_iterator it1{l1.begin()};
  List<int>::const_iterator it2{l2.begin()};

  while (it1 != l1.end()) {
    ASSERT_EQ(*it1, *it2);
    ++it1;
    ++it2;
  }
}

TEST_CASE(shouldBeAbleToQuerySize)
{
  List<int> l{makeTestList()};
  l.push_back(1);
  ASSERT_EQ(11, l.size());
  l.push_front(1);
  ASSERT_EQ(12, l.size());
  l.insert(std::next(l.begin(), 5), 1);
  ASSERT_EQ(13, l.size());
}

int main()
{
  try {
    for (TestFunction testFunction : testFunctions) { testFunction(); }

    std::cout << ">>>>> TEST SUCCESS\n";
  }
  catch (const AssertionViolationException& ex) {
    std::cerr << ex.what();

    std::cerr << "\n\n\n";
    std::cerr << ">>>>>>> TEST FAILURE!!!! <<<<\n";
  }

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
