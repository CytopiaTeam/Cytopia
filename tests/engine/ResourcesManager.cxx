#include <catch.hpp>
#include "../../src/engine/ResourcesManager.hxx"
#include "Exception.hxx"

using string = std::string;

TEST_CASE("Get ResourcesManager instance", "[!mayfail][engine][resourcesmanager]")
{
  void *singleton1 = static_cast<void *>(&ResourcesManager::instance());
  void *singleton2 = static_cast<void *>(&ResourcesManager::instance());
  CHECK(singleton1 == singleton2);
}

TEST_CASE("Get UI Texture", "[engine][resourcesmanager]")
{
  REQUIRE_THROWS_AS(ResourcesManager::instance().getUITexture("UNLOADED"), UIError);
}

TEST_CASE("Get Tile Texture", "[engine][resourcesmanager]")
{
  REQUIRE_THROWS_AS(ResourcesManager::instance().getTileTexture("UNLOADED"), UIError);
  REQUIRE_THROWS_AS(ResourcesManager::instance().getTileTexture("UNLOADED"), UIError);
}

TEST_CASE("Get Tile Surface", "[engine][resourcesmanager]")
{
  REQUIRE_THROWS_AS(ResourcesManager::instance().getTileSurface("UNLOADED"), UIError);
  REQUIRE_THROWS_AS(ResourcesManager::instance().getTileSurface("UNLOADED"), UIError);
}

TEST_CASE("Load Texture", "[engine][resourcesmanager]")
{
  REQUIRE_THROWS_AS(ResourcesManager::instance().loadTexture("TEXTURE", "__NOT_A_FILE__"), ConfigurationError);
  REQUIRE_THROWS_AS(ResourcesManager::instance().loadTexture("TEXTURE", "__NOT_A_FILE__"), ConfigurationError);
}

SCENARIO("I can load and use textures", "[engine][resourcesmanager][!mayfail]")
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
