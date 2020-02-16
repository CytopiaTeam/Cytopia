#ifndef JSON_SERIALIZATION_HXX_
#define JSON_SERIALIZATION_HXX_

#include "json.hxx"
#include "point.hxx"
#include "GameObjects/MapNode.hxx"
#include "TerrainGenerator.hxx"
#include "Settings.hxx"
#ifdef USE_AUDIO
#include "../../services/AudioMixer.hxx"
#endif // USE_AUDIO

using json = nlohmann::json;

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
inline void from_json(const json &j, MapNodeData &mapNodeData) { mapNodeData.tileID = j.at("tileID").get<std::string>(); }

// JSON deserializer for Settings struct
inline void from_json(const json &j, SettingsData &s)
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
  s.audioConfigJSONFile = j["ConfigFiles"].value("AudioConfigJSONFile", "resources/data/AudioConfig.json");
  s.audio3DStatus = j["Audio"].value("Audio3DStatus", true);
  s.playMusic = j["Audio"].value("PlayMusic", true);
  s.playSoundEffects = j["Audio"].value("PlaySoundEffects", false);
  s.audioChannels = j["Audio"].value("AudioChannels", 2);
  s.musicVolume = j["Audio"].value("MusicVolume", static_cast<uint8_t>(50));
  s.soundEffectsVolume = j["Audio"].value("SoundEffectsVolume", static_cast<uint8_t>(100));
  s.buildMenuPosition = j["User Interface"].value("BuildMenu Position", "BOTTOM");
  s.gameLanguage = j["User Interface"].value("Language", "en");
  s.fontFileName = j["User Interface"].value("Font Filename", "en");
  s.subMenuButtonWidth = j["User Interface"].value("SubMenuButtonWidth", 32);
  s.subMenuButtonHeight = j["User Interface"].value("SubMenuButtonHeight", 32);
}

// JSON deserializer for BiomeData struct (Terrain Gen)
inline void from_json(const json &j, BiomeData &b)
{
  if (j.find("trees") != j.end())
  {
    for (const auto &it : j["trees"].items())
    {
      if (it.key() == "small")
      {
        std::vector<std::string> temp = it.value();
        b.treesSmall = temp;
      }
      if (it.key() == "normal")
      {
        std::vector<std::string> temp = it.value();
        b.treesNormal = temp;
      }
      if (it.key() == "dense")
      {
        std::vector<std::string> temp = it.value();
        b.treesDense = temp;
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
  if (j.find("terrainFlowers") != j.end())
  {
    std::vector<std::string> temp = j["terrainFlowers"];
    b.terrainFlowers = temp;
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
inline void to_json(json &j, const MapNodeData &mapNodeData) { j = json{{"tileID", mapNodeData.tileID}}; }

// JSON serializer for MapNode class
inline void to_json(json &j, const std::unique_ptr<MapNode> &m)
{
  if (m.get())
  {
    j = json{{"coordinates", m->getCoordinates()}, {"mapNodeData", m->getMapNodeData()}};
  }
  else
  {
    j = nullptr;
  }
}

// JSON serializer for Settings struct
inline void to_json(json &j, const SettingsData &s)
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
      {std::string("User Interface"),
       {{std::string("BuildMenu Position"), s.buildMenuPosition}, {std::string("Language"), s.gameLanguage}}},
      {std::string("ConfigFiles"),
       {{std::string("UIDataJSONFile"), s.uiDataJSONFile.get()},
        {std::string("TileDataJSONFile"), s.tileDataJSONFile.get()},
        {std::string("UILayoutJSONFile"), s.uiLayoutJSONFile.get()},
        {std::string("AudioConfigJSONFile"), s.audioConfigJSONFile.get()}}},
      {std::string("Audio"),
       {
           {std::string("Audio3DStatus"), s.audio3DStatus},
           {std::string("PlayMusic"), s.playMusic},
           {std::string("PlaySoundEffects"), s.playSoundEffects},
           {std::string("AudioChannels"), s.audioChannels},
           {std::string("MusicVolume"), s.musicVolume.get()},
           {std::string("SoundEffectsVolume"), s.soundEffectsVolume.get()},
       }},

  };
}

#endif
