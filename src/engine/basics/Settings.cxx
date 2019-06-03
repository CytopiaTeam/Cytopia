#include "Settings.hxx"

#include "LOG.hxx"
#include "../common/Constants.hxx"

#include "../../ThirdParty/json.hxx"

Settings::Settings() { readFile(); }
using json = nlohmann::json;

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

  settings = _settingsJSONObject;

  // init the actual resolution with the desired resolution
  settings.currentScreenWidth = settings.screenWidth;
  settings.currentScreenHeight = settings.screenHeight;
}

void Settings::writeFile()
{
  const json settingsJsonObject = settings;

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

// JSON serializer for Settings struct
void to_json(json &j, const Settings::SettingsStruct &s)
{
  j = {
      {std::string("Graphics"),
       {
           {std::string("VSYNC"), s.vSync},
           {std::string("FullScreen"), s.fullScreen},
           {std::string("FullScreenMode"), s.fullScreenMode},
           {std::string("Resolution"),
            {{std::string("Screen_Width"), s.screenWidth}, {std::string("Screen_Height"), s.screenHeight}}},
       }},
      {std::string("Game"), {{std::string("MapSize"), s.mapSize}, {std::string("MaxElevationHeight"), s.maxElevationHeight}}},
      {std::string("User Interface"), {{std::string("BuildMenu Position"), s.buildMenuPosition}}},
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
  s.fullScreenMode = j["Graphics"].value("FullScreenMode", 0);
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
  s.buildMenuPosition = j["User Interface"].value("BuildMenu Position", "BOTTOM");
}
