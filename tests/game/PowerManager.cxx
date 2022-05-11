#include <catch.hpp>
#include "../../src/engine/common/Constants.hxx"
#include "../../src/engine/Map.hxx"
#include "../../src/services/GameClock.hxx"
#include "../../src/game/PowerManager.hxx"
#include "../../src/game/ZoneManager.hxx"

#define private public
#define protected public

#define class struct

TEST_CASE("Zone can be powered", "[powermanager][powermanager]")
{
  Map *map = new Map(64, 64, false);
  CHECK(map != nullptr);

  PowerManager powerManager;
  ZoneManager zoneManager;
  map->setTileID("pow_5x5_Kohlekraftwerk_Durnrohr_FN", Point{20, 20});
  for (int x = 21; x <= 30; x++)
  {
    map->setTileID("pow_RuralPowerLines", Point{x, 20});
  }
  for (int x = 31; x <= 40; x++)
    for (int y = 20; y <= 30; y++)
    {
      map->setTileID("zone_residential_dense", Point{x, y});
    }
  bool powered = false;
  SignalMediator::instance().registerCbUpdatePower(
      [&powered](const std::vector<PowerGrid> &grids) { // If we place a power tile, add it to the cache to update next tick
        for (auto grid : grids)
        {
          if (grid.getPowerLevel() > 0)
          {
            powered = true;
          }
        }
      });

  GameClock::instance().tick();
  CHECK(powered == TRUE);
}
