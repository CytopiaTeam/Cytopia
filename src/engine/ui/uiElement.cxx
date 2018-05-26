#include "uiElement.hxx"

UiElement::UiElement()
{
  _renderer = Resources::getRenderer();
  _window = Resources::getWindow();
  //Point _screenCoordinates = Point(90, 90);
}

UiElement::UiElement(int x, int y, int uiSpriteID) : _screenCoordinates(Point(x,y))
{
  _renderer = Resources::getRenderer();
  _window = Resources::getWindow();
  _texture = TextureManager::Instance().getUITexture(uiSpriteID);

  //Point _screenCoordinates = Point(90, 90);
}

void UiElement::render()
{
  int screen_width = Resources::settings.screenWidth;
  int screen_height = Resources::settings.screenHeight;

  renderTexture();
}

void UiElement::changeTexture(int tileID)
{
  _texture = TextureManager::Instance().getUITexture(tileID);
}

void UiElement::renderTexture(int w, int h)
{
  _destRect.x = _screenCoordinates.getX();
  _destRect.y = _screenCoordinates.getY();
  _destRect.w = w;
  _destRect.h = h;
  SDL_RenderCopy(_renderer, _texture, nullptr, &_destRect);
}

void UiElement::renderTexture()
{
  int width, height;
  SDL_QueryTexture(_texture, NULL, NULL, &width, &height);
  renderTexture(width, height);
}

bool UiElement::isClicked(int x, int y)
{
  if (x > _destRect.x && x < _destRect.x + _destRect.w
  &&  y > _destRect.y && y < _destRect.y + _destRect.h)
  {
    LOG() << "uiELEMENT is clicked!";
    return true;
  }
  return false;
}