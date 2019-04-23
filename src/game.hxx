#ifndef GAME_HXX_
#define GAME_HXX_

#ifndef VERSION
#define VERSION 0
#endif

class Game
{
public:
  Game() = default;
  virtual ~Game() = default;

  virtual bool initialize();
  virtual void run();
  virtual void shutdown();
  virtual void mainMenu();
};

#endif