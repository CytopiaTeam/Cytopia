#include <catch.hpp>
#include "../../src/engine/ResourcesManager.hxx"
#include "Exception.hxx"
#include "../Common.hxx"

using string = std::string;

TEST_CASE("Get ResourcesManager instance", "[!mayfail][engine][resourcesmanager]")
{
  START_TEST

  void *singleton1 = static_cast<void *>(&ResourcesManager::instance());
  void *singleton2 = static_cast<void *>(&ResourcesManager::instance());
  CHECK(singleton1 == singleton2);

  FINISH_TEST
}

TEST_CASE("Get UI Texture", "[engine][resourcesmanager]")
{
  START_TEST

  void *notexture = ResourcesManager::instance().getUITexture("UNLOADED");
  CHECK(notexture == nullptr);
  notexture = ResourcesManager::instance().getUITexture("UNLOADED", BUTTONSTATE_HOVERING);
  CHECK(notexture == nullptr); ResourcesManager::instance().getTileTexture("UNLOADED");
  notexture = ResourcesManager::instance().getUITexture("UNLOADED", BUTTONSTATE_CLICKED);
  CHECK(notexture == nullptr);

  FINISH_TEST
}

TEST_CASE("Get Tile Texture", "[engine][resourcesmanager]")
{
  START_TEST

  void *notexture = ResourcesManager::instance().getTileTexture("UNLOADED");
  CHECK(notexture == nullptr);
  notexture = ResourcesManager::instance().getTileTexture("UNLOADED");
  CHECK(notexture == nullptr);

  FINISH_TEST
}

TEST_CASE("Get Tile Surface", "[engine][resourcesmanager]")
{
  START_TEST

  REQUIRE_THROWS_AS(ResourcesManager::instance().getTileSurface("UNLOADED"), UIError);
  REQUIRE_THROWS_AS(ResourcesManager::instance().getTileSurface("UNLOADED"), UIError);

  FINISH_TEST
}

TEST_CASE("Load Texture", "[engine][resourcesmanager]")
{
  START_TEST

  REQUIRE_THROWS_AS(ResourcesManager::instance().loadTileTextureSync("TEXTURE", "__NOT_A_FILE__"), ConfigurationError);
  REQUIRE_THROWS_AS(ResourcesManager::instance().loadTileTextureSync("TEXTURE", "__NOT_A_FILE__"), ConfigurationError);

  FINISH_TEST
}

SCENARIO("I can load and use textures", "[engine][resourcesmanager][!mayfail]")
{
  START_TEST

  GIVEN("A texture named resources/images/app_icons/logo_big_textured.png")
  {
    string texture_file = "resources/images/app_icons/logo_big_textured.png";
    WHEN("I load that texture as \"texture\"")
    {
      string texture_name = "texture";
      ResourcesManager::instance().loadTileTextureSync(texture_name, texture_file);
      THEN("I can get its Tile Texture")
      {
        SDL_Texture *texture = ResourcesManager::instance().getTileTexture(texture_name);
        CHECK(texture != nullptr);
      }
    }
  }

  FINISH_TEST
}
