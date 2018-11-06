#include "textureManager.hxx"

#include "SDL2/SDL_image.h"
#include "../ThirdParty/json.hxx"

#include "basics/resources.hxx"
#include "basics/log.hxx"
#include "tile.hxx"

using json = nlohmann::json;

json tileDataJSON;
json uiDataJSON;

TextureManager::TextureManager()
{
  // Read JSON File.
  std::ifstream i("resources/data/TileData.json");
  if (i.fail())
  {
    LOG(LOG_ERROR) << "File "
                   << "resources/data/TileData.json"
                   << " does not exist! Cannot load settings from INI File!";
    return;
  }

  // check if json file can be parsed
  tileDataJSON = json::parse(i, nullptr, false);
  if (tileDataJSON.is_discarded())
    LOG(LOG_ERROR) << "Error parsing JSON File "
                   << "resources/data/TileData.json";
  i.close();

  loadUITexture();
}

void TextureManager::loadTexture(TileType type, TileOrientation orientation, bool colorKey)
{
  std::string fileName;
  std::string typeString = tileTypesEnumToString(type);
  size_t idx = 0;

  while (!tileDataJSON[typeString][idx].is_null())
  {
    for (auto &it : tileDataJSON[typeString][idx].items())
    {
      if (it.key() == "orientation" && it.value() == tileOrientationEnumToString(orientation))
      {
        if (!tileDataJSON[typeString][idx]["image"].is_null())
        {
          fileName = tileDataJSON[typeString][idx]["image"].get<std::string>();
        }
      }
    }
    idx++;
  }

  SDL_Surface *loadedImage = IMG_Load(fileName.c_str());

  if (loadedImage)
  {
    _surfaceMap[type][orientation] = loadedImage;

    if (colorKey)
      SDL_SetColorKey(loadedImage, SDL_TRUE, SDL_MapRGB(loadedImage->format, 0xFF, 0, 0xFF));

    SDL_Texture *texture = SDL_CreateTextureFromSurface(Resources::getRenderer(), loadedImage);

    if (texture)
    {
      _tileTextureMap[type][orientation] = texture;
    }
    else
      LOG(LOG_ERROR) << "Texture could not be created! SDL Error: " << SDL_GetError();
  }
  else
    LOG(LOG_ERROR) << "Could not load Texture from file " << fileName << "\nSDL_IMAGE Error: " << IMG_GetError();
}

void TextureManager::loadUITexture()
{
  std::ifstream i("resources/data/UiData.json");
  if (i.fail())
  {
    LOG(LOG_ERROR) << "File "
                   << "resources/data/UiData.json"
                   << " does not exist! Cannot load settings from INI File!";
    return;
  }

  // check if json file can be parsed
  uiDataJSON = json::parse(i, nullptr, false);

  if (uiDataJSON.is_discarded())
  {
    LOG(LOG_ERROR) << "Error parsing JSON File resources/data/UiData.json";
    return;
  }

  i.close();

  for (auto tileID : uiDataJSON.items())
  {
    for (auto it = uiDataJSON[tileID.key()].begin(); it != uiDataJSON[tileID.key()].end(); ++it)
    {
      _uiTextureMapNew[tileID.key()][it.key()] = createTextureFromSurface(createSurfaceFromFile(it.value()));
    }
  }
}

SDL_Surface *TextureManager::createSurfaceFromFile(std::string fileName)
{
  SDL_Surface *surface = IMG_Load(fileName.c_str());

  if (surface)
  {
    return surface;
  }

  LOG(LOG_ERROR) << "Could not load Texture from file " << fileName << "\nSDL_IMAGE Error: " << IMG_GetError();
  return nullptr;
}

SDL_Texture *TextureManager::createTextureFromSurface(SDL_Surface *surface)
{
  SDL_Texture *texture = SDL_CreateTextureFromSurface(Resources::getRenderer(), surface);

  if (texture)
  {
    return texture;
  }

  LOG(LOG_ERROR) << "Texture could not be created! SDL Error: " << SDL_GetError();
  return nullptr;
}

SDL_Texture *TextureManager::getTileTexture(TileType type, TileOrientation orientation)
{
  // If the texture isn't in the map, load it first.
  if (!_tileTextureMap[type][orientation])
  {
    loadTexture(type, orientation);
  }

  return _tileTextureMap[type][orientation];
}

SDL_Texture *TextureManager::getUITexture(std::string uiElement, int buttonState)
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

  if (_uiTextureMapNew[uiElement].count(texture))
    return _uiTextureMapNew[uiElement][texture];

  return _uiTextureMapNew[uiElement]["Texture_Default"];
}

const SDL_Color TextureManager::getPixelColor(TileType type, TileOrientation orientation, int X, int Y)
{
  SDL_Color Color = {0, 0, 0, SDL_ALPHA_TRANSPARENT};

  if (_surfaceMap[type].count(orientation))
  {
    SDL_Surface *surface = _surfaceMap[type][orientation];

    int Bpp = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)surface->pixels + Y * surface->pitch + X * Bpp;
    Uint32 pixel = *(Uint32 *)p;

    SDL_GetRGBA(pixel, surface->format, &Color.r, &Color.g, &Color.b, &Color.a);
  }
  else
  {
    LOG(LOG_ERROR) << "No surface in map for type " << tileTypesEnumToString(type)
                   << " with orientation: " << tileOrientationEnumToString(orientation);
  }
  return Color;
}

std::string TextureManager::tileOrientationEnumToString(TileOrientation orientation)
{
  switch (orientation)
  {
  case TileOrientation::DEFAULT:
    return "default";
  case TileOrientation::CENTER:
    return "center";
  case TileOrientation::N:
    return "n";
  case TileOrientation::E:
    return "e";
  case TileOrientation::S:
    return "s";
  case TileOrientation::W:
    return "w";
  case TileOrientation::NE:
    return "ne";
  case TileOrientation::NW:
    return "nw";
  case TileOrientation::SE:
    return "se";
  case TileOrientation::SW:
    return "sw";
  case TileOrientation::N_AND_E:
    return "n_and_e";
  case TileOrientation::N_AND_W:
    return "n_and_w";
  case TileOrientation::S_AND_E:
    return "s_and_e";
  case TileOrientation::S_AND_W:
    return "s_and_w";
  }
  return "";
}

std::string TextureManager::tileTypesEnumToString(TileType tileType)
{
  switch (tileType)
  {
  case TileType::TERRAIN:
    return "Terrain";
  case TileType::WATER:
    return "Water";
  case TileType::ROAD:
    return "Road";
  case TileType::RESIDENTIAL:
    return "Residential";
  case TileType::COMMERCIAL:
    return "Commercial";
  case TileType::INDUSTRIAL:
    return "Industrial";
  }
  return "";
}