#include <cstdint>
#include <cstdlib>

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
#define FUNCTION __FUNCSIG__
#else
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

#define TEST(testName)                                                     \
  void testName();                                                         \
  struct testName##Struct {                                                \
    testName##Struct()                                                     \
    {                                                                      \
      testFunctions.push_back(TestFunctionWithName{&testName, #testName}); \
    }                                                                      \
  } testName##StructInstance{};                                            \
  void testName()

List<int> makeTestList()
{
  List<int> list{};

  for (int i{0}; i < 10; ++i) { list.push_back(i); }

  return list;
}

using namespace std::string_literals;

TEST(shouldBeAbleToConstructEmptyList)
{
  List<int> l{};
  ASSERT_EQ(true, l.empty());
  ASSERT_EQ(0, l.size());
}

TEST(shouldBeAbleToCopyConstructAList)
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

TEST(shouldBeAbleToCreateListFromInitializerList)
{
  const List<int> l{1, 2, 3};
  ASSERT_EQ(3, l.size());
  ASSERT_EQ(1, l[0]);
  ASSERT_EQ(2, l[1]);
  ASSERT_EQ(3, l[2]);
}

TEST(shouldBeAbleToCopyAssign)
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

TEST(shouldBeAbleToQuerySize)
{
  List<int> l{makeTestList()};
  l.push_back(1);
  ASSERT_EQ(11, l.size());
  l.push_front(1);
  ASSERT_EQ(12, l.size());
  l.insert(std::next(l.begin(), 5), 1);
  ASSERT_EQ(13, l.size());
}

TEST(shouldNotConsiderANonEmptyListEmpty)
{
  const List<int> l{makeTestList()};
  ASSERT_EQ(false, l.empty());
}

TEST(shouldBeAbleToGetFirstElement)
{
  const List<int> l{makeTestList()};
  ASSERT_EQ(0, l.front());
}

TEST(shouldThrowWhenFrontIsCalledOnEmptyList)
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

TEST(shouldBeAbleToGetLastElement)
{
  const List<int> l{makeTestList()};
  ASSERT_EQ(9, l.back());
}

TEST(shouldThrowWhenFetchingTheLastElementFromAnEmptyList)
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

TEST(shouldBeAbleToAccessElementsByIndex)
{
  const List<int> l{makeTestList()};

  for (std::size_t i{0}; i < l.size(); ++i) { ASSERT_EQ(i, l[i]); }
}

TEST(shouldThrowAnExceptionWhenIndexIsOutOfBounds)
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

TEST(shouldBeAbleToSort)
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

TEST(shouldBeAbleToSortInDescendingOrder)
{
  List<int>       l{1, 2, 3, 7, 9, 1, 4, 8, 1, 2, -1};
  const List<int> expected{9, 8, 7, 4, 3, 2, 2, 1, 1, 1, -1};
  l.sort(std::greater<int>{});
  ASSERT_EQ(expected, l);
}

TEST(shouldBeAbleToAddElementsToTheBack)
{
  List<int> l{};

  for (int i{0}; i < 20; ++i) {
    ASSERT_EQ(i, l.size());
    l.push_back(i);
    ASSERT_EQ(i + 1, l.size());
    ASSERT_EQ(i, l.back());
  }

  List<int>::const_iterator it{l.begin()};

  for (int i{0}; i < 20; ++i, ++it) { ASSERT_EQ(i, *it); }
}

TEST(shouldBeAbleToAddElementsToTheFront)
{
  List<int> l{};

  for (int i{0}; i < 20; ++i) {
    ASSERT_EQ(i, l.size());
    l.push_front(i);
    ASSERT_EQ(i + 1, l.size());
    ASSERT_EQ(i, l.front());
  }

  List<int>::const_iterator it{l.begin()};

  for (int i{0}; i < 20; ++i, ++it) { ASSERT_EQ(19 - i, *it); }
}

TEST(shouldBeAbleToRemoveElementsFromTheBack)
{
  List<int> l{makeTestList()};
  l.pop_back();
  ASSERT_EQ(9, l.size());
  l.pop_back();
  ASSERT_EQ(8, l.size());
  ASSERT_EQ((List<int>{0, 1, 2, 3, 4, 5, 6, 7}), l);

  List<int> l2{1};
  ASSERT_EQ(1, l2.size());
  l2.pop_back();
  ASSERT_EQ(true, l2.empty());
  l2.pop_back();
  ASSERT_EQ(true, l2.empty());
}

TEST(shouldBeAbleToRemoveElementsFromTheFront)
{
  List<int> l{makeTestList()};
  l.pop_front();
  ASSERT_EQ(9, l.size());
  l.pop_front();
  ASSERT_EQ(8, l.size());
  ASSERT_EQ((List<int>{2, 3, 4, 5, 6, 7, 8, 9}), l);

  List<int> l2{1};
  ASSERT_EQ(1, l2.size());
  l2.pop_front();
  ASSERT_EQ(true, l2.empty());
  l2.pop_front();
  ASSERT_EQ(true, l2.empty());
}

TEST(shouldBeAbleToInsertAtTheFront)
{
  List<int>  l{makeTestList()};
  const auto it{l.insert(l.begin(), 99)};
  ASSERT_EQ(99, *it);
  ASSERT_EQ(11, l.size());
  ASSERT_EQ((List<int>{99, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9}), l);
}

TEST(shouldBeAbleToInsertAtTheFrontUsingAnEmptyList)
{
  List<int>  l{};
  const auto it{l.insert(l.begin(), 5)};
  ASSERT_EQ(5, *it);
  ASSERT_EQ(1, l.size());
  ASSERT_EQ(5, l[0]);
}

TEST(shouldBeAbleToInsertInTheMiddle)
{
  List<int>  l{makeTestList()};
  const auto it{l.insert(std::next(l.begin(), 3), 999)};
  ASSERT_EQ(999, *it);
  ASSERT_EQ(11, l.size());
  ASSERT_EQ((List<int>{0, 1, 2, 999, 3, 4, 5, 6, 7, 8, 9}), l);
}

TEST(shouldBeAbleToInsertAtTheEnd)
{
  List<int>  l{makeTestList()};
  const auto it{l.insert(l.end(), 123)};
  ASSERT_EQ(123, *it);
  ASSERT_EQ(11, l.size());
  ASSERT_EQ((List<int>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 123}), l);
}

TEST(shouldBeAbleToInsertAtTheEndUsingAnEmptyList)
{
  List<int>  l{};
  const auto it{l.insert(l.end(), 1)};
  ASSERT_EQ(1, *it);
  ASSERT_EQ(1, l.size());
  ASSERT_EQ(1, l.front());
}

TEST(shouldBeAbleToEraseAtTheFront)
{
  List<int>  l{makeTestList()};
  const auto it{l.erase(l.begin())};
  ASSERT_EQ(1, *it);
  ASSERT_EQ(9, l.size());
  ASSERT_EQ(1, l.front());
  ASSERT_EQ((List<int>{1, 2, 3, 4, 5, 6, 7, 8, 9}), l);
}

TEST(shouldBeAbleToEraseAtTheEnd)
{
  List<int>  l{makeTestList()};
  const auto it{l.erase(std::prev(l.end()))};
  ASSERT_EQ(l.end(), it);
  ASSERT_EQ(9, l.size());
  ASSERT_EQ(8, l.back());
  ASSERT_EQ((List<int>{0, 1, 2, 3, 4, 5, 6, 7, 8}), l);
}

TEST(shouldBeAbleToEraseInTheMiddle)
{
  List<int>  l{makeTestList()};
  const auto it{l.erase(std::next(l.begin(), 5))};
  ASSERT_EQ(6, *it);
  ASSERT_EQ(9, l.size());
  ASSERT_EQ(6, l[5]);
  ASSERT_EQ((List<int>{0, 1, 2, 3, 4, 6, 7, 8, 9}), l);
}

TEST(shouldBeAbleToRemoveElements)
{
  List<int> l{1, 2, 3, 2, 3, 4, 5, 6, 2, 7, 8, 9, 2, 1, 2};
  l.remove(2);
  ASSERT_EQ(10, l.size());
  ASSERT_EQ((List<int>{1, 3, 3, 4, 5, 6, 7, 8, 9, 1}), l);
}

TEST(shouldBeAbleToRemoveElementsByPredicate)
{
  List<std::string> l{
    "test",
    "text",
    "long string",
    "hi",
    "lorem ipsum",
    "more text here",
    "testing",
    "abc"};
  l.remove_if([](const std::string& s) { return s.size() > 4; });
  ASSERT_EQ(4, l.size());
  ASSERT_EQ((List<std::string>{"test", "text", "hi", "abc"}), l);
}

TEST(shouldBeAbleToGrowUsingResize)
{
  List<int> l{makeTestList()};
  l.resize(12);
  ASSERT_EQ(12, l.size());
  ASSERT_EQ((List<int>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0}), l);
}

TEST(shouldBeAbleToGrowFromAnEmptyListUsingResize)
{
  List<int> l{};
  l.resize(3);
  ASSERT_EQ(3, l.size());
  const int a[]{0, 0, 0};
  ASSERT_EQ(true, std::equal(l.begin(), l.end(), std::begin(a), std::end(a)));
}

TEST(shouldDoNothingWhenResizingToTheSameSize)
{
  List<int>       l{makeTestList()};
  const List<int> copy{l};
  l.resize(10);
  ASSERT_EQ(10, l.size());
  ASSERT_EQ(10, copy.size());
  ASSERT_EQ(copy, l);
}

TEST(shouldBeAbleShrinkUsingResize)
{
  List<int> l{makeTestList()};
  l.resize(4);
  ASSERT_EQ(4, l.size());
  ASSERT_EQ(0, l[0]);
  ASSERT_EQ(1, l[1]);
  ASSERT_EQ(2, l[2]);
  ASSERT_EQ(3, l[3]);
}

TEST(shouldBeAbleToShrinkToTheEmptyListUsingResize)
{
  List<int> l{makeTestList()};
  l.resize(0);
  ASSERT_EQ(true, l.empty());
  ASSERT_EQ(l.begin(), l.end());
  ASSERT_EQ(List<int>{}, l);
}

TEST(shouldBeAbleToClear)
{
  List<int> l{makeTestList()};
  l.clear();
  ASSERT_EQ(true, l.empty());
  ASSERT_EQ(l.begin(), l.end());
  ASSERT_EQ(List<int>{}, l);
}

TEST(shouldDoNothingWhenClearingAnEmptyList)
{
  List<int> l{};
  l.clear();
  ASSERT_EQ(true, l.empty());
  ASSERT_EQ(l.begin(), l.end());
  ASSERT_EQ(List<int>{}, l);
}

TEST(shouldBeAbleToSwapLists)
{
  List<int> l1{1, 2, 3, 4};
  List<int> l2{makeTestList()};
  swap(l1, l2);

  ASSERT_EQ(10, l1.size());
  ASSERT_EQ(4, l2.size());

  ASSERT_EQ((List<int>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}), l1);
  ASSERT_EQ((List<int>{1, 2, 3, 4}), l2);
}

TEST(shouldBeAbleToIterate)
{
  const List<int> l{makeTestList()};

  int                       counter{0};
  List<int>::const_iterator it{l.begin()};

  while (it != l.end()) {
    ASSERT_EQ(counter, *it);
    ++it;
    ++counter;
  }
}

TEST(shouldBeAbleToIterateBackwards)
{
  const List<int> l{makeTestList()};

  int                               counter{9};
  List<int>::const_reverse_iterator it{l.rbegin()};

  while (it != l.rend()) {
    ASSERT_EQ(counter, *it);
    ++it;
    --counter;
  }
}

TEST(shouldBeAbleToCompareIterators)
{
  List<int> l{makeTestList()};

  ASSERT_EQ(l.begin(), std::prev(l.end(), 10));
  ASSERT_EQ(l.cbegin(), std::prev(l.cend(), 10));
  ASSERT_NE(l.begin(), l.end());
  ASSERT_NE(l.cbegin(), l.cend());
}

TEST(shouldBeAbleToPrintIterators)
{
  std::ostringstream oss1{};
  std::ostringstream oss2{};

  List<int> l{makeTestList()};

  oss1 << l.begin();
  oss2 << l.cbegin();

  const std::string str1{oss1.str()};
  const std::string str2{oss2.str()};

  const std::string expectedStart1{"List::iterator{"};
  const std::string expectedStart2{"List::const_iterator{"};

  ASSERT_EQ(true, str1.size() >= expectedStart1.size());
  ASSERT_EQ(true, str2.size() >= expectedStart2.size());

  const bool beginsWith1{std::equal(
    expectedStart1.begin(),
    expectedStart1.end(),
    str1.begin(),
    str1.begin() + expectedStart1.size())};
  const bool beginsWith2{std::equal(
    expectedStart2.begin(),
    expectedStart2.end(),
    str2.begin(),
    str2.begin() + expectedStart2.size())};

  ASSERT_EQ(true, beginsWith1);
  ASSERT_EQ(true, beginsWith2);
}

TEST(shouldBeAbleToPostfixIncrementIterators)
{
  List<int> l{makeTestList()};

  List<int>::iterator       it{l.begin()};
  List<int>::const_iterator cit{l.begin()};

  List<int>::iterator it2{it++};
  ASSERT_EQ(l.begin(), it2);
  ASSERT_EQ(std::next(l.begin()), it);

  List<int>::const_iterator cit2{cit++};
  ASSERT_EQ(l.cbegin(), cit2);
  ASSERT_EQ(std::next(l.cbegin()), cit);
}

TEST(shouldBeAbleToPrefixDecrementIterators)
{
  List<int> l{makeTestList()};

  List<int>::iterator       it{l.end()};
  List<int>::const_iterator cit{l.end()};

  List<int>::iterator       it2{--it};
  List<int>::const_iterator cit2{--cit};

  const List<int>::iterator       lastElemIt{std::prev(l.end())};
  const List<int>::const_iterator lastElemCit{std::prev(l.cend())};

  ASSERT_EQ(lastElemIt, it);
  ASSERT_EQ(lastElemIt, it2);

  ASSERT_EQ(lastElemCit, cit);
  ASSERT_EQ(lastElemCit, cit2);
}

TEST(shouldBeAbleToPostfixDecrementIterators)
{
  List<int> l{makeTestList()};

  List<int>::iterator       it{l.end()};
  List<int>::const_iterator cit{l.end()};

  List<int>::iterator       it2{it--};
  List<int>::const_iterator cit2{cit--};

  const List<int>::iterator       lastElemIt{std::prev(l.end())};
  const List<int>::const_iterator lastElemCit{std::prev(l.cend())};

  ASSERT_EQ(lastElemIt, it);
  ASSERT_EQ(l.end(), it2);

  ASSERT_EQ(lastElemCit, cit);
  ASSERT_EQ(l.cend(), cit2);
}

TEST(shouldBeAbleToPrintAList)
{
  const List<int>    l{makeTestList()};
  std::ostringstream oss{};
  oss << l;
  const std::string actual{oss.str()};
  ASSERT_EQ("List[0, 1, 2, 3, 4, 5, 6, 7, 8, 9]"s, actual);

  const List<int>    empty{};
  std::ostringstream oss2{};
  oss2 << empty;
  ASSERT_EQ("List[]", oss2.str());
}

TEST(shouldBeAbleToCompareListsForEquality)
{
  const List<int> l1{makeTestList()};
  const List<int> l2{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  const List<int> l3{1, 2, 3};
  const List<int> l4{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  ASSERT_EQ(l1, l1);
  ASSERT_EQ(l1, l2);
  ASSERT_NE(l1, l3);
  ASSERT_NE(l1, l4);

  ASSERT_EQ(l2, l1);
  ASSERT_EQ(l2, l2);
  ASSERT_NE(l2, l3);
  ASSERT_NE(l2, l4);

  ASSERT_NE(l3, l1);
  ASSERT_NE(l3, l2);
  ASSERT_EQ(l3, l3);
  ASSERT_NE(l3, l4);

  ASSERT_NE(l4, l1);
  ASSERT_NE(l4, l2);
  ASSERT_NE(l4, l3);
  ASSERT_EQ(l4, l4);
}

TEST(shouldBeAbleToCompareListsLexicographically)
{
  // a shorter list should come before a larger one
  {
    List<int> l1{1, 2, 3};
    List<int> l2{1, 2, 3, 4};
    ASSERT_EQ(true, l1 < l2);
  }

  // less than test
  {
    List<int> l1{1, 2, 3, 3};
    List<int> l2{1, 2, 3, 4};
    ASSERT_EQ(true, l1 < l2);
  }

  // a longer list should be greater than a shorter list
  {
    List<int> l1{makeTestList()};
    List<int> l2{0, 1, 2, 3, 4};
    ASSERT_EQ(true, l1 > l2);
  }

  // greater than test
  {
    List<int> l1{1, 2, 5};
    List<int> l2{1, 2, 4};
    ASSERT_EQ(true, l1 > l2);
  }

  // <= with shorter list
  {
    List<int> l1{0, 1, 2, 3, 4};
    List<int> l2{makeTestList()};
    ASSERT_EQ(true, l1 <= l2);
  }

  // <= with lesser list
  {
    List<int> l1{1, 2, 3};
    List<int> l2{1, 2, 4};
    ASSERT_EQ(true, l1 <= l2);
  }

  // <= with equal list
  {
    List<int> l1{makeTestList()}, l2{l1};
    ASSERT_EQ(true, l1 <= l2);
  }

  // >= with longer list
  {
    List<int> l1{1, 2};
    List<int> l2{1, 2, 3};
    ASSERT_EQ(true, l2 >= l1);
  }

  // >= with greater lest
  {
    List<int> l1{1, 2, 4};
    List<int> l2{1, 2, 3};
    ASSERT_EQ(true, l1 >= l2);
  }

  // >= with equal list
  {
    List<int> l1{1, 2, 3, 4};
    List<int> l2{1, 2, 3, 4};
    ASSERT_EQ(true, l1 >= l2);
  }
}

int main(int argc, char* argv[])
{
  (void)argc;
  int exitStatus{EXIT_SUCCESS};

  try {
    std::vector<std::string> successfulFunctions{};

    for (const auto& [func, name] : testFunctions) {
      func();
      successfulFunctions.push_back(name);
    }

    std::cout << ">>>>> TEST SUCCESS\n";

    std::size_t testCaseNumber{1};

    for (const std::string& functionName : successfulFunctions) {
      std::cout << "Test case " << testCaseNumber << " \"" << functionName
                << "\": SUCCESS.\n";
      ++testCaseNumber;
    }

    std::cout << ">>>> ALL TESTS RAN SUCCESSFULLY\n";
  }
  catch (const AssertionViolationException& ex) {
    std::cerr << ex.what();

    std::cerr << "\n\n\n";
    std::cerr << ">>>>>>> TEST FAILURE!!!! <<<<\n";

    exitStatus |= EXIT_FAILURE;
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
    exitStatus |= EXIT_FAILURE;
    std::cerr << leaks.size() << " memory leaks found.\n";

    for (void* addr : leaks) { std::cerr << addr << '\n'; }
  }

  std::cout << argv[0] << ": exiting with code " << exitStatus << '\n';
  return exitStatus;
}
