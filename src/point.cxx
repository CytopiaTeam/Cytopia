#include "point.hxx"

Point::Point(int xVal, int yVal) :
x(xVal), y(yVal)
{

}

Point::~Point()
{

}

void Point::setCoords(int xVal, int yVal)
{
  x = xVal;
  y = yVal;

}