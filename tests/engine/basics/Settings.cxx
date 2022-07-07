#include <catch.hpp>
#include "../../src/engine/common/Constants.hxx"
#include "../../src/util/Filesystem.hxx"
#include "../../src/engine/basics/Settings.hxx"
#include "../../Common.hxx"

using string = std::string;

TEST_CASE("Read settings from file", "[settings][settings]")
{
  START_TEST

  // store retrieved mapsize
  int mapSize = Settings::instance().mapSize;
  // set mapSize to a bogus value
  Settings::instance().mapSize = -9999;
  // read back all values
  Settings::instance().readFile();

  CHECK(mapSize == Settings::instance().mapSize);
  //TODO Repeat for more properties

  FINISH_TEST
}

TEST_CASE("Write settings to file", "[settings][settings]")
{
  START_TEST

  int mapSize = Settings::instance().mapSize;
  int bogusMapSize = -9999;

  Settings::instance().mapSize = bogusMapSize;
  Settings::instance().writeFile();
  Settings::instance().readFile();

  CHECK(Settings::instance().mapSize == bogusMapSize);
  
  Settings::instance().mapSize = mapSize;
  Settings::instance().writeFile();
  Settings::instance().readFile();
  
  CHECK(Settings::instance().mapSize == mapSize);

  FINISH_TEST
}

TEST_CASE("Write settings to data dir", "[settings][settings]")
{
  START_TEST

  std::string pathToSettingsFile = CYTOPIA_DATA_DIR + (std::string)SETTINGS_FILENAME;
  Settings::instance().writeFile();

  CHECK(fs::fileExists(pathToSettingsFile) == true);

  FINISH_TEST
}
