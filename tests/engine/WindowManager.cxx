#include <catch.hpp>
#include "../../src/engine/WindowManager.hxx"
#include "../../src/engine/basics/Settings.hxx"
#include "../Common.hxx"

#include "SDL.h"
#include <string>

using string = std::string;

TEST_CASE("Get WindowManager instance", "[engine][WindowManager]")
{
  START_TEST

  void *singleton1 = static_cast<void *>(&WindowManager::instance());
  void *singleton2 = static_cast<void *>(&WindowManager::instance());
  CHECK(singleton1 == singleton2);

  FINISH_TEST
}

///@todo deactivate mayfail after the unittest can be run on travis with xvfb
TEST_CASE("Create SDL_Renderer and SDL_Window", "[engine][WindowManager][!mayfail]")
{
  START_TEST

  WindowManager &windowManager = WindowManager::instance();
  CHECK(windowManager.getRenderer() != nullptr);
  CHECK(windowManager.getWindow() != nullptr);

  FINISH_TEST
}

///@todo deactivate mayfail after the unittest can be run on travis with xvfb
TEST_CASE("Toggle Fullscreen", "[engine][WindowManager][!mayfail]")
{
  START_TEST

  bool oldFullScreen = Settings::instance().fullScreen;
  WindowManager &windowManager = WindowManager::instance();
  windowManager.toggleFullScreen();
  CHECK(Settings::instance().fullScreen != oldFullScreen);
  ///@todo: check for fullscreenmode here!
  oldFullScreen = Settings::instance().fullScreen;
  windowManager.toggleFullScreen();
  CHECK(Settings::instance().fullScreen != oldFullScreen);
  CHECK(windowManager.getRenderer() != nullptr);
  CHECK(windowManager.getWindow() != nullptr);

  FINISH_TEST
}

TEST_CASE("Set Window Title", "[!mayfail][engine][WindowManager]")
{
  START_TEST

  WindowManager &windowManager = WindowManager::instance();
  const std::string windowTitle = "_TEST_";
  windowManager.setWindowTitle(windowTitle);
  CHECK(windowTitle == SDL_GetWindowTitle(windowManager.getWindow()));

  FINISH_TEST
}
