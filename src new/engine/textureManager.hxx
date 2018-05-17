#ifndef TEXTURE_MANAGER_HXX_
#define TEXTURE_MANAGER_HXX_

#include <iostream>
#include <unordered_map>

#include "basics/point.hxx"
#include "basics/resources.hxx"

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"


class TextureManager
{
public:

  // Singleton
  static TextureManager& Instance() {
    static TextureManager instance;
    return instance;
  }

  // Disable copy and assignemnt operators
  TextureManager(TextureManager const&) = delete;
  TextureManager& operator=(TextureManager const&) = delete;

  /**
  retrieves texture for a tileID
  */
  SDL_Texture* getTexture(int tileID);

  std::unordered_map<int, SDL_Texture*> _textureMap;

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