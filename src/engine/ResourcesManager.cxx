#include "ResourcesManager.hxx"

#include "WindowManager.hxx"
#include "TileManager.hxx"
#include "Settings.hxx"
#include "LOG.hxx"
#include "Exception.hxx"
#include "Filesystem.hxx"

#include <SDL_image.h>

#include "json.hxx"

#include <thread>
#include <chrono>

using json = nlohmann::json;

ResourcesManager::ResourcesManager() { loadUITextures(); }
ResourcesManager::~ResourcesManager() { flush(); }

void ResourcesManager::loadTileTextureSync(const std::string &id, const std::string &fileName)
{
  if (m_tileTextureMap.count(id))
    return;

  m_surfaceMap[id] = createSurfaceFromFile(fileName);
  createTextureAsync({m_surfaceMap[id], /*destroySurface*/false, std::string("load texture ").append(fileName), [this, id] (auto *texture) {
    this->setupTileTexture(id, texture);
  }});

  performDelayedActions();
}

void ResourcesManager::loadTileTextureAsync(const std::string &id, const std::string &fileName)
{
  if (m_tileTextureMap.count(id))
    return;

  m_surfaceMap[id] = createSurfaceFromFile(fileName);
  createTextureAsync({m_surfaceMap[id], /*destroySurface*/false, std::string("load texture ").append(fileName), [this, id] (auto *texture) {
    this->setupTileTexture(id, texture);
  }});
}

void ResourcesManager::setupTileTexture(const std::string &id, SDL_Texture *texture) {
  std::lock_guard _(this->m_textureAccess);
  m_tileTextureMap[id] = texture;
}

void ResourcesManager::setupUiTexture(const std::string &tileId, const std::string &id, SDL_Texture *texture) {
  std::lock_guard _(this->m_textureAccess);
  m_uiTextureMap[tileId][id] = texture;
}

void ResourcesManager::loadUITextures()
{
  std::string jsonFileContent = fs::readFileAsString(Settings::instance().uiDataJSONFile.get());
  const json uiDataJSON = json::parse(jsonFileContent, nullptr, false);

  // check if json file can be parsed
  if (uiDataJSON.is_discarded())
    throw ConfigurationError(TRACE_INFO "Error parsing JSON File " + Settings::instance().uiDataJSONFile.get());

  for (const auto &tileID : uiDataJSON.items())
  {
    for (auto it = uiDataJSON[tileID.key()].begin(); it != uiDataJSON[tileID.key()].end(); ++it)
    {
      auto *surface = createSurfaceFromFile(it.value());
      createTextureAsync({surface, /*destroySurface*/true, std::string("load ui texture").append(it.value()), [this, tileKey = tileID.key(), itKey = it.key()] (auto *texture) {
        this->setupUiTexture(tileKey, itKey, texture);
      }});
    }
    performDelayedActions();
  }
}

SDL_Texture *ResourcesManager::getUITexture(const std::string &uiElement, int buttonState)
{
  std::string texture;
  switch (buttonState)
  {
  case BUTTONSTATE_CLICKED:
  texture = "Texture_Clicked";
  break;
  case BUTTONSTATE_HOVERING:
  texture = "Texture_Hovering";
  break;
  default:
  texture = "Texture_Default";
  }
  if (m_uiTextureMap[uiElement].find(texture) != m_uiTextureMap[uiElement].end())
  {
    return m_uiTextureMap[uiElement].at(texture);
  }
  if (m_uiTextureMap[uiElement].find("Texture_Default") != m_uiTextureMap[uiElement].end())
  {
    // If no texture is found, check if there's a default texture
    return m_uiTextureMap[uiElement].at("Texture_Default");
  }
  LOG(LOG_DEBUG) << "No texture loaded for " << uiElement;
  return nullptr;
}

SDL_Texture *ResourcesManager::getTileTexture(const std::string &id)
{
  auto it = m_tileTextureMap.find(id);
  return ((it != m_tileTextureMap.end()) ? it->second : nullptr);
}

SDL_Surface *ResourcesManager::getTileSurface(const std::string &id)
{
  if (m_surfaceMap.find(id) != m_surfaceMap.end())
  {
    return m_surfaceMap.at(id);
  }
  throw UIError(TRACE_INFO "No surface found for " + id);
}

SDL_Surface *ResourcesManager::createSurfaceFromFile(const std::string &fileName)
{
  std::unique_lock lock(m_actionAccess);
  string fName = fs::getBasePath() + fileName;

  if (!fs::fileExists(fName))
    throw ConfigurationError(TRACE_INFO "File " + fName + " doesn't exist");

  SDL_Surface *surface = IMG_Load(fName.c_str());

  if (surface)
    return surface;

  throw ConfigurationError(TRACE_INFO "Could not load Texture from file " + fName + ": " + IMG_GetError());
}

void ResourcesManager::destroyTexture(SDL_Texture *texture)
{
  addDelayedAction(new DelayedActionT([texture] {
    SDL_DestroyTexture(texture);
  }, std::string("destroy texture")));
}

void ResourcesManager::waitAsyncTexture(SDL_Texture **texture) {
  if (!texture)
    return;

  while (!*texture) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

//bool is_main_thread() { return get_current_thread_id() == get_main_thread_id(); }
void ResourcesManager::performDelayedActions () {
  std::unique_lock lock(m_actionAccess);

  while (!m_delayedActions.empty()) {
    DelayedAction *action = m_delayedActions.top();
    m_delayedActions.pop();

    if (!action)
      continue;

    action->perform();
    delete action;
  }
}

void ResourcesManager::addDelayedAction(DelayedAction *action) {
  std::unique_lock lock(m_actionAccess);

  m_delayedActions.push(action);
}

void ResourcesManager::createTextureAsync(CreateTextureAsyncParams params)
{
  addDelayedAction(new DelayedActionT([params] {
    if (!params.callback) {
      if (params.deleteSurface)
        SDL_FreeSurface(params.surface);
      return;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(WindowManager::instance().getRenderer(), params.surface);
    if (params.deleteSurface)
    {
      SDL_FreeSurface(params.surface);
    }

    if (params.callback) {
      params.callback(texture);
    }
  }, params.dbg_info));
}

void ResourcesManager::flush()
{
  for (const auto &it : m_surfaceMap)
  {
    SDL_FreeSurface(it.second);
  }
  m_surfaceMap.clear();

  for (const auto &it : m_tileTextureMap)
  {
    SDL_DestroyTexture(it.second);
  }
  m_tileTextureMap.clear();

  for (const auto &it : m_uiTextureMap)
  {
    for (const auto &ita : it.second)
    {
      SDL_DestroyTexture(ita.second);
    }
  }
  m_uiTextureMap.clear();
}

SDL_Color ResourcesManager::getColorOfPixelInSurface(const std::string &tileID, int x, int y)
{
  SDL_Color Color{0, 0, 0, SDL_ALPHA_TRANSPARENT};
  // create and initialize a variable within the condition
  if (SDL_Surface *surface = getTileSurface(tileID); surface)
  {
    const int bpp = surface->format->BytesPerPixel;
    Uint8 *p = &static_cast<Uint8 *>(surface->pixels)[y * surface->pitch + x * bpp];
    const Uint32 pixel = *reinterpret_cast<Uint32 *>(p);

    SDL_GetRGBA(pixel, surface->format, &Color.r, &Color.g, &Color.b, &Color.a);
  }

  return Color;
}