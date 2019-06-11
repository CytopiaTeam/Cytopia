#ifndef JSON_SERIALIZATION_HXX_
#define JSON_SERIALIZATION_HXX_

#include "json.hxx"
#include "point.hxx"
#include "GameObjects/MapNode.hxx"
#include "Settings.hxx"

using json = nlohmann::json;

// ************** DE-SERIALIZER **************

// JSON deserializer for Point class
void from_json(const json &j, Point &point)
{
  point.x = j.at("x").get<int>();
  point.y = j.at("y").get<int>();
  point.z = j.at("z").get<int>();
  point.height = j.at("height").get<int>();
}

// JSON deserializer for Point class
void from_json(const json &j, MapNodeData &mapNodeData) { mapNodeData.tileID = j.at("tileID").get<std::string>(); }

// JSON deserializer for Settings struct
void from_json(const json &j, SettingsData &s)
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
  s.musicVolume = j["Audio"].value("MusicVolume", static_cast<uint8_t>(50));
  s.soundEffectsVolume = j["Audio"].value("SoundEffectsVolume", static_cast<uint8_t>(100));
  s.buildMenuPosition = j["User Interface"].value("BuildMenu Position", "BOTTOM");
}

// ************** SERIALIZER **************

// JSON serializer for Point class
void to_json(json &j, const Point &point) { j = json{{"x", point.x}, {"y", point.y}, {"z", point.z}, {"height", point.height}}; }

// JSON serializer for MapNodeData struct
void to_json(json &j, const MapNodeData &mapNodeData) { j = json{{"tileID", mapNodeData.tileID}}; }

// JSON serializer for MapNode class
void to_json(json &j, const std::unique_ptr<MapNode> &m)
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
void to_json(json &j, const SettingsData &s)
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

#endif
