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
    //SDL_FreeSurface(loadedImage);

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


// THIS WORKS!!!!!!!!!!!!!!
bool TextureManager::isPixelTransparent(int tileID, int ScreenX, int ScreenY)
{

  SDL_Texture* tex = _textureMap[tileID];

  // texture rect
  int width, height;
  SDL_Rect spriteInformation;
  SDL_QueryTexture(_textureMap[tileID], NULL, NULL, &width, &height);
  
  int X = ScreenX;
  int Y = ScreenY;
  //int X = ScreenX - spriteInformation.x;
  //int Y = ScreenY - spriteInformation.y;
  
  //std::cout << "X = " << X << "and rect X = " << spriteInformation.x << std::endl;
  //std::cout << "Y = " << X << "and rect Y = " << spriteInformation.y <<  std::endl;

  //int x = clickedCoords.getX();
  //int y = clickedCoords.getY();

  SDL_Surface* pSurface = _surfaceMap[tileID];

  //if (x >= myRect.x && x < (myRect.x + myRect.w))

  int      Bpp = pSurface->format->BytesPerPixel;
  Uint8*   pPixel = (Uint8*)pSurface->pixels + Y * pSurface->pitch + X * Bpp;

  Uint32 PixelColor = *(Uint32*)pPixel;

  SDL_Color Color = { 0, 0, 0, 0 };
  Uint8 Alpha = SDL_ALPHA_OPAQUE;

  SDL_GetRGBA(PixelColor, pSurface->format, &Color.r, &Color.g, &Color.b, &Alpha);

  std::cout << "Pixel Color = " << (int)Color.r << "," << (int)Color.g << "," << (int)Color.b << std::endl ;
  std::cout << "Pixel Alpha detected = " << (int)Alpha << std::endl;

  if (Color.a == SDL_ALPHA_TRANSPARENT)
    return true;
  else
    return false;
}