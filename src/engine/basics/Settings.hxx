#ifndef SETTINGS_HXX_
#define SETTINGS_HXX_

#include <string>

#include "../../util/Singleton.hxx"

/* Settings Types */
using ScreenDimension = int;
using VolumeLevel = uint8_t;

struct SettingsData
{

  /**
   * @todo document what this field is
   * @todo add a typename
   */
  int mapSize;

  /**
   * @brief the screen width
   * @pre only apply for windowed or fullscreen mode
   */
  ScreenDimension screenWidth;

  /** 
   * @brief the screen height
   * @pre only apply for windowed or fullscreen mode
   */
  ScreenDimension screenHeight;

  /** 
   * @brief the actual screen width (can differ from the one that's set in borderless fullscreen)
   */
  ScreenDimension currentScreenWidth;

  /**
   * @brief the actual screen height (can differ from the one that's set in borderless fullscreen)
   */
  ScreenDimension currentScreenHeight;

  /**
   * @todo document what this field is
   * @todo add a typename
   */
  int maxElevationHeight;

  /**
   * @todo document what this field is
   * @todo add a typename
   */
  bool vSync;

  /**
   * @todo document what this field is
   * @todo add a typename
   */
  bool fullScreen;

  /**
   * @todo document what this field is
   * @todo add a typename
   */
  int fullScreenMode;

  /**
   * @brief the volume of music
   */
  VolumeLevel musicVolume;

  /**
   * @brief the volume of sound effects
   */
  VolumeLevel soundEffectsVolume;

  /**
   * @todo document what this field is
   * @todo add a typename
   */
  bool playMusic;

  /**
   * @todo document what this field is
   * @todo add a typename
   */
  bool playSoundEffects;

  /**
   * @todo document what this field is
   * @todo add a typename
   */
  int audioChannels;

  /**
   * @todo document what this field is
   * @todo add a typename
   * @todo replace by enum when BetterEnums is implemented
   */
  std::string buildMenuPosition;

  /**
   * @todo document what this field is
   * @todo add a typename
   */
  std::string uiDataJSONFile;

  /**
   * @todo document what this field is
   * @todo add a typename
   */
  std::string tileDataJSONFile;

  /**
   * @todo document what this field is
   * @todo add a typename
   */
  std::string uiLayoutJSONFile;
};

/**
 * @class Settings
 * @brief the settings of the client
 */
class Settings : public SettingsData, public Singleton<Settings>
{
public:

  friend Singleton<Settings>;

  /**
   * @brief Load settings from file
   */
  void readFile();

  /**
   * @brief Save settings to file
   */
  void writeFile();

  using SettingsData::operator=;

private:
  Settings();
  ~Settings() = default;
};

#endif
