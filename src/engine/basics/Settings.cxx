#include "Settings.hxx"

// #include "LOG.hxx"
#include "Exception.hxx"
#include "Constants.hxx"
#include "Filesystem.hxx"
#include "LOG.hxx"
#include <JsonSerialization.hxx>

#include <iomanip>

Settings::Settings() { readFile(); }

void Settings::readFile()
{
  const std::string pathToCachedSettingsFile = CYTOPIA_DATA_DIR + SETTINGS_FILENAME;
  const std::string pathToLocalSettingsFile = fs::getBasePath() + CYTOPIA_RESOURCES_DIR + SETTINGS_FILENAME;

  const json localJsonObject = parseSettingsFile(pathToLocalSettingsFile);
  const json cachedJsonObject = parseSettingsFile(pathToCachedSettingsFile);

  if (localJsonObject.empty() || localJsonObject.is_discarded())
    throw ConfigurationError(TRACE_INFO "Error parsing local JSON File " + string{pathToLocalSettingsFile});

  SettingsData settingsData;

  if (cachedJsonObject.empty() || cachedJsonObject.is_discarded())
  {
    settingsData = localJsonObject;
  }
  else
  {
    int cacheVersion = cachedJsonObject.value("SettingsVersion", -1);
    int localVersion = localJsonObject.value("SettingsVersion", -1);
    if (localVersion <= cacheVersion)
    {
      settingsData = cachedJsonObject;
    }
    else
    {
      LOG(LOG_INFO) << "The settings file version has changed. Overwriting local cached settings file with default settings.";
    }
  }

  *this = settingsData;

  // init the actual resolution with the desired resolution
  currentScreenWidth = screenWidth;
  currentScreenHeight = screenHeight;

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
    pathToDataDir = fs::getBasePath() + CYTOPIA_RESOURCES_DIR;
  }
  else
  {
    pathToDataDir = CYTOPIA_DATA_DIR;
  }
  std::string pathToSettingsFile = pathToDataDir + SETTINGS_FILENAME;
  fs::createDirectory(pathToDataDir);
  fs::writeStringToFile(pathToSettingsFile, settingsJsonObject.dump());
}

json Settings::parseSettingsFile(const std::string &fileName) const
{
  json settingsJSONObject;

  if (fs::fileExists(fileName))
  {
    std::string jsonFile = fs::readFileAsString(fileName);
    settingsJSONObject = json::parse(jsonFile, nullptr, false);
  }

  return settingsJSONObject;
}

void Settings::resetSettingsToDefaults()
{
  const std::string pathToLocalSettingsFile = fs::getBasePath() + CYTOPIA_RESOURCES_DIR + SETTINGS_FILENAME;
  const json localJsonObject = parseSettingsFile(pathToLocalSettingsFile);

  if (localJsonObject.empty() || localJsonObject.is_discarded())
    throw ConfigurationError(TRACE_INFO "Error parsing local JSON File " + string{pathToLocalSettingsFile});

  SettingsData settingsData;
  settingsData = localJsonObject;
  *this = settingsData;
}