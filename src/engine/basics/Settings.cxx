#include "Settings.hxx"

#include "LOG.hxx"
#include "Exception.hxx"
#include "Constants.hxx"
#include "JsonSerialization.hxx"

#include <iomanip>

Settings::Settings() { readFile(); }

void Settings::readFile()
{
  std::string jsonFile = FileSystem::readFileAsString(SETTINGS_FILE_NAME);
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

  std::string settingsFileName = SDL_GetBasePath();
  settingsFileName += SETTINGS_FILE_NAME;
  std::ofstream settingsFile(settingsFileName);

  if (!settingsFile)
    throw ConfigurationError(TRACE_INFO "Could not write to file " + settingsFileName);

  settingsFile << std::setw(4) << settingsJsonObject << std::endl;
  settingsFile.close();
}
