#ifndef JSON_SERIALIZATION_HXX_
#define JSON_SERIALIZATION_HXX_

#include <json.hxx>
#include <unordered_map>
#include <array>
#include "point.hxx"
#include "GameObjects/MapNode.hxx"
#include "TerrainGenerator.hxx"
#include "Settings.hxx"
#ifdef USE_AUDIO
#include "../../services/AudioMixer.hxx"
#endif // USE_AUDIO
#include "../../util/ZipRange.hxx"
#include "../../util/Color.hxx"
#include "../../util/Exception.hxx"

using json = nlohmann::json;
using DisplayMap = std::unordered_map<std::string, std::array<int, 2>>;
// ************** DE-SERIALIZER **************

// JSON deserializer for Point class
inline void from_json(const json &j, Point &point)
{
  point.x = j.at("x").get<int>();
  point.y = j.at("y").get<int>();
  point.z = j.at("z").get<int>();
  point.height = j.at("height").get<int>();
}

// JSON deserializer for Point class
inline void from_json(const json &j, MapNodeData &mapNodeData)
{
  mapNodeData.tileID = j.at("tileID").get<std::string>();
  mapNodeData.tileIndex = j.at("tileIndex").get<std::int32_t>();
  mapNodeData.origCornerPoint = j.at("origCornerPoint").get<Point>();
}

// JSON deserializer for Settings struct
inline void from_json(const json &j, SettingsData &s)
{

  s.audio3DStatus = j.value("/Audio/Audio3DStatus"_json_pointer, true);
  s.playMusic = j.value("/Audio/PlayMusic"_json_pointer, true);
  s.playSoundEffects = j.value("/Audio/PlaySoundEffects"_json_pointer, true);
  s.audioChannels = j.value("/Audio/AudioChannels"_json_pointer, 2);
  s.musicVolume = j.value("/Audio/MusicVolume"_json_pointer, static_cast<uint8_t>(100));
  s.soundEffectsVolume = j.value("/Audio/SoundEffectsVolume"_json_pointer, static_cast<uint8_t>(100));

  s.uiDataJSONFile = j.value("/ConfigFiles/UIDataJSONFile"_json_pointer, "resources/data/UIData.json");
  s.tileDataJSONFile = j.value("/ConfigFiles/TileDataJSONFile"_json_pointer, "resources/data/TileData.json");
  s.uiLayoutJSONFile = j.value("/ConfigFiles/UILayoutJSONFile"_json_pointer, "resources/data/UILayout.json");
  s.audioConfigJSONFile = j.value("/ConfigFiles/AudioConfigJSONFile"_json_pointer, "resources/data/AudioConfig.json");

  s.mapSize = j.value("/Game/MapSize"_json_pointer, 64);
  s.biome = j.value("/Game/Biome"_json_pointer, "GrassLands");
  s.maxElevationHeight = j.value("/Game/MaxElevationHeight"_json_pointer, 32);
  s.zoneLayerTransparency = j.value("/Game/ZoneLayerTransperancy"_json_pointer, 0.5f);
  s.showBuildingsInBlueprint = j.value("/Game/ShowBuildingsInBluePrint"_json_pointer, false);
  s.gameLanguage = j.value("/Game/Language"_json_pointer, "en");

  s.fullScreen = j.value("/Graphics/FullScreen"_json_pointer, false);
  s.vSync = j.value("/Graphics/VSYNC"_json_pointer, false);
  std::string defaultMode = j.value("/Graphics/DefaultDisplayMode"_json_pointer, "");
  s.defaultDisplayMode = 0;
  for (auto &[key, values] : j.value("/Graphics/DisplayModes"_json_pointer, DisplayMap()))
  {
    // Merge all display modes
    s.displayModes.push_back(values);
    // Preserve all display mode names
    s.displayModeNames.push_back(key);
    if (defaultMode == key)
    {
      s.defaultDisplayMode = s.displayModeNames.size() - 1;
    }
  }

  s.skipMenu = j.value("/UI/SkipMenu"_json_pointer, false);
  s.newUI = j.value("/UI/NewUI"_json_pointer, false);
  s.buildMenuPosition = j.value("/UI/BuildMenuPosition"_json_pointer, "BOTTOM");
  s.fontFileName = j.value("/UI/FontFilename"_json_pointer, "resources/fonts/arcadeclassics.ttf");
  s.subMenuButtonHeight = j.value("/UI/SubMenuButtonHeight"_json_pointer, 32);
  s.subMenuButtonWidth = j.value("/UI/SubMenuButtonWidth"_json_pointer, 32);
}

// JSON deserializer for Color (Magic Pixel color for buildings)

inline void from_json(const json &j, RGBAColor &color)
{
  std::string value;
  j.get_to(value);
  bool invalid = value.size() != 7;
  invalid &= value.size() != 9;
  if (invalid || value[0] != '#')
  {
    throw ConfigurationError{TRACE_INFO "Invalid color '" + value + "'"};
  }
  value = value.substr(1);
  try
  {
    uint32_t color_raw = std::stoi(value, nullptr, 16);
    if (value.size() == 6)
    {
      color_raw = (color_raw << 8) | 0xFF;
    }
    color = RGBAColor{color_raw};
  }
  catch (std::invalid_argument &e)
  {
    throw ConfigurationError{TRACE_INFO "Invalid color '#" + value + "'"};
  }
}

// JSON deserializer for BiomeData struct (Terrain Gen)
inline void from_json(const json &j, BiomeData &b)
{
  if (j.find("trees") != j.end())
  {
    for (const auto &it : j["trees"].items())
    {
      if (it.key() == "light")
      {
        std::vector<std::string> temp = it.value();
        b.treesLight = temp;
      }
      if (it.key() == "medium")
      {
        std::vector<std::string> temp = it.value();
        b.treesMedium = temp;
      }
      if (it.key() == "dense")
      {
        std::vector<std::string> temp = it.value();
        b.treesDense = temp;
      }
    }
  }

  if (j.find("terrainFlora") != j.end())
  {
    for (const auto &it : j["terrainFlora"].items())
    {
      if (it.key() == "light")
      {
        std::vector<std::string> temp = it.value();
        b.terrainFloraLight = temp;
      }
      if (it.key() == "medium")
      {
        std::vector<std::string> temp = it.value();
        b.terrainFloraMedium = temp;
      }
      if (it.key() == "dense")
      {
        std::vector<std::string> temp = it.value();
        b.terrainFloraDense = temp;
      }
    }
  }

  if (j.find("bushes") != j.end())
  {
    for (const auto &it : j["bushes"].items())
    {
      if (it.key() == "light")
      {
        std::vector<std::string> temp = it.value();
        b.bushesLight = temp;
      }
      if (it.key() == "medium")
      {
        std::vector<std::string> temp = it.value();
        b.bushesMedium = temp;
      }
      if (it.key() == "dense")
      {
        std::vector<std::string> temp = it.value();
        b.bushesDense = temp;
      }
    }
  }

  if (j.find("waterFlora") != j.end())
  {
    for (const auto &it : j["waterFlora"].items())
    {
      if (it.key() == "light")
      {
        std::vector<std::string> temp = it.value();
        b.waterFloraLight = temp;
      }
      if (it.key() == "medium")
      {
        std::vector<std::string> temp = it.value();
        b.waterFloraMedium = temp;
      }
      if (it.key() == "dense")
      {
        std::vector<std::string> temp = it.value();
        b.waterFloraDense = temp;
      }
    }
  }

  if (j.find("terrain") != j.end())
  {
    std::vector<std::string> temp = j["terrain"];
    b.terrain = temp;
  }
  if (j.find("water") != j.end())
  {
    std::vector<std::string> temp = j["water"];
    b.water = temp;
  }
  if (j.find("waterdecoration") != j.end())
  {
    std::vector<std::string> temp = j["waterdecoration"];
    b.waterDecoration = temp;
  }
  if (j.find("terrainRocks") != j.end())
  {
    std::vector<std::string> temp = j["terrainRocks"];
    b.terrainRocks = temp;
  }
  if (j.find("terrainDecoration") != j.end())
  {
    std::vector<std::string> temp = j["terrainDecoration"];
    b.terrainDecoration = temp;
  }
}

#ifdef USE_AUDIO
// JSON deserializer for AudioTrigger
inline void from_json(const json &j, AudioTrigger &trigger) { trigger = AudioTrigger::_from_string(j.get<string>().c_str()); }

// JSON deserializer for SoundtrackConfiguration
inline void from_json(const json &j, AudioConfig::SoundtrackConfiguration &config)
{
  j["path"].get_to(config.stereoFilePath);
  j["monopath"].get_to(config.monoFilePath);
  std::vector<string> triggers;
  j["triggers"].get_to(triggers);
  std::transform(triggers.begin(), triggers.end(), std::back_inserter(config.triggers),
                 [](const string &trigger) { return AudioTrigger::_from_string(trigger.c_str()); });
}

// JSON deserializer for AudioConfig
inline void from_json(const json &j, AudioConfig &config)
{
  j["Music"].get_to(config.Music);
  j["Sound"].get_to(config.Sound);
}
#endif // USE_AUDIO

// ************** SERIALIZER **************

// JSON serializer for Point class
inline void to_json(json &j, const Point &point)
{
  j = json{{"x", point.x}, {"y", point.y}, {"z", point.z}, {"height", point.height}};
}

// JSON serializer for MapNodeData struct
inline void to_json(json &j, const MapNodeData &mapNodeData)
{
  j = json{
      {"tileID", mapNodeData.tileID}, {"tileIndex", mapNodeData.tileIndex}, {"origCornerPoint", mapNodeData.origCornerPoint}};
}

// JSON serializer for MapNode class
inline void to_json(json &j, const MapNode &m)
{
  j = json{{"coordinates", m.getCoordinates()}, {"mapNodeData", m.getMapNodeData()}};
}

// JSON serializer for Settings struct
inline void to_json(json &j, const SettingsData &s)
{
  j = json();
  j["Audio"] = json();
  j["/Audio/Audio3DStatus"_json_pointer] = s.audio3DStatus;
  j["/Audio/PlayMusic"_json_pointer] = s.playMusic;
  j["/Audio/PlaySoundEffects"_json_pointer] = s.playSoundEffects;
  j["/Audio/AudioChannels"_json_pointer] = s.audioChannels;
  j["/Audio/MusicVolume"_json_pointer] = s.musicVolume.get();
  j["/Audio/SoundEffectsVolume"_json_pointer] = s.soundEffectsVolume.get();

  j["ConfigFiles"] = json();
  j["/ConfigFiles/UIDataJSONFile"_json_pointer] = s.uiDataJSONFile.get();
  j["/ConfigFiles/TileDataJSONFile"_json_pointer] = s.tileDataJSONFile.get();
  j["/ConfigFiles/UILayoutJSONFile"_json_pointer] = s.uiLayoutJSONFile.get();
  j["/ConfigFiles/AudioConfigJSONFile"_json_pointer] = s.audioConfigJSONFile.get();

  j["Game"] = json();
  j["/Game/MapSize"_json_pointer] = s.mapSize;
  j["/Game/Biome"_json_pointer] = s.biome;
  j["/Game/MaxElevationHeight"_json_pointer] = s.maxElevationHeight;
  j["/Game/ZoneLayerTransperancy"_json_pointer] = s.zoneLayerTransparency;
  j["/Game/ShowBuildingsInBluePrint"_json_pointer] = s.showBuildingsInBlueprint;
  j["/Game/Language"_json_pointer] = s.gameLanguage;

  j["Graphics"] = json();
  j["/Graphics/FullScreen"_json_pointer] = s.fullScreen;
  j["/Graphics/VSYNC"_json_pointer] = s.vSync;
  j["/Graphics/DefaultDisplayMode"_json_pointer] = s.displayModeNames.at(s.defaultDisplayMode);
  auto &modes = j["/Graphics/DisplayModes"_json_pointer] = json();
  for (const auto &&[key, value] : ZipRange{s.displayModeNames, s.displayModes})
  {
    modes[key] = value;
  }

  j["UI"] = json();
  j["/UI/SkipMenu"_json_pointer] = s.skipMenu;
  j["/UI/NewUI"_json_pointer] = s.newUI;
  j["/UI/BuildMenuPosition"_json_pointer] = s.buildMenuPosition;
  j["/UI/FontFilename"_json_pointer] = s.fontFileName.get();
  j["/UI/SubMenuButtonHeight"_json_pointer] = s.subMenuButtonHeight;
  j["/UI/SubMenuButtonWidth"_json_pointer] = s.subMenuButtonWidth;
}

#endif
