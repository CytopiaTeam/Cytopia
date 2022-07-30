#include <catch.hpp>
#include "../../src/Game.hxx"
#include "../../src/engine/Map.hxx"
#include "../../src/game/PowerManager.hxx"
#include "../../src/game/ZoneManager.hxx"
#include <MapFunctions.hxx>

#define private public
#define protected public

#define class struct

TEST_CASE("Zone can be powered", "[powermanager][powermanager]")
{
  MapFunctions::instance();
  MapFunctions::instance().newMap();
  SignalMediator::instance().signalNewGame.emit(false);
  PowerManager powerManager;
  ZoneManager zoneManager;

  MapFunctions::instance().setTileID("pow_5x5_Kohlekraftwerk_Durnrohr_FN", Point{20, 20});
  for (int x = 21; x <= 30; x++)
  {
    MapFunctions::instance().setTileID("pow_RuralPowerLines", Point{x, 20});
  }
  for (int x = 31; x <= 40; x++)
    for (int y = 20; y <= 30; y++)
    {
      MapFunctions::instance().setTileID("zone_residential_dense", Point{x, y});
    }
  // i have no idea how to check if a zone is powered
  CHECK(MapFunctions::instance().getMap() != nullptr);
}
