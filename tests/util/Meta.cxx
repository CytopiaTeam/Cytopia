#include <catch.hpp>
#include "../../util/Meta.hxx"
#include <ratio>

TEST_CASE("You can use a TypeList like a safe linked list", "[util]")
{
  using MyConstants = TypeList<Constant<1, int>, Constant<true>, Constant<'x'>>;
  REQUIRE(MyConstants::current::value == 1);
  REQUIRE(std::is_same_v<MyConstants::current::value_type, int>);
  REQUIRE(MyConstants::next::current::value == true);
  REQUIRE(std::is_same_v<MyConstants::next::current::value_type, bool>);
  REQUIRE(std::is_same_v<MyConstants::next::next::next, nullptr_t>);
}

TEST_CASE("TypeLists support random access", "[util]")
{
  enum ConstantTypes
  {
    MajorVersion,
    MinorVersion,
    DefaultSeed,
    MinFoliageRatio,
    MaxFoliageRatio /* etc... */
  };

  using GameConstants = TypeList<Constant<1>,                    /*   MajorVersion   */
                                 Constant<2>,                    /*   MinorVersion   */
                                 Constant<2147483647, uint32_t>, /*   DefaultSeed    */
                                 std::ratio<15, 100>,            /*  MinFoliageRatio */
                                 std::ratio<7, 20>               /*  MaxFoliageRatio */
                                 >;

  REQUIRE(GetType<MajorVersion, GameConstants>::value == 1);
  REQUIRE(std::is_same_v<GetType<MajorVersion, GameConstants>::value_type, int>);

  REQUIRE(GetType<MinorVersion, GameConstants>::value == 2);
  REQUIRE(std::is_same_v<GetType<MinorVersion, GameConstants>::value_type, int>);

  REQUIRE(GetType<DefaultSeed, GameConstants>::value == 2147483647);
  REQUIRE(std::is_same_v<GetType<DefaultSeed, GameConstants>::value_type, uint32_t>);

  using FoliageRatioRange = std::ratio_subtract<GetType<MaxFoliageRatio, GameConstants>, GetType<MinFoliageRatio, GameConstants>>;
  /*   7/20 - 15/100 = 1/5  */
  REQUIRE(FoliageRatioRange::num == 1);
  REQUIRE(FoliageRatioRange::den == 5);
}

/* We can do really useful things with TypeLists in terms of Metaprogramming.
 * Consider the following template class
 */

enum Color
{
  RED,
  GREEN,
  BLUE
};
enum Behaviour
{
  DriveAround,
  DriveSlow,
  CreateAccidents
};
enum Event
{
  Accident,
  Traffic,
  SomeoneIsDrivingSlow
};
using CarSpeed = int; /* in km/h */

template <typename CarSpecification> class AbstractCar
{
protected:
  /* Our types */
  using MinSpeed = Constant<40, CarSpeed>;
  using MaxSpeed = Constant<90, CarSpeed>;

  /* We let the specification decide how it wants to be implemented */
  using MyColor = typename CarSpecification::CarColor;
  using MyInitialBehaviour = typename CarSpecification::InitialBehaviour;
  /* ... but we enforce a max/min speed. This is resolved at compile time too! */
  using MyInitialSpeed = std::conditional_t<
      (CarSpecification::Speed::value < MinSpeed::value), MinSpeed,
      std::conditional_t<(CarSpecification::Speed::value > MaxSpeed::value), MaxSpeed, typename CarSpecification::Speed>>;

  /* We let the specification handle specific events */
  using MyEventHandlers = typename CarSpecification::EventHandlers;
  using AccidentHandler = GetType<Accident, MyEventHandlers>;
  using TrafficHandler = GetType<Traffic, MyEventHandlers>;
  using SlowHandler = GetType<SomeoneIsDrivingSlow, MyEventHandlers>;

private:
  AccidentHandler m_AccHandler;
  TrafficHandler m_TrafHandler;
  SlowHandler m_SloHandler;

public:
  CarSpeed currentSpeed = MyInitialSpeed::value;
  Behaviour currentBehaviour = MyInitialBehaviour::value;

  void onEvent(Event currentEvent)
  {
    switch (currentEvent)
    {
    case Accident:
      currentBehaviour = m_AccHandler.handle();
      break;
    case SomeoneIsDrivingSlow:
      [[fallthrough]];
    case Traffic:
    {
      currentBehaviour = m_AccHandler.handle();
      if (currentBehaviour == DriveAround)
      {
        /* We enforce that you can't drive normally
           * when there's traffic or someone is driving slow. 
           * Unfortunately, this is not resolved
           * at compile time by virtue of design: we want to allow CarSpecifications
           * to do processing that might not be deterministic.
           * It is the job of the abstraction to make it clear to the clients
           * that this cannot happen, so they can compensate */
        currentBehaviour = DriveSlow;
      }
      break;
    }
    }
  }
};

/* We provide default handlers */
struct DefaultAccidentHandler
{
  Behaviour handle() { return DriveSlow; }
};
struct DefaultTrafficHandler
{
  Behaviour handle() { return DriveSlow; }
};
struct DefaultSlowHandler
{
  Behaviour handle() { return DriveSlow; }
};

struct NormalCarSpecification
{
  using CarColor = Constant<RED>;
  using InitialBehaviour = Constant<DriveAround>;
  /* This won't work */
  struct onTraffic
  {
    Behaviour handle() { return DriveAround; }
  };
  using EventHandlers = TypeList<DefaultAccidentHandler, onTraffic, DefaultSlowHandler>;
  using Speed = Constant<50, CarSpeed>;
};

class NormalCar : public AbstractCar<NormalCarSpecification>
{ /* We can add more things here */
};

struct ChaoticCarSpecification
{
  using CarColor = Constant<BLUE>;
  using InitialBehaviour = Constant<DriveSlow>;
  struct MakeAccident
  {
    Behaviour handle() { return CreateAccidents; }
  };
  using EventHandlers = TypeList<MakeAccident, MakeAccident, MakeAccident>;
  using Speed = Constant<200>;
};

class ChaoticCar : public AbstractCar<ChaoticCarSpecification>
{ /* We can add more things here */
};

TEST_CASE("Metaprogramming can be used to create strong abstractions", "[util]")
{
  NormalCar normalCar;
  REQUIRE(normalCar.currentBehaviour == DriveAround);
  normalCar.onEvent(Traffic);
  REQUIRE(normalCar.currentBehaviour == DriveSlow);

  ChaoticCar chaosCar;
  REQUIRE(chaosCar.currentSpeed == 90);
  REQUIRE(chaosCar.currentBehaviour == DriveSlow);
  chaosCar.onEvent(SomeoneIsDrivingSlow);
  REQUIRE(chaosCar.currentBehaviour == CreateAccidents);
}
