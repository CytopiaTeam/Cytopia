#include "settings.hxx"
#include "log.hxx"
#include "../common/Constants.hxx"
#include "../../ThirdParty/json.hxx"

Settings::Settings() { readFile(); }
using json = nlohmann::json;

void Settings::readFile()
{
  json _settingsJSONObject;
  std::ifstream i(SETTINGS_FILE_NAME);
  if (i.fail())
  {
    LOG(LOG_ERROR) << "File " << SETTINGS_FILE_NAME << " does not exist! Cannot load settings from INI File!";
    // Application should quit here, without settings from the ini file we can't continue
    return;
  }

  // check if json file can be parsed
  _settingsJSONObject = json::parse(i, nullptr, false);
  if (_settingsJSONObject.is_discarded())
  {
    LOG(LOG_ERROR) << "Error parsing JSON File " << SETTINGS_FILE_NAME;
  }

  settings.screenWidth = _settingsJSONObject["Graphics"]["Resolution"]["Width"].get<int>();
  settings.screenHeight = _settingsJSONObject["Graphics"]["Resolution"]["Height"].get<int>();
  settings.vSync = _settingsJSONObject["Graphics"]["VSYNC"].get<bool>();
  settings.fullScreen = _settingsJSONObject["Graphics"]["FullScreen"].get<bool>();
  settings.mapSize = _settingsJSONObject["Game"]["MapSize"].get<int>();
  settings.maxElevationHeight = _settingsJSONObject["Game"]["MaxElevationHeight"].get<int>();
  settings.uiDataJSONFile = _settingsJSONObject["ConfigFiles"]["UIDataJSONFile"].get<std::string>();
  settings.tileDataJSONFile = _settingsJSONObject["ConfigFiles"]["TileDataJSONFile"].get<std::string>();
  settings.uiLayoutJSONFile = _settingsJSONObject["ConfigFiles"]["UILayoutJSONFile"].get<std::string>();
  settings.playMusic = _settingsJSONObject["Audio"]["PlayMusic"].get<bool>();
  settings.playSoundEffects = _settingsJSONObject["Audio"]["PlaySoundEffects"].get<bool>();
  settings.audioChannels = _settingsJSONObject["Audio"]["AudioChannels"].get<int>();
  settings.musicVolume = _settingsJSONObject["Audio"]["MusicVolume"].get<int>();
  settings.soundEffectsVolume = _settingsJSONObject["Audio"]["SoundEffectsVolume"].get<int>();
}

void Settings::writeFile()
{
  json _settingsJSONObject;
  _settingsJSONObject["Graphics"]["Resolution"]["Width"] = settings.screenWidth;
  _settingsJSONObject["Graphics"]["Resolution"]["Height"] = settings.screenHeight;
  _settingsJSONObject["Graphics"]["VSYNC"] = settings.vSync;
  _settingsJSONObject["Graphics"]["FullScreen"] = settings.fullScreen;
  _settingsJSONObject["Game"]["MapSize"] = settings.mapSize;
  _settingsJSONObject["Game"]["MaxElevationHeight"] = settings.maxElevationHeight;
  _settingsJSONObject["ConfigFiles"]["UIDataJSONFile"] = settings.uiDataJSONFile;
  _settingsJSONObject["ConfigFiles"]["TileDataJSONFile"] = settings.tileDataJSONFile;
  _settingsJSONObject["ConfigFiles"]["UILayoutJSONFile"] = settings.uiLayoutJSONFile;
  _settingsJSONObject["Audio"]["PlayMusic"] = settings.playMusic;
  _settingsJSONObject["Audio"]["PlaySoundEffects"] = settings.playSoundEffects;
  _settingsJSONObject["Audio"]["AudioChannels"] = settings.audioChannels;
  _settingsJSONObject["Audio"]["MusicVolume"] = settings.musicVolume;
  _settingsJSONObject["Audio"]["SoundEffectsVolume"] = settings.soundEffectsVolume;

  std::ofstream settingsFile(SETTINGS_FILE_NAME);

  if (settingsFile.is_open())
  {
    settingsFile << std::setw(4) << _settingsJSONObject << std::endl;
    settingsFile.close();
  }
  else
  {
    LOG(LOG_ERROR) << "Could not write file " << SETTINGS_FILE_NAME;
  }
}