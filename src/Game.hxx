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
  Game() :
#ifdef USE_SDL2_MIXER
          m_AudioMixer{m_GameContext},
#endif
          m_Randomizer{m_GameContext},
          m_UILoopMQ{},
          m_GameLoopMQ{},
          m_GameContext(m_UILoopMQ, m_GameLoopMQ, m_AudioMixer, m_Randomizer),
          m_UILoop(&LoopMain<UILoopMQ, UIVisitor>, std::ref(m_GameContext), UIVisitor{}),
          m_EventLoop(&LoopMain<GameLoopMQ, GameVisitor>, std::ref(m_GameContext), GameVisitor{m_GameContext})
          { }
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

  template <typename MQType, typename Visitor>
  static void LoopMain(GameContext& context, Visitor visitor);

  struct UIVisitor
  {

    template<typename ArgumentType>
    void operator()(ArgumentType&& event)
    {
      static_assert(std::is_void_v<std::void_t<ArgumentType>>, "UIVisitor does not know how to handle this event. You must specialize the functor");
    }

    template<typename TransitiveType>
    void operator()(TransitiveStateChange<TransitiveType>&& event)
    {
      if(auto uiTarget = event.target.lock())
      {
        uiTarget->update(event.data);
      }
    }

  };

  struct GameVisitor : public GameService
  {

    using AudioEvents = TypeList<
      AudioTriggerEvent,
      AudioTriggerEvent,
      AudioTrigger3DEvent,
      AudioPlayEvent, 
      AudioPlay3DEvent, 
      AudioMusicVolumeChangeEvent, 
      AudioSoundVolumeChangeEvent, 
      AudioSetMutedEvent
      >;

    template <typename AudioEventType>
    EnableIf<ContainsType<AudioEvents, AudioEventType>, void>
    operator()(AudioEventType&& event)
    {
      GetService<AudioMixer>().handleEvent(std::move(event));
    }

    template<typename ArgumentType>
    void operator()(const ArgumentType&& event)
    {
      throw UnimplementedError(TRACE_INFO "Unimplemented Error");
    }

  };
};

#endif
