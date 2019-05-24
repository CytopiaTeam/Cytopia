#include "engine.hxx"

#include "basics/camera.hxx"
#include "basics/isoMath.hxx"
#include "basics/mapEdit.hxx"
#include "basics/settings.hxx"
#include "common/enums.hxx"
#include "GameObjects/mapNode.hxx"
#include "resourcesManager.hxx"

Engine::Engine() { newGame(); }

Engine::~Engine() { delete map; }

void Engine::increaseHeight(const Point &isoCoordinates) const
{
  terrainEditMode = TerrainEdit::RAISE;
  map->increaseHeight(isoCoordinates);
}

void Engine::decreaseHeight(const Point &isoCoordinates) const
{
  terrainEditMode = TerrainEdit::LOWER;
  map->decreaseHeight(isoCoordinates);
}

void Engine::toggleFullScreen() { WindowManager::instance().toggleFullScreen(); };

void Engine::setTileIDOfNode(const Point &isoCoordinates, const std::string &tileID) const
{
  map->setTileIDOfNode(isoCoordinates, tileID);
}

void Engine::demolishNode(const Point &isoCoordinates) const { map->demolishNode(isoCoordinates, true); }

void Engine::loadGame(const std::string &fileName)
{
  Map *newMap = Map::loadMapFromFile(fileName);

  if (newMap)
  {
    delete map;
    map = newMap;
  }
}

void Engine::newGame()
{
  delete map;

  const int mapSize = Settings::instance().settings.mapSize;

  map = new Map(mapSize, mapSize);
  map->initMap();
}
