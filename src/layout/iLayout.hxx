#ifndef I_LAYOUT_HXX
#define I_LAYOUT_HXX

#include <vector>

#include "../util/Range.hxx"
#include "../view/iView.hxx"
#include "../view/iViewElement.hxx"
#include "../util/BoxSizing.hxx"

class iLayout : public virtual iView
{
  using ElementList = std::vector<iViewPtr>;
  ElementList m_Elements;
  PaddingConfiguration m_PaddingConfiguration;
  Rectangle m_Bounds;

public:
  /**
   * @returns the PaddingConfiguration of the iLayout
   * @details the PaddingConfiguration only contains pixel values
   */
  const PaddingConfiguration &getPadding() const;

  virtual void draw(iRenderer &) const noexcept override;

  iLayout(Rectangle && = Rectangle{0, 0, 0, 0});
  virtual ~iLayout() = 0;

  void setup(GameService &) noexcept override;

  /**
   * @brief Get number of elements in the layout.
   */
  size_t getElemenentsCount() const noexcept { return m_Elements.size(); }

protected:
  /**
   * @brief Adds a iView to the iLayout
   * @param iViewPtr the iView to add
   */
  iViewPtr addElement(iViewPtr);

  /**
   * @brief Sets the PaddingConfiguration of a iLayout
   */
  void setPadding(PaddingConfiguration &&) noexcept;

  Range<typename ElementList::iterator> elements() noexcept;
  Range<typename ElementList::const_iterator> elements() const noexcept;

  /**
   * @brief compute the boundaries of all iViews
   */
  virtual void arrangeElements() noexcept = 0;

  const Rectangle &getBounds() const noexcept final;

  /**
   * @brief Provide access to the layout elements by index operator.
   */
  iViewPtr operator[](int idx) { return m_Elements[idx]; }

private:
  void setBounds(Rectangle &&) noexcept final;
};

#endif // I_LAYOUT_HXX
