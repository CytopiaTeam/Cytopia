#ifndef ZONEMANAGER_HXX_
#define ZONEMANAGER_HXX_

#include "../engine/GameObjects/MapNode.hxx"

class ZoneManager
{
public:
  ZoneManager();
  void update();

private:
  void updateZones();
  void spawnBuildings();
  void spawn();

  std::vector<const MapNode *> m_MapNodes;
};

#endif