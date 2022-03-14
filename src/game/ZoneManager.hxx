#ifndef ZONEMANAGER_HXX_
#define ZONEMANAGER_HXX_

#include "../engine/GameObjects/MapNode.hxx"

class ZoneManager
{
public:
  ZoneManager() = default;
  ~ZoneManager() = default;

  void updateZones();
  void spawnBuildings();

private:
  std::vector<const MapNode *> m_MapNodes;
};

#endif