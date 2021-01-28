#include "SelectionLayoutController.hxx"
#include "../services/MouseController.hxx"
#include "../util/Rectangle.hxx"

SelectionLayoutController::SelectionLayoutController(SelectionLayoutModel &model, MouseState &mouse_state,
                                                     const SelectionLayout &view)
    : m_Model(model), m_MouseState(mouse_state),
      m_View(view), m_Size{0}, m_Step{0}, m_ContentHeight{0}, m_ElementHeight{view.getElementHeight()},
      m_ElementPadding{view.getElementPadding()}, m_ElementCount{view.getElemenentsCount()}
{
}

SelectionLayoutController::~SelectionLayoutController() {}

void SelectionLayoutController::onScroll(class ScrollEvent &&event) { m_Model.moveScrollPos(m_Step * event.yDisplacement); }

const iShape &SelectionLayoutController::getShape() { return m_View.getBounds(); }

void SelectionLayoutController::setup(GameService &context) noexcept
{
  auto &mouseCtrl = context.GetService<MouseController>();
  mouseCtrl.addHandler(this);

  auto &layoutView = dynamic_cast<const SelectionLayout &>(m_View);
  m_ContentHeight = layoutView.getContentHeight();
  m_Step = 5.0f / m_ContentHeight;
}

void SelectionLayoutController::onMouseLeftButtonUp(class ClickEvent &&event)
{
  Rectangle layoutBounds = m_View.getBounds();

  if (layoutBounds.contains(Point2D{event.xPosition, event.yPosition}))
  {
    int y1 = event.yPosition - layoutBounds.p1().second;
    int wholeYPos = y1 - m_ElementPadding + (m_ContentHeight * m_Model.getScrollPos());

    if ((wholeYPos % (m_ElementHeight + m_ElementPadding)) <= m_ElementHeight)
    {
      const int index = wholeYPos / (m_ElementHeight + m_ElementPadding);

      if ((index >= 0) && (index < m_ElementCount))
      {
        m_Model.setSelectIndex(wholeYPos / (m_ElementHeight + m_ElementPadding));
      }
    }
  }
}
