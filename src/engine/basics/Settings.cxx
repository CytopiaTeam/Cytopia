#include "Settings.hxx"

#include "LOG.hxx"
#include "Constants.hxx"
#include "JsonSerialization.hxx"

#include <iomanip>

Settings::Settings() { readFile(); }

void Settings::readFile()
{
  std::string settingsFileName = SDL_GetBasePath();
  settingsFileName.append(SETTINGS_FILE_NAME);
  std::ifstream i(settingsFileName);

  if (i.fail())
  {
    LOG(LOG_ERROR) << "File " << SETTINGS_FILE_NAME << " does not exist! Cannot load settings from INI File!";
    // Application should quit here, without settings from the ini file we can't continue
    return;
  }

  // check if json file can be parsed
  const json _settingsJSONObject = json::parse(i, nullptr, false);
  if (_settingsJSONObject.is_discarded())
  {
    LOG(LOG_ERROR) << "Error parsing JSON File " << SETTINGS_FILE_NAME;
  }

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

  if (settingsFile.is_open())
  {
    settingsFile << std::setw(4) << settingsJsonObject << std::endl;
    settingsFile.close();
  }
  else
  {
    LOG(LOG_ERROR) << "Could not write file " << SETTINGS_FILE_NAME;
  }
}
