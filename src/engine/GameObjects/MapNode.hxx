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
  TileMap tileMap = TileMap::DEFAULT; // store information wheter we use normal, slope or shore tiles
};

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
    * @param newIsoCoordinates the new iso coordinates for the node
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

  bool isPlacementAllowed(const std::string &newTileID) const;


  /// Overwrite m_mapData with the one loaded from a savegame. This function to be used only by loadGame
  void setMapNodeData(std::vector<MapNodeData> &&mapNodeData, const Point &isoCoordinates);

  const std::vector<MapNodeData> &getMapNodeData() const { return m_mapNodeData; };
  const MapNodeData &getMapNodeDataForLayer(Layer layer) const { return m_mapNodeData[layer]; };

  const MapNodeData &getActiveMapNodeData() const;

  /** @brief tileID placeable on slope tile.
    * check if tileID is placeable on slope.
    * @param tileID - the tileID which need to be checked whether allowing placement on slope or not.
    * @param layer - what layer should be checked on, in case this is not BUILDING layer the placement is OK.
    */
  bool isPlacableOnSlope(const std::string &tileID) const;

  /** @brief check if current Node Terrain is Slope Terrain.
    */
  bool isSlopeNode(void) const;

  /**
 * @brief Demolish a node
 * Removes all tiles on a node. This effects all layers where something to demolish is placed. (BUILDINGS, GROUND_DECORATION, UNDERGROUND) per default, but can be restricted to a single Layer. 
 * @param isoCoordinates all coordinates that should be demolished
 * @param updateNeighboringTiles wether the adjecent tiles should be updated. (only relevant for autotiling)
 * @param layer restrict demolish to a single layer 
 * @see MapNode#demolishNode
 */
  void demolishNode(const Layer &layer = Layer::NONE);

  /** @brief clear specific layer of a Node.
    * @param layer - what layer should be cleared.
    */
  void clearLayer(const Layer &layer);

  void setTileID(const std::string &tileType, const Point &origPoint);

  const Point &getOrigCornerPoint(Layer layer) const { return getMapNodeDataForLayer(layer).origCornerPoint; }

  /** @brief return topmost active layer.
    * check layers in order of significance for the topmost active layer that has an active tile on that layer
    * @return Layer enum of the topmost active layer
    */
  Layer getTopMostActiveLayer() const;

  void setRenderFlag(Layer layer, bool shouldRender) { m_mapNodeData[layer].shouldRender = shouldRender; }

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

  void updateTexture(const Layer &layer = Layer::NONE);
};

#endif
