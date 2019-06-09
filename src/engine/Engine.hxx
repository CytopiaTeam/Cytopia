#ifndef ENGINE_HXX_
#define ENGINE_HXX_

#include <SDL.h>

#include "WindowManager.hxx"
#include "basics/point.hxx"
#include "Map.hxx"
#include "../util/Singleton.hxx"

class Engine : public Singleton<Engine>
{
public:

  friend Singleton<Engine>;

  // Disable copy and assignemnt operators
  Engine(Engine const &) = delete;
  Engine &operator=(Engine const &) = delete;

  /** \brief Enable Drawing Layer
    * Enable Drawing Layer (use bitwise OR to add layer)
    * @param bitmapped Uint from enum "Layers"
    * @see Resources#Layers
    */
  void enableLayer(Layer layer);

  /** \brief Disable Drawing Layer
    * Disable Drawing Layer ( Turned off by using bitwise AND with inversed pattern)
    * @param bitmapped Uint from enum "Layers"
    * @see Resources#Layers
    */
  void disableLayer(Layer layer);

  /** \brief Toggle Drawing Layer
    * Toggle Drawing Layer (use bitwise XOR to toggle layer)
    * @param bitmapped Uint from enum "Layers"
    * @see Resources#Layers
    */
  void toggleLayer(Layer layer);

  /** @brief Increase Height
    * Increases the height of the given map node 
    * Calls the according function of the Map object that holds the terrain node and draws the slopes
    * @param isoCoordinates the isometric coordinates of the map node that should be raised
    * @see Map#increaseHeight
    */
  void increaseHeight(const Point &isoCoordinates) const;

  /** @brief Decrease Height
    * Decreases the height of the given map node
    * Calls the according function of the Map object that holds the terrain node and draws the slopes.
    * @param isoCoordinates the isometric coordinates of the map node that should be lowered
    * @see Map#decreaseHeight
    */
  void decreaseHeight(const Point &isoCoordinates) const;

  void toggleFullScreen();

  void setTileIDOfNode(const Point &isoCoordinates, const std::string &tileID) const;

  void demolishNode(const Point &isoCoordinates) const;

  bool isGameRunning() { return m_running; };
  void quitGame() { m_running = false; };

  /** @brief Loads a saved game
  * Loads a saved Game
  * @param fileName FileName of the saved game
  * @see Map#loadMapFromFile
  */
  void loadGame(const std::string &fileName);

  void saveGame(const std::string &fileName) { map->saveMapToFile(fileName); };

  void newGame();

  Map *map;

private:
  Engine();
  ~Engine();
  bool m_running = false;
};

#endif
