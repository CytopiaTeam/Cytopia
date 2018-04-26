#include "texture.hxx"


SDL_Texture *loadTexture(std::string file, SDL_Renderer *renderer, bool colorkey)
{
  SDL_Texture *texture = nullptr;
  SDL_Surface *loadedImage = IMG_Load(file.c_str());

  if (loadedImage != nullptr)
  {
    if (colorkey)
    {
      SDL_SetColorKey(loadedImage, SDL_TRUE, SDL_MapRGB(loadedImage->format, 0, 0xFF, 0xFF));	//Cyan.
    }
    texture = SDL_CreateTextureFromSurface(renderer, loadedImage);
    SDL_FreeSurface(loadedImage);
    if (texture == nullptr)
      printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
  }
  else
    printf("Error loading File %s", IMG_GetError());

  return texture;
}


void renderTexture(SDL_Texture *texture, SDL_Renderer *renderer, Point tileScreenCoords, int w, int h)
{
  SDL_Rect dst;
  dst.x = tileScreenCoords.getX();
  dst.y = tileScreenCoords.getY();
  dst.w = w;
  dst.h = h;
  SDL_RenderCopy(renderer, texture, nullptr, &dst);
}

void renderTexture(SDL_Texture *texture, SDL_Renderer *renderer, Point tileScreenCoords)
{
  int w, h;
  SDL_QueryTexture(texture, NULL, NULL, &w, &h);
  renderTexture(texture, renderer, tileScreenCoords, w, h);
}
