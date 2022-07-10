#ifndef GAME_HXX_
#define GAME_HXX_

#include "scripting/ScriptEngine.hxx"
#include "engine/MessageQueue.hxx"
#include "util/Meta.hxx"
#ifdef USE_AUDIO
#include "services/AudioMixer.hxx"
#endif
#include "services/Randomizer.hxx"
#include "services/GameClock.hxx"
#include "services/ResourceManager.hxx"
#include "LOG.hxx"
#include "Exception.hxx"
#include "../game/GamePlay.hxx"
#include "ThreadWorkers.hxx"

#include <thread>
#include <mutex>
#include <queue>

using Thread = std::thread;
using RuntimeError = std::runtime_error;
struct BaseScene;

namespace Cytopia
{

class Game
{
public:
  /**
   * @brief Creates a game
   * @details Initializes all GameServices and starts the threads
   */
  Game();

  /**
   * @brief Destroy a game
   */
  ~Game();

  /** @brief starts setting up the game
    * @details starts game initialization.
    */
  void initialize();

  /** @brief begins the game
    * @details starts running the game
    * @param SkipMenu if the main menu should be skipped or not
    */
  void run();

  inline void nextScene(BaseScene *scene)
  {
    std::unique_lock lock(m_scenes_access);

    // don't allow enqueueing after stopping the pool
    if (m_shutDown)
      return;

    m_scenes.emplace(scene);
  }

  inline ThreadWorkers &mt() { return m_mt; }

  /// ends the game
  virtual void shutdown();

  void newGame(bool generateTerrain);
  void loadGame(const std::string &fileName);

private:
  void quit();
  bool m_shutDown = false;
  Uint32 m_fpsFrames = 0;

  GamePlay m_GamePlay;
  std::queue<BaseScene *> m_scenes;
  std::mutex m_scenes_access;

  ThreadWorkers m_mt;
};

} // namespace Cytopia

extern Cytopia::Game *g_game;
#endif
