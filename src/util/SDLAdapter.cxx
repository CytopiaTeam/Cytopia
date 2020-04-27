#include "SDLAdapter.hxx"
#include "../events/MouseEvents.hxx"
#include "../util/Color.hxx"
#include "../util/Rectangle.hxx"

MousePositionEvent sdl::toMousePositionEvent(const SDL_MouseMotionEvent & e) noexcept
{
  return { e.x, e.y, xrel, yrel};
}

ScrollEvent sdl::toScrollEvent(const SDL_MouseWheelEvent & e) noexcept
{
  return {e.x, e.y};
}

ClickEvent sdl::toClickEvent(const SDL_MouseButtonEvent & e) noexcept
{
  uint8_t state = 0;
  state |= (e.state == SDL_PRESSED ? ClickEvent::Pressed : ClickEvent::Released);
  switch(e.button)
  {
    case SDL_BUTTON_LEFT:
      state |= ClickEvent::Left;
      break;
    case SDL_BUTTON_RIGHT:
      state |= ClickEvent::Right;
      break;
    case SDL_BUTTON_MIDDLE:
      state |= ClickEvent::Middle;
      break;
  }
  return {e.x, e.y, state};
}

RGBAColor sdl::toColor(const SDL_Color & c) noexcept
{
  uint32_t value = c.r;
  value <<= 8;
  value |= c.g;
  value <<= 8;
  value |= c.b;
  value <<= 8;
  value |= c.a;
  return { value };
}

SDL_Color sdl::toColor(RGBAColor c) noexcept
{
  return {
    static_cast<uint8_t>((c.m_Color >> 24) & 0xFF),
    static_cast<uint8_t>((c.m_Color >> 16) & 0xFF),
    static_cast<uint8_t>((c.m_Color >> 8) & 0xFF),
    static_cast<uint8_t>((c.m_Color >> 0) & 0xFF)
  };
}

Rectangle sdl::toRect(const SDL_Rect & r) noexcept
{
  return {
    r.x,
    r.y,
    r.w + r.x - 1,
    r.h + r.y - 1
  };
}

SDL_Rect sdl::toRect(const Rectangle & r) noexcept
{
  return {
    r.m_x1, 
    r.m_y1,
    r.width(), 
    r.height()
  };
}
