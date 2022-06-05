#include <catch.hpp>
#include "../../src/engine/common/Constants.hxx"
#include "../../src/util/Filesystem.hxx"
#include "../../src/engine/basics/Settings.hxx"

using string = std::string;

TEST_CASE("Read settings from file", "[settings][settings]")
{
  // store retrieved mapsize
  int mapSize = Settings::instance().mapSize;
  // set mapSize to a bogus value
  Settings::instance().mapSize = -9999;
  // read back all values
  Settings::instance().readFile();

  CHECK(mapSize == Settings::instance().mapSize);
  //TODO Repeat for more properties
}

TEST_CASE("Write settings to file", "[settings][settings]")
{
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
}

TEST_CASE("Write settings to data dir", "[settings][settings]")
{
  std::string pathToSettingsFile = CYTOPIA_DATA_DIR + (std::string)SETTINGS_FILENAME;
  Settings::instance().writeFile();

  CHECK(fs::fileExists(pathToSettingsFile) == true);
}
