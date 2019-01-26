#ifndef ENGINE_HXX_
#define ENGINE_HXX_

#include <SDL.h>

#include "windowManager.hxx"
#include "basics/point.hxx"
#include "map.hxx"

class Engine
{
public:
  /// Retrieves instance of Singleton class Engine
  static Engine &instance()
  {
    static Engine engine;
    return engine;
  }

  // Disable copy and assignemnt operators
  Engine(Engine const &) = delete;
  Engine &operator=(Engine const &) = delete;

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

  void toggleFullScreen();

  void setTileIDOfNode(const Point &isoCoordinates, const std::string &tileType);

  void demolishNode(const Point &isoCoordinates);

  Map *getMap() { return &_map; };

  bool isGameRunning() { return _running; };
  void quitGame() { _running = false; };

private:
  Engine();
  virtual ~Engine() = default;
  bool _running = true;

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