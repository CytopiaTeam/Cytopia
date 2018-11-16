#ifndef MAPNODE_HXX_
#define MAPNODE_HXX_

#include "SDL2/SDL.h"

#include <memory>
#include <string>

#include "../sprite.hxx"
#include "../basics/point.hxx"

#include "../tile.hxx"

/** @brief Class that holds map nodes
 * Each tile is represented by the map nodes class.
 */
class MapNode
{
public:
  explicit MapNode(Point isoCoordinates);
  ~MapNode() = default;

  /** @brief get Sprite
    * get the Sprite* object for this nodes
    * @see Sprite
    */
  Sprite *getSprite() { return _sprite.get(); };

  /// get iso coordinates of this node
  const Point &getCoordinates() { return _isoCoordinates; };

  /** @brief get Tile ID
    * Retrieves the current Tile ID of this map node
    * @return Returns the current Tile ID as Integer
    */
  int getTileID() { return _tileID; };

  /** @brief set Tile ID
  * Change the texture of the map node to a specific tile id and changes the texture of the nodes sprite
  * @see Resources#readTileListFile
  * @param tileID The tileID that should be rendered for this map node
  */
  void setTileID(int tileID)
  {
    //_sprite->changeTexture(tileID);
    _tileID = tileID;
  };

  /** @brief Increase Height
    * Increases the height of the node and its sprite
    * This function should not be called directly, but only from where the neighboring nodes slopes are determined
    */
  void increaseHeight();

  /** @brief Decrease Height 
    * Decreases the height of the node and its sprite
    * This function should not be called directly, but only from where the neighboring nodes slopes are determined
    */
  void decreaseHeight();

  /** @brief Render MapNode
  * Renders the sprite object(s) of the node
  */
  void render();

  void setElevationBitmask(unsigned char bitmask);

  unsigned char getElevationBitmask() { return _elevationBitmask; };

  std::string getType() { return _type; };
  const std::string &getTileType() { return _tileType; };
  void setTileType(const std::string &tileType);
  std::string getOrientation() { return _orientation; };
  size_t getUsedTileMap() { return _tileMap; };
  //SDL_Rect getClipRect() { return _cli;
private:
  Point _isoCoordinates;
  std::unique_ptr<Sprite> _sprite;

  SDL_Renderer *_renderer;
  SDL_Window *_window;

  int _tileID;
  int _maxHeight = 32;

  std::string _type = "Terrain";
  std::string _tileType = "terrain";
  std::string _orientation = "default";

  size_t _tileMap = TileMap::DEFAULT;
  size_t _orientationNew = TileSlopes::DEFAULT_ORIENTATION;

  TileInformation *_tileData;

  unsigned char _elevationBitmask = 0;

  void updateTexture();
};

#endif
