#ifndef GAME_FIXTURE_HXX
#define GAME_FIXTURE_HXX

#include "../src/Initializer.hxx"
#include "../src/Game.hxx"

/**
 *  @brief  Simulates a Game for testing purposes
 */
class GameFixture
{
public:
  GameFixture();
  ~GameFixture() = default;
protected:
  Initializer m_Initializer;
  Game m_Game;
};

#endif // GAME_FIXTURE_HXX
