#ifndef RESOURCES_MANAGER_HXX_
#define RESOURCES_MANAGER_HXX_

#include <iostream>
#include <unordered_map>

#include <SDL.h>

#include "TileManager.hxx"

enum ButtonState
{
  BUTTONSTATE_DEFAULT,
  BUTTONSTATE_HOVERING,
  BUTTONSTATE_CLICKED,
  BUTTONSTATE_DISABLED
};

class ResourcesManager : public Singleton<ResourcesManager>
{
public:
  friend Singleton<ResourcesManager>;

  // Disable copy and assignemnt operators
  ResourcesManager(ResourcesManager const &) = delete;
  ResourcesManager &operator=(ResourcesManager const &) = delete;

  /** retrieves texture for a tileID */
  SDL_Texture *getUITexture(const std::string &uiElement, int buttonState = BUTTONSTATE_DEFAULT);

  /** Retrieves Color of a specific tileID at coordinates with the texture */

  SDL_Texture *getTileTexture(const std::string &id);
  SDL_Surface *getTileSurface(const std::string &id);

  void loadTexture(const std::string &id, const std::string &fileName);
  
  void flush();

private:
  ResourcesManager();
  ~ResourcesManager();

  /** Load Texture in to Texture Map, if an entry for this tile ID does not yet exist.
  Keep in Mind that the texture id is unique and there can't be two textures with the same id.

  If colorkey is set - Use Magic Pink (255,255,0) for transparency
  */
  void loadUITexture();


  SDL_Surface *createSurfaceFromFile(const std::string &fileName);
  SDL_Texture *createTextureFromSurface(SDL_Surface *surface);

  std::unordered_map<std::string, std::unordered_map<std::string, SDL_Texture *>> m_uiTextureMap;

  std::unordered_map<std::string, SDL_Texture *> m_tileTextureMap;
  std::unordered_map<std::string, SDL_Surface *> m_surfaceMap;
};

#endif
