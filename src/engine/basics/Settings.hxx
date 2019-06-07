#ifndef SETTINGS_HXX_
#define SETTINGS_HXX_

#include <string>

class Settings
{
public:
  /// Retrieves instance of Singleton class Settings
  static Settings &instance()
  {
    static Settings settings;
    return settings;
  }

  Settings();
  ~Settings() = default;

  void readFile();
  void writeFile();

  typedef struct
  {
    int mapSize;
    int screenWidth;         // the screen width that's been set. Does only apply for windowed / fullscreen mode
    int screenHeight;        // the screen height that's been set. Does only apply for windowed / fullscreen mode
    int currentScreenWidth;  // the actual screen width (can differ from the one that's set in borderless fullscreen)
    int currentScreenHeight; // the actual screen height (can differ from the one that's set in borderless fullscreen)
    int maxElevationHeight;
    bool vSync;
    bool fullScreen;
    int fullScreenMode;
    uint8_t musicVolume;
    uint8_t soundEffectsVolume;
    bool playMusic;
    bool playSoundEffects;
    int audioChannels;
    std::string buildMenuPosition; // TODO: Replace by enum when BetterEnums is implemented
    std::string uiDataJSONFile;
    std::string tileDataJSONFile;
    std::string uiLayoutJSONFile;
  } SettingsStruct;

  SettingsStruct settings;
};

#endif
