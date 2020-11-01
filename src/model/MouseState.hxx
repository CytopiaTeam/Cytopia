#ifndef MOUSE_STATE_HXX_
#define MOUSE_STATE_HXX_

#include <betterEnums.hxx>
#include "DataModel.hxx"

BETTER_ENUM(CursorType, uint8_t, Arrow, Pointer, Grab, Grabbing, Caret, ResizeN, ResizeE, ResizeNE, ResizeSW, None);

struct MouseStateModel
{
  int xPosition;
  int yPosition;
  int xVelocity;
  int yVelocity;
  CursorType cursorType = CursorType::Arrow;
};

using MouseStateNotification = TypeList<MouseStateModel>;

class MouseState : public DataModel<MouseStateNotification>
{
  MouseStateModel m_Model;
public:
  using DataModel::DataModel;
  void setPosition(int, int, int, int);
  void setCursor(CursorType);
  ~MouseState();
};

using MouseObserver = Observer<MouseState::Notification>; 

#endif // MOUSE_STATE_HXX_
