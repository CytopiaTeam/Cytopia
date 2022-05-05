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
  Map(int columns, int rows, const bool generateTerrain = true);
  ~Map();
  Map(Map &other) = delete;
  Map &operator=(const Map &other) = delete;
  Map(Map &&fp) = delete;
  Map const &operator=(Map &&fp) = delete;

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

  /** \brief Render the elements contained in the Map
    * @details call the render() function of the sprite in the all contained MapNode elements
    * @see Sprite#render
    */
  void renderMap() const;

  /**
 * @brief Sets a node to be highlit
 * @details This sets a node to be highlit, the highlighting is done during rendering
 * @param isoCoordinates which node should be highlit.
 * @param rgbColor The SpriteRGBColor that should be used for highlighting
 */
  void highlightNode(const Point &isoCoordinates, const SpriteRGBColor &rgbColor);

  /**
 * @brief Sets a node to be unhighlit
 * @details This sets a node to be unhighlit, the highlighting is done during rendering
 * @param isoCoordinates which node should be unhighlit.
 */
  void unHighlightNode(const Point &isoCoordinates);

  /**
 * @brief Returns the node at given screen coordinates
 *
 * @param screenCoordinates
 * @return Point
 */
  Point findNodeInMap(const SDL_Point &screenCoordinates, const Layer &layer = Layer::NONE);

  /**
 * @brief Set the Tile ID Of Node object
 * @details Also invokes all necessary texture updates (auto-tiling, slopes, ...)
 * @param tileID the new tileID to set
 * @param coordinate Point where the tileID which should be set
 */
  void setTileID(const std::string &tileID, Point coordinate);

  /**
 * @brief Set the Tile ID Of multiple Node objects
 * @details Also invokes all necessary texture updates (auto-tiling, slopes, ...)
 * @param tileID the new tileID to set
 * @param coordinates a vector of Points where the tileIDs which should be set
 */
  void setTileID(const std::string &tileID, const std::vector<Point> &coordinates);

  /**
 * @brief Demolish a node
 * @details This function gathers all tiles that should be demolished and invokes the nodes demolish function. When a building bigger than 1x1 is selected, all it's coordinates are added to the demolishing points.
 * @param isoCoordinates all coordinates that should be demolished
 * @param updateNeighboringTiles whether the adjacent tiles should be updated. (only relevant for autotiling)
 * @param layer restrict demolish to a single layer
 * @see MapNode#demolishNode
 */
  void demolishNode(const std::vector<Point> &isoCoordinates, bool updateNeighboringTiles = false, Layer layer = Layer::NONE);

  /**
   * @brief Refresh all the map tile textures
   *
   * @see Sprite#refresh
   */
  void refresh();

  /**
   * @brief Get original corner point of given point within building borders.
   * @param isoCoordinates Point to get corner point of
   * @param layer
   * @returns original corner point
   */
  Point getNodeOrigCornerPoint(const Point &isoCoordinates, Layer layer = Layer::NONE);

  /** \brief Save Map to file
  * @details Serializes the Map class to json and writes the data to a file.
  * @param fileName The file the map should be written to
  */
  void saveMapToFile(const std::string &fileName);

  /** \brief Load Map from file
  * @details Deserializes the Map class from a json file, creates a new Map and returns it.
  * @param fileName The file the map should be written to
  * @returns Map* Pointer to the newly created Map.
  */
  static Map *loadMapFromFile(const std::string &fileName);

  /**
 * @brief Debug MapNodeData to Console
 * Used as Tile-Inspector until we implement a GUI variant
 * @param isoCoordinates Tile to inspect
 */
  void getNodeInformation(const Point &isoCoordinates) const;

  /** \brief check if Tile is occupied
  * @param isoCoordinates Tile to inspect
  * @param tileID tileID which should be checked
  */
  bool isPlacementOnNodeAllowed(const Point &isoCoordinates, const std::string &tileID) const;

  /** \brief get Tile ID of specific layer of specific iso coordinates
  * @param isoCoordinates Tile to inspect
  * @param layer layer to check.
  */
  std::string getTileID(const Point &isoCoordinates, Layer layer);

  /** \brief Get pointer to a single mapNode at specific iso coordinates.
  * @param isoCoords The node to retrieve.
  */
  MapNode &getMapNode(Point isoCoords) { return mapNodes[isoCoords.toIndex()]; };

  /** \brief Get all mapnodes as a vector
   */
  const std::vector<MapNode> &getMapNodes() { return mapNodes; };

private:
  /** \brief Update all mapNodes
  * @details Updates all mapNode and its adjacent tiles regarding height information, draws slopes for adjacent tiles and
  * sets tiling for mapNode sprite if applicable
  */
  void updateAllNodes();

  /** \brief Get a bitmask that represents same-tile neighbors
  * @details Checks all neighboring tiles and returns the elevated neighbors in a bitmask:
  * [ BR BL TR TL  R  L  B  T ]
  * [ 0  0  0  0   0  0  0  0 ]
  * @param coordinate Point on the map to calculate bitmask for.
  * @return Uint that represents a bitmask of the neighbor tiles and their elevation to the center coordinate
  */
  std::vector<uint8_t> calculateAutotileBitmask(Point coordinate);

  SDL_Color getColorOfPixelInSurface(SDL_Surface *surface, int x, int y) const;

  bool isClickWithinTile(const SDL_Point &screenCoordinates, Point isoCoordinate, const Layer &layer) const;

  /** \brief Filter out tiles which should not be set over existing one.
  * @param layer Layer in which tileId should be set.
  * @param pMapNode pointer to the MapNode which ID should be set.
  * @return true in case that tileId is allowed to be set, otherwise false (filter it out).
  */
  bool isAllowSetTileId(const Layer layer, const MapNode *const pMapNode);

  /** \brief Get all neighbor nodes from provided map node.
  * @param isoCoordinates iso coordinates.
  * @param includeCentralNode if set to true include the central node in the result.
  * @return All neighbor nodes.
  */
  std::vector<NeighborNode> getNeighborNodes(const Point &isoCoordinates, const bool includeCentralNode);

  /** \brief Change map node height.
  * @param isoCoordinates the Point on the map node to change height.
  * @param elevate if set to true make node higher, otherwise lower.
  */
  void changeHeight(const Point &isoCoordinates, const bool elevate);

  /** \brief Update the nodes and all affected node with the change.
  * @param nodes vector of coordinates to be updated.
  */
  void updateNodeNeighbors(std::vector<Point> nodes);

  /** \brief Get elevated bit mask of the map node.
  * @param centerCoordinates Point on the map node to calculate elevated bit mask.
  * @return Map node elevated bit mask.
  */
  unsigned char getElevatedNeighborBitmask(Point centerCoordinates);

  /** \brief Change map node height.
  * @param coordinate the Point on the map node to change height
  * @param elevate if set to true make node higher, otherwise lower.
  * @return true in case that height has been changed, otherwise false.
  */
  bool updateHeight(Point coordinate, const bool elevate);

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
