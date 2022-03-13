#ifndef GAME_HXX_
#define GAME_HXX_

#ifndef VERSION
#define VERSION 0
#endif

#include "Scripting/ScriptEngine.hxx"
#include "engine/MessageQueue.hxx"
#include "util/Meta.hxx"
#include "GameService.hxx"
#ifdef USE_AUDIO
#include "services/AudioMixer.hxx"
#endif
#include "services/Randomizer.hxx"
#include "services/GameClock.hxx"
#include "services/ResourceManager.hxx"
#include "LOG.hxx"
#include "Exception.hxx"

#include <thread>

using Thread = std::thread;
using RuntimeError = std::runtime_error;

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
    * @return true in case game has been quit, otherwise false.
    */
  virtual bool mainMenu();

private:
  /* Game context */
  using GameContext = GameService::ServiceTuple;
  GameContext m_GameContext;

  /* Services */
  Randomizer m_Randomizer;
  ResourceManager m_ResourceManager;
  GameLoopMQ m_GameLoopMQ;
#ifdef USE_AUDIO
  AudioMixer m_AudioMixer;
#endif

  /* Threads */
  Thread m_EventLoop;

  template <typename MQType, typename Visitor> static void LoopMain(GameContext &context, Visitor visitor);

  struct GameVisitor : public GameService
  {

#ifdef USE_AUDIO
    /**
     * @brief handles valid Audio events
     * @tparam AudioEventType the Audio event
     */
    template <typename AudioEventType>
    EnableIf<ContainsType<AudioEvents, AudioEventType>, void> operator()(AudioEventType &&event);
#endif // USE_AUDIO

    /**
     * @brief handles invalid game events
     * @tparam ArgumentType the invalid game event
     */
    template <typename ArgumentType> void operator()(const ArgumentType &&event);
  };

  void quit();
};

#include "Game.inl.hxx"

#endif
