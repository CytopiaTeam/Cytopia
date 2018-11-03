#ifndef CELL_HXX_
#define CELL_HXX_

#include "SDL2/SDL.h"

#include <memory>
// TODO_ Remove this after changing type to an enum
#include <string>
#include "sprite.hxx"
#include "basics/point.hxx"

#include "tile.hxx"

/** @brief Class that holds map cells
 * Each tile is represented by the map cell class.   
 */
class Cell
{
public:
  explicit Cell(Point isoCoordinates);
  ~Cell() = default;

  /** @brief get Sprite
    * get the Sprite* object for this cell
    * @see Sprite
    */
  Sprite *getSprite() { return _sprite.get(); };

  /// get iso coordinates of this cell
  const Point &getCoordinates() { return _isoCoordinates; };

  /** @brief get Tile ID
    * Retrieves the current Tile ID of this map cell
    * @return Returns the current Tile ID as Integer
    */
  int getTileID() { return _tileID; };

  /** @brief set Tile ID
  * Change the texture of the map cell to a specific tile id and changes the texture of the cells sprite
  * @see Resources#readTileListFile
  * @param tileID The tileID that should be rendered for this map cell
  */
  void setTileID(int tileID)
  {
    //_sprite->changeTexture(tileID);
    _tileID = tileID;
  };

  /** @brief Increase Height
    * Increases the height of the cell and its sprite
    * This function should not be called directly, but only from where the neighboring cells slopes are determined
    */
  void increaseHeightOfCell();

  /** @brief Decrease Height 
    * Decreases the height of the cell and its sprite
    * This function should not be called directly, but only from where the neighboring cells slopes are determined
    */
  void decreaseHeightOfCell();

  /** @brief Render Cell
  * Renders the sprite object(s) of the cell
  */
  void render();

  void setElevationBitmask(unsigned char bitmask);

  unsigned char getElevationBitmask() { return _elevationBitmask; };

  void setType(TileType type) { _type = type; };

  TileType getType() { return _type; };
  TileOrientation getOrientation() { return _orientation; };

private:
  Point _isoCoordinates;
  std::unique_ptr<Sprite> _sprite;

  SDL_Renderer *_renderer;
  SDL_Window *_window;

  int _heightOffset = 20; // Offset for Y Coordinate between two height levels
  int _tileID;
  int _maxCellHeight = 32;

  TileType _type = TileType::TERRAIN;
  TileOrientation _orientation = TileOrientation::DEFAULT;

  unsigned char _elevationBitmask = 0;
};

#endif
