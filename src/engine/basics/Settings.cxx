#include "Settings.hxx"

// #include "LOG.hxx"
#include "Exception.hxx"
#include "Constants.hxx"
#include "JsonSerialization.hxx"
#include "Filesystem.hxx"
#include "LOG.hxx"

#include <iomanip>

Settings::Settings() { readFile(); }

void Settings::readFile()
{
  std::string pathToDataDir;
  std::string pathToSettingsFile = CYTOPIA_DATA_DIR + (std::string)SETTINGS_FILENAME;

  if (!fs::fileExists(pathToSettingsFile))
  { // if there's no custom settings file in data dir, use the provided one
    pathToSettingsFile = fs::getBasePath() + (std::string)CYTOPIA_RESOURCES_DIR + (std::string)SETTINGS_FILENAME;
  }
  std::string jsonFile = fs::readFileAsString(pathToSettingsFile);
  const json _settingsJSONObject = json::parse(jsonFile, nullptr, false);

  // check if json file can be parsed
  if (_settingsJSONObject.is_discarded())
    throw ConfigurationError(TRACE_INFO "Error parsing JSON File " + string{pathToSettingsFile});

  SettingsData data = _settingsJSONObject;
  *this = data;

  // init the actual resolution with the desired resolution
  currentScreenWidth = screenWidth;
  currentScreenHeight = screenHeight;
 LOG(LOG_DEBUG) << "Using settings file from: " << pathToSettingsFile << " with version " << data.settingsVersion;

#ifdef __ANDROID__
  subMenuButtonHeight *= 2;
  subMenuButtonWidth *= 2;
#endif
}

void Settings::writeFile()
{
  const json settingsJsonObject = *this;
  std::string pathToDataDir;
  if (CYTOPIA_DATA_DIR_BASE.empty())
  {
    LOG(LOG_ERROR) << "CYTOPIA_DATA_DIR_BASE is not set! Please report this issue on github. Falling back to cytopia base dir.";
    pathToDataDir = fs::getBasePath() + (std::string)CYTOPIA_RESOURCES_DIR;
  }
  else
  {
    pathToDataDir = CYTOPIA_DATA_DIR;
  }
  std::string pathToSettingsFile = pathToDataDir + (std::string)SETTINGS_FILENAME;
  fs::createDirectory(pathToDataDir);
  fs::writeStringToFile(pathToSettingsFile, settingsJsonObject.dump());
}
