#include "engine.hxx"

#include "basics/camera.hxx"
#include "basics/isoMath.hxx"
#include "basics/mapEdit.hxx"
#include "basics/settings.hxx"
#include "GameObjects/mapNode.hxx"
#include "resourcesManager.hxx"

Engine::Engine()
{
  int map_size = Settings::instance().settings.mapSize;

  map = new Map(map_size, map_size);
  map->initMap();
  // Default: Floor and Buildings are drawn
  _activeLayers = LAYER_FLOOR | LAYER_BUILDINGS;
}

Engine::~Engine() { delete map; }

void Engine::increaseHeight(const Point &isoCoordinates)
{
  terrainEditMode = TerrainEdit::RAISE;
  map->increaseHeight(isoCoordinates);
}

void Engine::decreaseHeight(const Point &isoCoordinates)
{
  terrainEditMode = TerrainEdit::LOWER;
  map->decreaseHeight(isoCoordinates);
}

void Engine::toggleFullScreen() { WindowManager::instance().toggleFullScreen(); };

void Engine::setTileIDOfNode(const Point &isoCoordinates, const std::string &tileID)
{
  map->setTileIDOfNode(isoCoordinates, tileID);
}

void Engine::demolishNode(const Point &isoCoordinates) { map->demolishNode(isoCoordinates); }

void Engine::loadSaveGame(const std::string &fileName)
{
  Map *newMap = Map::loadMapFromFile(fileName);

  if (newMap)
  {
    delete map;
    map = newMap;
  }
}