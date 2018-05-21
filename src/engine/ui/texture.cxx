#include "texture.hxx"

Texture::Texture()
{
  _renderer = Resources::getRenderer();
  _window = Resources::getWindow();
  Point _screenCoordinates = Point(90, 90);
}

void Texture::render()
{
  int screen_width = Resources::settings.screenWidth;
  int screen_height = Resources::settings.screenHeight;

  renderTexture();
}

void Texture::changeTexture(int tileID)
{
  _texture = TextureManager::Instance().getTexture(tileID);
}

void Texture::renderTexture(int w, int h)
{
  _destRect.x = _screenCoordinates.getX();
  _destRect.y = _screenCoordinates.getY();
  _destRect.w = w;
  _destRect.h = h;
  SDL_RenderCopy(_renderer, _texture, nullptr, &_destRect);
}

void Texture::renderTexture()
{
  int width, height;
  SDL_QueryTexture(_texture, NULL, NULL, &width, &height);
  renderTexture(width, height);
}