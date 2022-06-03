#include <catch.hpp>
#include "../../src/engine/ui/basics/ButtonGroup.hxx"
#include "../../src/engine/ui/widgets/Button.hxx"

TEST_CASE("I can create a ButtonGroup", "[ui][group][buttongroup]")
{
  std::unique_ptr<ButtonGroup> buttonGroup = std::make_unique<ButtonGroup>();
  CHECK(buttonGroup != nullptr);
  CHECK(buttonGroup->getAllButtons().empty());
  CHECK(buttonGroup->count() == 0);
}

// further tests on hold pending major reworking of ButtonGroups