#ifndef GAME_SERVICE_HXX_
#define GAME_SERVICE_HXX_

#include "Events.hxx"

class GameService
{

public:
  /**
   * @brief all concrete game services
   */
  using Types = TypeList<
    class GameLoopMQ *, 
#ifdef USE_AUDIO
    class AudioMixer *, 
#endif // USE_AUDIO
    class Randomizer *, 
    class ResourceManager * 
    /* Add other services here */
    >;

  /**
   * @brief a tuple of all services
   */
  using ServiceTuple = typename TupleType<Types>::type;

  /**
   * @brief Construct a GameService
   */
  GameService(ServiceTuple &services);

  /**
   * @brief A constraint that requires a type to be a GameService type
   * @details ::type is defined if ServiceType is part of the known GameServices or if
   *          it's a StrongType to a type part of the GameServices
   * @tparam ServiceType the type to verify
   */
  template <typename ServiceType> struct require_service_type;

private:
  TupleType<Types>::type &m_Services;

protected:
  /**
   * @brief returns the required service
   * @tparam ServiceType the type of the service
   */
  template <typename ServiceType> ServiceType &GetService();
};

#include "GameService.inl.hxx"

#endif