#ifndef TEXTURE_MANAGER_HXX_
#define TEXTURE_MANAGER_HXX_

#include <iostream>
#include <unordered_map>

#include "basics/point.hxx"
#include "basics/resources.hxx"
#include "basics/log.hxx"

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"


class TextureManager
{
public:

  /// Retrieves Instance of Singleton class Texture Manager
  static TextureManager& Instance() {
    static TextureManager instance;
    return instance;
  }

  // Disable copy and assignemnt operators
  TextureManager(TextureManager const&) = delete;
  TextureManager& operator=(TextureManager const&) = delete;

  /** retrieves texture for a tileID */
  SDL_Texture* getTexture(int tileID);

  SDL_Surface * getSurface(int tileID);

  /** Retrieves Color of a specific tileID at coordinates with the texture */
  SDL_Color GetPixelColor(int tileID,  int X, int Y);

  std::unordered_map<int, SDL_Texture*> _textureMap;
  /** Keep surfaces in map for collision detection when selecting tiles*/
  std::unordered_map<int, SDL_Surface*> _surfaceMap;

private:
  TextureManager();
  ~TextureManager() { };

  SDL_Renderer* _renderer;
  SDL_Window* _window;

  /** Load Texture in to Texture Map, if an entry for this tile ID does not yet exist.
  Keep in Mind that the texture id is unique and there can't be two textures with the same id.

  If colorkey is set - Use Magic Pink for transparency
  */
  void loadTexture(int tileID, bool colorKey = false);
};

#endif