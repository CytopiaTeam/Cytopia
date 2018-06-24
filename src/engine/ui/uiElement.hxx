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
  //UiElement() = default;
  UiElement(int x, int y, int uiSpriteID, int groupID, int actionID, int parentOfGroup);
  UiElement(int x, int y, const std::string &text, int groupID, int actionID, int parentOfGroup);
  UiElement(int x, int y, int w, int h, int groupID, int actionID, int parentOfGroup);
  virtual ~UiElement() = default;

  //Renders texture at given point
  virtual void draw();
  void changeTexture(int tileID);

  // TTF Implementation
  void drawText(const std::string &textureText, const SDL_Color &textColor);

  void drawSolidRect(SDL_Rect &rect, const SDL_Color &color);
  void drawLine(int x1, int y1, int x2, int y2, const SDL_Color &color);

  /** \brief checks if the current UI Element is clicked
    * if the derived object provides an isClicked() method, handle click events here
    * @param x, y coordinates of the mouseclick
    */
  bool isClicked(int x, int y);

  bool isVisible() { return _visible; };
  void setVisibility(bool visibility) { _visible = visibility; };

  int getGroupID() { return _groupID; };
  void setGroupID(int groupID) { _groupID = groupID; };

  int getParentID() { return _parentOf; };

  int getActionID() { return _actionID; };
  void setActionID(int actionID) { _actionID = actionID; };

  std::string &getUiElementType() { return _uiElementType; };

private:
  SDL_Texture *_texture = nullptr;
  SDL_Renderer *_renderer = Resources::getRenderer();
  SDL_Window *_window = Resources::getWindow();
  TTF_Font *_font;

  SDL_Rect _destRect;
  SDL_Surface *_surface = nullptr;

  Point _screenCoordinates;

  SDL_Color _color = SDL_Color{255, 255, 255};

  std::string _uiElementType = "";

  int _uiID;
  int _width = 0;
  int _height = 0;
  bool _visible = true;
  int _groupID = 0;
  int _actionID = 0;
  int _parentOf = 0;

  void renderTexture(int w, int h);

protected:

  void renderTexture();
};

#endif