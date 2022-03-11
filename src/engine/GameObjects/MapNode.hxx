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
  Point origCornerPoint = Point::INVALID();
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

  /** @brief Move constructor.
    */
  MapNode(MapNode &&mn) noexcept
      : m_isoCoordinates(std::move(mn.m_isoCoordinates)), m_sprite(std::move(mn.m_sprite)),
        m_previousTileID(std::move(mn.m_previousTileID)), m_autotileOrientation(std::move(mn.m_autotileOrientation)),
        m_elevationOrientation(mn.m_elevationOrientation), m_clippingWidth(mn.m_clippingWidth),
        m_mapNodeData(std::move(mn.m_mapNodeData)), m_autotileBitmask(std::move(mn.m_autotileBitmask)),
        m_elevationBitmask(mn.m_elevationBitmask){};

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

  /** @brief Change Height
    * Increases or decrease the height of the node and its sprite
    * This function should not be called directly, but only from where the neighboring nodes slopes are determined
    *
    * @param higher pass true in case that height should be increased or false in case that height should be decreased.
    * @return true in case that height is changed, otherwise false.
    */
  bool changeHeight(const bool higher);

  /** @brief Render MapNode
  * Renders the sprite object(s) of the node
  */
  void render() const;

  void setBitmask(unsigned char elevationBitmask, std::vector<uint8_t> tileTypeBitmask);

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

  /** @brief demolish specific layer of a Node.
    * @param layer - what layer should be demolished.
    */
  void demolishLayer(const Layer &layer);

  void setTileID(const std::string &tileType, const Point &origPoint);

  const Point &getOrigCornerPoint(Layer layer) const { return getMapNodeDataForLayer(layer).origCornerPoint; }

  /** @brief return topmost active layer.
    * check layers in order of significance for the topmost active layer that has an active tile on that layer
    * @return Layer enum of the topmost active layer
    */
  Layer getTopMostActiveLayer() const;

  /** @brief check if specific layer can be autotiled.
    * @return bool indicating whether layer item can be autotiled.
    */
  bool isLayerAutoTile(const Layer &layer) const;

  /** @brief check if specific Tile Data is autotile category.
    * @return bool indicating whether Tile Data item can be autotiled.
    */
  static bool isDataAutoTile(const TileData *tileData);

  bool isLayerOccupied(const Layer &layer) const { return m_mapNodeData[layer].tileData != nullptr; }

  void setRenderFlag(Layer layer, bool shouldRender) { m_mapNodeData[layer].shouldRender = shouldRender; }

  /** @brief Set elevation bit mask.
    */
  inline void setElevationBitMask(const unsigned char bitMask) { m_elevationBitmask = bitMask; }

  /** @brief Set autotile bit mask.
    */
  inline void setAutotileBitMask(std::vector<unsigned char> &&bitMask) { m_autotileBitmask = std::move(bitMask); }

  /** @brief Update texture.
    */
  void updateTexture(const Layer &layer = Layer::NONE);

  /**
   * @brief Sets a node to be Transparent
   * This sets a node to be Transparent.
   * @parameter transparencyFactor (0-1.0) - The percentage of node transparency. 1 -> invisible, 0 -> opaque.
   * @parameter layer - what layer in Sprite should it's transperancy altered.
   */
  void setNodeTransparency(const float transparencyFactor, const Layer &layer) const;

  /**
   * @brief Maximum height of the node.
   */
  static const int maxHeight = 32;

private:
  Point m_isoCoordinates;
  std::unique_ptr<Sprite> m_sprite;
  std::string m_previousTileID = "terrain";
  std::vector<TileOrientation> m_autotileOrientation;
  size_t m_elevationOrientation = TileSlopes::DEFAULT_ORIENTATION;
  int m_clippingWidth = 0;
  std::vector<MapNodeData> m_mapNodeData;
  std::vector<unsigned char> m_autotileBitmask;
  unsigned char m_elevationBitmask = 0;
};
#endif
