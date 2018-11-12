#ifndef ENGINE_HXX_
#define ENGINE_HXX_

#include "SDL2/SDL.h"

#include "windowManager.hxx"
#include "basics/point.hxx"
#include "map.hxx"

class Engine
{
public:
  /// Retrieves Instance of Singleton class Texture Manager
  static Engine &Instance()
  {
    static Engine instance;
    return instance;
  }

  // Disable copy and assignemnt operators
  Engine(Engine const &) = delete;
  Engine &operator=(Engine const &) = delete;

  /** Renders the sprites in each Map MapNode */
  void render();

  /** \brief Check if given coordinates are within boundaries
    * Checks if coordinates are within map boundaries
    * @param Point object - coordinates to check
    * @return bool - true if coordinates are inside the map bounds.
    */
  bool isPointWithinBoundaries(const Point &isoCoordinates);

  /** \brief Centers camera on given isometric coordinates
  * Centers the camera on the given isometric coordinates.
  * @param Point object - coordinates to center the camera on
  */
  void centerScreenOnPoint(const Point &isoCoordinates);

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

  /** @brief Increase Height
    * Increases the height of the given map node 
    * Calls the according function of the Map object that holds the terrain node and draws the slopes
    * @param isoCoordinates the isometric coordinates of the map node that should be raised
    * @see Map#increaseHeight
    */
  void increaseHeight(const Point &isoCoordinates);

  /** @brief Decrease Height
    * Decreases the height of the given map node
    * Calls the according function of the Map object that holds the terrain node and draws the slopes.
    * @param isoCoordinates the isometric coordinates of the map node that should be lowered
    * @see Map#decreaseHeight
    */
  void decreaseHeight(const Point &isoCoordinates);

  void increaseZoomLevel();
  void decreaseZoomLevel();

  /** Returns a MapNode at given screen coordinates, determined by pixel collison and Z-Order
    * @param screenCoordinates   Point Oject - Mouseclick coordinates in screen Format
    * @return Isocoordinates of the tile that has been found
    */
  Point findNodeInMap(const Point &screenCoordinates);

  bool isGameRunning();

  void quitGame();
  void toggleFullScreen();

  void setTileTypeOfNode(const Point &isoCoordinates, const std::string tileType);

private:
  Engine();
  virtual ~Engine() = default;

  WindowManager *_windowManager;

  SDL_Renderer *_renderer;
  SDL_Window *_window;

  int _map_size;
  int _screen_width;
  int _screen_height;
  int _tileSize;

  float _zoomLevel;
  Point _cameraOffset;
  Point _centerIsoCoordinates;

  /// Map that holds shared_ptr MapNode objects
  Map _map;

  /// Uint for storing a bitmask (Layers Enum)
  unsigned int _activeLayers;

  // Layer ENUM
public:
  enum Layers : unsigned int
  {
    LAYER_FLOOR = 1u << 0,
    LAYER_BUILDINGS = 1u << 1,
    LAYER_GRID = 1u << 2,
    LAYER_SELECTION = 1u << 3
  };
};

#endif