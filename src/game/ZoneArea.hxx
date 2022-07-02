#ifndef ZONE_AREA_HXX_
#define ZONE_AREA_HXX_

#include "../engine/basics/Point.hxx"
#include "../engine/basics/tileData.hxx"
#include "../engine/GameObjects/MapGrid.hxx"

struct ZoneNode
{
  Point coordinate = Point::INVALID();
  ZoneType zoneType;
  ZoneDensity zoneDensity;
  bool occupied = false;
};

class ZoneArea;
void mergeZoneAreas(ZoneArea &mainZone, ZoneArea &toBeMerged);

class ZoneArea : public MapGrid<ZoneNode>
{
public:
  ZoneArea(ZoneNode zoneNode);

  /**
   * @brief Add a zoneNode to this zoneArea
   * 
   * @param zoneNode ZoneNode to add
   */
  void addNode(ZoneNode zoneNode) override;

  /**
   * @brief Remove a zoneNode on a given coordinate from this zoneArea
   * 
   * @param coordinate Coordinates of the mapNode with a zone tile
   */
  void removeZoneNode(Point coordinate);

  /**
   * @brief Set the power supply for this Area
   * 
   * @param hasPower whether the area has power supply or not
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
   * @param hasWater whether the area has water supply or not
   */
  void setWaterSupply(bool hasWater) { m_hasWater = hasWater; };

  /**
   * @brief Get the water supply for this Area
   * 
   * @return if the area has water supply
   */
  bool hasWaterSupply() { return m_hasWater; };

  /**
   * @brief Get the zone for this Area
   * 
   * @return zone for this area
   */
  ZoneType getZone() const { return m_zoneType; };

  /**
   * @brief Get the zone density for this Area
   * 
   * @return zone density for this area
   */
  ZoneDensity getZoneDensity() { return m_zoneDensity; };

  /**
   * @brief If this area has unoccupied nodes left
   * 
   * @return if this zoneArea is vacant or not
   */
  bool isVacant() const { return m_isVacant; };

  /**
   * @brief Spawn buildings on nodes in this area if all demands are fulfilled
   */
  void spawnBuildings();

  /**
   * @brief Check if a given point is within the boundaries of this zone area
   * 
   * @param coordinate point to check
   * @return if a given point is with the boundaries of this zone area
   */
  bool isWithinBoundaries(Point coordinate) const
  {
    return (xmin <= coordinate.x) && (xmax >= coordinate.x) && (ymin <= coordinate.y) && (ymax >= coordinate.y);
  }

  /**
   * @brief Set vacancy for this tile
   * 
   * @param coordinate What tile to vacate / occupy
   * @param vacancy true if vacant, false if occupied
   */
  void setVacancy(Point coordinate, bool vacancy);

private:
  ZoneType m_zoneType;
  ZoneDensity m_zoneDensity;

  bool m_hasPower = false;
  bool m_hasWater = false;
  bool m_isVacant = false;
  int xmin, xmax, ymin, ymax;

  /// internal function to check for vacancy
  bool checkVacancy() const;

  friend void mergeZoneAreas(ZoneArea &mainZone, ZoneArea &toBeMerged);

  /**
   * @brief Returns the possible size of buildings that can be placed on this coordinate in a zone
   * 
   * @param originPoint coordinate where we want to know how many free zone tiles there are next to it
   * @return struct with height and with for the possible tilesize that can be placed on this coordinate
   */
  TileSize getMaximumTileSize(Point originPoint);
};

#endif
