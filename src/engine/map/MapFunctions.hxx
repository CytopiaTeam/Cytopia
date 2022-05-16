#ifndef MAPFUNCTIONS_HXX_
#define MAPFUNCTIONS_HXX_

// #include "WindowManager.hxx"
// #include "basics/point.hxx"
#include "../Map.hxx"
// #include "../../util/Singleton.hxx"
#include <Singleton.hxx>
#include <Point.hxx>

class MapFunctions : public Singleton<MapFunctions>
{
public:
  Map *getMap() { return m_map; };
  void registerMap(Map *map) { m_map = map; };

  /** \brief Increase Height
    * @details Increases the height of the node and checks the surrounding tiles. Either draw a slope sprite or elevate the tile if
    * necessary.
    * @param isoCoordinates the isometric coordinates of the tile that should be elevated
    */
  void increaseHeight(const Point &isoCoordinates);

  /** \brief Decrease Height
    * @details Decreases the height of the node and checks the surrounding tiles. Either draw a slope sprite or lower the tile if
    * necessary.
    * @param isoCoordinates the isometric coordinates of the tile that should be lowered
    */
  void decreaseHeight(const Point &isoCoordinates);

  /** \brief Change map node height.
  * @param isoCoordinates the Point on the map node to change height.
  * @param elevate if set to true make node higher, otherwise lower.
  */
  void changeHeight(const Point &isoCoordinates, const bool elevate);

  /** \brief Change map node height.
  * @param coordinate the Point on the map node to change height
  * @param elevate if set to true make node higher, otherwise lower.
  * @return true in case that height has been changed, otherwise false.
  */
  bool updateHeight(Point coordinate, const bool elevate);

private:
  Map *m_map;

    /** \brief Update the nodes and all affected node with the change.
  * @param nodes vector of coordinates to be updated.
  */
  void updateNodeNeighbors(std::vector<Point> nodes);

    /** \brief Update all mapNodes
  * @details Updates all mapNode and its adjacent tiles regarding height information, draws slopes for adjacent tiles and
  * sets tiling for mapNode sprite if applicable
  */
  void updateAllNodes();
};

#endif