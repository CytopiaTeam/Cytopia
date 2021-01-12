#include <catch.hpp>
#include "BoxSizing.hxx"

TEST_CASE("I can use ScreenMeasurement", "[util]")
{
  REQUIRE((1_lh).lh == 1);
  REQUIRE((1_lw).lw == 1);
  REQUIRE((1_eh).eh == 1);
  REQUIRE((1_ew).ew == 1);
  REQUIRE((1_px).px == 1);

  REQUIRE((1_lh + 1_lh).lh == 2);
  REQUIRE((1_lw + 1_lw).lw == 2);
  REQUIRE((1_eh + 1_eh).eh == 2);
  REQUIRE((1_ew + 1_ew).ew == 2);
  REQUIRE((1_px + 1_px).px == 2);

  REQUIRE((2_lh - 1_lh).lh == 1);
  REQUIRE((2_lw - 1_lw).lw == 1);
  REQUIRE((2_eh - 1_eh).eh == 1);
  REQUIRE((2_ew - 1_ew).ew == 1);
  REQUIRE((2_px - 1_px).px == 1);

  ScreenMeasurementValuation smv{100, 200, 300, 400};
  REQUIRE((1_ew).measure(smv) == 1);
  REQUIRE((1_eh).measure(smv) == 2);
  REQUIRE((1_lw).measure(smv) == 3);
  REQUIRE((1_lh).measure(smv) == 4);
  REQUIRE((5_px).measure(smv) == 5);
  REQUIRE((1_ew + 1_eh + 1_lw + 1_lh + 5_px).measure(smv) == 15);
}
