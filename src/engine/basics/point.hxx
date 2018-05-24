#pragma once

#ifndef POINT_HXX_
#define POINT_HXX_

//#include "resources.hxx"

class Point
{
public:
  Point() : _x(0), _y(0), _z(0), _height(0) { };
  Point(int x, int y) : _x(x), _y(y), _z(0), _height(0) { };
  Point(int x, int y, int z) : _x(x), _y(y), _z(z), _height(0) { };
  Point(int x, int y, int z, int height) : _x(x), _y(y), _z(z), _height(height) { };
  ~Point() = default;

  // set point to Coordinates
  void setCoords(int x, int y, int height = 0);

  /** Sets the X Coordinate*/
  void setX(int x) { _x = x; };

  /** Sets the Y Coordinate*/
  void setY(int y) { _y = y; };;

  /** Sets the Z Coordinate (Drawing Order)*/
  void setZ(int z) { _z = z; };;

  /** Sets the Heightlevel (Tileheight)*/
  void setHeight(int height) { _height = height; };

  /** Gets the X Coordinate*/
  int getX() { return _x; };
  
  /** Gets the Y Coordinate*/
  int getY() { return _y; };

  /** Gets the Z Coordinate (Drawing Order)*/
  int getZ() { return _z; };
  
  /** Gets the Heightlevel (Tileheight)*/
  int getHeight() { return _height; };

private:
  int _x, _y, _z, _height;
};

#endif