#include <catch.hpp>
#include "../../src/engine/common/Constants.hxx"
#include "../../src/engine/ResourcesManager.hxx"
#include "../Common.hxx"
#include <MapFunctions.hxx>
#include <SignalMediator.hxx>

using string = std::string;

TEST_CASE("Create new Map", "[mapfunctions][mapfunctions]")
{
  START_TEST

  MapFunctions::instance(); //init
  MapFunctions::instance().newMap();
  SignalMediator::instance().signalNewGame.emit(true);

  CHECK(MapFunctions::instance().getMap() != nullptr);

  FINISH_TEST
}

TEST_CASE("Create empty Map without mapnodes", "[engine][engine]")
{
  START_TEST

  Map *map = new Map(64, 64, true);

  CHECK(map != nullptr);

  FINISH_TEST
}

TEST_CASE("Create Map with procedural terrain", "[engine][engine]")
{
  START_TEST

  Map *map = new Map(64, 64, true);

  CHECK(map != nullptr);

  FINISH_TEST
}

TEST_CASE("Create Map without procedural terrain", "[engine][engine]")
{
  START_TEST

  Map *map = new Map(64, 64, false);

  CHECK(map != nullptr);

  FINISH_TEST
}

TEST_CASE("Save and load Map", "[engine][engine]")
{
  START_TEST

  MapFunctions::instance(); //init
  MapFunctions::instance().newMap();
  SignalMediator::instance().signalNewGame.emit(true);
  SignalMediator::instance().signalSaveGame.emit("saveTest");
  SignalMediator::instance().signalLoadGame.emit("saveTest");

  CHECK(MapFunctions::instance().getMap() != nullptr);

  FINISH_TEST
}
