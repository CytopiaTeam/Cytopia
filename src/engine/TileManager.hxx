#ifndef TILEMANAGER_HXX_
#define TILEMANAGER_HXX_

#include <SDL.h>
#include <unordered_map>
#include <unordered_set>
#include <string>

#include "tileData.hxx"
#include "json.hxx"
#include "Singleton.hxx"
#include "../common/enums.hxx"

enum TileMap : size_t
{
  DEFAULT,
  SLOPES,
  SHORE
};

enum TileSlopes : size_t
{
  N,
  W,
  E,
  S,
  NW,
  NE,
  SW,
  SE,
  N_AND_W,
  N_AND_E,
  S_AND_W,
  S_AND_E,
  BETWEEN,
  DEFAULT_ORIENTATION
};

enum TileOrientation : size_t
{
  TILE_DEFAULT_ORIENTATION,
  TILE_E,
  TILE_N,
  TILE_N_AND_E,
  TILE_W,
  TILE_E_AND_W,
  TILE_N_AND_W,
  TILE_N_AND_E_AND_W,
  TILE_S,
  TILE_S_AND_E,
  TILE_N_AND_S,
  TILE_N_AND_E_AND_S,
  TILE_S_AND_W,
  TILE_S_AND_E_AND_W,
  TILE_N_AND_S_AND_W,
  TILE_ALL_DIRECTIONS,
  TILE_N_AND_E_RECT,
  TILE_S_AND_W_RECT,
  TILE_S_AND_E_RECT,
  TILE_N_AND_W_RECT
};

/**
 * @brief This class holds all the information about tileIDs gathered from TileData.json
 * 
 */
class TileManager : public Singleton<TileManager>
{
public:
  friend Singleton<TileManager>;

  // Disable copy and assignemnt operators
  TileManager(TileManager const &) = delete;
  TileManager &operator=(TileManager const &) = delete;

  /**
   * @brief Get the Texture for the tileID
   * 
   * @param tileID - TileID
   * @return An SDL Texture we can render
   */
  SDL_Texture *getTexture(const std::string &tileID) const;

  /**
   * @brief Get the TileData struct for this tileID with all informations associated with it
   * 
   * @param id - TileID
   * @return A pointer to the TileData Struct
   */
  TileData *getTileData(const std::string &id) noexcept;

  /**
   * @brief Get the Layer that is associated with a tileID. The Tile will be placed on this layer
   * 
   * @param tileID the tileID to get the Layer for
   * @return The layer this tileID has to be placed on
   */
  Layer getTileLayer(const std::string &tileID) const;

  /**
   * @brief Calculates the TileOrintation for elevated tiles to pick the correct slope Sprite
   * 
   * @param bitMaskElevation a bitmask of neighboring tiles and their elevation
   * @return Elevation bitmask
   */
  size_t calculateSlopeOrientation(unsigned char bitMaskElevation);

  /**
   * @brief Calculates the TileOrintation for elevated tiles to pick the correct Sprite
   * 
   * @param bitMaskElevation a bitmask of neighboring tiles and their elevation
   * @return The orientation the tile should have to it's elevated neighbors.
   */
  TileOrientation calculateTileOrientation(unsigned char bitMaskElevation);

  const std::unordered_map<std::string, TileData> &getAllTileData() const { return m_tileData; };

  /**
   * @brief Get the All Tile IDs for a zone with a certain tileSize
   * 
   * @param zone - The Zone we want tileIDs for
   * @param tileSize - Get buildings with a certain tile size
   * @return All tileIDs that match the zone and tileSize
   */
  std::vector<std::string> getAllTileIDsForZone(ZoneType zone, ZoneDensity zoneDensity, TileSize tileSize = {1, 1});

  /**
   * @brief Pick a single random tileID for a zone with a random tilesize within the supplied max Size
   * 
   * @param zone - The Zone we want tileIDs for
   * @param maxTileSize - maximum tileSize we want 
   * @return A random tileID matching the supplied parameters
   */
  std::optional<std::string> getRandomTileIDForZoneWithRandomSize(ZoneType zone, ZoneDensity zoneDensity,
                                                                  TileSize maxTileSize = {1, 1});

  /**
  * @brief Parse the tileData JSON and set up the tileManager
  * 
  */
  void init();

private:
  TileManager();
  ~TileManager() = default;

  std::unordered_map<std::string, TileData> m_tileData;
  std::unordered_set<TileSize> m_tileSizeCombinations;
  void addJSONObjectToTileData(const nlohmann::json &tileDataJSON, size_t idx, const std::string &id);
};

#endif
