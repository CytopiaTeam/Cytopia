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
  UiElement(int x, int y, int w, int h);
  virtual ~UiElement() = default;

  //Renders texture at given point
  void render();
  void changeTexture(int tileID);

 
  // TTF Implementation
  void createTextTexture(const std::string& textureText, const SDL_Color& textColor);

  void drawSolidRect(SDL_Rect& rect);

  /** \brief checks if the current UI Element is clicked
    * if the derived object provides an isClicked() method, handle click events here
    * @param x, y coordinates of the mouseclick
    */
  bool getClickedUiElement(int x, int y);

  bool isClicked(int x, int y);

  Point _screenCoordinates;
private:
  SDL_Texture * _texture = nullptr;
  SDL_Renderer* _renderer = Resources::getRenderer();
  SDL_Window* _window = Resources::getWindow();
  TTF_Font* _font;

  SDL_Rect _destRect;
  SDL_Surface* _surface = nullptr;

  Point _isoCoordinates;
  int _uiID;
  SDL_Color _color = SDL_Color{ 255,255,255 };
  int _width, _height;

  void renderTexture();
  void renderTexture(int w, int h);
};

#endif