#include "textureManager.hxx"

void TextureManager::loadTexture(int tileID, bool colorKey)
{
  std::string fileName = Resources::getTileDataFromJSON("terrain", tileID, "filename");
  SDL_Surface *loadedImage = IMG_Load(fileName.c_str());

  if (loadedImage)
  {
    if (colorKey)
      SDL_SetColorKey(loadedImage, SDL_TRUE, SDL_MapRGB(loadedImage->format, 0xFF, 0, 0xFF));

    _surfaceMap[tileID] = loadedImage;
    SDL_Texture *_texture = SDL_CreateTextureFromSurface(Resources::getRenderer(), loadedImage);

    if (_texture)
      _textureMap[tileID] = _texture;
    else
      LOG(LOG_ERROR) << "Renderer could not be created! SDL Error: " << SDL_GetError();
  }
  else
    LOG(LOG_ERROR) << "Could not load Texture from file " << fileName << "\nSDL_IMAGE Error: " << IMG_GetError();
}

void TextureManager::loadUITexture(int uiSpriteID, bool colorKey)
{
  std::string fileName = Resources::getUISpriteDataFromJSON("button", uiSpriteID, "filename");
  std::string fileNameHover = Resources::getUISpriteDataFromJSON("button", uiSpriteID, "textureHover");
  std::string fileNamePressed = Resources::getUISpriteDataFromJSON("button", uiSpriteID, "texturePressed");
  SDL_Surface *loadedImage = IMG_Load(fileName.c_str());

  if (loadedImage)
  {
    if (colorKey)
    {
      SDL_SetColorKey(loadedImage, SDL_TRUE, SDL_MapRGB(loadedImage->format, 0xFF, 0, 0xFF));
    }

    _uiSurfaceMap[uiSpriteID] = loadedImage;
    SDL_Texture *_texture = SDL_CreateTextureFromSurface(Resources::getRenderer(), loadedImage);

    if (_texture != nullptr)
      _uiTextureMap[uiSpriteID] = _texture;
    else
      LOG(LOG_ERROR) << "Renderer could not be created! SDL Error: " << SDL_GetError();

    // Load hover / clicked textures if they are available.

    SDL_FreeSurface(loadedImage);

    if (!fileNameHover.empty())
    {
      SDL_Surface *loadedImage = IMG_Load(fileNameHover.c_str());
      if (loadedImage)
      {
        if (colorKey)
        {
          SDL_SetColorKey(loadedImage, SDL_TRUE, SDL_MapRGB(loadedImage->format, 0xFF, 0, 0xFF));
        }

        SDL_Texture *_texture = SDL_CreateTextureFromSurface(Resources::getRenderer(), loadedImage);

        if (_texture != nullptr)
          _uiTextureMapHover[uiSpriteID] = _texture;
        else
          LOG(LOG_ERROR) << "Renderer could not be created! SDL Error: " << SDL_GetError();
        SDL_FreeSurface(loadedImage);
      }
    }
    if (!fileNamePressed.empty())
    {
      SDL_Surface *loadedImage = IMG_Load(fileNamePressed.c_str());
      if (loadedImage)
      {
        if (colorKey)
        {
          SDL_SetColorKey(loadedImage, SDL_TRUE, SDL_MapRGB(loadedImage->format, 0xFF, 0, 0xFF));
        }

        SDL_Texture *_texture = SDL_CreateTextureFromSurface(Resources::getRenderer(), loadedImage);

        if (_texture != nullptr)
          _uiTextureMapPressed[uiSpriteID] = _texture;
        else
          LOG(LOG_ERROR) << "Renderer could not be created! SDL Error: " << SDL_GetError();
        SDL_FreeSurface(loadedImage);
      }
    }
  }
  else
    LOG(LOG_ERROR) << "Could not load Texture from file " << fileName << "\nSDL_IMAGE Error: " << IMG_GetError();
}

SDL_Texture *TextureManager::getTileTexture(int tileID)
{
  // If the texture isn't in the map, load it first.
  if (!_textureMap.count(tileID))
  {
    loadTexture(tileID);
  }
  return _textureMap[tileID];
}

SDL_Texture *TextureManager::getUITexture(int uiSpriteID, int buttonState)
{
  if (!_uiTextureMap.count(uiSpriteID))
  {
    loadUITexture(uiSpriteID);
  }

  switch (buttonState)
  {
  case HOVERING:
    if (_uiTextureMapHover.count(uiSpriteID))
    {
      return _uiTextureMapHover[uiSpriteID];
    }
    else
    {
      return _uiTextureMap[uiSpriteID];
    }
  case CLICKED:
    if (_uiTextureMapPressed.count(uiSpriteID))
    {
      return _uiTextureMapPressed[uiSpriteID];
    }
    else
    {
      return _uiTextureMap[uiSpriteID];
    }
    return nullptr;
  default:
    return _uiTextureMap[uiSpriteID];
  }
  // If the texture isn't in the map, load it first.
}

SDL_Surface *TextureManager::getTileSurface(int tileID)
{
  // If the surface isn't in the map, load the texture first.
  if (!_textureMap.count(tileID))
  {
    loadTexture(tileID);
  }
  return _surfaceMap[tileID];
}

SDL_Surface *TextureManager::getUISurface(int uiSpriteID)
{
  // If the surface isn't in the map, load the texture first.
  if (!_textureMap.count(uiSpriteID))
  {
    loadTexture(uiSpriteID);
  }
  return _surfaceMap[uiSpriteID];
}

const SDL_Color TextureManager::getPixelColor(int tileID, int X, int Y)
{
  SDL_Color Color = {0, 0, 0, SDL_ALPHA_TRANSPARENT};

  if (_surfaceMap.find(tileID) != _surfaceMap.end())
  {
    SDL_Surface *surface = _surfaceMap[tileID];

    int Bpp = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)surface->pixels + Y * surface->pitch + X * Bpp;
    Uint32 pixel = *(Uint32 *)p;

    SDL_GetRGBA(pixel, surface->format, &Color.r, &Color.g, &Color.b, &Color.a);
  }
  else
  {
    LOG(LOG_ERROR) << "No surface in map for tileID " << tileID;
  }
  return Color;
}