#ifndef MAPFUNCTIONS_HXX_
#define MAPFUNCTIONS_HXX_

// #include "WindowManager.hxx"
// #include "basics/Point.hxx"
#include <vector>
#include "../Map.hxx"
// #include "../../util/Singleton.hxx"
#include <Singleton.hxx>
#include <Point.hxx>

class MapFunctions : public Singleton<MapFunctions>
{
public:
  MapFunctions();
  Map *getMap() { return m_map; };

  /** \brief Change map node height.
  * @param isoCoordinates the Point on the map node to change height.
  * @param elevate if set to true make node higher, otherwise lower.
  */
  void changeHeight(const Point &isoCoordinates, const bool elevate);

  /** \brief Change map node height.
  * @param coordinate the Point on the map node to change height
  * @param elevate if set to true make node higher, otherwise lower.
  * @return true in case that height has been changed, otherwise false.
  */
  bool updateHeight(Point coordinate, const bool elevate);

  /** \brief level area of map nodes.
	* @param startcoordinate the starting point whose height is used for levelling
	* @param levelArea the area that is to be leveled.
	*/
  void levelHeight(const Point &startCoordinate, const std::vector<Point> levelArea);

  /** \brief Get pointer to a single mapNode at specific iso coordinates.
  * @param isoCoords The node to retrieve.
  */
  inline MapNode &getMapNode(Point isoCoords)
  {
    uint32_t index = isoCoords.toIndex();
    return m_map->mapNodes[index < m_map->mapNodes.size() ? index : 0];
  };

  /** \brief Get all mapnodes as a vector
   */
  const std::vector<MapNode> &getMapNodes() { return m_map->mapNodes; };

  /** \brief Get all neighbor nodes from provided map node.
  * @param isoCoordinates iso coordinates.
  * @param includeCentralNode if set to true include the central node in the result.
  * @return All neighbor nodes.
  */
  std::vector<NeighborNode> getNeighborNodes(const Point &isoCoordinates, const bool includeCentralNode);

  /** \brief Get elevated bit mask of the map node.
  * @param centerCoordinates Point on the map node to calculate elevated bit mask.
  * @return Map node elevated bit mask.
  */
  unsigned char getElevatedNeighborBitmask(Point centerCoordinates);

  /** \brief check if Tile is occupied
  * @param isoCoordinates Tile to inspect
  * @param tileID tileID which should be checked
  */
  bool isPlacementOnNodeAllowed(const Point &isoCoordinates, const std::string &tileID) const;

  /** \brief check if Tile can be placed in an area
  * @param targetCoordinates Tile array to inspect
  * @param tileID tileID which should be checked
  */
  bool isPlacementOnAreaAllowed(const std::vector<Point> &targetCoordinates, const std::string &tileID) const;

  /**
 * @brief Set the Tile ID Of Node object
 * @details Also invokes all necessary texture updates (auto-tiling, slopes, ...)
 * @param tileID the new tileID to set
 * @param coordinate Point where the tileID which should be set
 * @returns setTileID success or not
 */
  bool setTileID(const std::string &tileID, Point coordinate);

  /**
 * @brief Set the Tile ID Of multiple Node objects
 * @details Also invokes all necessary texture updates (auto-tiling, slopes, ...)
 * @param tileID the new tileID to set
 * @param coordinates a vector of Points where the tileIDs which should be set
 * @returns setTileID success or not
 */
  bool setTileID(const std::string &tileID, const std::vector<Point> &coordinates);

  /**
 * @brief Demolish a node
 * @details This function gathers all tiles that should be demolished and invokes the node's demolish function. When a building bigger than 1x1 is selected, all its coordinates are added to the demolishing points.
 * @param isoCoordinates all coordinates that should be demolished
 * @param updateNeighboringTiles whether the adjacent tiles should be updated. (only relevant for autotiling)
 * @param layer restrict demolish to a single layer
 * @see MapNode#demolishNode
 */
  void demolishNode(const std::vector<Point> &isoCoordinates, bool updateNeighboringTiles = false, Layer layer = Layer::NONE);

  /**
   * @brief Get original corner point of given point within building borders.
   * @param isoCoordinates Point to get corner point of
   * @param layer
   * @returns original corner point
   */
  Point getNodeOrigCornerPoint(const Point &isoCoordinates, Layer layer = Layer::NONE);

  /**
 * @brief Debug MapNodeData to Console
 * @details Used as Tile-Inspector until we implement a GUI variant
 * @param isoCoordinates Tile to inspect
 */
  void getNodeInformation(const Point &isoCoordinates) const;

  /**
 * @brief Sets a node to be highlighted
 * @details This sets a node to be highlighted. The highlighting is done during rendering
 * @param isoCoordinates which node should be highlit.
 * @param rgbColor The SpriteRGBColor that should be used for highlighting
 */
  void highlightNode(const Point &isoCoordinates, const SpriteRGBColor &rgbColor);

  /**
 * @brief Sets a node to be unhighlighred
 * @details This sets a node to be unhighlighted, which is done during rendering
 * @param isoCoordinates which node should be unhighlit.
 */
  void unHighlightNode(const Point &isoCoordinates);

  /**
 * @brief Returns the node at given screen coordinates
 *
 * @param screenCoordinates
 * @return Point
 */
  Point findNodeInMap(const SDL_Point &screenCoordinates, const Layer &layer = Layer::NONE);

  /** @brief get Tile ID of specific layer of specific iso coordinates
  * @param isoCoordinates Tile to inspect
  * @param layer layer to check.
  */
  std::string getTileID(const Point &isoCoordinates, Layer layer);

  /** @brief Refresh the visible part of the map
   *
   * @see Sprite#refresh
   */
  void refreshVisibleMap();

  /**
   * @brief Creates a new map object with terrain gen
   * @param generateTerrain flat map or procedural
   */
  void newMap(bool generateTerrain = true);

  /** \brief Load Map from file
  * @details Deserializes the Map class from a json file, creates a new Map and updates m_map.
  * @param fileName The file the map is stored in
  * @throws ConfigurationError when loading savegame file results in an error
  * @throws CytopiaError when savegame version is not compatible with current version of Cytopia
  */
  void loadMapFromFile(const std::string &fileName);

private:
  Map *m_map;

  /** \brief Update the nodes and all affected node with the change.
  * @param nodes vector of coordinates to be updated.
  */
  void updateNodeNeighbors(const std::vector<Point> &nodes);

  /** \brief Update all mapNodes
  * @details Updates all mapNode and its adjacent tiles regarding height information, draws slopes for adjacent tiles and
  * sets tiling for mapNode sprite if applicable
  */
  void updateAllNodes();

  /**
   * @brief Check if a click is within a non transparent part of a sprite
   * 
   * @param screenCoordinates click coordinates on the scrren
   * @param isoCoordinate isocoordinates of the mapnode to check
   * @param layer layer to check
   * @return if a click is within this node
   */
  bool isClickWithinTile(const SDL_Point &screenCoordinates, Point isoCoordinate, const Layer &layer) const;

  /** \brief Save Map to file
  * @details Serializes the Map class to json and writes the data to a file.
  * @param fileName The file the map should be written to
  */
  void saveMapToFile(const std::string &fileName);

  /** \brief Get a bitmask that represents same-tile neighbors
  * @details Checks all neighboring tiles and returns the elevated neighbors in a bitmask:
  * [ BR BL TR TL  R  L  B  T ]
  * [ 0  0  0  0   0  0  0  0 ]
  * @param coordinate Point on the map to calculate bitmask for.
  * @return Uint that represents a bitmask of the neighbor tiles and their elevation to the center coordinate
  */
  std::vector<uint8_t> calculateAutotileBitmask(Point coordinate);
};

#endif
