#include "textureManager.hxx"

TextureManager::TextureManager()
{
  _renderer = Resources::getRenderer();
  _window = Resources::getWindow();
}

void TextureManager::loadTexture(int tileID, bool colorKey) 
{
  std::string fileName = Resources::getTileDataFromJSON("terrain", tileID, "filename");
  SDL_Surface* loadedImage = IMG_Load(fileName.c_str());

  if ( loadedImage != nullptr )
  {
    if ( colorKey )
      SDL_SetColorKey(loadedImage, SDL_TRUE, SDL_MapRGB(loadedImage->format, 0, 0xFF, 0xFF));	

    _surfaceMap[tileID] = loadedImage;
    SDL_Texture* _texture = SDL_CreateTextureFromSurface(_renderer, loadedImage);

    if ( _texture != nullptr )
      _textureMap[tileID] = _texture;
    else
      LOG(LOG_ERROR) << "Renderer could not be created! SDL Error: " << SDL_GetError();
  }
  else
    LOG(LOG_ERROR) << "Could not load Texture from file " << fileName << "\nSDL_IMAGE Error: " << IMG_GetError();
}

SDL_Texture* TextureManager::getTexture(int tileID)
{
  // If the texture isn't in the map, load it first.
  if (! _textureMap.count(tileID))
  {
    loadTexture(tileID);
  }
  return _textureMap[tileID];
}

SDL_Surface* TextureManager::getSurface(int tileID)
{
  // If the surface isn't in the map, load the texture first.
  if (!_textureMap.count(tileID))
  {
    loadTexture(tileID);
  }
  return _surfaceMap[tileID];
}

SDL_Color TextureManager::GetPixelColor(int tileID, int X, int Y)
{
  SDL_Surface* surface = _surfaceMap[tileID];
  int Bpp = surface->format->BytesPerPixel;
  Uint8* p = (Uint8*)surface->pixels + Y * surface->pitch + X * Bpp ;
    
  Uint32 pixel = *(Uint32*)p;

  SDL_Color Color = { 0, 0, 0, SDL_ALPHA_TRANSPARENT };
  SDL_GetRGBA(pixel, surface->format, &Color.r, &Color.g, &Color.b, &Color.a);
  
  return Color;
}