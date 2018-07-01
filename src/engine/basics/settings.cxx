#include "settings.hxx"

Settings::Settings() { readINIFile(); }

void Settings::readINIFile()
{
  std::ifstream i(iniFileName);
  if (i.fail())
  {
    LOG(LOG_ERROR) << "File " << iniFileName << " does not exist! Cannot load settings from INI File!";
    // Application should quit here, without settings from the ini file we can't continue
    return;
  }

  // check if json file can be parsed
  _iniFile = json::parse(i, nullptr, false);
  if (_iniFile.is_discarded())
  {
    LOG(LOG_ERROR) << "Error parsing JSON File " << iniFileName;
  }

  settings.screenWidth = _iniFile["Graphics"]["Resolution"]["Width"].get<int>();
  settings.screenHeight = _iniFile["Graphics"]["Resolution"]["Height"].get<int>();
  settings.vSync = _iniFile["Graphics"]["VSYNC"].get<bool>();
  settings.fullScreen = _iniFile["Graphics"]["FullScreen"].get<bool>();
  settings.mapSize = _iniFile["Game"]["MapSize"].get<int>();
  settings.maxElevationHeight = _iniFile["Game"]["MaxElevationHeight"].get<int>();
  settings.uiDataJSONFile = _iniFile["ConfigFiles"]["UIDataJSONFile"].get<std::string>();
  settings.tileDataJSONFile = _iniFile["ConfigFiles"]["TileDataJSONFile"].get<std::string>();
  settings.uiLayoutJSONFile = _iniFile["ConfigFiles"]["UILayoutJSONFile"].get<std::string>();
  settings.playMusic = _iniFile["Audio"]["PlayMusic"].get<bool>();
  settings.playSoundEffects = _iniFile["Audio"]["PlaySoundEffects"].get<bool>();
  settings.audioChannels = _iniFile["Audio"]["AudioChannels"].get<int>();
  settings.musicVolume = _iniFile["Audio"]["MusicVolume"].get<int>();
  settings.soundEffectsVolume = _iniFile["Audio"]["SoundEffectsVolume"].get<int>();
}