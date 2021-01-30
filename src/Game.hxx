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
#include "services/MouseController.hxx"
#include "LOG.hxx"
#include "Exception.hxx"
#include "view/Window.hxx"
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
  virtual ~Game();

  /** @brief starts setting up the game
    * starts game initialization.
    */
  virtual bool initialize();

  /** @brief begins the game
    * starts running the game
    * @param SkipMenu if the main menu should be skipped or not
    */
  virtual void run(bool SkipMenu = false);

  /** @brief initializes and displays the main menu
    * initializes and displays the main menu
    * @return true in case game has been quit, othewise false.
    */
  virtual bool mainMenu();

  void quit();

  void newUI();

private:
  /* Game context */
  using GameContext = GameService::ServiceTuple;
  GameContext m_GameContext;

  /* Services */
  GameClock m_GameClock;
  Randomizer m_Randomizer;
  ResourceManager m_ResourceManager;
  MouseController m_MouseController;
  UILoopMQ m_UILoopMQ;
  GameLoopMQ m_GameLoopMQ;
#ifdef USE_AUDIO
  AudioMixer m_AudioMixer;
#endif

  /* Threads */
  Thread m_UILoop;
  Thread m_EventLoop;

  /* Window */
  Window m_Window;

  template <typename MQType, typename Visitor> static void LoopMain(GameContext &context, Visitor visitor);

  struct UIVisitor
  {
    friend class Game;
    UIVisitor(Window &, GameContext &);
    
    /**
     * @brief   Do not call.
     * @throws  CytopiaError
     */
    void operator()(TerminateEvent &&);

    /**
     * @brief   Handles window changes
     */
    void operator()(WindowResizeEvent &&);

    /**
     * @brief   Handles window redrawing requests
     */
    void operator()(WindowRedrawEvent &&);

    /**
     * @brief   Handles changes in the UI
     */
    void operator()(UIChangeEvent &&);

    /**
     * @brief   Handles switching to a new Activity
     */
    void operator()(ActivitySwitchEvent &&);

    /**
     * @brief   handles invalid UI events
     * @tparam  ArgumentType the invalid event
     */
    template <typename ArgumentType> void operator()(ArgumentType &&event);
  
    private:
      Window & m_Window;
      GameContext & m_GameContext;
  };

  struct GameVisitor : public GameService
  {

#ifdef USE_AUDIO
    /**
     * @brief   handles valid Audio events
     * @tparam  AudioEventType the Audio event
     */
    template <typename AudioEventType>
    EnableIf<AudioEvents::ContainsType<AudioEventType>, void> operator()(AudioEventType &&event);
#endif // USE_AUDIO

    /**
     * @brief   Handles window changes
     */
    void operator()(WindowResizeEvent &&);

    /**
     * @brief   Handles window redrawing requests
     */
    void operator()(WindowRedrawEvent &&);

    /**
     * @brief   Handles switching to a new Activity
     */
    void operator()(ActivitySwitchEvent &&);

    /**
     * @brief   Do not call
     * @throws  CytopiaError
     */
    void operator()(TerminateEvent &&);

    /**
     * @brief   handles invalid game events
     * @tparam  ArgumentType the invalid game event
     */
    template <typename ArgumentType> void operator()(const ArgumentType &&event);
    
  };
};

#include "Game.inl.hxx"

#endif
