#include "SelectionLayoutController.hxx"
#include "../services/MouseController.hxx"
#include "../util/Rectangle.hxx"

SelectionLayoutController::SelectionLayoutController(SelectionLayoutModel &model, MouseState &mouse_state,
                                                     const SelectionLayout &view, OnItemSelectedCbk onItemSelectCbk)
    : m_Model(model), m_HandleItemSelection{onItemSelectCbk}, m_MouseState(mouse_state), m_View(view), m_Size{0}, m_Step{0},
      m_ContentHeight{0}, m_ElementHeight{view.getElementHeight()}, m_ElementPadding{view.getElementPadding()},
      m_IsThumbCaptured{false}, m_ThumbCapturedYMin{0}, m_ThumbCapturedYMax{0}, m_ThumbHeight{view.getThumbHeight()},
      m_ScrollPading{view.getScrollBarPading()}, m_ThumbWidth{view.getScrollBarWidth()}, m_ElementCount{view.getElemenentsCount()}
{
}

SelectionLayoutController::~SelectionLayoutController() {}

void SelectionLayoutController::onScroll(class ScrollEvent &&event)
{
  m_Model.moveScrollPos(m_Step * event.yDisplacement);
  m_Model.setHoverIndex(SelectionLayoutModel::NOT_SELECTED_INDEX);
}

const iShape &SelectionLayoutController::getShape() { return m_View.getBounds(); }

void SelectionLayoutController::setup(GameService &context) noexcept
{
  auto &mouseCtrl = context.GetService<MouseController>();
  mouseCtrl.addHandler(this);

  auto &layoutView = dynamic_cast<const SelectionLayout &>(m_View);
  m_ContentHeight = layoutView.getContentHeight();
  m_Step = SCROLL_STEP_PX / m_ContentHeight;
}

void SelectionLayoutController::onMouseMove(class MousePositionEvent &&event)
{
  if (m_IsThumbCaptured)
  {
    int ThumbPos = std::clamp(event.yPosition, m_ThumbCapturedYMin, m_ThumbCapturedYMax);
    m_Model.setScrollPos((float)(ThumbPos - m_ThumbCapturedYMin) / (m_ThumbCapturedYMax - m_ThumbCapturedYMin));
  }
  else
  {
    int wholeYPos = event.yPosition - m_ElementPadding + (m_ContentHeight * m_Model.getScrollPos());

    if ((wholeYPos % (m_ElementHeight + m_ElementPadding)) <= m_ElementHeight)
    {
      const int index = wholeYPos / (m_ElementHeight + m_ElementPadding);

      if ((index >= 0) && (index < m_ElementCount))
      {
        m_Model.setHoverIndex(index);
      }
      else
      {
        m_Model.setHoverIndex(SelectionLayoutModel::NOT_SELECTED_INDEX);
      }
    }
    else
    {
      m_Model.setHoverIndex(SelectionLayoutModel::NOT_SELECTED_INDEX);
    }
  }
}

void SelectionLayoutController::onMouseLeftButtonDown(class ClickEvent &&event)
{
  auto viewBounds = m_View.getBounds();
  int y1 = (viewBounds.y2() - viewBounds.y1() - 2 * m_ScrollPading - m_ThumbHeight) * m_Model.getScrollPos() +
           (viewBounds.y1() + m_ScrollPading);
  auto thumb =
      Rectangle{viewBounds.x2() - m_ScrollPading - m_ThumbWidth, y1, viewBounds.x2() - m_ScrollPading, y1 + m_ThumbHeight};

  if (thumb.contains({event.xPosition, event.yPosition}))
  {
    m_IsThumbCaptured = true;
    int thumbOffset = event.yPosition - thumb.y1();
    m_ThumbCapturedYMin = m_ScrollPading + thumbOffset;
    m_ThumbCapturedYMax = viewBounds.height() - m_ScrollPading - m_ThumbHeight + thumbOffset;
    m_MouseState.setCursor(CursorType::Grab);
    m_Model.setHoverIndex(SelectionLayoutModel::NOT_SELECTED_INDEX);
  }
}

void SelectionLayoutController::onMouseLeftButtonUp(class ClickEvent &&event)
{
  if (m_IsThumbCaptured)
  {
    m_IsThumbCaptured = false;
    m_MouseState.setCursor(CursorType::Arrow);
  }
  else
  {
    Rectangle layoutBounds = m_View.getBounds();

    if (layoutBounds.contains(Point2D{event.xPosition, event.yPosition}))
    {
      int y1 = event.yPosition - layoutBounds.p1().second;
      int wholeYPos = y1 - m_ElementPadding + (m_ContentHeight * m_Model.getScrollPos());

      if ((wholeYPos % (m_ElementHeight + m_ElementPadding)) <= m_ElementHeight)
      {
        const int index = wholeYPos / (m_ElementHeight + m_ElementPadding);
        setSelectionIndex(index);
      }
    }
  }
}

void SelectionLayoutController::setSelectionIndex(int index)
{
  if ((index >= 0) && (index < m_ElementCount))
  {
    if (m_Model.setSelectIndex(index))
    {
      m_HandleItemSelection(index);
    }
  }
}
