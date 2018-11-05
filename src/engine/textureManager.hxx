#ifndef TEXTURE_MANAGER_HXX_
#define TEXTURE_MANAGER_HXX_

#include <iostream>
#include <unordered_map>

#include "SDL2/SDL.h"

#include "tile.hxx"

enum ButtonState
{
  BUTTONSTATE_DEFAULT,
  BUTTONSTATE_HOVERING,
  BUTTONSTATE_CLICKED,
  BUTTONSTATE_DISABLED
};

class TextureManager
{
public:
  /// Retrieves Instance of Singleton class Texture Manager
  static TextureManager &Instance()
  {
    static TextureManager instance;
    return instance;
  }

  // Disable copy and assignemnt operators
  TextureManager(TextureManager const &) = delete;
  TextureManager &operator=(TextureManager const &) = delete;

  /** retrieves texture for a tileID */
  SDL_Texture *getTileTexture(TileType type, TileOrientation orientation);
  SDL_Texture *getUITexture(std::string uiElement, int buttonState = BUTTONSTATE_DEFAULT);

  /** Retrieves Color of a specific tileID at coordinates with the texture */
  const SDL_Color getPixelColor(TileType type, TileOrientation orientation, int X, int Y);

private:
  TextureManager();
  ~TextureManager() = default;

  // provide a functor to generate a hash for enum class, so it can be used in std::map
  struct EnumClassHash
  {
    template <typename T> std::size_t operator()(T t) const { return static_cast<std::size_t>(t); }
  };

  SDL_Renderer *_renderer;
  SDL_Window *_window;

  /** Load Texture in to Texture Map, if an entry for this tile ID does not yet exist.
  Keep in Mind that the texture id is unique and there can't be two textures with the same id.

  If colorkey is set - Use Magic Pink (255,255,0) for transparency
  */
  void loadUITexture();
  void loadTexture(TileType type, TileOrientation orientation, bool colorKey = false);

  SDL_Surface *createSurfaceFromFile(std::string fileName);
  SDL_Texture *createTextureFromSurface(SDL_Surface *surface);

  std::unordered_map<std::string, std::unordered_map<std::string, SDL_Texture *>> _uiTextureMapNew;

  std::unordered_map<int, SDL_Texture *> _uiTextureMap;
  std::unordered_map<int, SDL_Texture *> _uiTextureMapHover;
  std::unordered_map<int, SDL_Texture *> _uiTextureMapPressed;

  // Map < type, <map<orientation, Texture>>
  std::unordered_map<TileType, std::unordered_map<TileOrientation, SDL_Texture *, EnumClassHash>, EnumClassHash> _tileTextureMap;

  /** Keep surfaces in map for collision detection when selecting tiles*/
  std::unordered_map<TileType, std::unordered_map<TileOrientation, SDL_Surface *, EnumClassHash>, EnumClassHash> _surfaceMap;
  std::unordered_map<int, SDL_Surface *> _uiSurfaceMap;

  std::string tileOrientationEnumToString(TileOrientation orientation);
  std::string tileTypesEnumToString(TileType tileType);
};

#endif
