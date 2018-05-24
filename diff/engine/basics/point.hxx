#pragma once

#ifndef POINT_HXX_
#define POINT_HXX_

//#include "resources.hxx"

class Point
{
public:
  Point();
  Point(int x, int y);
  Point(int x, int y, int z);
  Point(int x, int y, int z, int height);
  ~Point();

  // set point to Coordinates
  void setCoords(int x, int y, int height = 0);
  void setX(int x);
  void setY(int y);
  /** Sets the Z Coordinate (Drawing Order)*/
  void setZ(int z);
  void setHeight(int height);

  int getX();
  int getY();
  /** Gets the Z Coordinate (Drawing Order)*/
  int getZ();
  int getHeight();

private:
  int _x, _y, _z, _height;
};

#endif