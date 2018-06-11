#include <string>

#include "uiElement.hxx"
#include "../engine/basics/resources.hxx"

#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"

class Text : public UiElement
{
public:
  Text(int x, int y, std::string& text);
  ~Text() = default;

private:
  std::string _text;




};