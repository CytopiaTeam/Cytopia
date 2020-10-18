#ifndef ABSOLUTE_LAYOUT_HXX
#define ABSOLUTE_LAYOUT_HXX

#include "iLayout.hxx"

class AbsoluteLayout : public iLayout
{
protected:
  void computeBoundaries() noexcept final;
  iViewPtr addElement(iViewPtr, BoxSizing&&, AbsolutePosition &&);
private:
  using iLayout::addElement;
  std::vector<BoxSizing> m_Sizes;
  std::vector<AbsolutePosition> m_Positions;
public:
  using iLayout::iLayout; 
};

#endif // ABSOLUTE_LAYOUT_HXX
