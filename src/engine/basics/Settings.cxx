#include "Settings.hxx"

#include "LOG.hxx"
#include "Exception.hxx"
#include "Constants.hxx"
#include "JsonSerialization.hxx"

#include <iomanip>

Settings::Settings() { readFile(); }

void Settings::readFile()
{
  std::string settingsFileName = SDL_GetBasePath();
  settingsFileName.append(SETTINGS_FILE_NAME);
  std::ifstream i(settingsFileName);

  if (!i)
    throw ConfigurationError(TRACE_INFO "Can't open file " + string{SETTINGS_FILE_NAME});

  // check if json file can be parsed
  const json _settingsJSONObject = json::parse(i, nullptr, false);
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
  settingsFileName.append(SETTINGS_FILE_NAME);
  std::ofstream settingsFile(settingsFileName);

  if (!settingsFile)
    throw ConfigurationError(TRACE_INFO "Could not write to file " + string{SETTINGS_FILE_NAME});

  settingsFile << std::setw(4) << settingsJsonObject << std::endl;
  settingsFile.close();
}
