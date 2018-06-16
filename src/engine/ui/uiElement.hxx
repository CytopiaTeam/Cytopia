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
  void drawText(const std::string& textureText, const SDL_Color& textColor);

  void drawSolidRect(SDL_Rect& rect, const SDL_Color& color);

  /** \brief checks if the current UI Element is clicked
    * if the derived object provides an isClicked() method, handle click events here
    * @param x, y coordinates of the mouseclick
    */
  bool isClicked(int x, int y);
  bool isVisible();
  void setVisibility(bool visibility) { _visible = visibility; };

  int getGroupID() { return _groupID; };
  void setGroupID(int groupID) { _groupID = groupID; };

  int getAction() { return _action; };
  void setAction(int action) { _action = action; };
  Point _screenCoordinates;
private:
  SDL_Texture * _texture = nullptr;
  SDL_Renderer* _renderer = Resources::getRenderer();
  SDL_Window* _window = Resources::getWindow();
  TTF_Font* _font;

  SDL_Rect _destRect;
  SDL_Surface* _surface = nullptr;

  Point _isoCoordinates;
  SDL_Color _color = SDL_Color{ 255,255,255 };
  int _uiID;
  int _width, _height;
  int _groupID = 0;
  bool _visible = true;

  int _action = 0;

  void renderTexture();
  void renderTexture(int w, int h);
};

#endif