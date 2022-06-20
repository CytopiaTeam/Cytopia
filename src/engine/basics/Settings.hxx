#ifndef SETTINGS_HXX_
#define SETTINGS_HXX_

#include <string>
#include <cstddef>

#include "../../util/Singleton.hxx"
#include "../../util/Meta.hxx"
#include <json.hxx>

using std::string;
using json = nlohmann::json;

/* Settings Types */
using ScreenDimension = int;
using FilePath = StrongType<string, struct FilePathTag>;

/**
 * @todo This and Settings must be refactored into a TransitiveModel class
 * @todo This TransitiveModel must be subscribed to by the Settings iView
 * @todo We must create a ServiceController class with setters and Events for each field
 */
struct SettingsData
{
  /**
   * @brief the version of the Settings file. Overwrite cache settings if a newer version exists
   */
  int settingsVersion;

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

  /// the actual screen width (can differ from the one that's set in borderless fullscreen)
  ScreenDimension currentScreenWidth;

  /// the actual screen height (can differ from the one that's set in borderless fullscreen)
  ScreenDimension currentScreenHeight;

  /**
   * @brief the maximum elevation height
   * @todo add a typename
   */
  int maxElevationHeight;

  /**
  * @brief the value of the zone layer transparency, (0 - 1.0).
  * where 0 is full opaque and 1 for full transparency.
  */
  float zoneLayerTransparency;

  /**
   * @brief if vSync is enabled or not
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

  /// the volume of music as float between [0, 1]
  float musicVolume;

  /// the volume of sound effects as float between [0, 1]
  float soundEffectsVolume;

  /// true if music should be played
  bool playMusic;

  /// true if sound effects should be played
  bool playSoundEffects;

  /**
   * @brief the number of channels used for sound playback
   * 1=Mono,2=Stereo
   */
  int audioChannels;

  /// whether to play 3D sound or not
  bool audio3DStatus;

  /**
   * @brief location of the build menu
   * @details Acceptable values are "LEFT", "RIGHT", "TOP", and "BOTTOM"
   * @todo add a typename
   * @todo replace by enum when BetterEnums is implemented
   */
  std::string buildMenuPosition;

  /**
   * @brief this is used for biomedata
   * @todo Remove this later when terraingen is using biomes
   * @todo replace by enum when BetterEnums is implemented
   */
  std::string biome;

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
   * @brief The file path to the UI layout file
   * @todo add a typename
   */
  FilePath uiLayoutJSONFile;

  /// file path to Audio Configuration
  FilePath audioConfigJSONFile;

  /// the file path to Audio Configuration 3D
  FilePath audioConfig3DJSONFile;

  /// the code for the current game language
  std::string gameLanguage;

  /// FilePath of the Font that should be used
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

  uint32_t defaultFontSize;

  /// indicates whether we want to see buildings inside Blueprint layer or not
  bool showBuildingsInBlueprint;

  /// Write errors to a log file
  bool writeErrorLogFile;
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
   * @throws ConfigurationError when loading configuration results in an error 
   */
  void readFile();

  /// Save settings to file
  void writeFile();

  /**
   * @brief Reset settings to defaults from local settings file
   * @throws ConfigurationError when loading configuration results in an error 
   */
  void resetSettingsToDefaults();

  using SettingsData::operator=;

private:
  Settings();
  ~Settings() = default;

  json parseSettingsFile(const std::string &fileName) const;
};

#endif
