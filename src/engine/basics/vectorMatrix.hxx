#ifndef VECTORMATRIX_HXX_
#define VECTORMATRIX_HXX_

#include <vector>
#include "../sprite.hxx"
#include "../cell.hxx"

class vectorMatrix
{
public:
  vectorMatrix() = default;
  vectorMatrix(int columns, int rows);
  ~vectorMatrix() = default;
  
  // CELL
  void addCell(int x, int y, int z) { _cellMatrix[x * _columns + y] = std::shared_ptr<Cell>(new Cell(Point(x, y, z))); };
  std::shared_ptr<Cell> getCell(int x, int y) { return _cellMatrix[x * _columns + y]; };
  std::vector<std::shared_ptr<Cell>> getCellNeighbors(int x, int y);

  /** \brief Initialize the vecotrMatrix with cell objects
  * Initialize the vectorMatrix with cell objects
  */
  void initCells();

  /** \brief Increase Height
    * Increases the height of the cell and checks the surrounding tiles. Either draw a slope sprite or elevate the tile if 
    * necessary. 
    * @param isoCoordinates the isometric coordinates of the tile that should be elevated
    */
  void increaseHeightOfCell(Point isoCoordinates);

  /** \brief Decrease Height
    * Decreases the height of the cell and checks the surrounding tiles. Either draw a slope sprite or lower the tile if
    * necessary.
    * @param isoCoordinates the isometric coordinates of the tile that should be elevated
    */
  void decreaseHeightOfCell(Point isoCoordinates);
  
  /** \brief set tileID for each neighbor of given coordinates
    * After a cell is raised / lowered, each neighbor must check which tileID it should have
    * This function should not be called directly
    * @param The isocoordinates of the cell that should have all of it's neighbors checked.
    * @see Cell#drawSurroundingTiles
    */
  void drawSurroundingTiles(Point isoCoordinates);
  
  /** \brief: determine which tile ID should be drawn for this cell
    * Checks all the neighbors and determines the tile ID of this mapcell according to it's
    * elevated / lowered neighbors.
    * This function should not be called directly
    * @param isoCoordinates the isometric coordinates of the cell whose tile ID should be determined.
    * @see vectorMatrix#drawSurroundingTiles
    */
  void determineTile(Point isoCoordinates);

  /** \brief Get elevated neighbor positions in a bitmask
    * Checks all neighboring tiles and returns the elevated neighbors in a bitmask: 
    * [ T B L R TL TR BL BR ]
    * [ 0 0 0 0  0  0  0  0 ]
    * @param isoCoordinates isometric coordinates of the tile whose neighbors should be retrieved
    * @returns  Uint that stores the elevated neighbor tiles
  ´ */
  unsigned int getElevatedNeighborBitmask(Point isoCoordinates);

private:
  std::vector<std::shared_ptr<Cell> > _cellMatrix;
  
  int _columns;
  int _rows;
};

#endif