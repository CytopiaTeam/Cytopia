#include "textureManager.hxx"

TextureManager::TextureManager()
{
  _renderer = Resources::getRenderer();
  _window = Resources::getWindow();
}

void TextureManager::loadTexture(int tileID, bool colorKey) 
{
  std::string fileName = Resources::getTileDataFromJSON("terrain", tileID, "filename");
  SDL_Surface* loadedImage = nullptr;
  loadedImage = IMG_Load(fileName.c_str());

  if ( loadedImage != nullptr )
  {
    if ( colorKey )
      SDL_SetColorKey(loadedImage, SDL_TRUE, SDL_MapRGB(loadedImage->format, 0, 0xFF, 0xFF));	

    _surfaceMap[tileID] = loadedImage;
    SDL_Texture* _texture = SDL_CreateTextureFromSurface(_renderer, loadedImage);

    if ( _texture != nullptr )
      _textureMap[tileID] = _texture;
    else
      std::cerr << "ERROR: Renderer could not be created! SDL Error: %s\n\t" << SDL_GetError();
  }
  else
    std::cerr << "ERROR : Couldn't load Texture from file : " << fileName << std::endl << "\t" << IMG_GetError();
}

SDL_Texture* TextureManager::getTexture(int tileID)
{
  if (! _textureMap.count(tileID))
  {
    loadTexture(tileID);
  }
  return _textureMap[tileID];
}

SDL_Surface* TextureManager::getSurface(int tileID)
{
  if (_surfaceMap[tileID] != nullptr)
  {
    return _surfaceMap[tileID];
  }
  else
    std::cerr << "EMPTY SURFACE at TILE ID " << tileID << std::endl;
}

SDL_Color TextureManager::GetPixelColor(int tileID, int X, int Y)
{
  SDL_Surface* pSurface = _surfaceMap[tileID];
  SDL_UnlockSurface(pSurface);
  int      Bpp = pSurface->format->BytesPerPixel;
  Uint8*   pPixel = (Uint8*)pSurface->pixels + Y * pSurface->pitch+ X * Bpp ;
    
  Uint32 pixelColor;
  Uint32 PixelColor = *(Uint32*)pPixel;

  SDL_Color Color = { 0, 0, 0, SDL_ALPHA_TRANSPARENT };
  SDL_GetRGBA(PixelColor, pSurface->format, &Color.r, &Color.g, &Color.b, &Color.a);
  return Color;
}