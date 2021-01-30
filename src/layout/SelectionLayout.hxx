#ifndef SELECTION_LAYOUT_HXX
#define SELECTION_LAYOUT_HXX

#include <memory>
#include <vector>
#include "iLayout.hxx"
#include "../model/SelectionLayoutModel.hxx"

class SelectionLayout : public iLayout, public SelectionLayoutListener
{
protected:
  void arrangeElements() noexcept final;

private:
  ScreenMeasurement m_ElementHeight;
  int m_ScrollPos;
  int m_ContentHeight;
  float m_ScrollPosPct;
  int m_SeletedElementIdx;
  int m_HoverElementIdx;
  static constexpr int PADDING_SCROLL_BAR = 8;
  static constexpr int PADDING_ELEMENT = 8;
  static constexpr int SCROLL_WIDTH = 10;
  static constexpr int THUMB_HEIGHT = 24;

  void scrollItems(int scrollPos);
  void calculateScrollPos();

public:
  SelectionLayout(Rectangle && = Rectangle{0, 0, 0, 0}, ScreenMeasurement &&elementHeight = 25_px);
  void draw(iRenderer &) const noexcept override;
  ~SelectionLayout() override;
  void setup(class GameService &context) noexcept final;
  void update(Notification) noexcept final;
  int getContentHeight() const { return m_ContentHeight; }
  int getElementPadding() const { return PADDING_ELEMENT; }
  int getElementHeight() const { return m_ElementHeight.px; }
  int getThumbHeight() const { return THUMB_HEIGHT; }
  int getScrollBarWidth() const { return SCROLL_WIDTH; }
  int getScrollBarPading() const { return PADDING_SCROLL_BAR; }
  using iLayout::addElement;
};

using SelectionLayoutPtr = std::shared_ptr<SelectionLayout>;

#endif // SELECTION_LAYOUT_HXX
