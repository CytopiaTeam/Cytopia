#ifndef MAPNODE_HXX_
#define MAPNODE_HXX_

#include <SDL.h>

#include <memory>
#include <string>
#include <algorithm>
#include <vector>

#include "../Sprite.hxx"
#include "../common/enums.hxx"
#include "../basics/Point.hxx"

#include "../TileManager.hxx"

struct MapNodeData
{
  std::string tileID;
  TileData *tileData = nullptr;
  int32_t tileIndex = 0;
  Point origCornerPoint = Point::INVALID();
  TileMap tileMap = TileMap::DEFAULT; // store information whether we use normal, slope or shore tiles
};

/** @brief Class that holds map nodes
 * @details Each tile is represented by the map nodes class.
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
    * @details get the Sprite* object for this nodes
    * @returns the Sprite of this node.
    * @see Sprite
    */
  Sprite *getSprite() const { return m_sprite.get(); };

  /** @brief get iso coordinates of this node
    * @returns a pointer to the node's iso coordinates
    */
  const Point &getCoordinates() const { return m_isoCoordinates; };

  /** @brief sets the iso coordinates of this node
    * @param newIsoCoordinates the new iso coordinates for the node
    */
  void setCoordinates(const Point &newIsoCoordinates);

  /** @brief Change Height
    * @details Increases or decrease the height of the node and its sprite.
    * This function should not be called directly, but only from where the neighboring nodes slopes are determined.
    *
    * @param higher pass true in case that height should be increased or false in case that height should be decreased.
    * @return true in case that height is changed, otherwise false.
    */
  bool changeHeight(const bool higher);

  /** @brief Render MapNode
  * @details Renders the sprite object(s) of the node
  */
  void render() const;

  void setBitmask(unsigned char elevationBitmask, std::vector<uint8_t> tileTypeBitmask);

  unsigned char getElevationBitmask() const { return m_elevationBitmask; };

  const TileData *getTileData(Layer layer) const { return m_mapNodeData[layer].tileData; };

  /** @brief get TileID of specific layer inside NodeData.
    * @param layer what layer should be checked on.
    */
  const std::string &getTileID(Layer layer) const { return m_mapNodeData[layer].tileID; };

  bool isPlacementAllowed(const std::string &newTileID) const;

  /// Overwrite m_mapData with the one loaded from a savegame. This function to be used only by loadGame
  void setMapNodeData(std::vector<MapNodeData> &&mapNodeData);

  const std::vector<MapNodeData> &getMapNodeData() const { return m_mapNodeData; };
  const MapNodeData &getMapNodeDataForLayer(Layer layer) const { return m_mapNodeData[layer]; };

  const MapNodeData &getActiveMapNodeData() const;

  /** @brief check if tileID placeable on slope tile.
    * @param tileID the tileID which need to be checked whether allowing placement on slope or not.
    */
  bool isPlacableOnSlope(const std::string &tileID) const;

  /** @brief check if current Node Terrain is Slope Terrain.
    */
  bool isSlopeNode(void) const;

  /** @brief check the conductivity of the node
    * @returns true if the node conducts electricity/power, false if not
    */
  const bool isConductive() const;

  /**
 * @brief Demolish a node
 * @details Removes all tiles on a node. This effects all layers where something to demolish is placed (BUILDINGS, GROUND_DECORATION, UNDERGROUND) per default, but can be restricted to a single Layer.
 * @param layer restrict demolish to a single layer
 * @see MapNode#demolishNode
 */
  void demolishNode(const Layer &layer = Layer::NONE);

  /** @brief demolish specific layer of a Node.
    * @param layer what layer should be demolished.
    */
  void demolishLayer(const Layer &layer);

  void setTileID(const std::string &tileType, const Point &origPoint);

  /**
   * @brief Get the Origin Corner Point of a multitile building
   * 
   * @param layer the layer that should be checked
   * @return const Point& 
   */
  const Point &getOrigCornerPoint(Layer layer) const { return getMapNodeDataForLayer(layer).origCornerPoint; }

  /**
   * @brief If this is the origin node of a multitile building. 
   * 
   * @param layer the layer that should be checked, defaults to the BUILDINGS layer
   * @return wheter or not this is the origin node of a multitile building
   */
  bool isOriginNode(Layer layer = Layer::BUILDINGS) const
  {
    return (m_isoCoordinates == getMapNodeDataForLayer(layer).origCornerPoint);
  }

  /** @brief return topmost active layer.
    * @details check layers in order of significance for the topmost active layer that has an active tile on that layer
    * @return Layer enum of the topmost active layer
    */
  Layer getTopMostActiveLayer() const;

  bool isLayerOccupied(const Layer &layer) const { return m_mapNodeData[layer].tileData != nullptr; }

  /** @brief Set elevation bit mask.
  * @param bitMask
    */
  inline void setElevationBitMask(const unsigned char bitMask) { m_elevationBitmask = bitMask; }

  /** @brief Set autotile bit mask.
  * @param bitMask
    */
  inline void setAutotileBitMask(std::vector<unsigned char> &&bitMask) { m_autotileBitmask = std::move(bitMask); }

  /** @brief Update texture.
  * @param layer
    */
  void updateTexture(const Layer &layer = Layer::NONE);

  /**
   * @brief Sets a node to be Transparent
   * @param transparencyFactor (0-1.0) - The percentage of node transparency. 1 -> invisible, 0 -> opaque.
   * @param layer what layer in Sprite should it's transparency altered.
   */
  void setNodeTransparency(const float transparencyFactor, const Layer &layer) const;

  /**
  * @brief Update the Z-Index of this mapNode
  * @param the new Z-Index
  */
  void setZIndex(int zIndex) { m_isoCoordinates.z = zIndex; };

  /// Maximum height of the node.
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
