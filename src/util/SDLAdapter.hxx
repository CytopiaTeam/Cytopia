#ifndef SDL_ADAPTER_HXX
#define SDL_ADAPTER_HXX

#include <SDL2/SDL.h>

class MousePositionEvent;
class ScrollEvent;
class ClickEvent;
class RGBAColor;
class Rectangle;

namespace sdl
{
  MousePositionEvent toMousePositionEvent(const SDL_MouseMotionEvent&) noexcept;
  ScrollEvent toScrollEvent(const SDL_MouseWheelEvent &) noexcept;
  ClickEvent toClickEvent(const SDL_MouseButtonEvent &) noexcept;
  RGBAColor toColor(const SDL_Color &) noexcept;
  SDL_Color toColor(RGBAColor) noexcept;
  Rectangle toRect(const SDL_Rect &) noexcept;
  SDL_Rect toRect(const Rectangle &) noexcept;
};

#endif // SDL_ADAPTER_HXX
