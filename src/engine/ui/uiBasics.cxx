#include "uiBasics.hxx"

UiBasics::UiBasics()
{
  _renderer = Resources::getRenderer();
  _window = Resources::getWindow();
  //Point _screenCoordinates = Point(90, 90);
}

UiBasics::UiBasics(int x, int y, int uiSpriteID) : _screenCoordinates(Point(x,y))
{
  _renderer = Resources::getRenderer();
  _window = Resources::getWindow();
  _texture = TextureManager::Instance().getUITexture(uiSpriteID);

  //Point _screenCoordinates = Point(90, 90);
}

void UiBasics::render()
{
  int screen_width = Resources::settings.screenWidth;
  int screen_height = Resources::settings.screenHeight;

  renderTexture();
}

void UiBasics::changeTexture(int tileID)
{
  _texture = TextureManager::Instance().getUITexture(tileID);
}

void UiBasics::renderTexture(int w, int h)
{
  _destRect.x = _screenCoordinates.getX();
  _destRect.y = _screenCoordinates.getY();
  _destRect.w = w;
  _destRect.h = h;
  SDL_RenderCopy(_renderer, _texture, nullptr, &_destRect);
}

void UiBasics::renderTexture()
{
  int width, height;
  SDL_QueryTexture(_texture, NULL, NULL, &width, &height);
  renderTexture(width, height);
}