#include "point.hxx"

Point::Point()
{
  _x = 0;
  _y = 0;
}

Point::Point(int x, int y) :
  _x(x), _y(y)
{

}

Point::~Point()
{

}


void Point::setCoords(int x, int y)
{
  _x = x;
  _y = y;
}

void Point::setX(int x)
{
  _x = x;
}

void Point::setY(int y)
{
  _y = y;
}

int Point::getX()
{
  return _x;
}

int Point::getY()
{
  return _y;
}