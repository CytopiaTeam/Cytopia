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
  SDL_Surface *loadedImage = IMG_Load(fileName.c_str());

  if (loadedImage)
  {
    if (colorKey)
      SDL_SetColorKey(loadedImage, SDL_TRUE, SDL_MapRGB(loadedImage->format, 0xFF, 0, 0xFF));

    _uiSurfaceMap[uiSpriteID] = loadedImage;
    SDL_Texture *_texture = SDL_CreateTextureFromSurface(Resources::getRenderer(), loadedImage);

    if (_texture != nullptr)
      _uiTextureMap[uiSpriteID] = _texture;
    else
      LOG(LOG_ERROR) << "Renderer could not be created! SDL Error: " << SDL_GetError();
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

SDL_Texture *TextureManager::getUITexture(int tileID)
{
  // If the texture isn't in the map, load it first.
  if (!_uiTextureMap.count(tileID))
  {
    loadUITexture(tileID);
  }
  return _uiTextureMap[tileID];
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

SDL_Color TextureManager::GetPixelColor(int tileID, int X, int Y)
{
  SDL_Surface *surface = _surfaceMap[tileID];
  int Bpp = surface->format->BytesPerPixel;
  Uint8 *p = (Uint8 *)surface->pixels + Y * surface->pitch + X * Bpp;

  Uint32 pixel = *(Uint32 *)p;

  SDL_Color Color = {0, 0, 0, SDL_ALPHA_TRANSPARENT};
  SDL_GetRGBA(pixel, surface->format, &Color.r, &Color.g, &Color.b, &Color.a);

  return Color;
}