#include "resources.hxx"
#include "../engine.hxx"

// Instantiate static variables
SDL_Renderer* Resources::_renderer = nullptr;
SDL_Window* Resources::_window = nullptr;
float Resources::_zoomLevel = 1.0;
Point Resources::_cameraOffset;
const int Resources::_tileSize = 32;
int Resources::_terrainEditMode = Resources::NO_TERRAIN_EDIT;
json Resources::_json;
json Resources::_iniFile;
json Resources::_uiTextureFile;
Resources::Settings Resources::settings;
bool Resources::editMode = false;


/** Enum (bitmask) for mapping neighbor tile positions
* [ T B L R TL TR BL BR ]
* [ 0 0 0 0  0  0  0  0 ]

* Our neighbor tiles look like this
* 2 5 8
* 1 X 7
* 0 3 6
*/
std::unordered_map<unsigned int, int> Resources::slopeTileIDMap = 
{
  { NO_NEIGHBORS, 14 },
  { ELEVATED_TOP, 3 },
  { ELEVATED_BOTTOM, 5 },
  { ELEVATED_RIGHT, 1 },
  { ELEVATED_LEFT, 7 },
  { ELEVATED_BOTTOM_RIGHT, 2 },
  { ELEVATED_BOTTOM_LEFT, 8 },
  { ELEVATED_TOP_RIGHT, 0 },
  { ELEVATED_TOP_LEFT, 6 },

  { ELEVATED_TOP | ELEVATED_TOP_LEFT, 3 },
  { ELEVATED_TOP | ELEVATED_TOP_RIGHT, 3 },
  { ELEVATED_TOP | ELEVATED_TOP_RIGHT | ELEVATED_TOP_LEFT, 3 },
  { ELEVATED_TOP_LEFT | ELEVATED_TOP_RIGHT, 3 },

  { ELEVATED_BOTTOM | ELEVATED_BOTTOM_LEFT, 5 },
  { ELEVATED_BOTTOM | ELEVATED_BOTTOM_RIGHT, 5 },
  { ELEVATED_BOTTOM | ELEVATED_BOTTOM_RIGHT | ELEVATED_BOTTOM_LEFT, 5 },
  { ELEVATED_BOTTOM_LEFT | ELEVATED_BOTTOM_RIGHT, 5 },

  { ELEVATED_LEFT | ELEVATED_BOTTOM_LEFT, 7 },
  { ELEVATED_LEFT | ELEVATED_TOP_LEFT, 7 },
  { ELEVATED_LEFT | ELEVATED_TOP_LEFT | ELEVATED_BOTTOM_LEFT, 7 },
  { ELEVATED_TOP_LEFT | ELEVATED_BOTTOM_LEFT, 7 },

  { ELEVATED_RIGHT | ELEVATED_BOTTOM_RIGHT, 1 },
  { ELEVATED_RIGHT | ELEVATED_TOP_RIGHT, 1 },
  { ELEVATED_RIGHT | ELEVATED_TOP_RIGHT | ELEVATED_BOTTOM_RIGHT, 1 },
  { ELEVATED_BOTTOM_RIGHT | ELEVATED_TOP_RIGHT, 1 },

  { ELEVATED_TOP | ELEVATED_LEFT, 9 },
  { ELEVATED_TOP | ELEVATED_LEFT | ELEVATED_BOTTOM_LEFT, 9 },
  { ELEVATED_TOP | ELEVATED_LEFT | ELEVATED_TOP_RIGHT, 9 },
  { ELEVATED_TOP | ELEVATED_LEFT | ELEVATED_BOTTOM_LEFT | ELEVATED_TOP_RIGHT, 9 },
  { ELEVATED_TOP | ELEVATED_LEFT | ELEVATED_TOP_LEFT, 9 },
  { ELEVATED_TOP | ELEVATED_LEFT | ELEVATED_TOP_LEFT | ELEVATED_TOP_RIGHT, 9 },
  { ELEVATED_TOP | ELEVATED_LEFT | ELEVATED_TOP_LEFT | ELEVATED_BOTTOM_LEFT, 9 },
  { ELEVATED_TOP | ELEVATED_LEFT | ELEVATED_TOP_LEFT | ELEVATED_BOTTOM_LEFT | ELEVATED_TOP_RIGHT, 9 },
  { ELEVATED_TOP | ELEVATED_LEFT | ELEVATED_TOP_RIGHT | ELEVATED_TOP_RIGHT, 9 },

  { ELEVATED_TOP | ELEVATED_RIGHT, 10 },
  { ELEVATED_TOP | ELEVATED_RIGHT | ELEVATED_BOTTOM_RIGHT, 10 },
  { ELEVATED_TOP | ELEVATED_RIGHT | ELEVATED_TOP_LEFT, 10 },
  { ELEVATED_TOP | ELEVATED_RIGHT | ELEVATED_BOTTOM_RIGHT | ELEVATED_TOP_LEFT, 10 },
  { ELEVATED_TOP | ELEVATED_RIGHT | ELEVATED_TOP_RIGHT, 10 },
  { ELEVATED_TOP | ELEVATED_RIGHT | ELEVATED_TOP_RIGHT | ELEVATED_TOP_LEFT, 10 },
  { ELEVATED_TOP | ELEVATED_RIGHT | ELEVATED_TOP_RIGHT | ELEVATED_BOTTOM_RIGHT, 10 },
  { ELEVATED_TOP | ELEVATED_RIGHT | ELEVATED_TOP_RIGHT | ELEVATED_TOP_LEFT | ELEVATED_BOTTOM_RIGHT, 10 },
  { ELEVATED_TOP | ELEVATED_RIGHT | ELEVATED_TOP_LEFT | ELEVATED_BOTTOM_RIGHT, 10 },

  { ELEVATED_BOTTOM | ELEVATED_RIGHT, 13 },
  { ELEVATED_BOTTOM | ELEVATED_RIGHT | ELEVATED_TOP_RIGHT, 13 },
  { ELEVATED_BOTTOM | ELEVATED_RIGHT | ELEVATED_BOTTOM_LEFT, 13 },
  { ELEVATED_BOTTOM | ELEVATED_RIGHT | ELEVATED_BOTTOM_RIGHT, 13 },
  { ELEVATED_BOTTOM | ELEVATED_RIGHT | ELEVATED_BOTTOM_RIGHT | ELEVATED_BOTTOM_LEFT , 13 },
  { ELEVATED_BOTTOM | ELEVATED_RIGHT | ELEVATED_BOTTOM_RIGHT | ELEVATED_TOP_RIGHT , 13 },
  { ELEVATED_BOTTOM | ELEVATED_RIGHT | ELEVATED_BOTTOM_RIGHT | ELEVATED_BOTTOM_LEFT | ELEVATED_TOP_RIGHT , 13 },
  { ELEVATED_BOTTOM | ELEVATED_RIGHT | ELEVATED_BOTTOM_LEFT | ELEVATED_TOP_RIGHT, 13 },
  { ELEVATED_BOTTOM | ELEVATED_BOTTOM_LEFT | ELEVATED_TOP_RIGHT, 13 },

  { ELEVATED_BOTTOM | ELEVATED_LEFT, 11 },
  { ELEVATED_BOTTOM | ELEVATED_LEFT | ELEVATED_TOP_LEFT, 11 },
  { ELEVATED_BOTTOM | ELEVATED_LEFT | ELEVATED_BOTTOM_RIGHT, 11 },
  { ELEVATED_BOTTOM | ELEVATED_LEFT | ELEVATED_BOTTOM_LEFT, 11 },
  { ELEVATED_BOTTOM | ELEVATED_LEFT | ELEVATED_BOTTOM_LEFT | ELEVATED_TOP_LEFT, 11 },
  { ELEVATED_BOTTOM | ELEVATED_LEFT | ELEVATED_BOTTOM_LEFT | ELEVATED_BOTTOM_RIGHT, 11 },
  { ELEVATED_BOTTOM | ELEVATED_LEFT | ELEVATED_BOTTOM_LEFT | ELEVATED_TOP_LEFT | ELEVATED_BOTTOM_RIGHT, 11 },
  { ELEVATED_BOTTOM | ELEVATED_LEFT | ELEVATED_TOP_LEFT | ELEVATED_BOTTOM_RIGHT, 11 },
  { ELEVATED_BOTTOM | ELEVATED_LEFT | ELEVATED_BOTTOM_RIGHT | ELEVATED_TOP_LEFT, 11 },

  //diagonal tiles - insert a block
  { ELEVATED_TOP_LEFT | ELEVATED_BOTTOM_RIGHT, 4 },
  { ELEVATED_BOTTOM_LEFT | ELEVATED_TOP_RIGHT, 4 },


  // special cases - elevate the tile
  { ELEVATED_RIGHT | ELEVATED_BOTTOM_LEFT, -1 },
  { ELEVATED_RIGHT | ELEVATED_TOP_LEFT, -1 },
  { ELEVATED_LEFT | ELEVATED_BOTTOM_RIGHT, -1 },
  { ELEVATED_LEFT | ELEVATED_TOP_RIGHT, -1 },
  { ELEVATED_TOP | ELEVATED_BOTTOM_LEFT, -1 },
  { ELEVATED_TOP | ELEVATED_BOTTOM_RIGHT, -1 },
  { ELEVATED_BOTTOM | ELEVATED_TOP_LEFT, -1 },
  { ELEVATED_BOTTOM | ELEVATED_TOP_RIGHT, -1 },

  { ELEVATED_LEFT | ELEVATED_RIGHT, -1 },
  { ELEVATED_TOP | ELEVATED_BOTTOM, -1 },

  // Missing
  // TOP LEFT - RIGHT
  // TOP LEFT - BOTTOM - RIGHT - BOTTOM RIGHT

};

void Resources::init()
{
  readINIFile();
  readTileListFile();
  readUITextureListFile();
}

Point Resources::convertScreenToIsoCoordinates(const Point& screenCoordinates)
{
  return Engine::Instance().findCellAt(screenCoordinates);
}

Point Resources::convertIsoToScreenCoordinates(const Point& isoCoordinates, bool calcWithoutOffset)
{
  int x, y;

  int height = isoCoordinates.getHeight();
  int heightOffset = 20;

  if (calcWithoutOffset)
  {
    x = static_cast<int>((_tileSize * _zoomLevel * isoCoordinates.getX() * 0.5)  + (_tileSize * _zoomLevel * isoCoordinates.getY() * 0.5));
    y = static_cast<int>((_tileSize * _zoomLevel * isoCoordinates.getX() * 0.25) - (_tileSize * _zoomLevel * isoCoordinates.getY() * 0.25));
  }
  else
  {
    x = static_cast<int>((_tileSize * _zoomLevel * isoCoordinates.getX() * 0.5)  + (_tileSize * _zoomLevel * isoCoordinates.getY() * 0.5)  - _cameraOffset.getX());
    y = static_cast<int>((_tileSize * _zoomLevel * isoCoordinates.getX() * 0.25) - (_tileSize * _zoomLevel * isoCoordinates.getY() * 0.25) - _cameraOffset.getY());
  }
  
  if (height > 0)
  {
    y = static_cast<int>(y - ((_tileSize - heightOffset) * height * _zoomLevel));
  }
  return Point (x, y);
}

// Temporary function that writes a json file and will be removed later (for debug purposes only)
void Resources::generateJSONFile()
{
  json tileIDJSON;

  tileIDJSON["terrain"]["0"]["filename"] = std::string("resources/images/floor/floor_0.png");
  tileIDJSON["terrain"]["1"]["filename"] = std::string("resources/images/floor/floor_1.png");
  tileIDJSON["terrain"]["2"]["filename"] = std::string("resources/images/floor/floor_2.png");
  tileIDJSON["terrain"]["3"]["filename"] = std::string("resources/images/floor/floor_3.png");
  tileIDJSON["terrain"]["4"]["filename"] = std::string("resources/images/floor/floor_4.png");
  tileIDJSON["terrain"]["5"]["filename"] = std::string("resources/images/floor/floor_5.png");
  tileIDJSON["terrain"]["6"]["filename"] = std::string("resources/images/floor/floor_6.png");
  tileIDJSON["terrain"]["7"]["filename"] = std::string("resources/images/floor/floor_7.png");
  tileIDJSON["terrain"]["8"]["filename"] = std::string("resources/images/floor/floor_8.png");
  tileIDJSON["terrain"]["9"]["filename"] = std::string("resources/images/floor/floor_9.png");
  tileIDJSON["terrain"]["10"]["filename"] = std::string("resources/images/floor/floor_10.png");
  tileIDJSON["terrain"]["11"]["filename"] = std::string("resources/images/floor/floor_11.png");
  tileIDJSON["terrain"]["12"]["filename"] = std::string("resources/images/floor/floor_12.png");
  tileIDJSON["terrain"]["13"]["filename"] = std::string("resources/images/floor/floor_13.png");
  tileIDJSON["terrain"]["14"]["filename"] = std::string("resources/images/floor/floor_14.png");

  tileIDJSON["buildings"]["20"]["filename"] = std::string("resources/images/buildings/house.png");
  tileIDJSON["buildings"]["20"]["type"] = "building";
  tileIDJSON["buildings"]["20"]["zone"] = "residential";

  std::ofstream myJsonFile(settings.tileDataJSONFile);
  if (myJsonFile.is_open())
  {

  myJsonFile << std::setw(4) << tileIDJSON << std::endl;
  myJsonFile.close();
  }
  else
  {
    LOG(LOG_ERROR) << "Couldn't write file " << settings.tileDataJSONFile;
  }
}

std::string Resources::getTileDataFromJSON(const std::string& tileType, int tileID, const std::string& attribute)
{
  for (json::iterator it = _json.begin(); it != _json.end(); ++it) 
  {
    if ( it.key() == tileType ) 
    {
      // more json stuff later...
    }
  }

  if (_json[tileType][std::to_string(tileID)]["filename"].is_null() )
  {
    LOG(LOG_ERROR) << "Can't retrieve filename from " << settings.tileDataJSONFile << " for ID " << tileID;
    // Application should quit here.
  }
  std::string retrievedFileName = _json[tileType][std::to_string(tileID)]["filename"].get<std::string>();
  return retrievedFileName;
}

std::string Resources::getUISpriteDataFromJSON(const std::string& uiType, int uiSpriteID, const std::string& attribute)
{
  for (json::iterator it = _uiTextureFile.begin(); it != _uiTextureFile.end(); ++it)
  {
    if (it.key() == uiType)
    {
      // more json stuff later...
    }
  }
  if ( _uiTextureFile[uiType][std::to_string(uiSpriteID)]["filename"].is_null() )
  {
    LOG(LOG_ERROR) << "Can't retrieve filename from " << settings.uiDataJSONFile << "for ID " << uiSpriteID;
  }
  std::string retrievedFileName = _uiTextureFile[uiType][std::to_string(uiSpriteID)]["filename"].get<std::string>();
  return retrievedFileName;
}

void Resources::readTileListFile()
{
  std::ifstream i(settings.tileDataJSONFile);
  if (i.fail())
  {
    LOG(LOG_ERROR) << "File " << settings.tileDataJSONFile << " does not exist! Cannot load settings from INI File!";
    // Application should quit here, without textureData from the tileList file we can't continue
    return;
  }
  
  // check if json file can be parsed
  _json = json::parse(i, nullptr, false);
  if (_json.is_discarded())
    LOG(LOG_ERROR) << "Error parsing JSON File " << settings.tileDataJSONFile;
  
}

// User Interface Sprites

void Resources::generateUITextureFile()
{
  json uiTextureFile;

  uiTextureFile["button"]["0"]["filename"] = std::string("resources/images/ui/buttons/build.png");
  uiTextureFile["button"]["0"]["filename"] = std::string("resources/images/ui/buttons/build.png");

  std::ofstream myJsonFile(settings.uiDataJSONFile);

  if (myJsonFile.is_open())
  {
    myJsonFile << std::setw(4) << settings.uiDataJSONFile << std::endl;
    myJsonFile.close();
  }
  else
  {
    printf("ERROR: Couldn't write file \"resources/UITextureList.json\"");
  }
}

void Resources::readUITextureListFile()
{
  std::ifstream i(settings.uiDataJSONFile);
  if (i.fail())
  {
    LOG(LOG_ERROR) << "File " << settings.uiDataJSONFile << " does not exist! Cannot load settings from INI File!";
    // Application should quit here, without textureData we can't continue
    return;
  }

  // check if json file can be parsed
  _uiTextureFile = json::parse(i, nullptr, false);
  if (_uiTextureFile.is_discarded())
    LOG(LOG_ERROR) << "Error parsing JSON File " << settings.uiDataJSONFile;

}


// Ini File Handling

void Resources::generateINIFile()
{
  json iniFile;

  iniFile["Graphics"]["Resolution"]["Width"] = 800;
  iniFile["Graphics"]["Resolution"]["Height"] = 600;
  iniFile["Graphics"]["VSYNC"] = false;
  iniFile["Graphics"]["FullScreen"] = false;
  iniFile["Game"]["MapSize"] = 128;
  iniFile["Game"]["MaxElevationHeight"] = 32;
  iniFile["ConfigFiles"]["UIDataJSONFile"] = "resources/data/UIData.json";
  iniFile["ConfigFiles"]["TileDataJSONFile"] = "resources/data/TileData.json";

  std::ofstream myJsonFile("resources/settings.json");

  if (myJsonFile.is_open())
  {
    myJsonFile << std::setw(4) << iniFile << std::endl;
    myJsonFile.close();
  }
  else
  {
    printf("ERROR: Couldn't write file \"resources/settings.json\"");
  }
}

void Resources::readINIFile()
{
  std::string iniFileName = "resources/settings.json";
  std::ifstream i(iniFileName);
  if (i.fail())
  {
    LOG(LOG_ERROR) << "File " << iniFileName << " does not exist! Cannot load settings from INI File!";
    // Application should quit here, without settings from the ini file we can't continue
    return;
  }

  // check if json file can be parsed
  _iniFile = json::parse(i, nullptr, false);
  if (_iniFile.is_discarded())
    LOG(LOG_ERROR) << "Error parsing JSON File " << iniFileName;

  settings.screenWidth = _iniFile["Graphics"]["Resolution"]["Width"].get<int>();
  settings.screenHeight = _iniFile["Graphics"]["Resolution"]["Height"].get<int>();
  settings.vSync = _iniFile["Graphics"]["VSYNC"].get<bool>();
  settings.fullScreen = _iniFile["Graphics"]["FullScreen"].get<bool>();
  settings.mapSize = _iniFile["Game"]["MapSize"].get<int>();
  settings.maxElevationHeight = _iniFile["Game"]["MaxElevationHeight"].get<int>();
  settings.uiDataJSONFile = _iniFile["ConfigFiles"]["UIDataJSONFile"].get<std::string>();
  settings.tileDataJSONFile = _iniFile["ConfigFiles"]["TileDataJSONFile"].get<std::string>();

}