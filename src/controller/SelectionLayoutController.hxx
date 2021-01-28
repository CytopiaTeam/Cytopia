#ifndef SELECTION_LAYOUT_CONTROLLER_HXX
#define SELECTION_LAYOUT_CONTROLLER_HXX

#include <functional>
#include "iMouseHandler.hxx"
#include "../model/MouseState.hxx"
#include "../view/iView.hxx"
#include "../model/SelectionLayoutModel.hxx"
#include "../util/Rectangle.hxx"
#include "../layout/SelectionLayout.hxx"

class SelectionLayoutController : public iMouseHandler
{
public:
  using Callback = std::function<void()>;
  SelectionLayoutController(SelectionLayoutModel &model, MouseState &mouse_state, const SelectionLayout &view);
  ~SelectionLayoutController() override;
  void onScroll(class ScrollEvent &&) override;
  void onMouseLeftButtonUp(class ClickEvent &&) override;
  const iShape &getShape() final;
  void setup(GameService &) noexcept override;

private:
  MouseState &m_MouseState;
  Callback m_HandleScroll;
  const iView &m_View;
  SelectionLayoutModel &m_Model;
  int m_Size;
  float m_Step;
  int m_ContentHeight;
  int m_ElementHeight;
  int m_ElementPadding;
  size_t m_ElementCount;
};

#endif // SELECTION_LAYOUT_CONTROLLER_HXX