#ifndef SETTINGS_HXX_
#define SETTINGS_HXX_

#include <string>

class Settings
{
public:
  /// Retrieves Instance of Singleton class Texture Manager
  static Settings &Instance()
  {
    static Settings instance;
    return instance;
  }

  Settings();
  ~Settings() = default;

  void readFile();
  void writeFile();

  typedef struct
  {
    int mapSize;
    int screenWidth;
    int screenHeight;
    int maxElevationHeight;
    bool vSync;
    bool fullScreen;
    int musicVolume;
    int soundEffectsVolume;
    bool playMusic;
    bool playSoundEffects;
    int audioChannels;
    std::string uiDataJSONFile;
    std::string tileDataJSONFile;
    std::string uiLayoutJSONFile;
  } SettingsStruct;

  SettingsStruct settings;

private:
  std::string settingsFileName = "resources/settings.json";
};

#endif