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

  settings = _settingsJSONObject;
}

void Settings::writeFile()
{
  json _settingsJSONObject = settings;

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

// JSON serializer for Settings struct
void to_json(json &j, const Settings::SettingsStruct &s)
{
  j = {
      {std::string("Graphics"),
       {
           {std::string("VSYNC"), s.vSync},
           {std::string("FullScreen"), s.fullScreen},
           {std::string("Resolution"),
            {{std::string("Screen_Width"), s.screenWidth}, {std::string("Screen_Height"), s.screenHeight}}},
       }},
      {std::string("Game"), {{std::string("MapSize"), s.mapSize}, {std::string("MaxElevationHeight"), s.maxElevationHeight}}},
      {std::string("ConfigFiles"),
       {{std::string("UIDataJSONFile"), s.uiDataJSONFile},
        {std::string("TileDataJSONFile"), s.tileDataJSONFile},
        {std::string("UILayoutJSONFile"), s.uiLayoutJSONFile}}},
      {std::string("Audio"),
       {
           {std::string("PlayMusic"), s.playMusic},
           {std::string("PlaySoundEffects"), s.playSoundEffects},
           {std::string("AudioChannels"), s.audioChannels},
           {std::string("MusicVolume"), s.musicVolume},
           {std::string("SoundEffectsVolume"), s.soundEffectsVolume},
       }},

  };
}

// JSON deserializer for Settings struct
void from_json(const json &j, Settings::SettingsStruct &s)
{
  s.screenWidth = j["Graphics"]["Resolution"].value("Screen_Width", 800);
  s.screenHeight = j["Graphics"]["Resolution"].value("Screen_Height", 600);
  s.vSync = j["Graphics"].value("VSYNC", false);
  s.fullScreen = j["Graphics"].value("FullScreen", false);
  s.mapSize = j["Game"].value("MapSize", 64);
  s.maxElevationHeight = j["Game"].value("MaxElevationHeight", 32);
  s.uiDataJSONFile = j["ConfigFiles"].value("UIDataJSONFile", "resources/data/TileData.json");
  s.tileDataJSONFile = j["ConfigFiles"].value("TileDataJSONFile", "resources/data/UIData.json");
  s.uiLayoutJSONFile = j["ConfigFiles"].value("UILayoutJSONFile", "resources/data/UILayout.json");
  s.playMusic = j["Audio"].value("PlayMusic", true);
  s.playSoundEffects = j["Audio"].value("PlaySoundEffects", false);
  s.audioChannels = j["Audio"].value("AudioChannels", 2);
  s.musicVolume = j["Audio"].value("MusicVolume", 50);
  s.soundEffectsVolume = j["Audio"].value("SoundEffectsVolume", 100);
}
