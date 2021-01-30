#include "iLayout.hxx"
#include "../util/LOG.hxx"
#include "../util/Exception.hxx"
#include "../GameService.hxx"

iViewPtr iLayout::addElement(iViewPtr pw)
{
  m_Elements.emplace_back(pw);
  return m_Elements.back();
}

void iLayout::setPadding(PaddingConfiguration &&pc) noexcept { std::swap(m_PaddingConfiguration, pc); }

const PaddingConfiguration &iLayout::getPadding() const { return m_PaddingConfiguration; }

void iLayout::draw(iRenderer &renderer) const noexcept
{
  debug_scope {
    LOG(LOG_DEBUG) << "Drawing a iLayout";
  }
  for (auto &element : m_Elements)
    element->draw(renderer);
}

Range<typename iLayout::ElementList::iterator> iLayout::elements() noexcept { return {m_Elements.begin(), m_Elements.end()}; }

Range<typename iLayout::ElementList::const_iterator> iLayout::elements() const noexcept
{
  return {m_Elements.cbegin(), m_Elements.cend()};
}

iLayout::iLayout(Rectangle &&r) : m_Bounds(r) {}

iLayout::~iLayout() {}

const Rectangle &iLayout::getBounds() const noexcept { return m_Bounds; }

void iLayout::setBounds(Rectangle &&r) noexcept { std::swap(r, m_Bounds); }

void iLayout::setup(GameService &context) noexcept
{
  arrangeElements();
  for (auto &view : m_Elements)
  {
    view->setup(context);
  }
}
