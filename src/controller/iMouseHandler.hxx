#ifndef I_MOUSE_HANDLER_HXX_
#define I_MOUSE_HANDLER_HXX_

#include <memory>
#include <betterEnums.hxx>

#include "../util/Point2D.hxx"
#include "../controller/iController.hxx"

class iMouseHandler : public iController
{
public:
  virtual ~iMouseHandler() = 0;
  virtual void onMouseClick(class ClickEvent &&);
  virtual void onMouseHover();
  virtual void onMouseLeave();
  virtual void onScroll(class ScrollEvent &&);
  virtual const class iShape & getShape() = 0;
};

using iMouseHandlerPtr = std::unique_ptr<iMouseHandler>;

#endif // I_MOUSE_HANDLER_HXX_
