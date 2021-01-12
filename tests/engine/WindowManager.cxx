#include <catch.hpp>
#include "../../src/engine/WindowManager.hxx"
#include "../GameFixture.hxx"
#include "SDL.h"
#include <string>

using string = std::string;

TEST_CASE_METHOD(GameFixture, "Get WindowManager instance", "[engine][WindowManager]")
{
  void *singleton1 = static_cast<void *>(&WindowManager::instance());
  void *singleton2 = static_cast<void *>(&WindowManager::instance());
  CHECK(singleton1 == singleton2);
}

TEST_CASE_METHOD(GameFixture, "Create SDL_Renderer and SDL_Window", "[engine][WindowManager]")
{
  WindowManager &windowManager = WindowManager::instance();
  CHECK(windowManager.getRenderer() != nullptr);
  CHECK(windowManager.getWindow() != nullptr);
}

TEST_CASE_METHOD(GameFixture, "Toggle Fullscreen", "[engine][WindowManager]")
{
  WindowManager &windowManager = WindowManager::instance();
  windowManager.toggleFullScreen();
  CHECK(windowManager.getRenderer() != nullptr);
  CHECK(windowManager.getWindow() != nullptr);
}

TEST_CASE_METHOD(GameFixture, "Set Window Title", "[engine][WindowManager]")
{
  WindowManager &windowManager = WindowManager::instance();
  const std::string windowTitle = "_TEST_";
  windowManager.setWindowTitle(windowTitle);
  CHECK(windowTitle == SDL_GetWindowTitle(windowManager.getWindow()));
}
