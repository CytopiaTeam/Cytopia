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

  _map = Map(map_size, map_size);

  // Default: Floor and Buildings are drawn
  _activeLayers = LAYER_FLOOR | LAYER_BUILDINGS;
}

void Engine::increaseHeight(const Point &isoCoordinates)
{
  terrainEditMode = TerrainEdit::RAISE;
  _map.increaseHeight(isoCoordinates);
}

void Engine::decreaseHeight(const Point &isoCoordinates)
{
  terrainEditMode = TerrainEdit::LOWER;
  _map.decreaseHeight(isoCoordinates);
}

void Engine::toggleFullScreen() { WindowManager::instance().toggleFullScreen(); };

void Engine::setTileIDOfNode(const Point &isoCoordinates, const std::string &tileID)
{
  _map.setTileIDOfNode(isoCoordinates, tileID);
}

void Engine::demolishNode(const Point &isoCoordinates) { _map.demolishNode(isoCoordinates); }
