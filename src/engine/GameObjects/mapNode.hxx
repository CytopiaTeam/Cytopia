#ifndef MAPNODE_HXX_
#define MAPNODE_HXX_

#include <SDL.h>

#include <memory>
#include <string>

#include "../Sprite.hxx"
#include "../basics/point.hxx"

#include "../TileManager.hxx"

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
  Sprite *getSprite() const { return m_sprite.get(); };

  /// get iso coordinates of this node
  const Point &getCoordinates() const { return m_isoCoordinates; };

  void setCoordinates(const Point &newIsoCoordinates);

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

  unsigned char getElevationBitmask() const { return m_elevationBitmask; };

  const TileData *getTileData() const { return m_tileData; };

  const std::string &getTileID() const { return m_tileID; };
  void setTileID(const std::string &tileType);

  size_t tileMap = TileMap::DEFAULT;

private:
  Point m_isoCoordinates;
  std::unique_ptr<Sprite> m_sprite;

  int m_maxHeight = 32;

  std::string m_tileID = "terrain";
  std::string m_previousTileID = "terrain";

  size_t m_orientation = TileSlopes::DEFAULT_ORIENTATION;

  int m_clippingWidth = 0;

  TileData *m_tileData;

  unsigned char m_elevationBitmask = 0;
  unsigned char m_tileIDBitmask = 0;

  void updateTexture();
};

#endif
