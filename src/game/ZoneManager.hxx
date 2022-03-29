#ifndef ZONEMANAGER_HXX_
#define ZONEMANAGER_HXX_

#include "ZoneArea.hxx"
#include "../engine/GameObjects/MapNode.hxx"


class ZoneManager
{
public:
  ZoneManager();
  void update();
  void addZoneNode(MapNode *node);
  void addZoneNode(Point coordinate, Zones zone, ZoneDensity ZoneDensity);
  void removeZoneNode(MapNode *node);

  /**
   * @brief Get the Area of connected zone nodes
   * 
   * @param node 
   */
  void getNodeArea(MapNode *node);

private:
  /**
   * @brief Spawn Buildings on the gathered tileMap
   * 
   */
  void spawnBuildings();

  /**
   * @brief Get a mapNode from m_mapNodes with the coordinate supplied
   * 
   * @param coordinate - What mapNode to retrieve 
   * @return MapNode pointer
   */
  ZoneNode getZoneNodeWithCoordinate(Point coordinate);

  /**
   * @brief Returns the possible size of buildings that can be placed on this coordinate in a zone
   * 
   * @param originPoint - coordinate where we want to know how many free zone tiles there are next to it
   * @return struct with height and with for the possible tilesize that can be placed on this coordinate
   */
  TileSize getMaximumTileSize(Point originPoint);

  /**
   * @brief Clears m_zoneAreas and fills it again.
   * 
   */
  void rebuildZoneAreas();

  std::vector<const MapNode *> m_MapNodes;
  std::vector<ZoneArea> m_zoneAreas;
  std::vector<ZoneNode> m_AllNodes; // keep track of all Points we have so we can sort them out
};

#endif