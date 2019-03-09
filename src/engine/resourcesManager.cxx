#include "resourcesManager.hxx"

#include <SDL_image.h>
#include "../ThirdParty/json.hxx"

#include "windowManager.hxx"
#include "tileManager.hxx"
#include "basics/settings.hxx"
#include "basics/log.hxx"

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
  json uiDataJSON;

  std::ifstream i(SDL_GetBasePath() + Settings::instance().settings.uiDataJSONFile);
  if (i.fail())
  {
    LOG(LOG_ERROR) << "File " << Settings::instance().settings.uiDataJSONFile << " does not exist!";
    return;
  }

  // check if json file can be parsed
  uiDataJSON = json::parse(i, nullptr, false);

  if (uiDataJSON.is_discarded())
  {
    LOG(LOG_ERROR) << "Error parsing JSON File " << Settings::instance().settings.uiDataJSONFile;
    return;
  }

  i.close();

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

  return m_uiTextureMap[uiElement]["Texture_Default"];
}

SDL_Texture *ResourcesManager::getTileTexture(const std::string &id, size_t tileMapType)
{
  std::string key = id + std::to_string(tileMapType);

  if (m_tileTextureMap.count(key))
  {
    return m_tileTextureMap[key];
  }
  return nullptr;
}

SDL_Surface *ResourcesManager::getTileSurface(const std::string &id, size_t tileMapType)
{
  std::string key = id + std::to_string(tileMapType);
  if (m_surfaceMap.count(key))
  {
    return m_surfaceMap[key];
  }
  return nullptr;
}

SDL_Surface *ResourcesManager::createSurfaceFromFile(const std::string &fileName)
{
  SDL_Surface *surface = IMG_Load(fileName.c_str());

  if (surface)
  {
    return surface;
  }

  LOG(LOG_ERROR) << "Could not load Texture from file " << fileName << "\nSDL_IMAGE Error: " << IMG_GetError();
  return nullptr;
}

SDL_Texture *ResourcesManager::createTextureFromSurface(SDL_Surface *surface)
{
  SDL_Texture *texture = SDL_CreateTextureFromSurface(WindowManager::instance().getRenderer(), surface);

  if (texture)
  {
    return texture;
  }

  LOG(LOG_ERROR) << "Texture could not be created! SDL Error: " << SDL_GetError();
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