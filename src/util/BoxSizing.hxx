#ifndef BOX_SIZING_HXX
#define BOX_SIZING_HXX

#include "../util/Meta.hxx"

/**
 * @class       ScreenMeasurement
 * @brief       Represents a measurement made on the screen
 * @details     This class is used to encode linear combinations
 *              of absolute and relative measurements. For example, you can
 *              specify that the width of an element should be 50_lw
 *              (50% of the enclosing layout)
 */
struct ScreenMeasurement
{
  /** @brief    Number of pixels */
  int px;
  /** @brief    Percentage of an element's width */
  int ew;
  /** @brief    Percentage of an element's height */
  int eh;
  /** @brief    Percentage of a layout's width */
  int lw;
  /** @brief    Percentage of a layout's height */
  int lh;
  /** @returns  The measurement result in pixels */
  int measure(const class ScreenMeasurementValuation &);
};

/* Literal operators */
constexpr ScreenMeasurement operator""_lh(unsigned long long);
constexpr ScreenMeasurement operator""_lw(unsigned long long);
constexpr ScreenMeasurement operator""_eh(unsigned long long);
constexpr ScreenMeasurement operator""_ew(unsigned long long);
constexpr ScreenMeasurement operator""_px(unsigned long long);

/* Arithmetic operators */
constexpr ScreenMeasurement operator+(const ScreenMeasurement &, const ScreenMeasurement &);
constexpr ScreenMeasurement operator-(const ScreenMeasurement &, const ScreenMeasurement &);

/**
 * @class       ScreenMeasurementValuation
 * @brief       Represents the values in pixels of the element width/height and layout width/height
 * @details     Used to measure a ScreenMeasurement
 */
struct ScreenMeasurementValuation
{
  int elementWidth;
  int elementHeight;
  int layoutWidth;
  int layoutHeight;
};

/**
 * @class       BoxSizing
 * @brief       Represents the desired dimensions of a Box
 */
struct BoxSizing
{
  ScreenMeasurement width;
  ScreenMeasurement height;
};

/**
 * @class       PositionHint
 * @brief       Represents a hint to the position of a (or multiple) Box
 * @details     A PositionHint is a way for Views to communicate their
 *              desired position. They have no certainty that this is the
 *              position they will end up being assigned.
 */
struct PositionHint
{
  ScreenMeasurement left;
  ScreenMeasurement right;
  ScreenMeasurement top;
  ScreenMeasurement bottom;
};

/**
 * @class       MarginConfiguration
 * @brief       Represents a PositionHint as margin around a Box
 */
class MarginConfiguration : public PositionHint
{
};

/**
 * @class       PaddingConfiguration
 * @brief       Represents a PositionHint as padding within a Box
 */
class PaddingConfiguration : public PositionHint
{
};

#include "BoxSizing.inl.hxx"

/**
 * @class       AbsolutePosition
 * @brief       Represents the absolute position of a Box
 * @details     Absolute here means ScreenMeasurements evaluate to
 *              the absolute offsets from the origin of the Window
 */
struct AbsolutePosition
{
  ScreenMeasurement left;
  ScreenMeasurement top;
  static constexpr AbsolutePosition Origin() { return {0_px, 0_px}; }
  static constexpr AbsolutePosition Center() { return {50_lw - 50_ew, 50_lh - 50_eh}; }
};

#endif // BOX_SIZING_HXX
