#ifndef I_MOUSE_HANDLER_HXX_
#define I_MOUSE_HANDLER_HXX_

#include <memory>

#include "../util/Point2D.hxx"
#include "../controller/iController.hxx"

class iMouseHandler : public iController
{
public:
  virtual ~iMouseHandler() = 0;
  virtual void onMouseLeftButtonDown(class ClickEvent &&);
  virtual void onMouseMiddleButtonDown(class ClickEvent &&);
  virtual void onMouseRightButtonDown(class ClickEvent &&);
  virtual void onMouseLeftButtonUp(class ClickEvent &&);
  virtual void onMouseMiddleButtonUp(class ClickEvent &&);
  virtual void onMouseRightButtonUp(class ClickEvent &&);
  virtual void onMouseMove(class MousePositionEvent &&);
  virtual void onMouseHover();
  virtual void onMouseLeave();
  virtual void onScroll(class ScrollEvent &&);
  virtual const class iShape &getShape() = 0;
};

using iMouseHandlerPtr = std::unique_ptr<iMouseHandler>;

#endif // I_MOUSE_HANDLER_HXX_
