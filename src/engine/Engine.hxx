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

  // Disable copy and assignment operators
  Engine(Engine const &) = delete;
  Engine &operator=(Engine const &) = delete;

  /** @brief Increase Height
    * @details Increases the height of the given map node
    * Calls the according function of the Map object that holds the terrain node and draws the slopes
    * @param isoCoordinates the isometric coordinates of the map node that should be raised
    * @see Map#increaseHeight
    */
  void increaseHeight(const Point &isoCoordinates) const;

  /** @brief Decrease Height
    * @details Decreases the height of the given map node
    * Calls the according function of the Map object that holds the terrain node and draws the slopes.
    * @param isoCoordinates the isometric coordinates of the map node that should be lowered
    * @see Map#decreaseHeight
    */
  void decreaseHeight(const Point &isoCoordinates) const;

  /** @brief Toggle Fullscreen Mode
    */
  void toggleFullScreen();

  /** @brief Checks if game is running
    * @returns Returns true if the game is running, and false otherwise
    */
  bool isGameRunning() const { return m_running; };

  /** @brief Quits the game
    * @details Sets m_running to false and quits the game.
    */
  void quitGame() { m_running = false; };

  /** @brief Loads a saved game
    * @param fileName FileName of the saved game
    * @see Map#loadMapFromFile
    */
  void loadGame(const std::string &fileName);

  /** @brief Saves the game to a file
  * @param fileName name of the file to save the data to
  */
  void saveGame(const std::string &fileName) const { map->saveMapToFile(fileName); };

  /// Creates a new game
  void newGame();

  Map *map;

private:
  Engine();
  ~Engine();
  bool m_running = false;
};

#endif
