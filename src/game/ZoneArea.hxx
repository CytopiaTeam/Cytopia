#ifndef ZONE_AREA_HXX_
#define ZONE_AREA_HXX_

#include "../engine/basics/point.hxx"
#include "../engine/basics/tileData.hxx"

class ZoneArea
{
public:
  ZoneArea();

  /**
   * @brief Add coordinates to a mapNode that has a zone placed on it.
   * 
   * @param Coordinates of the mapNode with a zone tile
   */
  void addZoneNode(Point Coordinate) { m_zoneNodes.push_back(Coordinate); };

  /**
   * @brief Remove a coordinates to a mapNode that has a zone placed on it.
   * 
   * @param Coordinates of the mapNode with a zone tile
   */
  void removeZoneNode(Point Coordinate);

  /**
   * @brief Add coordinates to a vacant mapNode (without a building) that has a zone placed on it.
   * 
   * @param Coordinates of the mapNode with a zone tile
   */
  void addEmptyZoneNode(Point Coordinate) { m_zoneNodesEmpty.push_back(Coordinate); };

  /**
   * @brief Remove coordinates to a vacant mapNode (without a building) that has a zone placed on it.
   * 
   * @param Coordinates of the mapNode with a zone tile
   */
  void removeEmptyZoneNode(Point Coordinate);

  /**
   * @brief Set the power supply for this Area
   * 
   * @param whether the area has power supply or not
   */
  void setPowerSupply(bool hasPower) { m_hasPower = hasPower; };

  /**
   * @brief Get the Power supply for this Area
   * 
   * @return if the area has power supply
   */
  bool hasPowerSupply() { return m_hasPower; };

  /**
   * @brief Set the water supply for this Area
   * 
   * @param whether the area has water supply or not
   */
  void setWaterSupply(bool hasWater) { m_hasWater = hasWater; };

  /**
   * @brief Get the water supply for this Area
   * 
   * @return if the area has water supply
   */
  bool hasWaterSupply() { return m_hasWater; };

  /**
   * @brief Set the zone for this Area
   * 
   * @param zone for this area
   */
  void setZone(Zones zone){m_zone = zone; };
  ;

  /**
   * @brief Get the zone for this Area
   * 
   * @return zone for this area
   */
  Zones getZone() { return m_zone; };

  /**
   * @brief Set the zone density for this Area
   * 
   * @param zone density for this area
   */
  void setZoneDensity(ZoneDensity zoneDensity) { m_zoneDensity = zoneDensity; };

  /**
   * @brief Get the zone density for this Area
   * 
   * @return zone density for this area
   */
  ZoneDensity getZoneDensity() { return m_zoneDensity; };

  /**
 * @brief Spawn buildings on nodes in this area if all demands are fulfilled
 * 
 */
  void spawnBuildings();

private:
  std::vector<Point> m_zoneNodes;
  std::vector<Point> m_zoneNodesEmpty;
  Zones m_zone;
  ZoneDensity m_zoneDensity;
  bool m_hasPower;
  bool m_hasWater;
};

#endif