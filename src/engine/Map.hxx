#ifndef MAP_HXX_
#define MAP_HXX_

#include <vector>

#include "GameObjects/MapNode.hxx"
#include "map/TerrainGenerator.hxx"

class Map
{
public:
  //fixed MapNode* array to store neighbors.
  using NeighborMatrix = MapNode * [9];

  std::vector<std::unique_ptr<MapNode>> mapNodes;
  std::vector<MapNode *> mapNodesInDrawingOrder;

  Map() = default;
  Map(int columns, int rows);
  ~Map() = default;

  /** \brief Initialize the Map with node objects
    * Initialize the Map with node objects
    */
  void initMap();

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
 * @param redHighlight should highlight it with red or gray color.
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
      Point origPoint = *begin;
      std::string id = tileID;
      auto it = begin;
      if (!isMultiObjects)
      {
        mapNodes[it->x * m_columns + it->y]->setTileID(tileID, origPoint);
        it++;
        id = "demy_node";
      }
      for (; it != end; ++it)
      {
        mapNodes[it->x * m_columns + it->y]->setTileID(id, isMultiObjects ? *it : origPoint);
        updateNeighborsOfNode(*it);
      }
    }
  }

  /**
 * @brief Demolish a node
 * Invokes the tiles demolish function
 * @param isoCoordinates 
 * @param updateNeighboringTiles 
 * @see MapNode#demolishNode
 */
  void demolishNode(const Point &isoCoordinates, bool updateNeighboringTiles = false);

  /**
   * @brief Refresh all the map tile textures
   * 
   * @see Sprite#refresh
   */
  void refresh();

  /**
   * @brief Get original corner point of given point within building borders.
   */
  Point getNodeOrigCornerPoint(const Point &isoCoordinates);

  /** \Brief Save Map to file
  * Serializes the Map class to json and writes the data to a file.
  * @param string fileName The file the map should be written to
  */
  void saveMapToFile(const std::string &fileName);

  /** \Brief Load Map from file
  * Deserializes the Map class from a json file, creates a new Map and returns it.
  * @param string fileName The file the map should be written to
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

private:
  int m_columns;
  int m_rows;

  TerrainGenerator terrainGen;

  static const size_t m_saveGameVersion;

  /**\brief Update mapNode and its adjacent tiles
  * Updates mapNode height information, draws slopes for adjacent tiles and sets tiling for mapNode sprite if applicable
  * @param Point isoCoordinates - isometric coordinates of the tile that should be updated.
  */
  void updateNeighborsOfNode(const Point &isoCoordinates);

  /**\brief Update all mapNodes 
  * Updates all mapNode and its adjacent tiles regarding height information, draws slopes for adjacent tiles and 
  * sets tiling for mapNode sprite if applicable
  */
  void updateAllNodes();

  /**\brief Get neighbor MapNode Objects
    * Stores pointers to the neighboring nodes of the given coordinates in the passed parameter.
    * @param Point isoCoordinates - isometric coordinates of the tile that's neighbors should be retrieved.
    * @param NeighborMatrix result - Pass a ref of type neighbormatrix to store the found neighbors in.
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
  unsigned char getNeighboringTilesBitmask(const Point &isoCoordinates);

  SDL_Color getColorOfPixelInSurface(SDL_Surface *surface, int x, int y) const;

  bool isClickWithinTile(const SDL_Point &screenCoordinates, int isoX, int isoY) const;
};

#endif
