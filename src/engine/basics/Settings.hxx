#ifndef SETTINGS_HXX_
#define SETTINGS_HXX_

#include <string>
#include <vector>

#include "../../util/Singleton.hxx"
#include "../../util/Meta.hxx"

using std::string;

#include <cstddef>

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
   * @todo  must be removed when maps will be 'infinite'
   */
  int mapSize;

  /**
   * @brief the maximum elevation height
   */
  int maxElevationHeight;

  /**
  * @brief the value of the zone layer transparency, (0 - 1.0).
  * where 0 is full opaque and 1 for full transparency.
  */
  float zoneLayerTransparency;

  /**
   * @brief True if VSYNC is enabled
   */
  bool vSync;

  /**
   * @brief True if the game must be in fullscreen mode
   */
  bool fullScreen;

  /**
   * @brief The volume of music between [0, 100]
   */
  VolumeLevel musicVolume;

  /**
   * @brief the volume of sound effects between [0, 100]
   */
  VolumeLevel soundEffectsVolume;

  /**
   * @brief True if music should be played
   */
  bool playMusic;

  /**
   * @brief True if sound effects should be played
   */
  bool playSoundEffects;

  /**
   * @brief the number of channels used for sound playback
   * 0=Mute, 1=Mono, 2=Stereo
   */
  uint8_t audioChannels;

  /**
   * @brief Whether to play 3D sound or not
   */
  bool audio3DStatus;
  
  /**
   * @brief Whether to use the new UI or not
   */
  bool newUI;
  
  /**
   * @brief Whether to start the game immediately at startup
   */
  bool skipMenu;

  /**
   * @todo document what this field is
   * @todo add a typename
   * @todo replace by enum when BetterEnums is implemented
   */
  std::string buildMenuPosition;

  /**
   * @brief The biome type
   * @todo  Remove this when we have support for Game saves
   *        and NewGameActivity
   */
  std::string biome;

  /**
   * @brief JSONFile that contains uiData
   */
  FilePath uiDataJSONFile;

  /**
   * @brief JSONFile that contains tileData
   */
  FilePath tileDataJSONFile;

  /**
   * @todo document what this field is
   */
  FilePath uiLayoutJSONFile;

  /**
   * The file path to Audio Configuration
   */
  FilePath audioConfigJSONFile;

  /**
   * The file path to Audio Configuration 3D
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

  /**
   * @brief Indicates whether we want to see buildings inside Blueprint layer or not.
   */
  bool showBuildingsInBlueprint;

  /**
   * @brief   All display modes 
   * @details Nth element is the (width, height) of the
   *          Nth displayModeName
   */
  std::vector<std::array<int, 2>> displayModes;
  
  /**
   * @details All the names of the display modes
   *          to be used in the UI
   */
  std::vector<std::string> displayModeNames;

  /**
   * @brief   The current default display mode
   * @details On game startup, is used if fullscreen is disabled
   */
  int defaultDisplayMode;
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

  int getDefaultWindowWidth() const noexcept;
  
  int getDefaultWindowHeight() const noexcept;
  
  /**
   * @brief Parses command line arguments to override settings
   */
  void parse_args(int argc, char** argv);

  using SettingsData::operator=;

private:
  Settings();
  ~Settings();
};

#endif
