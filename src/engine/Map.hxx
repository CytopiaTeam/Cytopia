#ifndef MAP_HXX_
#define MAP_HXX_

#include <vector>
#include <random>

#include "GameObjects/MapNode.hxx"
#include "map/TerrainGenerator.hxx"

/** \brief Position of the surrounding nodes and its bit mask values.
  */
BETTER_ENUM(NeighbourNodesPosition, unsigned char, BOTOM_LEFT = 1U << 6, LEFT = 1U << 2, TOP_LEFT = 1U << 4, BOTTOM = 1U << 1,
            CENTAR = 0U, TOP = 1U, BOTOM_RIGHT = 1U << 7, RIGHT = 1U << 3, TOP_RIGHT = 1U << 5);

struct NeighborNode
{
  MapNode *pNode;
  NeighbourNodesPosition position;
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
    * Increases the height of the node and checks the surrounding tiles. Either draw a slope sprite or elevate the tile if
    * necessary.
    * @param isoCoordinates the isometric coordinates of the tile that should be elevated
    */
  void increaseHeight(const Point &isoCoordinates);

  /** \brief Decrease Height
    * Decreases the height of the node and checks the surrounding tiles. Either draw a slope sprite or lower the tile if
    * necessary.
    * @param isoCoordinates the isometric coordinates of the tile that should be elevated
    */
  void decreaseHeight(const Point &isoCoordinates);

  /** \Brief Render the elements contained in the Map
    * call the render() function of the sprite in the all contained MapNode elements
    * @see Sprite#render
    */
  void renderMap() const;

  /**
 * @brief Sets a node to be highlit
 * This sets a node to be highlit, the highlighting is done during rendering
 * @param isoCoordinates which node should be highlit.
 * @param rgbColor The SpriteRGBColor that should be used for highlighting
 */
  void highlightNode(const Point &isoCoordinates, const SpriteRGBColor &rgbColor);

  /**
 * @brief Sets a node to be unhighlit
 * This sets a node to be unhighlit, the highlighting is done during rendering
 * @param isoCoordinates which node should be unhighlit.
 */
  void unHighlightNode(const Point &isoCoordinates);

  /**
 * @brief Returns the node at given screencoordinates
 *
 * @param screenCoordinates
 * @return Point
 */
  Point findNodeInMap(const SDL_Point &screenCoordinates, const Layer &layer = Layer::NONE) const;

  /**
 * @brief Set the Tile ID Of Node object
 * Also invokes all necessary texture updates (auto-tiling, slopes, ...)
 * @param isoCoordinates
 * @param tileID tileID which should be set
 */
  template <typename Iterator>
  void setTileIDOfNode(const Iterator &begin, const Iterator &end, const std::string &tileID, bool isMultiObjects)
  {
    static_assert(std::is_same_v<Point, typename std::iterator_traits<Iterator>::value_type>,
                  "Iterator value must be a const Point");

    std::vector<MapNode *> nodesToBeUpdated;

    for (Iterator it = begin; it != end; ++it)
    {
      if (!isPlacementOnNodeAllowed(*it, tileID))
      {
        return;
      }
    }

    int groundtileIndex = -1;
    for (auto it = begin; it != end; ++it)
    {
      const bool shouldRender = !(!isMultiObjects && (it != begin));
      Layer layer = TileManager::instance().getTileLayer(tileID);
      MapNode &currentMapNode = mapNodes[nodeIdx(it->x, it->y)];

      if (!isAllowSetTileId(layer, &currentMapNode))
      {
        continue;
      }

      // only demolish nodes before placing if this is a bigger than 1x1 building
      if (!isMultiObjects)
      {
        demolishNode(std::vector<Point>{*it}, 0, Layer::BUILDINGS);
      }

      currentMapNode.setRenderFlag(layer, shouldRender);
      currentMapNode.setTileID(tileID, isMultiObjects ? *it : *begin);
      auto pTileData = currentMapNode.getMapNodeDataForLayer(layer).tileData;

      if (pTileData && !pTileData->groundDecoration.empty() && groundtileIndex == -1)
      {
        const int groundDecoSize = pTileData->groundDecoration.size();
        std::uniform_int_distribution uniformDistribution(0, groundDecoSize - 1);
        groundtileIndex = uniformDistribution(randomEngine);
      }

      if (groundtileIndex != -1)
      {
        currentMapNode.setTileID(pTileData->groundDecoration[groundtileIndex], isMultiObjects ? *it : *begin);
      }

      //For layers that autotile to each other, we need to update their neighbors too
      if (MapNode::isDataAutoTile(TileManager::instance().getTileData(tileID)))
      {
        nodesToBeUpdated.push_back(&currentMapNode);
      }
    }

    if (!nodesToBeUpdated.empty())
    {
      updateNodeNeighbors(nodesToBeUpdated);
    }
  }

  /**
 * @brief Demolish a node
 * This function gathers all tiles that should be demolished and invokes the nodes demolish function. When a building bigger than 1x1 is selected, all it's coordinates are added to the demolishing points.
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
   */
  Point getNodeOrigCornerPoint(const Point &isoCoordinates, Layer layer = Layer::NONE);

  /** \Brief Save Map to file
  * Serializes the Map class to json and writes the data to a file.
  * @param fileName The file the map should be written to
  */
  void saveMapToFile(const std::string &fileName);

  /** \Brief Load Map from file
  * Deserializes the Map class from a json file, creates a new Map and returns it.
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

  /** \Brief check if Tile is occupied
  * @param isoCoordinates Tile to inspect
  * @param tileID tileID which should be checked
  */
  bool isPlacementOnNodeAllowed(const Point &isoCoordinates, const std::string &tileID) const;

  /** \brief Return vector of Points of an Object Tiles selection.
  *
  */
  std::vector<Point> getObjectCoords(const Point &isoCoordinates, const std::string &tileID);

  /** \Brief get Tile ID of specific layer of specific iso coordinates
  * @param isoCoordinates: Tile to inspect
  * @param layer: layer to check.
  */
  std::string getTileID(const Point &isoCoordinates, Layer layer);

  /** \brief Get pointer to a single mapNode at specific iso coordinates.
  * @param isoCoordinates: The node to retrieve.
  */
  const MapNode *getMapNode(Point isoCoords) const { return &mapNodes[nodeIdx(isoCoords.x, isoCoords.y)]; };

  /**
   * @brief Sets the Window
   * @todo  Remove this when the New UI is complete
   */
  static void setWindow(class Window *);

private:
  /**\brief Update all mapNodes
  * Updates all mapNode and its adjacent tiles regarding height information, draws slopes for adjacent tiles and
  * sets tiling for mapNode sprite if applicable
  */
  void updateAllNodes();

  /** \brief Get a bitmask that represents same-tile neighbors
  * Checks all neighboring tiles and returns the elevated neighbors in a bitmask:
  * [ BR BL TR TL  R  L  B  T ]
  * [ 0  0  0  0   0  0  0  0 ]
  * @param pMapNode Pointer to the map node to calculate mask for.
  * @param neighborNodes Neighbor nodes.
  * @return Uint that stores the neighbor tiles
  */
  std::vector<uint8_t> calculateAutotileBitmask(const MapNode *const pMapNode, const std::vector<NeighborNode> &neighborNodes);

  SDL_Color getColorOfPixelInSurface(SDL_Surface *surface, int x, int y) const;

  bool isClickWithinTile(const SDL_Point &screenCoordinates, int isoX, int isoY, const Layer &layer) const;

  /* \brief Filter out tiles which should not be set over existing one.
  * @param layer Layer in which tileId should be set.
  * @param pMapNode pointer to the MapNode which ID should be set.
  * @return true in case that tileId is allowed to be set, otherwise false (filter it out).
  */
  bool isAllowSetTileId(const Layer layer, const MapNode *const pMapNode);

  /* \brief Calculate map index from coordinates.
  * @param x x coordinate.
  * @param y y coordinate.
  * @return Index of map node.
  */
  inline int nodeIdx(const int x, const int y) const { return x * m_columns + y; }

  /* \brief Get all neighbor nodes from provided map node.
  * @param isoCoordinates iso coordinates.
  * @param includeCentralNode if set to true include the central node in the result.
  * @return All neighbor nodes.
  */
  std::vector<NeighborNode> getNeighborNodes(const Point &isoCoordinates, const bool includeCentralNode);

  /* \brief Change map node height.
  * @param isoCoordinates iso coordinates.
  * @param higher if set to true make node higher, otherwise lower.
  */
  void changeHeight(const Point &isoCoordinates, const bool higher);

  /* \brief Update the nodes and all affected node with the change.
  * @param nodes Nodes which have to be updated.
  */
  void updateNodeNeighbors(std::vector<MapNode *> &nodes);

  /* \brief Get elevated bit mask of the map node.
  * @param pMapNode Pointer to the map node to calculate elevated bit mask.
  * @param neighbors All neighbor map nodes.
  * @return Map node elevated bit mask.
  */
  unsigned char getElevatedNeighborBitmask(MapNode *pMapNode, const std::vector<NeighborNode> &neighbors);

  /* \brief Change map node height.
  * @param mapNode Map node to change height.
  * @param higher if set to true make node higher, otherwise lower.
  * @param neighbors All neighbor map nodes.
  * @return true in case that height has been changed, otherwise false.
  */
  bool updateHeight(MapNode &mapNode, const bool higher, std::vector<NeighborNode> &neighbors);

  /* \brief For implementing frustum culling, find all map nodes which are visible on the screen. Only visible nodes will be rendered.
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
  static class Window * m_Window;
  static const size_t m_saveGameVersion;
};

#endif
