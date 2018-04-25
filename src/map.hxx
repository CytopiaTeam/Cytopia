#ifndef MAP_HXX_
#define MAP_HXX_


#include <string>
#include <cmath>
#include <vector>

//#include "yaml-cpp/yaml.h"
#include "SDL.h"

#include "sprite.hxx"
#include "point.hxx"

class Map
{
public:

  int _width = 16;
  int _height = 16;

  Map(SDL_Renderer *renderer, SDL_Window *window);
  virtual ~Map();

  void parseMapFile();

  void render(int cameraoffset_x, int cameraoffset_y, float zoom = 1);

  Point getMaxScreenCoords(float zoom = 1.0);
  Point getMinScreenCoords(float zoom = 1.0);

  Point getIsoCoords(Point mouseCoords, int cameraoffset_x, int cameraoffset_y, float zoom = 1.0);
  Point getScreenCoords(Point isoCoords, int cameraoffset_x = 0, int cameraoffset_y = 0, float zoom = 1.0);


  bool getDrawGrid() { return _drawGrid; }
  void toggleGrid() { _drawGrid = !_drawGrid; }

  std::vector<Sprite*> _tiles;
  std::vector<Sprite*> _grid;

  int mmap[16][16];

private:
  bool _drawGrid = false;

  int _maxXTile = 0;
  int _minXTile = 0;
  int _maxYTile = 0;
  int _minYTile = 0;

};


#endif 