#ifndef SETTINGS_HXX_
#define SETTINGS_HXX_

#include <string>

#include "../../util/Singleton.hxx"
#include "../../util/Meta.hxx"

using std::string;

#include <stddef.h>

/* Settings Types */
using ScreenDimension = int;
using VolumeLevel = StrongType<uint8_t, struct VolumeLevelTag>;
using FilePath = StrongType<string, struct FilePathTag>;

/* @todo This and Settings must be refactored into a TransitiveModel class
 * @todo This TransitiveModel must be subscribed to by the Settings iView
 * @todo We must create a ServiceController class with setters and Events for each field
 */
struct SettingsData
{

  /**
   * @brief the size of the map
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
   * @brief the maximum elevation height
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
   * @brief the 
   * 1=Mono,2=Stereo
   */
  int audioChannels;
  
  /**
   * @brief Whether to play 3D sound or not
   */
  bool audio3DStatus;
  
  /**
   * @todo document what this field is
   * @todo add a typename
   * @todo replace by enum when BetterEnums is implemented
   */
  std::string buildMenuPosition;

  /**
   * @brief JSONFile that contains uiData
   * @todo add a typename
   */
  FilePath uiDataJSONFile;

  /**
   * @brief JSONFile that contains tileData
   * @todo add a typename
   */
  FilePath tileDataJSONFile;

  /**
   * @todo document what this field is
   * @todo add a typename
   */
  FilePath uiLayoutJSONFile;

  /**
   * The file path to Audio Configuration
   * @todo add a typename
   */
  FilePath audioConfigJSONFile;
  
  /**
   * The file path to Audio Configuration 3D
   * @todo add a typename
   */
  FilePath audioConfig3DJSONFile;

  /**
   * @brief The Game language 
   */
  std::string gameLanguage;

  /**
   * @brief FilePath of the Font that should be used.
   */
  FilePath fontFileName;

  /**
   * @brief The width in pixels of the buttons used in the 
   * build sub menues on the UI
   */
  int subMenuButtonWidth;

  /**
   * @brief The height in pixels of the buttons used in the 
   * build sub menues on the UI
   */
  int subMenuButtonHeight;

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
