#include "Engine.hxx"

#include "basics/Camera.hxx"
#include "basics/mapEdit.hxx"
#include "basics/Settings.hxx"
#include "ResourcesManager.hxx"
#include "../util/LOG.hxx"

Engine::Engine() {}

Engine::~Engine() { 
  debug_scope {
    LOG(LOG_DEBUG) << "Destroying Engine";
  }
  delete map;
}

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

void Engine::loadGame(const std::string &fileName)
{
  Map *newMap = Map::loadMapFromFile(fileName);

  if (newMap)
  {
    delete map;
    map = newMap;
    m_running = true;
  }
}

void Engine::newGame()
{
  delete map;
  m_running = true;

  const int mapSize = Settings::instance().mapSize;

  map = new Map(mapSize, mapSize);
}
