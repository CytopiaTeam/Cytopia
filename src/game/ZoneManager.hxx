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

  /**
   * @brief Get the Area of connected zone nodes
   * 
   * @param node 
   */
  void getNodeArea(MapNode *node);

private:
  void spawnBuildings();
  void spawn();

  /**
   * @brief Get a mapNode from m_mapNodes with the coordinate supplied
   * 
   * @param coordinate - What mapNode to retrieve 
   * @return const MapNode* 
   */
  const MapNode *getZoneNodeWithCoordinate(Point coordinate);

  /**
   * @brief Returns the possible size of buildings that can be placed on this coordinate in a zone
   * 
   * @param originPoint - coordinate where we want to know how many free zone tiles there are next to it
   * @return TileSize - struct with height and with for the possible tilesize that can be placed on this coordinate
   */
  TileSize getMaximumTileSize(Point originPoint);

  std::vector<const MapNode *> m_MapNodes;
};

#endif