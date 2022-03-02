#ifndef TILEMANAGER_HXX_
#define TILEMANAGER_HXX_

#include <SDL.h>
#include <unordered_map>
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

class TileManager : public Singleton<TileManager>
{
public:
  friend Singleton<TileManager>;

  // Disable copy and assignemnt operators
  TileManager(TileManager const &) = delete;
  TileManager &operator=(TileManager const &) = delete;

  SDL_Texture *getTexture(const std::string &tileID) const;
  TileData *getTileData(const std::string &id) noexcept;
  Layer getTileLayer(const std::string &tileID) const;
  size_t calculateSlopeOrientation(unsigned char bitMaskElevation);
  TileOrientation calculateTileOrientation(unsigned char bitMaskElevation);
  const std::unordered_map<std::string, TileData> &getAllTileData() const { return m_tileData; };
  void init();

private:
  TileManager();
  ~TileManager();

  std::unordered_map<std::string, TileData> m_tileData;
  void addJSONObjectToTileData(const nlohmann::json &tileDataJSON, size_t idx, const std::string &id);
};

#endif
