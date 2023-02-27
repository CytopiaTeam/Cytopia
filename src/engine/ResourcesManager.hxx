#ifndef RESOURCES_MANAGER_HXX_
#define RESOURCES_MANAGER_HXX_

#include <iostream>
#include <unordered_map>

#include <SDL.h>

#include "TileManager.hxx"

class ResourcesManager : public Singleton<ResourcesManager>
{
public:
  friend Singleton<ResourcesManager>;

  // Disable copy and assignemnt operators
  ResourcesManager(ResourcesManager const &) = delete;
  ResourcesManager &operator=(ResourcesManager const &) = delete;

  /** Retrieves default texture for a tileID and type */
  SDL_Texture *getUITexture(const std::string &uiElement, const std::string &uiTextureType);

  /** Retrieves default texture for a tileID */
  SDL_Texture *getUITexture(const std::string &uiElement);

  /** Retrieves Color of a specific tileID at coordinates with the texture */

  SDL_Texture *getTileTexture(const std::string &id);
  SDL_Surface *getTileSurface(const std::string &id);

  void loadTexture(const std::string &id, const std::string &fileName);
  /**
   * @brief Get the Color Of Pixel In Surface object at a given coordinate
   * 
   * @param tileID The tileID of object to check 
   * @param x x ccoordinate within the surface
   * @param y y ccoordinate within the surface
   * @return SDL_Color at the given coordinate
   */
  SDL_Color getColorOfPixelInSurface(const std::string &tileID, int x, int y);

private:
  ResourcesManager();
  ~ResourcesManager();

  /** 
   * @brief Load Texture in to Texture Map, if an entry for this tile ID does not yet exist.
   * @details Keep in Mind that the texture id is unique and there can't be two textures with the same id.
   * If colorkey is set - Use Magic Pink (255,255,0) for transparency
   * @throws ConfigurationError when loading configuration results in an error
  */
  void loadUITexture();

  /// Delete everything. Should be called from the destuctor only
  void flush();

  SDL_Surface *createSurfaceFromFile(const std::string &fileName);
  SDL_Texture *createTextureFromSurface(SDL_Surface *surface);

  std::unordered_map<std::string, std::unordered_map<std::string, SDL_Texture *>> m_uiTextureMap;

  std::unordered_map<std::string, SDL_Texture *> m_tileTextureMap;
  std::unordered_map<std::string, SDL_Surface *> m_surfaceMap;
};

#endif
