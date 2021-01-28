#include "SelectionLayout.hxx"
#include "../util/LOG.hxx"
#include "../GameService.hxx"
#include "../renderer/SDLRenderer.hxx"
#include "../util/PixelBuffer.hxx"
#include "../util/Filesystem.hxx"
#include "../view/SelectionItem.hxx"

SelectionLayout::SelectionLayout(Rectangle &&rect, ScreenMeasurement &&elementHeight)
    : iLayout(std::move(rect)),
      m_ElementHeight(elementHeight), m_ScrollPos{0}, m_ContentHeight{0}, m_ScrollPosPct{0.0f}, m_SeletedElementIdx{0}
{
}

void SelectionLayout::arrangeElements() noexcept
{
  auto &bounds = getBounds();
  int y1 = 0;
  const int width = bounds.width() - 2 * PADDING_SCROLL_BAR - SCROLL_WIDTH;
  const int height = m_ElementHeight.px;

  for (auto &element : elements())
  {
    Rectangle rect{PADDING_SCROLL_BAR, y1, width, y1 + height};
    element->setBounds(std::move(rect));
    y1 += height + PADDING_ELEMENT;
  }

  m_ContentHeight = std::max(0, y1 - bounds.height() + 2 * PADDING_SCROLL_BAR);
  calculateScrollPos();
}

SelectionLayout::~SelectionLayout() = default;

void SelectionLayout::draw(iRenderer &renderer) const noexcept
{
  std::string fimage = fs::getBasePath() + "resources/images/ui/selectionLayout/frame.png";

  /* First, we load the sprite */
  PixelBuffer pb = fs::readPNGFile(fimage);
  Rectangle o_bounds = pb.bounds();
  Rectangle target = getBounds();
  if (o_bounds.width() * o_bounds.height() == 0)
  {
    LOG(LOG_WARNING) << "Could not properly load texture " << fimage;
    return;
  }

  pb.scale(2.0);

  /* Next, we expand the center of the sprite */
  pb.expandCenter(Rectangle{target});
  renderer.drawPicture(target, pb.data());

  // Scroll bar
  Rectangle scroller = Rectangle{target.x2() - PADDING_SCROLL_BAR - SCROLL_WIDTH, target.y1() + PADDING_SCROLL_BAR,
                                 target.x2() - PADDING_SCROLL_BAR, target.y2() - PADDING_SCROLL_BAR};
  renderer.drawShape(scroller, RGBAColor(50, 255, 50, 255));

  Rectangle thumb = Rectangle{target.x2() - PADDING_SCROLL_BAR - SCROLL_WIDTH, target.y1() + PADDING_SCROLL_BAR + m_ScrollPos,
                              target.x2() - PADDING_SCROLL_BAR, target.y1() + PADDING_SCROLL_BAR + m_ScrollPos + 10};
  renderer.drawShape(thumb, RGBAColor(255, 50, 50, 255));

  auto &bonds = getBounds();
  Rectangle contentWindow = Rectangle{bonds.x1() + PADDING_SCROLL_BAR, bonds.y1() + PADDING_SCROLL_BAR,
                                      bonds.x2() - 2 * PADDING_SCROLL_BAR - SCROLL_WIDTH, bonds.y2() - PADDING_SCROLL_BAR};
  renderer.setViewport(contentWindow);

  for (auto &element : elements())
  {
    element->draw(renderer);
  }

  renderer.resetViewport();
}

void SelectionLayout::setup(class GameService &context) noexcept { iLayout::setup(context); }

void SelectionLayout::update(Notification notif) noexcept
{
  // Selection index changed
  if (std::holds_alternative<int>(notif))
  {
    static_cast<SelectionItem *>((*this)[m_SeletedElementIdx].get())->select(false);
    m_SeletedElementIdx = std::get<int>(notif);
    static_cast<SelectionItem *>((*this)[m_SeletedElementIdx].get())->select(true);
  }
  // Scroll position changed
  else
  {
    m_ScrollPosPct = std::get<float>(notif);
    calculateScrollPos();
  }
}

void SelectionLayout::scrollItems(int scrollPos)
{
  for (auto &element : elements())
  {
    static_cast<SelectionItem *>(element.get())->translateY(scrollPos);
  }
}

void SelectionLayout::calculateScrollPos()
{
  m_ScrollPos = m_ScrollPosPct * (getBounds().height() - 2 * PADDING_SCROLL_BAR - SCROLL_WIDTH);
  const int scroll = m_ScrollPosPct * m_ContentHeight;
  scrollItems(-scroll);
}