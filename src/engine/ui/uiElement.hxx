#ifndef UIELEMENT_HXX_
#define UIELEMENT_HXX_

#include <string>
#include "../basics/point.hxx"
#include "../basics/resources.hxx"
#include "../textureManager.hxx"
#include "../basics/log.hxx"

#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"


class UiElement
{
public:
  //Initializes variables
  UiElement() = default;
  UiElement(int x, int y, int uiSpriteID);
  UiElement(int x, int y, std::string text);
  virtual ~UiElement() = default;

  //Renders texture at given point
  void render();
  void changeTexture(int tileID);

  void renderTexture(int w, int h);
 
  // TTF Implementation
  void createTextTexture(std::string &textureText, SDL_Color textColor);


  void renderTexture();
  bool isClicked(int x, int y);

  Point _screenCoordinates;
private:
  SDL_Texture * _texture;
  SDL_Renderer* _renderer = Resources::getRenderer();
  SDL_Window* _window = Resources::getWindow();
  TTF_Font* _font;

  SDL_Rect _destRect;

  Point _isoCoordinates;
  int _uiID;
  Uint8 _color;
  int _width, _height;

};

#endif