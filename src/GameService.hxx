#ifndef GAME_SERVICE_HXX_
#define GAME_SERVICE_HXX_

#include "Events.hxx"
#include "util/MessageQueue.hxx"

using UILoopMQ = MessageQueue<typename VariantType<UIEvents>::type>;
using GameLoopMQ = MessageQueue<typename VariantType<GameEvents>::type>;
class AudioMixer;
class Randomizer;

class GameService
{

public:

  using Types = TypeList<UILoopMQ&, GameLoopMQ&, 
        AudioMixer&, Randomizer& /* Add other services here */ >;

  using ServiceTuple = typename TupleType<Types>::type;
  GameService(ServiceTuple& services) : m_Services(services) { } 

  template <typename ServiceType>
  struct require_service_type;

private:
  
  TupleType<Types>::type& m_Services;

protected:

  template<typename ServiceType>
  ServiceType& GetService()
  {
    return std::get<typename require_service_type<ServiceType&>::type>(m_Services);
  }

};

#include "GameService.inl.hxx"

#endif
