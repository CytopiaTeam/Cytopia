#include "Engine.hxx"

#include "Constants.hxx"
#include "basics/Camera.hxx"
#include "basics/mapEdit.hxx"
#include "basics/Settings.hxx"
#include "ResourcesManager.hxx"

Engine::Engine() {}

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

void Engine::saveGame(const std::string &fileName) const { map->saveMapToFile(CYTOPIA_SAVEGAME_DIR + (std::string)fileName); }
void Engine::loadGame(const std::string &fileName)
{
  Map *newMap = Map::loadMapFromFile(CYTOPIA_SAVEGAME_DIR + (std::string)fileName);

  if (newMap)
  {
    delete map;
    map = newMap;
    m_running = true;
    map->refresh();
  }
}

void Engine::newGame()
{
  const int mapSize = Settings::instance().mapSize;
  Map *newMap = new Map(mapSize, mapSize, true);

  if (newMap)
  {
    delete map;
    map = newMap;
    m_running = true;
    map->refresh();
  }
}
