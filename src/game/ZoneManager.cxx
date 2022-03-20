#include "ZoneManager.hxx"
#include "Engine.hxx"
#include "LOG.hxx"
#include "../services/GameClock.hxx"
#include "../services/Randomizer.hxx"

#include <algorithm>

ZoneManager::ZoneManager()
{
  GameClock::instance().addRealTimeClockTask(
      [this]()
      {
        spawnBuildings();
        return false;
      },
      1s, 1s);
}

// Nothing here right now
void ZoneManager::update() {}

void ZoneManager::spawnBuildings() { spawn(); }

void ZoneManager::spawn()
{
  constexpr int amountOfBuildingsToSpawn = 5;
  auto &randomizer = Randomizer::instance();

  // shuffle mapNodes
  randomizer.shuffle(m_MapNodes.begin(), m_MapNodes.end());

  // pick every single zone node we have
  int buildingsSpawned = 0;
  for (auto &node : m_MapNodes)
  {
    if (buildingsSpawned >= amountOfBuildingsToSpawn)
    {
      break;
    }

    if (!node->getTileData(Layer::ZONE))
    {
      LOG(LOG_ERROR) << "Something is wrong with a zone tile - Report this to the Cytopia Team!";
      continue;
    }

    // get the right zone enum of the zone tile
    std::vector<std::string> availableZoneTiles;
    for (auto &zone : node->getTileData(Layer::ZONE)->zones)
    {
      const auto &zoneTiles = TileManager::instance().getTileIDsOfCategory(zone);

      if (zoneTiles.empty())
      {
        LOG(LOG_ERROR) << "No buildings available for zone: " << zone;
        continue;
      }
      else
      {
        // add all the tileIDs that are available for this zone to our vector
        availableZoneTiles.insert(availableZoneTiles.end(), zoneTiles.begin(), zoneTiles.end());
      }
    }

    if (availableZoneTiles.empty())
    {
      LOG(LOG_ERROR) << "No buildings available! Aborting!";
      break;
    }

    // pick a random tileID from the vector
    // TODO: Check how many neighboring tiles are available and pick a building with correct tilesize
    std::string building = *randomizer.choose(availableZoneTiles.begin(), availableZoneTiles.end());
    std::vector targetObjectNodes = Engine::instance().map->getObjectCoords(node->getCoordinates(), building);
    Engine::instance().setTileIDOfNode(targetObjectNodes.begin(), targetObjectNodes.end(), building, false);
    buildingsSpawned++;
  }
}

void ZoneManager::addZoneNode(MapNode *node) { m_MapNodes.emplace_back(node); }
void ZoneManager::removeZoneNode(MapNode *node)
{
  if (node)
  {
    m_MapNodes.erase(std::remove(m_MapNodes.begin(), m_MapNodes.end(), node));
  }
}