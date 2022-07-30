#include <catch.hpp>
#include "../../src/engine/UIManager.hxx"
#include "../../src/game/ui/BuildMenu.hxx"
#include "../../src/game/ui/PauseMenu.hxx"

TEST_CASE("Get UIManager instance", "[engine][UIManager][ui]")
{
  void *singleton1 = static_cast<void *>(&UIManager::instance());
  void *singleton2 = static_cast<void *>(&UIManager::instance());
  CHECK(singleton1 == singleton2);
}

TEST_CASE("UIManager will close all open menus except the build menu", "[engine][UIManager][ui][!mayfail]")
{
  UIManager &uiManager = UIManager::instance();

  uiManager.addPersistentMenu<GameMenu>();
  uiManager.openMenu<PauseMenu>();

  // TODO: need to do smth to set it up. like run init or draw or manually set menu visibilities

  uiManager.closeOpenMenus();
  CHECK(!uiManager.isAnyMenuOpen());
  CHECK(!!uiManager.findMenu<GameMenu>());
}