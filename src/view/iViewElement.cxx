#include "iViewElement.hxx"
#include "../util/Exception.hxx"
#include "../util/LOG.hxx"

iViewElement::~iViewElement() { }
  
const Rectangle & iViewElement::getBounds() const noexcept
{
  return m_Bounds;
}

void iViewElement::setBounds(Rectangle && r) noexcept 
{
  std::swap(r, m_Bounds);
}
