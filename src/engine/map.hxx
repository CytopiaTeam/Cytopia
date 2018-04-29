#ifndef ENGINE_HXX_
#define ENGINE_HXX_


#include <string>
#include <cmath>
#include <vector>

//#include "yaml-cpp/yaml.h"
#include "SDL2/SDL.h"

#include "sprite.hxx"
#include "basics/point.hxx"
#include "basics/vectorMatrix.hxx"


class Engine
{
public:

  int _width = 16;
  int _height = 16;
  int mmap[16][16];

  Engine(SDL_Renderer *renderer, SDL_Window *window);
  virtual ~Engine();

  void parseMapFile();

  void render();

  void centerScreenOnPoint(Point isoCoordinates);

  Point getIsoCoords(Point screenCoordinates, bool calcWithoutOffset = false);
  Point getScreenCoords(Point isoCoordinates, bool calcWithoutOffset = false);
  void findNeighbors(Point isoCoords);

  Point getCameraOffset();
  void setCameraOffset(Point offset);

  bool checkBoundaries(Point isoCoordinates);

  float getZoomLevel();
  void setZoomLevel(float zoomLevel);

  void enableLayer(unsigned int layer);
  void disableLayer(unsigned int layer);
  void toggleLayer(unsigned int layer);


private:
  int _screen_width;
  int _screen_height;

  float _zoom = 1.0;
  Point _cameraOffset;
  Point _centerScreenCoords;

  vectorMatrix _floorTilesMatrix;
  vectorMatrix _gridTilesMatrix;
  vectorMatrix _buildingsTilesMatrix;

public:
  enum Layers : unsigned int
  {
    LAYER_FLOOR       = 1u<<0,
    LAYER_BUILDINGS   = 1u<<1,
    LAYER_GRID        = 1u<<2
  };

private:
  unsigned int _activeLayers;

};


#endif 