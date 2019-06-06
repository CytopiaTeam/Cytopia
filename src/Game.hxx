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

  virtual bool initialize();
  virtual void run(bool SkipMenu = false);
  virtual void shutdown();
  virtual void mainMenu();
};

#endif