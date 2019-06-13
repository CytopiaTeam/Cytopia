#include "../../util/Meta.hxx"

SECTION("Type lists are useful", "[util]")
{
  using MyConstants = TypeList<
    Constant<int, 1>
    Constant<bool, true>
    Constant<char, 'x'>
    >;
}
