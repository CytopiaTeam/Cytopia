#ifndef FRAME_HXX_
#define FRAME_HXX_

#include "uiElement.hxx"

class Frame : public UiElement
{
public:
  Frame(int x, int y, int w, int h, const std::string &groupID = "", int actionID = 0, const std::string &parentOfGroup = "");

  ~Frame() = default;

  virtual void draw();

private:
  SDL_Rect rect;
};

#endif