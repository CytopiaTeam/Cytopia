#include <catch.hpp>
#include "../../src/engine/common/Constants.hxx"
#include "../../src/engine/Engine.hxx"

using string = std::string;

TEST_CASE("Get Engine instance", "[engine][engine]")
{
  void *singleton1 = static_cast<void *>(&Engine::instance());
  void *singleton2 = static_cast<void *>(&Engine::instance());
  CHECK(singleton1 == singleton2);
}

TEST_CASE("Engine new game", "[engine][engine]")
{
  Engine &engine = Engine::instance();
  engine.newGame();
  CHECK(engine.isGameRunning() == true);
  engine.quitGame();
}

TEST_CASE("Create Map with procedural terrain", "[engine][engine]")
{
  Map *map = new Map(64, 64, true);
  CHECK(map != nullptr);
}

TEST_CASE("Create Map without procedural terrain", "[engine][engine]")
{
  Map *map = new Map(64, 64, false);
  CHECK(map != nullptr);
}

TEST_CASE("Save and load Map", "[engine][engine]")
{
  Engine &engine = Engine::instance();
  engine.newGame();
  engine.saveGame("saveTest");
  engine.loadGame("saveTest");

  CHECK(Engine::instance().map != nullptr);
  CHECK(Engine::instance().isGameRunning() == true);

  Engine::instance().quitGame();

  CHECK(Engine::instance().isGameRunning() == false);
}

TEST_CASE("verify loaded Map", "[engine][engine]")
{
  Engine &engine = Engine::instance();
  engine.newGame();
  engine.saveGame("saveTest");
  engine.loadGame("saveTest");

  Map *mapToLoad = Map::loadMapFromFile(CYTOPIA_SAVEGAME_DIR + (std::string) "saveTest");
  Map *map = engine.map;

  CHECK(mapToLoad != nullptr);
  CHECK(map != nullptr);
  CHECK(map->getMapNodes().size() == mapToLoad->getMapNodes().size());

  for (int i = 0; i < map->getMapNodes().size(); i++)
  {
    const MapNode &oldnode = map->getMapNodes()[i];
    const MapNode &newnode = mapToLoad->getMapNodes()[i];
    CHECK(oldnode.getElevationBitmask() == newnode.getElevationBitmask());
    CHECK(oldnode.getCoordinates() == newnode.getCoordinates());
    CHECK(oldnode.getTopMostActiveLayer() == newnode.getTopMostActiveLayer());
    CHECK(oldnode.getTileID(newnode.getTopMostActiveLayer()) == newnode.getTileID(newnode.getTopMostActiveLayer()));
    // Sprite checks
    CHECK(oldnode.getSprite()->getActiveDestRect().x == newnode.getSprite()->getActiveDestRect().x);
    CHECK(oldnode.getSprite()->getActiveDestRect().y == newnode.getSprite()->getActiveDestRect().y);
    CHECK(oldnode.getSprite()->getActiveDestRect().w == newnode.getSprite()->getActiveDestRect().w);
    CHECK(oldnode.getSprite()->getActiveDestRect().h == newnode.getSprite()->getActiveDestRect().h);
    CHECK(oldnode.getSprite()->getActiveClipRect().x == newnode.getSprite()->getActiveClipRect().x);
    CHECK(oldnode.getSprite()->getActiveClipRect().y == newnode.getSprite()->getActiveClipRect().y);
    CHECK(oldnode.getSprite()->getActiveClipRect().w == newnode.getSprite()->getActiveClipRect().w);
    CHECK(oldnode.getSprite()->getActiveClipRect().h == newnode.getSprite()->getActiveClipRect().h);
  }
  Engine::instance().quitGame();
  CHECK(Engine::instance().isGameRunning() == false);
}
