#ifndef UIELEMENT_HXX_
#define UIELEMENT_HXX_

#include <string>
#include "SDL2/SDL.h"
#include "../basics/point.hxx"
#include "../basics/resources.hxx"
#include "../textureManager.hxx"
#include "../basics/log.hxx"

class UiElement
{
public:
  //Initializes variables
  UiElement();
  UiElement(int x, int y, int uiSpriteID);


  //Deallocates memory
  ~UiElement() { };

  //Renders texture at given point
  void render();
  void changeTexture(int tileID);

  void renderTexture(int w, int h);

  void renderTexture();

  Point _screenCoordinates;
private:
  SDL_Texture * _texture;
  SDL_Renderer* _renderer;
  SDL_Window* _window;
  SDL_Rect _destRect;

  Point _isoCoordinates;
  int _uiID;

};

#endif