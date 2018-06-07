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
  explicit Cell(const Point& isoCoordinates);
  ~Cell() = default;

  /** @brief get Sprite
    * get the Sprite* object for this cell
    * @see Sprite
    */
  std::shared_ptr<Sprite> getSprite() { return _sprite; };
  //const Sprite& getSprite() const { return *_sprite; };

  /// get iso coordinates of this cell
  Point getCoordinates() { return _isoCoordinates; };

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
  void setTileID(int tileID) {
    _sprite->changeTexture(tileID);
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

private:
  Point _isoCoordinates;
  std::shared_ptr<Sprite> _sprite;

  SDL_Renderer* _renderer;
  SDL_Window* _window;

  int _heightOffset = 20; // Offset for Y Coordinate between two height levels
  int _tileID;
  int _maxCellHeight = 32;

};


#endif