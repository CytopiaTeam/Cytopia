#ifndef MAP_HXX_
#define MAP_HXX_

#include <vector>
#include <random>

#include "GameObjects/MapNode.hxx"
#include "map/TerrainGenerator.hxx"

class Map
{
private:
  std::vector<std::unique_ptr<MapNode>> mapNodes;
  std::vector<MapNode *> mapNodesInDrawingOrder;
  int m_columns;
  int m_rows;
  std::default_random_engine randomEngine;
  TerrainGenerator m_terrainGen;

  static const size_t m_saveGameVersion;

public:
  //fixed MapNode* array to store neighbors.
  using NeighborMatrix = MapNode *[9];

  Map() = delete;
  Map(int columns, int rows);
  ~Map() = default;

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
  Point findNodeInMap(const SDL_Point &screenCoordinates) const;

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
    bool isOkToSet = true;
    for (Iterator it = begin; it != end; ++it)
    {
      if (!isPlacementOnNodeAllowed(*it, tileID))
      {
        isOkToSet = false;
        break;
      }
    }

    if (isOkToSet)
    {
      int groundtileIndex = -1;
      for (auto it = begin; it != end; ++it)
      {
        bool shouldRender = !(!isMultiObjects && it != begin);
        Layer layer = TileManager::instance().getTileLayer(tileID);
        const auto pCurrentMapNode = mapNodes[it->x * m_columns + it->y].get();

        if (!isAllowSetTileId(layer, pCurrentMapNode))
        {
          continue;
        }

        // only demolish nodes before placing if this is a bigger than 1x1 building
        if (!isMultiObjects)
        {
          demolishNode(std::vector<Point>{*it}, 0, Layer::BUILDINGS);
        }

        pCurrentMapNode->setRenderFlag(layer, shouldRender);
        pCurrentMapNode->setTileID(tileID, isMultiObjects ? *it : *begin);
        if (pCurrentMapNode->getMapNodeDataForLayer(layer).tileData &&
            !pCurrentMapNode->getMapNodeDataForLayer(layer).tileData->groundDecoration.empty() && groundtileIndex == -1)
        {
          const int groundDecoSize = pCurrentMapNode->getMapNodeDataForLayer(layer).tileData->groundDecoration.size();
          std::uniform_int_distribution uniformDistribution(0, groundDecoSize - 1);
          groundtileIndex = uniformDistribution(randomEngine);
        }
        if (groundtileIndex != -1)
        {
          pCurrentMapNode->setTileID(pCurrentMapNode->getMapNodeDataForLayer(layer).tileData->groundDecoration[groundtileIndex],
                                     isMultiObjects ? *it : *begin);
        }
        //For layers that autotile to each other, we need to update their neighbors too
        if (MapNode::isDataAutoTile(TileManager::instance().getTileData(tileID)))
        {
          updateNeighborsOfNode(*it);
        }
      }
    }
  }

  /**
 * @brief Demolish a node
 * This function gathers all tiles that should be demolished and invokes the nodes demolish function. When a building bigger than 1x1 is selected, all it's coordinates are added to the demolishing points.
 * @param isoCoordinates all coordinates that should be demolished
 * @param updateNeighboringTiles wether the adjecent tiles should be updated. (only relevant for autotiling)
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

  /**
 * @brief whether a node is a multiobject or not (bigger than 1x1 building)
 * @param isoCoordinates Tile to inspect
 * @param layer Which layer to inspect (default BUILDINGS)
 */
  bool isNodeMultiObject(const Point &isoCoordinates, Layer layer = Layer::BUILDINGS);

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

  /** \Get a single mapNode at specific iso coordinates
  * @param isoCoordinates: The node to retrieve
  */
  const MapNode *getMapNode(Point isoCoords) const { return mapNodes[isoCoords.x * m_columns + isoCoords.y].get(); };

  bool isLayerAutoTile(const Point &isoCoordinates, const Layer &layer) const;

private:
  /** \brief Initialize the Map with node objects
    * Initialize the Map with node objects
    */
  void initMap();

  /**\brief Update mapNode and its adjacent tiles
  * Updates mapNode height information, draws slopes for adjacent tiles and sets tiling for mapNode sprite if applicable
  * @param isoCoordinates - isometric coordinates of the tile that should be updated.
  */
  void updateNeighborsOfNode(const Point &isoCoordinates);

  /**\brief Update all mapNodes
  * Updates all mapNode and its adjacent tiles regarding height information, draws slopes for adjacent tiles and
  * sets tiling for mapNode sprite if applicable
  */
  void updateAllNodes();

  /**\brief Get neighbor MapNode Objects
    * Stores pointers to the neighboring nodes of the given coordinates in the passed parameter.
    * @param isoCoordinates - isometric coordinates of the tile that's neighbors should be retrieved.
    * @param result - Pass a ref of type NeighborMatrix to store the found neighbors in.
    */
  void getNeighbors(const Point &isoCoordinates, NeighborMatrix &result) const;

  /** \brief Get elevated neighbor positions in a bitmask
  * Checks all neighboring tiles and returns the elevated neighbors in a bitmask:
  * [ BR BL TR TL  R  L  B  T ]
  * [ 0  0  0  0   0  0  0  0 ]
  * @param isoCoordinates isometric coordinates of the tile whose neighbors should be retrieved
  * @returns  Uint that stores the elevated neighbor tiles
  */
  unsigned char getElevatedNeighborBitmask(const Point &isoCoordinates);

  /** \brief Get a bitmask that represents same-tile neighbors
  * Checks all neighboring tiles and returns the elevated neighbors in a bitmask:
  * [ BR BL TR TL  R  L  B  T ]
  * [ 0  0  0  0   0  0  0  0 ]
  * @param isoCoordinates isometric coordinates of the tile whose neighbors should be retrieved
  * @returns  Uint that stores the neighbor tiles
  */
  std::vector<uint8_t> calculateAutotileBitmask(const Point &isoCoordinates);

  SDL_Color getColorOfPixelInSurface(SDL_Surface *surface, int x, int y) const;

  bool isClickWithinTile(const SDL_Point &screenCoordinates, int isoX, int isoY) const;

  /* \brief Filter out tiles which should not be set over existing one.
  * @param layer Layer in which tileId should be set.
  * @param pMapNode pointer to the MapNode which ID should be set.
  * @return true in case that tileId is allowed to be set, otherwise false (filter it out).
  */
  bool isAllowSetTileId(const Layer layer, const MapNode *const pMapNode);
};

#endif
