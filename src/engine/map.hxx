#ifndef MAP_HXX_
#define MAP_HXX_

#include <vector>

#include "GameObjects/mapNode.hxx"

class Map
{
public:
  //fixed MapNode* array to store neighbors.
  using NeighborMatrix = MapNode * [9];

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

  void highlightNode(const Point &isoCoordinates);

  Point findNodeInMap(const SDL_Point &screenCoordinates) const;

  void setTileIDOfNode(const Point &isoCoordinates, const std::string &tileType);

  void demolishNode(const Point &isoCoordinates, bool updateNeighboringTiles = 0);
  void refresh();

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

  std::vector<std::unique_ptr<MapNode>> mapNodes;
  std::vector<MapNode *> mapNodesInDrawingOrder;

private:
  MapNode *m_highlitNode = nullptr;

  int m_columns;
  int m_rows;

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

  SDL_Color getColorOfPixelInSurface(SDL_Surface *surface, int x, int y, const SDL_Rect &clipRect) const;

  bool isClickWithinTile(const SDL_Point &screenCoordinates, int isoX, int isoY) const;
};

#endif
