#include "Engine.hxx"

#include "Constants.hxx"
#include "basics/Settings.hxx"

Engine::Engine() {}

Engine::~Engine() {}

// void Engine::saveGame(const std::string &fileName) const { map->saveMapToFile(CYTOPIA_SAVEGAME_DIR + (std::string)fileName); }
// void Engine::loadGame(const std::string &fileName)
// {
//   Map *newMap = Map::loadMapFromFile(CYTOPIA_SAVEGAME_DIR + (std::string)fileName);

//   if (newMap)
//   {
//     delete map;
//     map = newMap;
//     m_running = true;
//     map->refresh();
//   }
// }

// void Engine::newGame()
// {
//   const int mapSize = Settings::instance().mapSize;
//   Map *newMap = new Map(mapSize, mapSize, true);

//   if (newMap)
//   {
//     delete map;
//     map = newMap;
//     m_running = true;
//     map->refresh();
//   }
// }
