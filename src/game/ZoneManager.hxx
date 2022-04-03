#ifndef ZONEMANAGER_HXX_
#define ZONEMANAGER_HXX_

#include "ZoneArea.hxx"
#include "../engine/GameObjects/MapNode.hxx"


class ZoneManager
{
public:
  ZoneManager();

private:
  /**
   * @brief Spawn Buildings on the gathered tileMap
   * 
   */
  void spawnBuildings();

  /**
   * @brief Add a zoneNode node. It will automatically added to zone areas
   * 
   * @param coordinate - coordinate of the zone to add
   * @param zone - Zone type of Zones enum
   * @param zoneDensity - Zone density of ZoneDensity enum
   */
  void addZoneNode(Point coordinate, Zones zone, ZoneDensity zoneDensity);
  /**
   * @brief Removes a zonenode
   * 
   * @param coordinate - coordinate of the zone to remove
   */
  void removeZoneNode(Point coordinate);

  /**
   * @brief get a list of neighboring zoneareas for a zoneNode
   * 
   * @param zoneNode - the zoneNode we need neighboring areas for
   * @param zoneAreas - a vector of areas that apply
   * @return a list of neighboring zoneareas for a zoneNode
   */
  std::vector<int> getAdjacentZoneAreas(const ZoneNode &zoneNode, std::vector<ZoneArea> &zoneAreas);

  /**
   * @brief Adds a zoneNode to a given area
   * 
   * @param zoneNode - node to add
   * @param zoneAreas - zone the node should be added too.
   */
  void addZoneNodeToArea(ZoneNode &zoneNode, std::vector<ZoneArea> &zoneAreas);

  /**
   * @brief rebuild a certain zone area
   * 
   * @param zoneArea - the area to rebuild
   * @return rebuilt zone area
   */
  std::vector<ZoneArea> rebuildZoneArea(ZoneArea &zoneArea);

  std::vector<ZoneArea> m_zoneAreas; /// All zoneAreas
  std::vector<ZoneNode> m_AllNodes; /// All Nodes that have a zone tile
};

#endif