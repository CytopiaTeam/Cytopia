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

  /// Retrieves Instance of Singleton class Texture Manager
  static Engine& Instance() {
    static Engine instance;
    return instance;
  }

  // Disable copy and assignemnt operators
  Engine(Engine const&) = delete;
  Engine& operator=(Engine const&) = delete;

  void parseMapFile();

  /** Renders the sprites in each Map Cell */
  void render();

  bool checkBoundaries(Point isoCoordinates);
  void centerScreenOnPoint(Point isoCoordinates);

  /** \brief Enable Drawing Layer
    * Enable Drawing Layer (use bitwise OR to add layer)
    * @param bitmapped Uint from enum "Layers"
    * @see Resources#Layers
    */
  void enableLayer(unsigned int layer) { _activeLayers |= layer; };

  /** \brief Disable Drawing Layer
  * Disable Drawing Layer ( Turned off by using bitwise AND with inversed pattern)
  * @param bitmapped Uint from enum "Layers"
  * @see Resources#Layers
  */
  void disableLayer(unsigned int layer) { _activeLayers &= ~layer; };
  
  /** \brief Toggle Drawing Layer
  * Toggle Drawing Layer (use bitwise XOR to toggle layer)
  * @param bitmapped Uint from enum "Layers"
  * @see Resources#Layers
  */
  void toggleLayer(unsigned int layer) { _activeLayers ^= layer; };

  /** Increases height of a tile at given iso coordinates.
  */
  void increaseHeight(Point isoCoordinates);
  /** Decreases height of a tile at given iso coordinates.
  */
  void decreaseHeight(Point isoCoordinates);
  void increaseZoomLevel();
  void decreaseZoomLevel();

  /** Returns a Cell at given screen coordinates, determined by pixel collison and Z-Order
  * @param screenCoordinates   Point Oject - Mouseclick coordinates in screen Format
  * @return Isocoordinates of the tile that has been found
  **/
  Point findCellAt(Point screenCoordinates);
private:
  Engine();
  virtual ~Engine() = default;

  SDL_Renderer *_renderer;
  SDL_Window *_window;

  int _map_size;
  int _screen_width;
  int _screen_height;
  int TILE_SIZE;

  float _zoomLevel;
  Point _cameraOffset;
  Point _centerIsoCoordinates;

  /// vectorMatrix that holds shared_ptr Cell objects
  vectorMatrix _floorCellMatrix;

  /// Uint for storing a bitmask (Layers Enum)
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