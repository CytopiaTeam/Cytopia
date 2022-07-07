#ifndef RESOURCES_MANAGER_HXX_
#define RESOURCES_MANAGER_HXX_

#include <iostream>
#include <unordered_map>
#include <stack>

#include <SDL.h>
#include <mutex>

#include "TileManager.hxx"

enum ButtonState
{
  BUTTONSTATE_DEFAULT,
  BUTTONSTATE_HOVERING,
  BUTTONSTATE_CLICKED,
  BUTTONSTATE_DISABLED
};

struct DelayedAction {
  explicit DelayedAction(const std::string &info) : dbg_info(info) {}
  virtual ~DelayedAction() = default;
  virtual void perform() = 0;
  const std::string dbg_info;
};

template<class T>
struct DelayedActionT : public DelayedAction {
  T func;

  DelayedActionT(const T &f, const std::string &dbg_info) : DelayedAction(dbg_info), func(f) {}
  void perform() override {
    func();
  }
};

struct CreateTextureAsyncParams {
  SDL_Surface *surface;
  bool deleteSurface;
  const std::string &dbg_info;
  std::function<void(SDL_Texture *)> callback;
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

  void loadTileTextureSync(const std::string &id, const std::string &fileName);
  void loadTileTextureAsync(const std::string &id, const std::string &fileName);

  void setupTileTexture(const std::string &id, SDL_Texture *texure);
  void setupUiTexture(const std::string &tileId, const std::string &id, SDL_Texture *texure);
  /**
  * @brief Get the Color Of Pixel In Surface object at a given coordinate
  * 
  * @param tileID The tileID of object to check 
  * @param x x ccoordinate within the surface
  * @param y y ccoordinate within the surface
  * @return SDL_Color at the given coordinate
  */
  SDL_Color getColorOfPixelInSurface(const std::string &tileID, int x, int y);

  void createTextureAsync(CreateTextureAsyncParams params);
  void destroyTexture(SDL_Texture *texture);

  static void waitAsyncTexture(SDL_Texture **texture);

  void performDelayedActions();
  void addDelayedAction(DelayedAction *action);

  template<class T>
  void addDelayedAction(const T &f, const std::string &dbg_info) {
    addDelayedAction(new DelayedActionT(f, dbg_info));
  }

private:
  ResourcesManager();
  ~ResourcesManager();

  /** Load Texture in to Texture Map, if an entry for this tile ID does not yet exist.
  Keep in Mind that the texture id is unique and there can't be two textures with the same id.

  If colorkey is set - Use Magic Pink (255,255,0) for transparency
  */
  void loadUITextures();

  /// Delete everything. should be called from the destuctor only
  void flush();

private:
  SDL_Surface *createSurfaceFromFile(const std::string &fileName);


  std::unordered_map<std::string, SDL_Surface *> m_surfaceMap;

  std::mutex m_textureAccess;
  std::unordered_map<std::string, SDL_Texture *> m_tileTextureMap;
  std::unordered_map<std::string, std::unordered_map<std::string, SDL_Texture *>> m_uiTextureMap;

  std::mutex m_actionAccess;
  std::stack<DelayedAction *> m_delayedActions;
};

#endif // RESOURCES_MANAGER_HXX_
