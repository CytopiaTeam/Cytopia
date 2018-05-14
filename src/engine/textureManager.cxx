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

    SDL_Texture* _texture = SDL_CreateTextureFromSurface(_renderer, loadedImage);
    SDL_FreeSurface(loadedImage);

    if ( _texture != nullptr )
      _textureMap[tileID] = _texture;
    else
      std::cerr << "Renderer could not be created! SDL Error: %s\n\t" << SDL_GetError();
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

