#include "ResourcesManager.hxx"

#include "WindowManager.hxx"
#include "TileManager.hxx"
#include "basics/Settings.hxx"
#include "LOG.hxx"
#include "Exception.hxx"

#include <SDL_image.h>

#include "json.hxx"

using json = nlohmann::json;

ResourcesManager::ResourcesManager() { loadUITexture(); }

ResourcesManager::~ResourcesManager() { flush(); }

void ResourcesManager::loadTexture(const std::string &id, const std::string &fileName, size_t tileMapType)
{
  std::string key = id + std::to_string(tileMapType);
  m_surfaceMap[key] = createSurfaceFromFile(fileName);
  m_tileTextureMap[key] = createTextureFromSurface(m_surfaceMap[key]);
}

void ResourcesManager::loadUITexture()
{
  string fName = SDL_GetBasePath() + Settings::instance().uiDataJSONFile.get();
  std::ifstream i(fName);
  if (!i)
    throw ConfigurationError(TRACE_INFO "Couldn't open file " + fName);

  // check if json file can be parsed
  const json uiDataJSON = json::parse(i, nullptr, false);

  if (uiDataJSON.is_discarded())
    throw ConfigurationError(TRACE_INFO "Error parsing JSON File " + fName);

  for (const auto &tileID : uiDataJSON.items())
  {
    for (auto it = uiDataJSON[tileID.key()].begin(); it != uiDataJSON[tileID.key()].end(); ++it)
    {
      m_uiTextureMap[tileID.key()][it.key()] = createTextureFromSurface(createSurfaceFromFile(it.value()));
    }
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

  if (m_uiTextureMap[uiElement].count(texture))
    return m_uiTextureMap[uiElement][texture];
  else if (m_uiTextureMap[uiElement].count("Texture_Default"))
  {
    // If no texture is found, check if there's a default texture
    return m_uiTextureMap[uiElement]["Texture_Default"];
  }
  throw UIError(TRACE_INFO "No texture found for " + uiElement);
  return nullptr;
}

SDL_Texture *ResourcesManager::getTileTexture(const std::string &id, size_t tileMapType)
{
  std::string key = id + std::to_string(tileMapType);

  if (m_tileTextureMap.count(key))
  {
    return m_tileTextureMap[key];
  }
  throw UIError(TRACE_INFO "No texture found for " + id);
  return nullptr;
}

SDL_Surface *ResourcesManager::getTileSurface(const std::string &id, size_t tileMapType)
{
  std::string key = id + std::to_string(tileMapType);
  if (m_surfaceMap.count(key))
    return m_surfaceMap[key];
  throw UIError(TRACE_INFO "No surface found for " + id);
  return nullptr;
}

SDL_Surface *ResourcesManager::createSurfaceFromFile(const std::string &fileName)
{
  string fName = SDL_GetBasePath() + fileName;
  SDL_Surface *surface = IMG_Load(fName.c_str());

  if (surface)
    return surface;

  throw UIError(TRACE_INFO "Could not load Texture from file " + fName + ": " + IMG_GetError());
  return nullptr;
}

SDL_Texture *ResourcesManager::createTextureFromSurface(SDL_Surface *surface)
{
  SDL_Texture *texture = SDL_CreateTextureFromSurface(WindowManager::instance().getRenderer(), surface);

  if (texture)
    return texture;

  throw UIError(TRACE_INFO "Texture could not be created! SDL Error: " + string{SDL_GetError()});
  return nullptr;
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
