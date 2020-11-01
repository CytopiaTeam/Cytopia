#include "ResourcesManager.hxx"

#include "WindowManager.hxx"
#include "TileManager.hxx"
#include "Settings.hxx"
#include "LOG.hxx"
#include "Exception.hxx"
#include "Filesystem.hxx"

#include <SDL_image.h>

#include "json.hxx"

using json = nlohmann::json;

ResourcesManager::ResourcesManager() { loadUITexture(); }

ResourcesManager::~ResourcesManager()
{
  LOG(LOG_DEBUG) << "Destroying ResourcesManager"; 
  flush(); 
}

void ResourcesManager::loadTexture(const std::string &id, const std::string &fileName)
{
  m_surfaceMap[id] = createSurfaceFromFile(fileName);
  m_tileTextureMap[id] = createTextureFromSurface(m_surfaceMap[id]);
}

void ResourcesManager::loadUITexture()
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
  if (m_uiTextureMap[uiElement].find(texture) != m_uiTextureMap[uiElement].end())
  {
    return m_uiTextureMap[uiElement].at(texture);
  }
  if (m_uiTextureMap[uiElement].find("Texture_Default") != m_uiTextureMap[uiElement].end())
  {
    // If no texture is found, check if there's a default texture
    return m_uiTextureMap[uiElement].at("Texture_Default");
  }
  throw UIError(TRACE_INFO "No texture found for " + uiElement);
  return nullptr;
}

SDL_Texture *ResourcesManager::getTileTexture(const std::string &id)
{
  if (m_tileTextureMap.find(id) != m_tileTextureMap.end())
  {
    return m_tileTextureMap.at(id);
  }
  throw UIError(TRACE_INFO "No texture found for " + id);
  return nullptr;
}

SDL_Surface *ResourcesManager::getTileSurface(const std::string &id)
{
  if (m_surfaceMap.find(id) != m_surfaceMap.end())
  {
    return m_surfaceMap.at(id);
  }
  throw UIError(TRACE_INFO "No surface found for " + id);
  return nullptr;
}

SDL_Surface *ResourcesManager::createSurfaceFromFile(const std::string &fileName)
{
  string fName = fs::getBasePath() + fileName;

  if (!fs::fileExists(fName))
    throw ConfigurationError(TRACE_INFO "File " + fName + " doesn't exist");

  SDL_Surface *surface = IMG_Load(fName.c_str());

  if (surface)
    return surface;

  throw ConfigurationError(TRACE_INFO "Could not load Texture from file " + fName + ": " + IMG_GetError());
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
