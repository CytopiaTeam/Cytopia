#ifndef VECTORMATRIX_HXX_
#define VECTORMATRIX_HXX_

#include <vector>

#include "../cell.hxx"

class vectorMatrix
{
public:
  //fixed Cell* array to store neighbors.
  using NeighborMatrix = Cell * [9];

  vectorMatrix() = default;
  vectorMatrix(int columns, int rows);
  ~vectorMatrix() = default;

  Cell *getCell(int x, int y) { return _cellMatrix[x * _columns + y]; };

  /** \brief Initialize the vecotrMatrix with cell objects
    * Initialize the vectorMatrix with cell objects
    */
  void initCells();

  /** \brief Increase Height
    * Increases the height of the cell and checks the surrounding tiles. Either draw a slope sprite or elevate the tile if 
    * necessary. 
    * @param isoCoordinates the isometric coordinates of the tile that should be elevated
    */
  void increaseHeightOfCell(const Point &isoCoordinates);

  /** \brief Decrease Height
    * Decreases the height of the cell and checks the surrounding tiles. Either draw a slope sprite or lower the tile if
    * necessary.
    * @param isoCoordinates the isometric coordinates of the tile that should be elevated
    */
  void decreaseHeightOfCell(const Point &isoCoordinates);

  /** \brief Get elevated neighbor positions in a bitmask
    * Checks all neighboring tiles and returns the elevated neighbors in a bitmask: 
    * [ T B L R TL TR BL BR ]
    * [ 0 0 0 0  0  0  0  0 ]
    * @param isoCoordinates isometric coordinates of the tile whose neighbors should be retrieved
    * @returns  Uint that stores the elevated neighbor tiles
    */
  unsigned int getElevatedNeighborBitmask(const Point &isoCoordinates);

  /** \Brief Render the elements contained in the matrixx
    * call the render() function of the sprite in the all contained Cell elements
    * @see Sprite#render
    */
  void renderMatrix();

  void refresh();

private:
  std::vector<Cell *> _cellMatrix;
  std::vector<Cell *> _cellMatrixDrawingOrder;

  int _columns;
  int _rows;

  void updateNeightbors(const Point &isoCoordinates);

  /**\brief Get neighbor Cell Objects
    * Stores pointers to the neighboring cells of the given coordinates in the passed parameter.
    * @param Point isoCoordinates - isometric coordinates of the tile that's neighbors should be retrieved.
    * @param NeighborMatrix result - Pass a ref of type neighbormatrix to store the found neighbors in.
    */
  void getNeighbors(const Point &isoCoordinates, NeighborMatrix &result) const;
};

#endif
