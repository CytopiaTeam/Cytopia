#pragma once

#ifndef POINT_HXX_
#define POINT_HXX_

class Point
{
public:
  Point();
  Point(int x, int y);
  ~Point();

  // set point to Coordinates
  void setCoords(int x, int y);
  void setX(int x);
  void setY(int y);

  int getX();
  int getY();

private:
  int _x, _y;
};

#endif