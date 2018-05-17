#ifndef ENGINE_HXX_
#define ENGINE_HXX_

#include "SDL2/SDL.h"

#include "textureManager.hxx"
#include "cell.hxx"
#include "basics/point.hxx"
#include "basics/vectorMatrix.hxx"
#include "basics/resources.hxx"


class Engine
{
public:
  // Singleton
  static Engine& Instance() {
    static Engine instance;
    return instance;
  }

  // Disable copy and assignemnt operators
  Engine(Engine const&) = delete;
  Engine& operator=(Engine const&) = delete;


  // Map Size
  int _width = 16;
  int _height = 16;

  void parseMapFile();

  /**Renders each Map Cell
  */
  void render();

  bool checkBoundaries(Point isoCoordinates);
  void centerScreenOnPoint(Point isoCoordinates);
  void enableLayer(unsigned int layer);
  void disableLayer(unsigned int layer);
  void toggleLayer(unsigned int layer);
  /** Increases height of a tile at given screen coordinates.
  */
  void increaseHeight(Point mouseClickCoordinates);
  /** Decreases height of a tile at given screen coordinates.
  */
  void decreaseHeight(Point mouseClickCoordinates);
  void increaseZoomLevel();
  void decreaseZoomLevel();

  /** Retrieves "real iso coordinates" with tileheight taken into account */
  Point findCellAtCoords(Point screenCoordinates);

private:
  Engine();
  virtual ~Engine() { };

  SDL_Renderer *_renderer;
  SDL_Window *_window;

  int _screen_width;
  int _screen_height;
  int TILE_SIZE;

  float _zoomLevel;
  Point _cameraOffset;
  Point _centerIsoCoordinates;

  // Cells
  vectorMatrix _floorCellMatrix;

  unsigned int _activeLayers;

  // Layer ENUM
public:
  enum Layers : unsigned int
  {
    LAYER_FLOOR       = 1u<<0,
    LAYER_BUILDINGS   = 1u<<1,
    LAYER_GRID        = 1u<<2,
    LAYER_SELECTION   = 1u<<3
  };
};

#endif 