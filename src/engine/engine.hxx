#ifndef ENGINE_HXX_
#define ENGINE_HXX_


#include <string>
#include <cmath>
#include <vector>

//#include "yaml-cpp/yaml.h"
#include "SDL2/SDL.h"

#include "sprite.hxx"
#include "cell.hxx"
#include "basics/point.hxx"
#include "basics/vectorMatrix.hxx"
#include "basics/resources.hxx"


class Engine
{
public:

  int _width = 16;
  int _height = 16;
  int mmap[16][16];

  Engine();
  virtual ~Engine();

  void parseMapFile();

  void render();

  void centerScreenOnPoint(Point isoCoordinates);
  void centerScreenOnMap();
  
  Point getScreenCoords(Point isoCoordinates, bool calcWithoutOffset = false);
  std::vector<Sprite*> findNeighbors(Point isoCoords);

  bool checkBoundaries(Point isoCoordinates);

  void enableLayer(unsigned int layer);
  void disableLayer(unsigned int layer);
  void toggleLayer(unsigned int layer);
  void increaseHeight(Point isoCoordinates);
  void decreaseHeight(Point isoCoordinates);
  void increaseZoomLevel();
  void decreaseZoomLevel();

private:
  SDL_Renderer *_renderer;
  SDL_Window *_window;

  int _screen_width;
  int _screen_height;

  float _zoomLevel;
  Point _cameraOffset;
  Point _centerIsoCoordinates;

  vectorMatrix _floorTilesMatrix;
  vectorMatrix _gridTilesMatrix;
  vectorMatrix _buildingsTilesMatrix;
  vectorMatrix _selectedTilesMatrix;

  // Cells
  vectorMatrix _floorCellMatrix;

public:
  enum Layers : unsigned int
  {
    LAYER_FLOOR       = 1u<<0,
    LAYER_BUILDINGS   = 1u<<1,
    LAYER_GRID        = 1u<<2,
    LAYER_SELECTION   = 1u<<3
  };


private:
  unsigned int _activeLayers;


};


#endif 