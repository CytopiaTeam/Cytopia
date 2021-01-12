#include <catch.hpp>
#include <ciso646>

using Catch::Contains;

/* You can ignore these */
template <int a, int b> struct StaticEquals : std::false_type
{
};
template <int a> struct StaticEquals<a, a> : std::true_type
{
};

/* This is a test! */
TEST_CASE("Example Test", "[!hide]")
{

  SECTION("true/false")
  {
    // clang-format off
    REQUIRE(1 != 0);
    REQUIRE_FALSE(not true);
    // clang-format on
  }

  SECTION("floating points")
  {
    REQUIRE(1.1f == Approx(1.1));
    {
      using namespace Catch::literals;
      REQUIRE(1.1f == 1.1_a);
    }
  }

  SECTION("exceptions")
  {
    REQUIRE_NOTHROW(0);
    REQUIRE_THROWS(throw 1);
    REQUIRE_THROWS_AS(throw "hello", const char *);
    REQUIRE_THROWS_MATCHES(throw "hello exception", const char *, Contains("hello"));
  }

  SECTION("Matchers") { REQUIRE_THAT("hello tests", Contains("hello")); }

  /* Be careful with commas!
     This code would not work because macros don't like commas in types
     REQUIRE(StaticEquals<1,1>::value);
     */
  using OneEqualsOne = StaticEquals<1, 1>;
  /* Now this works! */
  REQUIRE(OneEqualsOne::value);
}

/* You can also do it BDD-style! */

#include <memory>

SCENARIO("C++ is cool", "[!hide]")
{
  GIVEN("a smart pointer")
  {
    std::unique_ptr<int> myInt = std::make_unique<int>(5);
    WHEN("I try to set it")
    {
      *myInt = 2;
      THEN("Its value changes") { REQUIRE(*myInt == 2); }
    }
    WHEN("I try to swap it with another pointer")
    {
      std::unique_ptr<int> otherInt;
      std::swap(otherInt, myInt);
      THEN("They swap values")
      {
        REQUIRE(myInt == nullptr);
        REQUIRE(*otherInt == 5);
      }
    }
    WHEN("I release it")
    {
      int *intData = myInt.release();
      THEN("It becomes nullptr")
      {
        REQUIRE(myInt == nullptr);
        REQUIRE(*intData == 5);
        delete intData;
      }
    }
  }
}

/* Note that all inner scopes in a scope must be uniquely schedulable.
 * That could mean memory leaks if you allocate pointers in a outer scope
 * and don't deallocate in case, which is why we provide an example with smart pointers
 */

/* To learn more about Catch2, see https://github.com/catchorg/Catch2 */
