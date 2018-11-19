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

  MapNode *getNode(int x, int y) { return _mapNodes[x * _columns + y]; };

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
  void renderMap();

  Point findNodeInMap(const Point &screenCoordinates, float zoomLevel) const;

  void setTileTypeOfNode(const Point &isoCoordinates, const std::string &tileType);

  void demolishNode(const Point &isoCoordinates);
  void refresh();

private:
  std::vector<MapNode *> _mapNodes;
  std::vector<MapNode *> _mapNodesInDrawingOrder;

  int _columns;
  int _rows;

  void updateNeighbors(const Point &isoCoordinates);

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
};

#endif
