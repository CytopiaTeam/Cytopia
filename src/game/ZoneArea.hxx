#ifndef ZONE_AREA_HXX_
#define ZONE_AREA_HXX_

#include "../engine/basics/point.hxx"
#include "../engine/basics/tileData.hxx"

struct ZoneNode
{
  Point coordinate = Point::INVALID();
  Zones zone = Zones::RESIDENTIAL;
  ZoneDensity zoneDensity = ZoneDensity::LOW;
  bool occupied = false;
};

class ZoneArea;
void mergeZoneAreas(ZoneArea &mainZone, ZoneArea &toBeMerged);

class ZoneArea
{
public:
  ZoneArea(ZoneNode zoneNode);

  /**
   * @brief Add coordinates to a mapNode that has a zone placed on it.
   * 
   * @param Coordinates of the mapNode with a zone tile
   */
  void addZoneNode(ZoneNode zoneNode);

  /**
   * @brief Remove a coordinates to a mapNode that has a zone placed on it.
   * 
   * @param Coordinates of the mapNode with a zone tile
   */
  void removeZoneNode(Point coordinate);

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
  void setZone(Zones zone) { m_zone = zone; };
  ;

  /**
   * @brief Get the zone for this Area
   * 
   * @return zone for this area
   */
  Zones getZone() const { return m_zone; };

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
   * @brief If this coordinate is a neighbor of one of the tiles in this zone area.
   * 
   * @param coordinate The point to check
   * @return neighbor of this zoneArea
   */
  bool isNeighborOfZone(Point coordinate) const;

  /**
   * @brief If this coordinate part of this zone area.
   * 
   * @param coordinate The point to check
   * @return neighbor of this zoneArea
   */
  bool isPartOfZone(Point coordinate);
  /**
 * @brief Spawn buildings on nodes in this area if all demands are fulfilled
 * 
 */
  void spawnBuildings();

  size_t getSize() { return m_zoneNodes.size(); };

  bool isWithinBoundaries(Point coordinate) const
  {
    return (xmin <= coordinate.x) && (xmax >= coordinate.x) && (ymin <= coordinate.y) && (ymax >= coordinate.y);
  }

  void occupyZoneNode(Point coordinate);
  void freeZoneNode(Point coordinate);

private:
  std::vector<ZoneNode> m_zoneNodes;
  Zones m_zone = Zones::RESIDENTIAL;
  ZoneDensity m_zoneDensity = ZoneDensity::LOW;
  bool m_hasPower;
  bool m_hasWater;

  int xmin, xmax, ymin, ymax;

  friend void mergeZoneAreas(ZoneArea &mainZone, ZoneArea &toBeMerged);

  /**
   * @brief Returns the possible size of buildings that can be placed on this coordinate in a zone
   * 
   * @param originPoint - coordinate where we want to know how many free zone tiles there are next to it
   * @return struct with height and with for the possible tilesize that can be placed on this coordinate
   */
  TileSize getMaximumTileSize(Point originPoint);
};

#endif