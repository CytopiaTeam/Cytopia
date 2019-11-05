#ifndef GAME_HXX_
#define GAME_HXX_

#ifndef VERSION
#define VERSION 0
#endif

#include "Scripting/ScriptEngine.hxx"
#include "util/MessageQueue.hxx"
#include "util/Meta.hxx"
#include "GameService.hxx"
#ifdef USE_SDL2_MIXER
#include "services/AudioMixer.hxx"
#endif
#include "services/Randomizer.hxx"
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
    */
  virtual void mainMenu();

private:
  /* Services */
#ifdef USE_SDL2_MIXER
  AudioMixer m_AudioMixer;
#endif
  Randomizer m_Randomizer;
  UILoopMQ m_UILoopMQ;
  GameLoopMQ m_GameLoopMQ;

  /* Game context */
  using GameContext = typename TupleType<GameService::Types>::type;
  GameContext m_GameContext;

  /* Threads */
  Thread m_UILoop;
  Thread m_EventLoop;

  template <typename MQType, typename Visitor> static void LoopMain(GameContext &context, Visitor visitor);

  struct UIVisitor
  {

    /**
     * @brief handles invalid UI events
     * @tparam ArgumentType the invalid event
     */
    template <typename ArgumentType> void operator()(ArgumentType &&event);

    /**
     * @brief handles TransitiveStateChange messages
     * @tparam TransitiveType the type which transitions
     */
    template <typename TransitiveType> void operator()(TransitiveStateChange<TransitiveType> &&event);
  };

  struct GameVisitor : public GameService
  {

    /**
     * @brief handles valid Audio events
     * @tparam AudioEventType the Audio event
     */
    template <typename AudioEventType>
    EnableIf<ContainsType<AudioEvents, AudioEventType>, void> operator()(AudioEventType &&event);

    /**
     * @brief handles invalid game events
     * @tparam ArgumentType the invalid game event
     */
    template <typename ArgumentType> void operator()(const ArgumentType &&event);
  };
};

#include "Game.inl.hxx"

#endif
