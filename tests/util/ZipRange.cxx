#include <catch.hpp>
#include <array>
#include <initializer_list>
#include <vector>
#include "ZipRange.hxx"

TEST_CASE("I can use ZipRanges", "[util]")
{
  const std::array myInts{0, 1, 2, 3, 4};
  const std::vector myChars{'0', '1', '2', '3', '4'};
  const std::initializer_list<std::string> myStrings{"0", "1", "2", "3", "4"};
  int count = 0;
  for (const auto &[i, c, s] : ZipRange{myInts, myChars, myStrings})
  {
    REQUIRE(std::to_string(i) == s);
    REQUIRE(std::string(1, c) == s);
    count++;
  }
  REQUIRE(count == 5);
}

TEST_CASE("ZipRanges will not go past std::end", "[util]")
{
  /* Consider a ZipRange of containers of different lengths... */
  const std::array myInts{0, 1, 2, 3};
  const std::vector<int> empty;
  const ZipRange range{myInts, empty};
  REQUIRE(range.begin() == range.end());
  REQUIRE(!(range.begin() != range.end()));
  int count = 0;
  for (const auto &[i1, i2] : range)
  {
    ++count;
  }
  REQUIRE(count == 0);
}
