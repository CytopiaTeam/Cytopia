#include "iLayout.hxx"
#include "../util/LOG.hxx"
#include "../util/Exception.hxx"
#include "../controller/iMouseHandler.hxx"
#include "../GameService.hxx"
#include "../services/MouseController.hxx"


iLayout::iLayout(Rectangle && r) : m_Bounds(r)
{
}

iLayout::~iLayout() = default;

void iLayout::draw(iRenderer & renderer) const noexcept
{
  LOG(LOG_DEBUG) << "Drawing a iLayout";
  for(auto & element : m_Elements)
    element->draw(renderer);
}

void iLayout::setup() noexcept
{
  computeBoundaries();
  for(auto & view : m_Elements)
  {
    view->setup();
  }
}

void iLayout::bindHandlers(GameService & context) noexcept
{
  for(auto & handlerptr : m_Controllers)
  {
    // TODO: Refactor this
    iMouseHandler* handler = dynamic_cast<iMouseHandler*>(handlerptr.get());
    if(handler)
    {
      context.GetService<MouseController>().addHandler(handler);
    }
  }
}

iViewPtr iLayout::addElement(iViewPtr pw)
{
  m_Elements.emplace_back(pw);
  return m_Elements.back();
}
  
Range<typename iLayout::ElementList::iterator> 
iLayout::elements() noexcept
{
  return {m_Elements.begin(), m_Elements.end()};
}

Range<typename iLayout::ElementList::const_iterator> 
iLayout::elements() const noexcept
{
  return {m_Elements.cbegin(), m_Elements.cend()};
}


const Rectangle & iLayout::getBounds() const noexcept
{
  return m_Bounds;
}

void iLayout::setBounds(Rectangle && r) noexcept 
{
  std::swap(r, m_Bounds);
}

