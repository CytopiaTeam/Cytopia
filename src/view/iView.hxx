#ifndef I_VIEW_HXX
#define I_VIEW_HXX

#include <memory>

class iRenderer;
class Rectangle;
class GameService;

/**
 * @class     iView
 * @brief     Represents a View object for drawing and positioning
 * @see       https://en.m.wikipedia.org/wiki/Model%E2%80%93view%E2%80%93controller
 */
class iView
{
public:
  /**
   * @brief   Called when the View is required to be drawn
   */
  virtual void draw(iRenderer &) const noexcept = 0;

  /**
   * @brief   Returns the smallest
   *          bounding box containing the iView
   * @details ::draw is never allowed to draw outside of
   *          this bounding box.
   */
  virtual const Rectangle & getBounds() const noexcept = 0;

  /**
   * @brief   Sets the smallest
   *          bounding box containing the iView
   * @details Only Layouts are allowed to call this function
   */
  virtual void setBounds(Rectangle &&) noexcept = 0;

  /**
   * @brief   Called when a Layout changed the bounding box
   *          of the iView
   */
  virtual void setup(GameService &) noexcept = 0;

  virtual ~iView() = 0;
};

using iViewPtr = std::shared_ptr<iView>;

#endif // I_VIEW_HXX
