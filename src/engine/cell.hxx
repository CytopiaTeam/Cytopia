#ifndef CELL_HXX_
#define CELL_HXX_

#include <vector>
#include <cmath>

#include "SDL2/SDL.h"


#include "sprite.hxx"
#include "basics/point.hxx"
#include "basics/resources.hxx"

/** @brief Class that holds map cells
 * Each tile is represented by the map cell class.   
 */
class Cell
{
public:
  Cell();
  explicit Cell(Point isoCoordinates);
  ~Cell();

  /** @brief get Sprite
    * get the Sprite* object for this cell
    * @see Sprite
    */
  Sprite* getSprite();

  /// get iso coordinates of this cell
  Point getCoordinates();

  /// add the cell to the renderer
  void renderCell();

  /// Sets the neighbors of this cell for fast access
  void setNeighbors(std::vector<Cell*> neighbors);

  /** @brief Increase Height
    * Increases the height of this map cell and checks which
    * tileID must be drawn for each neighbor
    */
  void increaseHeight();
  /** @brief Decrease Height 
    * Decreases the height of this map cell and checks which 
    * tileID must be drawn for each neighbor
    */
  void decreaseHeight();

  /** @brief set Tile ID 
    * Change the texture of the map cell to a specific tile id
    * @see Resources#readTileListFile
    * @param tileID The tileID that should be rendered for this map cell
    */
  void setTileID(int tileID);
  /** @brief get Tile ID
    * Retrieves the current Tile ID of this map cell
    * @return Returns the current Tile ID as Integer
    */
  int getTileID();

private:
  Point _isoCoordinates;
  Sprite* _sprite;

  std::vector<Cell*> _neighbors;
  SDL_Renderer* _renderer;
  SDL_Window* _window;

  int _heightOffset = 20; // Offset for Y Coordinate between two height levels
  int _tileID;
  int _maxCellHeight = 32;

  /** \brief: determine which tile ID should be drawn for this cell
    * Checks all the neighbors and determines the tile ID of this mapcell according to it's
    * elevated / lowered neighbors.
    */
  void determineTile();

  /** \brief set tileID for each neighbor
    * After a cell is raised / lowered, each neighbor must check which tileID it should have
    * @see Cell#drawSurroundingTiles
  */
  void drawSurroundingTiles(Point isoCoordinates);
  
  /** UInt to elevate store tile position in */
  unsigned int _elevatedTilePosition;
};


#endif