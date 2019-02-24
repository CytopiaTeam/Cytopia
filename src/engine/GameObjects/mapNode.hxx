#ifndef MAPNODE_HXX_
#define MAPNODE_HXX_

#include <SDL.h>

#include <memory>
#include <string>

#include "../sprite.hxx"
#include "../basics/point.hxx"

#include "../tileManager.hxx"

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
  Sprite *getSprite() const { return _sprite.get(); };

  /// get iso coordinates of this node
  const Point &getCoordinates() const { return _isoCoordinates; };

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
  void render() const;

  void setBitmask(unsigned char elevationBitmask, unsigned char tileTypeBitmask);

  unsigned char getElevationBitmask() const { return _elevationBitmask; };

  const TileData *getTileData() const { return _tileData; };

  const std::string &getTileID() const { return _tileID; };
  void setTileID(const std::string &tileType);
  size_t getUsedTileMap() const { return _tileMap; };

private:
  Point _isoCoordinates;
  std::unique_ptr<Sprite> _sprite;

  SDL_Renderer *_renderer;
  SDL_Window *_window;

  int _maxHeight = 32;

  std::string _tileID = "terrain";
  std::string _previousTileID = "terrain";

  size_t _tileMap = TileMap::DEFAULT;
  size_t _orientation = TileSlopes::DEFAULT_ORIENTATION;
  size_t _spriteCount = 1;

  int _clippingWidth = 0;

  TileData *_tileData;

  unsigned char _elevationBitmask = 0;
  unsigned char _tileIDBitmask = 0;

  void updateTexture();
};

#endif
