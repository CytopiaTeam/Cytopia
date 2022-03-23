#ifndef ZONEMANAGER_HXX_
#define ZONEMANAGER_HXX_

#include "../engine/GameObjects/MapNode.hxx"

class ZoneManager
{
public:
  ZoneManager();
  void update();
  void addZoneNode(MapNode *node);
  void removeZoneNode(MapNode *node);
  void getNodeArea(MapNode *node);

private:
  void spawnBuildings();
  void spawn();
  const MapNode* getZoneNodeWithCoordinate(Point coordinate);

  /** Returns the possible size of buildings we can add.
   * 
   **/
  TileSize getPossibleTileSize(Point originPoint);

  std::vector<const MapNode *> m_MapNodes;
};

#endif