#pragma once

#ifndef POINT_HXX_
#define POINT_HXX_

class Point
{
public:
  Point();
  Point(int x, int y, int height = 0);
  ~Point();

  // set point to Coordinates
  void setCoords(int x, int y, int height = 0);
  void setX(int x);
  void setY(int y);
  void setHeight(int height);

  int getX();
  int getY();
  int getHeight();

private:
  int _x, _y, _height;
};

#endif