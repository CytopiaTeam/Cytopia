#ifndef FRAME_HXX_
#define FRAME_HXX_

#include "uiElement.hxx"

class Frame : public UiElement
{
public:
  Frame(int x, int y, int w, int h, int groupID = 0, int actionID = 0, int parentOfGroup = 0);

  ~Frame() = default;

  virtual void draw();

private:
  SDL_Rect rect;

};

#endif