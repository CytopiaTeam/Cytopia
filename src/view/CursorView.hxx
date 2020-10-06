#ifndef CURSOR_VIEW_HXX_
#define CURSOR_VIEW_HXX_
#include <memory>
#include <SDL.h>
#include "../model/MouseState.hxx"

class CursorView : public MouseObserver
{
  using CursorScaleSize = Constant<5<<16>;
  struct CursorDeleter
  {
    void operator()(SDL_Cursor*) const noexcept;
  };
  using CursorPtr = std::unique_ptr<SDL_Cursor, CursorDeleter>;
  std::array<CursorPtr, CursorType::_size()> m_Cursors;
public:
  CursorView();
  void update(Notification) noexcept final;
};

using CursorViewPtr = std::shared_ptr<CursorView>;

#endif // CURSOR_VIEW_HXX_
