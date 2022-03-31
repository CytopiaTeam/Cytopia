#ifndef ZONEMANAGER_HXX_
#define ZONEMANAGER_HXX_

#include "ZoneArea.hxx"
#include "../engine/GameObjects/MapNode.hxx"


class ZoneManager
{
public:
  ZoneManager();
  void addZoneNode(Point coordinate, Zones zone, ZoneDensity ZoneDensity);
  void removeZoneNode(Point coordinate);

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

  std::vector<int> findAllSuitableZoneArea(const ZoneNode &zoneNode);

  std::vector<ZoneArea> m_zoneAreas;
  std::vector<ZoneNode> m_AllNodes; // keep track of all Points we have so we can sort them out
};

#endif