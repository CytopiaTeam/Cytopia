#ifndef I_LAYOUT_HXX
#define I_LAYOUT_HXX

#include <vector>

#include "../util/Range.hxx"
#include "../util/BoxSizing.hxx"
#include "../view/iView.hxx"
#include "../view/iViewElement.hxx"

class iLayout : public virtual iView
{

  using ElementList = std::vector<iViewPtr>;
  ElementList m_Elements;
  Rectangle m_Bounds;

public:
  virtual void draw(iRenderer &) const noexcept override;

  iLayout(Rectangle && = Rectangle{0, 0, 0, 0});
  virtual ~iLayout() = 0;

  void setup(GameService &) noexcept override;

protected:
  /**
   * @brief Adds a iView to the iLayout
   * @param iViewPtr the iView to add
   */
  iViewPtr addElement(iViewPtr);

  Range<typename ElementList::iterator> elements() noexcept;
  Range<typename ElementList::const_iterator> elements() const noexcept;

  /**
   * @brief compute the boundaries of all iViews
   */
  virtual void computeBoundaries() noexcept = 0;

  const Rectangle &getBounds() const noexcept final;

private:
  void setBounds(Rectangle &&) noexcept final;
};

#endif // I_LAYOUT_HXX
