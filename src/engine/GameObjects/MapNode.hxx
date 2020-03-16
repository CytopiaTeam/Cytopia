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
  int32_t tileIndex = 0;
  Point origCornerPoint = UNDEFINED_POINT;
  bool shouldRender = true;
};

const std::string DEMY_NODE_ID = "demy_node";

/** @brief Class that holds map nodes
 * Each tile is represented by the map nodes class.
 */

class MapNode
{
public:
  MapNode(Point isoCoordinates, const std::string &terrainID, const std::string &newTileID = "");

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

  void setBitmask(unsigned char elevationBitmask, std::vector<uint8_t> tileTypeBitmask);

  //bool isLayerActive(Layer layer) const { return std::find(layers.begin(), layers.end(), layer) != layers.end(); };

  unsigned char getElevationBitmask() const { return m_elevationBitmask; };

  const TileData *getTileData(Layer layer) const { return m_mapNodeData[layer].tileData; };

  /** @brief get TileID of specific layer inside NodeData.
    * @param layer - what layer should be checked on.
    */
  const std::string &getTileID(Layer layer) const { return m_mapNodeData[layer].tileID; };

  bool isPlacementAllowed(const std::string &tileID) const;

  /// Overwrite m_mapData with the one loaded from a savegame. This function to be used only by loadGame
  void setMapNodeData(std::vector<MapNodeData> &&mapNodeData, const Point &isoCoordinates);

  const std::vector<MapNodeData> getMapNodeData() const { return m_mapNodeData; };
  const MapNodeData getMapNodeDataForLayer(Layer layer) const { return m_mapNodeData[layer]; };

  const MapNodeData &getActiveMapNodeData() const;

  /** @brief tileID placeable on slope tile.
    * check if tileID is placeable on slope.
    * @param tileID - the tileID which need to be checked whether allowing placement on slope or not.
    * @param layer - what layer should be checked on, in case this is not BUILDING layer the placement is OK.
    */
  bool isPlacableOnSlope(const std::string &tileID) const;

  void demolishNode();

  void setTileID(const std::string &tileType, const Point &origPoint);

  Point getOrigCornerPoint(Layer layer) { return getMapNodeDataForLayer(layer).origCornerPoint; }
  Point getOrigCornerPoint() { return m_origCornerPoint; }

  size_t tileMap = TileMap::DEFAULT;

  /** @brief return topmost active layer.
    * check layers in order of significance for the topmost active layer that has an active tile on that layer
    * @return Layer enum of the topmost active layer
    */
  unsigned int getTopMostActiveLayer() const;

private:
  Point m_isoCoordinates;
  std::unique_ptr<Sprite> m_sprite;

  int m_maxHeight = 32;

  std::string m_previousTileID = "terrain";

  std::vector<TileOrientation> m_autotileOrientation;
  size_t m_elevationOrientation = TileSlopes::DEFAULT_ORIENTATION;

  int m_clippingWidth = 0;

  std::vector<MapNodeData> m_mapNodeData;
  std::vector<unsigned char> m_autotileBitmask;
  unsigned char m_elevationBitmask = 0;

  Point m_origCornerPoint;

  void updateTexture();
};

#endif
