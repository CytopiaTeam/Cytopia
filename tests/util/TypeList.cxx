#include <catch.hpp>
#include "../../util/TypeList.hxx"
#include "../../util/Meta.hxx"
#include <ratio>
#include <iterator>

/* These are compile-time unit tests for TypeLists */
static_assert(!TypeList<>::ContainsType<int>, "Test failure");
static_assert(TypeList<int, double, char>::ContainsType<int>, "Test failure");
static_assert(TypeList<double, int, char>::ContainsType<int>, "Test failure");
static_assert(TypeList<double, char, int>::ContainsType<int>, "Test failure");
static_assert(std::is_same_v<TypeList<int, double, char>::Map<std::add_pointer>, TypeList<int*, double*, char*>>, "Test failure");

template <typename Type, typename Reduced>
struct MetaAdd {
  using type = Constant<Reduced::value + Type::value>;
};

static_assert(TypeList<Constant<1>, Constant<2>, Constant<3>>::Reduce<MetaAdd, Constant<0>>::value == 6, "Test failure");
static_assert(std::is_same_v<TypeList<int>::Get<0>, int>, "Test failure");
static_assert(std::is_same_v<TypeList<double, int>::Get<1>, int>, "Test failure");
static_assert(std::is_same_v<TypeList<int, double>::Get<0>, int>, "Test failure");
static_assert(std::is_same_v<TypeList<int, double, char>::VariantType, std::variant<int, double, char>>, "Test failure");
static_assert(is_variant<TypeList<int, double, char>::VariantType>::value, "Test failure");
static_assert(std::is_same_v<TypeList<int, double, char>::TupleType, std::tuple<int, double, char>>, "Test failure");
static_assert(is_tuple<TypeList<int, double, char>::TupleType>::value, "Test failure");
static_assert(std::is_same_v<TypeList<int, float, double>::CommonType, double>, "Test failure");
static_assert(std::is_same_v<TypeList<std::string, int>::CommonType, std::variant<std::string, int>>, "Test failure");

template <typename Type>
struct NameEnumerator {
  template <typename OutputIterator>
  static void run(OutputIterator it) {
    *it++ = Type::NAME;
  }
};

struct NamedBase
{
  virtual ~NamedBase() = default;
  virtual std::string getName() = 0;
};
struct NamedType1 : public NamedBase
{
  static constexpr const char * NAME = "Name1";
  std::string getName() override { return NAME; };
};
struct NamedType2 : public NamedBase
{
  static constexpr const char * NAME = "Name2";
  std::string getName() override { return NAME; };
};
struct NamedType3 : public NamedBase
{
  static constexpr const char * NAME = "Name3";
  std::string getName() override { return NAME; };
};

TEST_CASE("I can apply metafunctors over TypeLists", "[util]")
{
  std::vector<std::string> names;
  using MyNamedTypes = TypeList<NamedType1, NamedType2, NamedType3>;
  MyNamedTypes::ForEach<NameEnumerator>(std::back_inserter(names));
  REQUIRE(names == std::vector<std::string>{"Name1", "Name2", "Name3"});
}

TEST_CASE("I can TypeSwitch over TypeLists", "[util]")
{
  using MyNamedTypes = TypeList<NamedType1, NamedType2, NamedType3>;
  using MySafeBase = std::unique_ptr<NamedBase>;
  MyNamedTypes::TypeSwitch<MySafeBase> ts;
  REQUIRE(ts(0)->getName() == NamedType1::NAME);
  REQUIRE(ts(1)->getName() == NamedType2::NAME);
  REQUIRE(ts(2)->getName() == NamedType3::NAME);
  REQUIRE_THROWS(ts(3));
  REQUIRE_THROWS(ts(4));
  REQUIRE_THROWS(ts(5));
}

template <typename Type>
struct NameSwitch
{
  constexpr bool operator()(const std::string & target_name)
  {
    return Type::NAME == target_name;
  }
};

TEST_CASE("I can apply custom TypeSwitches over TypeLists", "[util]")
{
  using MyNamedTypes = TypeList<NamedType1, NamedType2, NamedType3>;
  using MySafeBase = std::unique_ptr<NamedBase>;
  MyNamedTypes::TypeSwitch<MySafeBase, NameSwitch> ts;
  REQUIRE(ts(NamedType1::NAME)->getName() == NamedType1::NAME);
  REQUIRE(ts(NamedType2::NAME)->getName() == NamedType2::NAME);
  REQUIRE(ts(NamedType3::NAME)->getName() == NamedType3::NAME);
  REQUIRE_THROWS(ts("Name4"));
  REQUIRE_THROWS(ts("Name5"));
  REQUIRE_THROWS(ts("Name6"));
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

  REQUIRE(GameConstants::Get<MajorVersion>::value == 1);
  REQUIRE(std::is_same_v<GameConstants::Get<MajorVersion>::value_type, int>);

  REQUIRE(GameConstants::Get<MinorVersion>::value == 2);
  REQUIRE(std::is_same_v<GameConstants::Get<MinorVersion>::value_type, int>);

  REQUIRE(GameConstants::Get<DefaultSeed>::value == 2147483647);
  REQUIRE(std::is_same_v<GameConstants::Get<DefaultSeed>::value_type, uint32_t>);

  using FoliageRatioRange = std::ratio_subtract<GameConstants::Get<MaxFoliageRatio>, GameConstants::Get<MinFoliageRatio>>;
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
  using AccidentHandler = typename MyEventHandlers::template Get<Accident>;
  using TrafficHandler = typename MyEventHandlers::template Get<Traffic>;
  using SlowHandler = typename MyEventHandlers::template Get<SomeoneIsDrivingSlow>;
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
  static_assert(std::is_same_v<EventHandlers::Get<Accident>, DefaultAccidentHandler>, "Not the same");
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
  static_assert(std::is_same_v<EventHandlers::Get<Accident>, MakeAccident>, "Not the same");
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
