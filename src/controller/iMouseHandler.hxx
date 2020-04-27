#ifndef I_MOUSE_HANDLER_HXX_
#define I_MOUSE_HANDLER_HXX_

#include <memory>
#include "iController.hxx"

class ClickEvent;
class ScrollEvent;
class iShape;

class iMouseHandler : public iController
{
public:
  virtual ~iMouseHandler() = 0;
  virtual void onMouseClick(ClickEvent &&);
  virtual void onMouseHover();
  virtual void onMouseLeave();
  virtual void onScroll(ScrollEvent &&);
  virtual const iShape & getShape() = 0;
};

using iMouseHandlerPtr = std::unique_ptr<iMouseHandler>;

#endif // I_MOUSE_HANDLER_HXX_
