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

struct TestFunctionWithName {
  TestFunction function;
  std::string  name;
};

std::vector<TestFunctionWithName> testFunctions{};

#define TEST_CASE(testName)                                                \
  NEVER_INLINE void testName();                                            \
  struct testName##Struct {                                                \
    testName##Struct()                                                     \
    {                                                                      \
      testFunctions.push_back(TestFunctionWithName{&testName, #testName}); \
    }                                                                      \
  } testName##StructInstance{};                                            \
  NEVER_INLINE void testName()

NEVER_INLINE List<int> makeTestList()
{
  List<int> list{};

  for (int i{0}; i < 10; ++i) { list.push_back(i); }

  return list;
}

using namespace std::string_literals;

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

TEST_CASE(shouldBeAbleToCreateListFromInitializerList)
{
  const List<int> l{1, 2, 3};
  ASSERT_EQ(3, l.size());
  ASSERT_EQ(1, l[0]);
  ASSERT_EQ(2, l[1]);
  ASSERT_EQ(3, l[2]);
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

TEST_CASE(shouldNotConsiderANonEmptyListEmpty)
{
  const List<int> l{makeTestList()};
  ASSERT_EQ(false, l.empty());
}

TEST_CASE(shouldBeAbleToGetFirstElement)
{
  const List<int> l{makeTestList()};
  ASSERT_EQ(0, l.front());
}

TEST_CASE(shouldThrowWhenFrontIsCalledOnEmptyList)
{
  const List<int> emptyList{};

  try {
    emptyList.front();
    ASSERT_EQ(true, false);
  }
  catch (const std::out_of_range& ex) {
    ASSERT_EQ("List::front called on empty list."s, ex.what());
  }
}

TEST_CASE(shouldBeAbleToGetLastElement)
{
  const List<int> l{makeTestList()};
  ASSERT_EQ(9, l.back());
}

TEST_CASE(shouldThrowWhenFetchingTheLastElementFromAnEmptyList)
{
  const List<int> emptyList{};

  try {
    emptyList.back();
    ASSERT_EQ(true, false);
  }
  catch (const std::out_of_range& ex) {
    ASSERT_EQ("List::back called on empty list."s, ex.what());
  }
}

TEST_CASE(shouldBeAbleToAccessElementsByIndex)
{
  const List<int> l{makeTestList()};

  for (std::size_t i{0}; i < l.size(); ++i) { ASSERT_EQ(i, l[i]); }
}

TEST_CASE(shouldThrowAnExceptionWhenIndexIsOutOfBounds)
{
  const List<int> l{makeTestList()};

  try {
    l[-1];
    ASSERT_EQ(true, false);
  }
  catch (const std::out_of_range& ex) {
    ASSERT_EQ(
      "List::operator[]: index out of bounds: " + std::to_string(SIZE_MAX)
        + " is >= size() (10)!",
      ex.what());
  }

  try {
    l[10];
    ASSERT_EQ(true, false);
  }
  catch (const std::out_of_range& ex) {
    ASSERT_EQ(
      "List::operator[]: index out of bounds: 10 is >= size() (10)!"s,
      ex.what());
  }
}

TEST_CASE(shouldBeAbleToSort)
{
  List<int> l{};
  l.push_back(5);
  l.push_back(1);
  l.push_back(-5);
  l.push_front(15);
  l.push_front(9);

  l.sort();

  const List<int> expected{-5, 1, 5, 9, 15};
  ASSERT_EQ(expected, l);
}

TEST_CASE(shouldBeAbleToSortInDescendingOrder)
{
  List<int>       l{1, 2, 3, 7, 9, 1, 4, 8, 1, 2, -1};
  const List<int> expected{9, 8, 7, 4, 3, 2, 2, 1, 1, 1, -1};
  l.sort(std::greater<int>{});
  ASSERT_EQ(expected, l);
}

int main()
{
  try {
    std::vector<std::string> successfulFunctions{};

    for (const auto& [func, name] : testFunctions) {
      func();
      successfulFunctions.push_back(name);
    }

    std::cout << ">>>>> TEST SUCCESS\n";

    for (const std::string& functionName : successfulFunctions) {
      std::cout << "Test case \"" << functionName << "\": SUCCESS.\n";
    }

    std::cout << ">>>> ALL TESTS RAN SUCCESSFULLY\n";
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

  std::cout << "\n\n     MEMORY LEAK CHECK     \n";

  if (leaks.empty()) { std::cout << "No memory leaks found\n"; }
  else {
    std::cerr << leaks.size() << " memory leaks found.\n";

    for (void* addr : leaks) { std::cerr << addr << '\n'; }
  }
}
