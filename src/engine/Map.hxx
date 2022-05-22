#ifndef MAP_HXX_
#define MAP_HXX_

#include <vector>
#include <random>

#include "GameObjects/MapNode.hxx"
#include "map/TerrainGenerator.hxx"
#include "../game/GamePlay.hxx"
#include "PointFunctions.hxx"
// #include "basics/signal.hxx"
#include <Signal.hxx>
#include <SignalMediator.hxx>

struct NeighborNode
{
  /// pointer to the mapNode
  MapNode *pNode;
  /// Enum of the position of this node the origin node
  NeighborNodesPosition position;
};

class Map
{
public:
  Map(int columns, int rows, const bool generateTerrain);
  Map(int columns, int rows);
  ~Map();
  Map(Map &other) = delete;
  Map &operator=(const Map &other) = delete;
  Map(Map &&fp) = delete;
  Map const &operator=(Map &&fp) = delete;

  // MapFunction needs to access private functions
  friend class MapFunctions;

  /** \brief Render the elements contained in the Map
    * @details call the render() function of the sprite in the all contained MapNode elements
    * @see Sprite#render
    */
  void renderMap() const;

private:
  /**
   * @brief Refresh all the map tile textures
   *
   * @see Sprite#refresh
   */
  void refresh();

  /** \brief Get pointer to a single mapNode at specific iso coordinates.
  * @param isoCoords The node to retrieve.
  */
  MapNode &getMapNode(Point isoCoords) { return mapNodes[isoCoords.toIndex()]; };

  /** \brief Get all mapnodes as a vector
   */
  const std::vector<MapNode> &getMapNodes() { return mapNodes; };

  // private:

  /** \brief For implementing frustum culling, find all map nodes which are visible on the screen. Only visible nodes will be rendered.
  */
  void calculateVisibleMap(void);

  std::vector<MapNode> mapNodes;
  std::vector<MapNode *> mapNodesInDrawingOrder;
  Sprite **pMapNodesVisible;
  int m_visibleNodesCount = 0;
  int m_columns;
  int m_rows;
  std::default_random_engine randomEngine;
  TerrainGenerator m_terrainGen;

  static const size_t m_saveGameVersion;
};

#endif
