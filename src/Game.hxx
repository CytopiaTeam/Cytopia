#ifndef GAME_HXX_
#define GAME_HXX_

#ifndef VERSION
#define VERSION 0
#endif

#include "Scripting/ScriptEngine.hxx"

class Game
{
public:
  Game() = default;
  virtual ~Game() = default;
  
  /** @brief starts setting up the game
    * starts game initialization.
    */
  virtual bool initialize();
  /** @brief begins the game
    * starts running the game
    * @param SkipMenu if the main menu should be skipped or not
    */
  virtual void run(bool SkipMenu = false);
  /** @brief ends the game
    * shuts down the game
    */
  virtual void shutdown();
  /** @brief initializes and displays the main menu
    * initializes and displays the main menu
    */
  virtual void mainMenu();
};

#endif
