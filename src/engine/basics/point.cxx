#include "point.hxx"

Point::Point()
{
  _x = 0;
  _y = 0;
}

Point::Point(int x, int y, int height) :
  _x(x), _y(y), _height(height)
{

}

Point::~Point()
{

}

void Point::setCoords(int x, int y, int height)
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

void Point::setHeight(int height)
{
  _height = height;
}

int Point::getX()
{
  return _x;
}

int Point::getY()
{
  return _y;
}

int Point::getHeight()
{
  return _height;
}