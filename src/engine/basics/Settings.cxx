#include "Settings.hxx"

#include "LOG.hxx"
#include "Exception.hxx"
#include "Constants.hxx"
#include "JsonSerialization.hxx"
#include "Filesystem.hxx"

#include <iomanip>

Settings::Settings() { readFile(); }

void Settings::readFile()
{
  std::string jsonFile = fs::readFileAsString(SETTINGS_FILE_NAME);
  const json _settingsJSONObject = json::parse(jsonFile, nullptr, false);

  // check if json file can be parsed
  if (_settingsJSONObject.is_discarded())
    throw ConfigurationError(TRACE_INFO "Error parsing JSON File " + string{SETTINGS_FILE_NAME});

  SettingsData data = _settingsJSONObject;
  *this = data;

  // init the actual resolution with the desired resolution
  currentScreenWidth = screenWidth;
  currentScreenHeight = screenHeight;
}

void Settings::writeFile()
{
  const json settingsJsonObject = *this;
  fs::writeStringToFile(SETTINGS_FILE_NAME, settingsJsonObject.dump());
}
