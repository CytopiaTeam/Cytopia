#ifndef ZONEMANAGER_HXX_
#define ZONEMANAGER_HXX_

#include "ZoneArea.hxx"
#include "PowerGrid.hxx"
#include <MapNode.hxx>

class ZoneManager
{
public:
  ZoneManager();

  void reset();

private:
  /**
   * @brief Spawn Buildings on the gathered tileMap
   * 
   */
  void spawnBuildings();

  /**
   * @brief Process previously cached nodes to update
   * 
   */
  void update();

  /**
   * @brief Removes a zonenode
   * 
   * @param coordinate coordinate of the zone to remove
   */
  void removeZoneNode(Point coordinate);

  /**
   * @brief get a list of neighboring zoneareas for a zoneNode
   * 
   * @param zoneNode the zoneNode we need neighboring areas for
   * @param zoneAreas a vector of areas that apply
   * @return a list of neighboring zoneareas for a zoneNode
   */
  std::vector<int> getAdjacentZoneAreas(const ZoneNode &zoneNode, std::vector<ZoneArea> &zoneAreas);

  /**
   * @brief Adds a zoneNode to a given area
   * 
   * @param zoneNode node to add
   * @param zoneAreas all zoneAreas
   */
  void addZoneNodeToArea(ZoneNode &zoneNode, std::vector<ZoneArea> &zoneAreas);

  /**
   * @brief rebuild a certain zone area
   * 
   * @param zoneArea the area to rebuild
   * @return rebuilt zone area
   */
  std::vector<ZoneArea> rebuildZoneArea(ZoneArea &zoneArea);

  void updatePower(const std::vector<PowerGrid> &powerGrid);

  void updateRemovedNodes(const MapNode *mapNode);

  void updatePlacedNodes(const MapNode &mapNode);

  std::vector<ZoneArea> m_zoneAreas;  /// All zoneAreas
  std::vector<ZoneNode> m_nodesToAdd; /// All zoneAreas
  std::vector<Point> m_nodesToOccupy; /// All zoneAreas
  std::vector<Point> m_nodesToVacate; /// All zoneAreas
  std::vector<Point> m_nodesToRemove; /// All zoneAreas
};

#endif