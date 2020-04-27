#ifndef I_LAYOUT_HXX
#define I_LAYOUT_HXX

#include <vector>
#include "../util/Range.hxx"
#include "../view/iView.hxx"
#include "../controller/iController.hxx"
#include "../util/Rectangle.hxx"

class iLayout : public virtual iView
{
  using ControllerList = std::vector<iControllerPtr>;
  using ElementList = std::vector<iViewPtr>;
  
  ControllerList m_Controllers;
  ElementList m_Elements;
  Rectangle m_Bounds;
  
public:
  
  /**
   * @param Rectangle The bounds of the Layout
   */
  iLayout(Rectangle && = Rectangle{0, 0, 0, 0});
  virtual ~iLayout() = 0;
  
  /**
   * @brief Draws each element
   */
  virtual void draw(iRenderer &) const noexcept override;
  
  /**
   * @brief Computes the boundaries and setups each View
   */
  void setup() noexcept override;
  
  /**
   * @brief   Binds all handlers registered from ::createController
   * @todo    Remove this function and move everything to a Factory method
   *          in iActivity which will bind everything with some metaprogramming
   */
  void bindHandlers(class GameService & context) noexcept override;

protected:

  /**
   * @brief Adds an iView to the iLayout
   * @param iViewPtr the iView to add
   * @returns The created iViewPtr
   */
  iViewPtr addElement(iViewPtr);

  /**
   * @brief Adds a new controller to the iLayout
   * @tparam ControllerType the type of the controller
   * @tparam Args the arguments to construct the controller
   */
  template <typename ControllerType, typename... Args>
  ControllerType & createController(Args &&... args);

  /**
   * @brief Range access to all iViewPtr elements added
   */
  Range<typename ElementList::iterator> elements() noexcept;
  
  /**
   * @brief Const Range access to all iViewPtr elements added
   */
  Range<typename ElementList::const_iterator> elements() const noexcept;

  /**
   * @brief     Computes the boundaries of all iViews
   * @details   Override this function with your layout algorithm
   */
  virtual void computeBoundaries() noexcept = 0;

  /**
   * @returns The layout's bounds
   */
  const Rectangle & getBounds() const noexcept final;

  /**
   * @brief Sets the bounds
   */
  void setBounds(Rectangle &&) noexcept final;
};

#include "iLayout.inl.hxx"

#endif // I_LAYOUT_HXX
