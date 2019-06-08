#include <catch2/catch.hpp>
#include "../../src/engine/ResourcesManager.hxx"

using string = std::string;

TEST_CASE("Get instance", "[engine]")
{
  void *singleton1 = static_cast<void *>(&ResourcesManager::instance());
  void *singleton2 = static_cast<void *>(&ResourcesManager::instance());
  CHECK(singleton1 == singleton2);
}

TEST_CASE("Get UI Texture", "[engine]")
{
  SDL_Texture *texture1 = ResourcesManager::instance().getUITexture("UNLOADED");
  SDL_Texture *texture2 = ResourcesManager::instance().getUITexture("UNLOADED", BUTTONSTATE_HOVERING);
  CHECK(texture1 == nullptr);
  CHECK(texture2 == nullptr);
}

TEST_CASE("Get Tile Texture", "[engine]")
{
  SDL_Texture *texture1 = ResourcesManager::instance().getTileTexture("UNLOADED");
  SDL_Texture *texture2 = ResourcesManager::instance().getTileTexture("UNLOADED", 1);
  CHECK(texture1 == nullptr);
  CHECK(texture2 == nullptr);
}

TEST_CASE("Get Tile Surface", "[engine]")
{
  SDL_Surface *surface1 = ResourcesManager::instance().getTileSurface("UNLOADED");
  SDL_Surface *surface2 = ResourcesManager::instance().getTileSurface("UNLOADED", 1);
  CHECK(surface1 == nullptr);
  CHECK(surface2 == nullptr);
}

TEST_CASE("Load Texture", "[engine]")
{
  CHECK_NOTHROW(ResourcesManager::instance().loadTexture("TEXTURE", "__NOT_A_FILE__"));
  CHECK_NOTHROW(ResourcesManager::instance().loadTexture("TEXTURE", "__NOT_A_FILE__", 1));
}

SCENARIO("I can load and use textures", "[engine][!mayfail]")
{
  GIVEN("A texture named resources/images/app_icons/logo_big_textured.png")
  {
    string texture_file = "resources/images/app_icons/logo_big_textured.png";
    WHEN("I load that texture as \"texture\"")
    {
      string texture_name = "texture";
      ResourcesManager::instance().loadTexture(texture_name, texture_file);
      THEN("I can get its Tile Texture")
      {
        SDL_Texture *texture = ResourcesManager::instance().getTileTexture(texture_name);
        CHECK(texture != nullptr);
      }
    }
  }
}
