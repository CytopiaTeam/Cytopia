#ifndef GAME_HXX_
#define GAME_HXX_

#ifndef VERSION
#define VERSION 0
#endif

#include "Scripting/ScriptEngine.hxx"
#include "util/MessageQueue.hxx"
#include "util/Meta.hxx"
#include "Events.hxx"

#include <thread>

using Thread = std::thread;

using UILoopMQ = StrongType<MessageQueue<typename VariantType<UIEvents>::type>, struct UITag>;
using GameLoopMQ = StrongType<MessageQueue<typename VariantType<GameEvents>::type>, struct GameTag>;

class GameService
{

public:

  using Types = TypeList<UILoopMQ, GameLoopMQ /* Add other services here */ >;

  using ServiceTuple = typename TupleType<Types>::type;
  GameService(ServiceTuple&& services) : m_Services(services) { } 

  template <typename ServiceType>
  struct require_service_type
  {
    static_assert(ContainsType<Types, ServiceType>, "ServiceType must be registered as a Service in GameService::Types");
    using type = ServiceType;
  };
  
  template <typename ServiceInner, typename Tag>
  struct require_service_type<StrongType<ServiceInner, Tag>>
  {
    static_assert(ContainsType<Types, StrongType<ServiceInner, Tag>>, "StrongType<ServiceInner, Tag> must be registered as a Service in GameService::Types");
    using type = ServiceInner;
  };

private:
  
  TupleType<Types>::type& m_Services;

protected:

  template<typename ServiceType>
  ServiceType& GetService()
  {
    return std::get<typename require_service_type<ServiceType>::type>(m_Services);
  }

};

class Game
{
public:
  Game() : m_GameContext(UILoopMQ{}, GameLoopMQ{}),
          m_UILoop(&LoopMain<UILoopMQ, UIVisitor>, std::ref(m_GameContext), UIVisitor{}), 
          m_EventLoop(&LoopMain<GameLoopMQ, GameVisitor>, std::ref(m_GameContext), GameVisitor{}) { }
  virtual ~Game() = default;

  virtual bool initialize();
  virtual void run(bool SkipMenu = false);
  virtual void shutdown();
  virtual void mainMenu();

private:

  using GameContext = typename TupleType<GameService::Types>::type;
  GameContext m_GameContext;
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

  struct GameVisitor
  {
    template<typename ArgumentType>
    void operator()(ArgumentType&& event)
    {
      static_assert(std::is_void_v<std::void_t<ArgumentType>>, "GameVisitor does not know how to handle this event. You must specialize the functor");
    }

  };

};

#endif
