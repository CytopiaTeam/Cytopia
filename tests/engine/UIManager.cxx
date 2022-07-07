#include <catch.hpp>
#include "../../src/engine/UIManager.hxx"
#include "../../src/engine/ui/basics/UIElement.hxx"
#include "../Common.hxx"

TEST_CASE("Get UIManager instance", "[engine][UIManager][ui]")
{
  START_TEST

  void *singleton1 = static_cast<void *>(&UIManager::instance());
  void *singleton2 = static_cast<void *>(&UIManager::instance());
  CHECK(singleton1 == singleton2);

  FINISH_TEST
}

TEST_CASE("UIManager will close all open menus except the build menu", "[engine][UIManager][ui][!mayfail]")
{
  START_TEST

  UIManager &uiManager = UIManager::instance();
  // TODO: need to do smth to set it up. like run init or draw or manually set menu visibilities
  uiManager.closeOpenMenus();
  for (const auto &[key, value] : uiManager.getAllUiGroups())
  {
    for (auto element : value)
    {
      if (key == "_BuildMenu_")
        CHECK(element->isVisible());
      else
        CHECK(!element->isVisible());
    }
  }

  FINISH_TEST
}