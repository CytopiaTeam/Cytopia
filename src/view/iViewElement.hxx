#ifndef I_VIEW_ELEMENT_HXX
#define I_VIEW_ELEMENT_HXX

#include <memory>
#include "iView.hxx"
#include "BoxSizing.hxx"
#include "Rectangle.hxx"

class iViewElement : public iView
{
  Rectangle m_Bounds = Rectangle{0, 0, 0, 0};
public:
  virtual ~iViewElement() = 0;
  virtual const Rectangle & getBounds() const noexcept final;
  friend class AbsoluteLayout;
private:
  void setBounds(Rectangle &&) noexcept override;
};

using iViewElementPtr = std::shared_ptr<iViewElement>;

#endif // I_VIEW_ELEMENT_HXX
