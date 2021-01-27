#ifndef RECTANGLE_HXX
#define RECTANGLE_HXX

#include <tuple>
#include <iostream>
#include "../util/iShape.hxx"
#include "Point2D.hxx"

class SDL_Rect;

/**
 * @class       Rectangle
 * @brief       Represents an inclusive rectangle
 * @details     All negative points are reserved
 *              for special values
 * @example     tests/util/Rectangle.cxx
 */
class Rectangle final : public iShape
{

private:
  int m_x1;
  int m_y1;
  int m_x2;
  int m_y2;

public:
  constexpr Rectangle(int x1, int y1, int x2, int y2);
  static Rectangle EMPTY() noexcept;

  /***********************
   *      Accessors      *
   ***********************/

  bool contains(const Point2D &) const noexcept final;

  Rectangle getBounds() const noexcept final;

  [[deprecated("Use sdl::toRect instead")]] SDL_Rect to_SDL() const noexcept;

  /**
   * @returns   The inclusive width of the Rectangle
   * @details   A Rectangle of { x1: 0, x2: 9 } has a width of 10
   */
  int width() const noexcept;

  /**
   * @returns   The inclusive height of the Rectangle
   * @details   A Rectangle of { y1: 1, y2: 10 } has a height of 10
   */
  int height() const noexcept;

  /**
   * @brief     Returns the smallest (x,y) of the Rectangle
   */
  std::pair<int, int> p1() const noexcept;

  /**
   * @brief     Returns the largest (x,y) of the Rectangle
   */
  std::pair<int, int> p2() const noexcept;

  /**
   * @returns   The smallest X coordinate
   */
  int x1() const noexcept;

  /**
   * @returns   The largest X coordinate
   */
  int x2() const noexcept;

  /**
   * @returns   The smallest Y coordinate
   */
  int y1() const noexcept;

  /**
   * @returns   The largest Y coordinate
   */
  int y2() const noexcept;

  bool isEmpty() const noexcept;

  /***********************
   *      Modifiers      *
   ***********************/

  /**
   * @brief     Increments all x coordinates
   */
  void translateX(int x) noexcept;

  /**
   * @brief     Increments all y coordinates
   */
  void translateY(int y) noexcept;

  /**
   * @brief     Intersects with another rectangle
   * @post      Becomes EMPTY if the intersection is empty
   *            or the other Rectangle is empty
   */
  void intersect(const Rectangle &) noexcept;

  /**
   * @brief     Scales by Factor
   *            around the center of the Rectangle
   * @tparam    Factor the Factor type
   * @post      Becomes EMPTY if the Factor is very
   *            small
   */
  template <typename Factor> void scaleCenter(Factor) noexcept;

  /***********************
   *      Helpers        *
   ***********************/

  friend std::ostream &operator<<(std::ostream &, const Rectangle &);

  friend bool operator==(const Rectangle &, const Rectangle &);
};

#include "Rectangle.inl.hxx"

#endif // RECTANGLE_HXX
