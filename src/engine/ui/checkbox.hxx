#ifndef CHECKBOX_HXX_
#define CHECKBOX_HXX_

#include "uiElement.hxx"

class Checkbox : public UiElement
{
public:
  Checkbox(int x, int y, const std::string &groupID);
  ~Checkbox() = default;

  SDL_Rect rect;
  virtual void draw();

private:
  bool checked = false;
};

#endif