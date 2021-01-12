#include "Range.hxx"
#include <catch.hpp>
#include <vector>
#include <list>
#include <unordered_map>
#include <array>
#include "LOG.hxx"

TEST_CASE("I can create a Range from a container", "[util]")
{
  /* Mutable Ranges */
  std::vector c1{0, 1, 2, 3};
  Range r1{c1};
  std::list c2{0, 1, 2, 3};
  Range r2{c2};
  std::unordered_map<int, int> c3{{0, 0}, {1, 1}, {2, 2}};
  Range r3{c3};
  /* Immutable Ranges */
  const std::vector c4{0, 1, 2, 3};
  Range r4{c4};
  const std::list c5{0, 1, 2, 3};
  Range r5{c5};
  const std::unordered_map<int, int> c6{{0, 0}, {1, 1}, {2, 2}};
  Range r6{c6};
}

TEST_CASE("I can create a Range from iterators", "[util]")
{
  /* Mutable Ranges */
  std::vector v{0, 1, 2, 3};
  Range r1{v.begin(), v.end()};
  Range r2{v.data(), v.data() + 4};
  std::array a{0, 1, 2, 3};
  Range r3{a.begin(), a.end()};
  Range r4{a.data(), a.data() + 4};
  /* Immutable Ranges */
  const std::vector cv{0, 1, 2, 3};
  Range r5{cv.cbegin(), cv.cend()};
  Range r6{cv.data(), cv.data() + 4};
  const std::array ca{0, 1, 2, 3};
  Range r7{ca.cbegin(), ca.cend()};
  Range r8{a.data(), a.data() + 4};
}

TEST_CASE("I can iterate over a Range", "[util]")
{
  /* Mutable Ranges */
  std::vector v{0, 1, 2, 3};
  Range r1{v};
  REQUIRE(r1.begin() == v.begin());
  REQUIRE(r1.end() == v.end());
  int j = 0;
  for (int &i : r1)
  {
    REQUIRE(i == j++);
    i = -1;
  }
  Range r2{v.data(), v.data() + 4};
  REQUIRE(r2.begin() == v.data());
  REQUIRE(r2.end() == v.data() + 4);
  for (int &i : r2)
  {
    REQUIRE(i == -1);
  }
  /* Immutable Ranges */
  const std::vector cv{0, 1, 2, 3};
  Range r3{cv};
  REQUIRE(r3.begin() == cv.begin());
  REQUIRE(r3.end() == cv.end());
  j = 0;
  for (const int &i : r3)
  {
    REQUIRE(i == j++);
  }
  Range r4{cv.data(), cv.data() + 1};
  REQUIRE(r4.begin() == cv.data());
  REQUIRE(r4.end() == cv.data() + 1);
  for (const int &i : r4)
  {
    REQUIRE(i == 0);
  }
}

/*
These are examples of misuse

[Instantiate temporary]
This will issue the following error:
  You cannot construct a Range from a temporary container
Don't do this
Range r { std::vector { 0, 1, 2 } };

[Use after death]
This will not issue any warning,
but accessing Range r will likely create
a segmentation fault.
Don't do this
Range r;
{
  std::vector v {0, 1, 2};
  r(v);
}

*/
