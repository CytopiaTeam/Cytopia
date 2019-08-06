#ifndef MAPNODE_HXX_
#define MAPNODE_HXX_

#include <SDL.h>

#include <memory>
#include <string>
#include <algorithm>
#include <vector>

#include "../Sprite.hxx"
#include "../common/enums.hxx"
#include "../basics/point.hxx"

#include "../TileManager.hxx"

struct MapNodeData
{
  std::string tileID;
  TileData *tileData = nullptr;
};

/** @brief Class that holds map nodes
 * Each tile is represented by the map nodes class.
 */

class MapNode
{
public:
  MapNode(Point isoCoordinates, const std::string &terrainID, const std::string &tileID = "");
  
  /**
    * @brief Destroys the MapNode object
    */
  ~MapNode() = default;

  /** @brief get Sprite
    * get the Sprite* object for this nodes
    * @returns the Sprite of this node.
    * @see Sprite
    */
  Sprite *getSprite() const { return m_sprite.get(); };

  /** @brief get iso coordinates of this node
    * gets the iso coordinates of this node
    * @returns a pointer to the node's iso coordinates
    */
  const Point &getCoordinates() const { return m_isoCoordinates; };

  /** @brief sets the iso coordinates of this node
    * sets the iso coordinates of this node
    * @param a pointer to the new iso coordinates for the node
    */
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

  //bool isLayerActive(Layer layer) const { return std::find(layers.begin(), layers.end(), layer) != layers.end(); };

  unsigned char getElevationBitmask() const { return m_elevationBitmask; };

  const TileData *getTileData(Layer layer) const { return m_mapNodeData[layer].tileData; };

  const std::string &getTileID(Layer layer) const { return m_mapNodeData[layer].tileID; };

  bool checkTileIsEmpty(const std::string &tileID) const;

  const std::vector<MapNodeData> getMapNodeData() const { return m_mapNodeData; };
  const MapNodeData getMapNodeDataForLayer(Layer layer) const { return m_mapNodeData[layer]; };

  const MapNodeData &getActiveMapNodeData() const;

  bool checkBuildingLayerSlopePlaceable(const std::string &tileID, const Layer &layer) const;

  void demolishNode();

  void setTileID(const std::string &tileType);
  size_t tileMap = TileMap::DEFAULT;

private:
  Point m_isoCoordinates;
  std::unique_ptr<Sprite> m_sprite;

  int m_maxHeight = 32;

  std::string m_previousTileID = "terrain";

  size_t m_orientation = TileSlopes::DEFAULT_ORIENTATION;
  size_t m_elevationOrientation = TileSlopes::DEFAULT_ORIENTATION;

  int m_clippingWidth = 0;

  std::vector<MapNodeData> m_mapNodeData;
  unsigned char m_elevationBitmask = 0;
  unsigned char m_tileIDBitmask = 0;

  void updateTexture();
};

#endif
