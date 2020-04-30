#ifndef I_SHAPE_HXX
#define I_SHAPE_HXX

class Point2D;
class Rectangle;

class iShape
{
public:
  /**
   * @returns   True when Point2D is contained within the Shape
   */
  virtual bool contains(const Point2D &) const noexcept = 0;
  
  /**
   * @returns   The bounding box around the shape
   * @details   A bounding box is a box that contains
   *            all points within the Shape with minimum area
   */
  virtual Rectangle getBounds() const noexcept = 0;
  
  virtual ~iShape() = 0;
};

#endif // I_SHAPE_HXX
