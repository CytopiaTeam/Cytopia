#include "resources.hxx"
#include "../engine.hxx"

// Instantiate static variables
SDL_Renderer *Resources::_renderer = nullptr;
SDL_Window *Resources::_window = nullptr;
float Resources::_zoomLevel = 1.0;
Point Resources::_cameraOffset;
const int Resources::_tileSize = 32;
int Resources::_terrainEditMode = Resources::NO_TERRAIN_EDIT;
json Resources::_json;
json Resources::_iniFile;
json Resources::_uiTextureFile;
json Resources::_uiLayout;
Resources::Settings Resources::settings;

/** Enum (bitmask) for mapping neighbor tile positions
* [ T B L R TL TR BL BR ]
* [ 0 0 0 0  0  0  0  0 ]

* Our neighbor tiles look like this
* 2 5 8
* 1 X 7
* 0 3 6
*/
std::unordered_map<unsigned int, int> Resources::slopeTileIDMap = {
    {NO_NEIGHBORS, 14},
    {ELEVATED_TOP, 3},
    {ELEVATED_BOTTOM, 5},
    {ELEVATED_RIGHT, 1},
    {ELEVATED_LEFT, 7},
    {ELEVATED_BOTTOM_RIGHT, 2},
    {ELEVATED_BOTTOM_LEFT, 8},
    {ELEVATED_TOP_RIGHT, 0},
    {ELEVATED_TOP_LEFT, 6},

    {ELEVATED_TOP | ELEVATED_TOP_LEFT, 3},
    {ELEVATED_TOP | ELEVATED_TOP_RIGHT, 3},
    {ELEVATED_TOP | ELEVATED_TOP_RIGHT | ELEVATED_TOP_LEFT, 3},
    {ELEVATED_TOP_LEFT | ELEVATED_TOP_RIGHT, 3},

    {ELEVATED_BOTTOM | ELEVATED_BOTTOM_LEFT, 5},
    {ELEVATED_BOTTOM | ELEVATED_BOTTOM_RIGHT, 5},
    {ELEVATED_BOTTOM | ELEVATED_BOTTOM_RIGHT | ELEVATED_BOTTOM_LEFT, 5},
    {ELEVATED_BOTTOM_LEFT | ELEVATED_BOTTOM_RIGHT, 5},

    {ELEVATED_LEFT | ELEVATED_BOTTOM_LEFT, 7},
    {ELEVATED_LEFT | ELEVATED_TOP_LEFT, 7},
    {ELEVATED_LEFT | ELEVATED_TOP_LEFT | ELEVATED_BOTTOM_LEFT, 7},
    {ELEVATED_TOP_LEFT | ELEVATED_BOTTOM_LEFT, 7},

    {ELEVATED_RIGHT | ELEVATED_BOTTOM_RIGHT, 1},
    {ELEVATED_RIGHT | ELEVATED_TOP_RIGHT, 1},
    {ELEVATED_RIGHT | ELEVATED_TOP_RIGHT | ELEVATED_BOTTOM_RIGHT, 1},
    {ELEVATED_BOTTOM_RIGHT | ELEVATED_TOP_RIGHT, 1},

    {ELEVATED_TOP | ELEVATED_LEFT, 9},
    {ELEVATED_TOP | ELEVATED_LEFT | ELEVATED_BOTTOM_LEFT, 9},
    {ELEVATED_TOP | ELEVATED_LEFT | ELEVATED_TOP_RIGHT, 9},
    {ELEVATED_TOP | ELEVATED_LEFT | ELEVATED_BOTTOM_LEFT | ELEVATED_TOP_RIGHT, 9},
    {ELEVATED_TOP | ELEVATED_LEFT | ELEVATED_TOP_LEFT, 9},
    {ELEVATED_TOP | ELEVATED_LEFT | ELEVATED_TOP_LEFT | ELEVATED_TOP_RIGHT, 9},
    {ELEVATED_TOP | ELEVATED_LEFT | ELEVATED_TOP_LEFT | ELEVATED_BOTTOM_LEFT, 9},
    {ELEVATED_TOP | ELEVATED_LEFT | ELEVATED_TOP_LEFT | ELEVATED_BOTTOM_LEFT | ELEVATED_TOP_RIGHT, 9},
    {ELEVATED_TOP | ELEVATED_LEFT | ELEVATED_TOP_RIGHT | ELEVATED_TOP_RIGHT, 9},

    {ELEVATED_TOP | ELEVATED_RIGHT, 10},
    {ELEVATED_TOP | ELEVATED_RIGHT | ELEVATED_BOTTOM_RIGHT, 10},
    {ELEVATED_TOP | ELEVATED_RIGHT | ELEVATED_TOP_LEFT, 10},
    {ELEVATED_TOP | ELEVATED_RIGHT | ELEVATED_BOTTOM_RIGHT | ELEVATED_TOP_LEFT, 10},
    {ELEVATED_TOP | ELEVATED_RIGHT | ELEVATED_TOP_RIGHT, 10},
    {ELEVATED_TOP | ELEVATED_RIGHT | ELEVATED_TOP_RIGHT | ELEVATED_TOP_LEFT, 10},
    {ELEVATED_TOP | ELEVATED_RIGHT | ELEVATED_TOP_RIGHT | ELEVATED_BOTTOM_RIGHT, 10},
    {ELEVATED_TOP | ELEVATED_RIGHT | ELEVATED_TOP_RIGHT | ELEVATED_TOP_LEFT | ELEVATED_BOTTOM_RIGHT, 10},
    {ELEVATED_TOP | ELEVATED_RIGHT | ELEVATED_TOP_LEFT | ELEVATED_BOTTOM_RIGHT, 10},

    {ELEVATED_BOTTOM | ELEVATED_RIGHT, 13},
    {ELEVATED_BOTTOM | ELEVATED_RIGHT | ELEVATED_TOP_RIGHT, 13},
    {ELEVATED_BOTTOM | ELEVATED_RIGHT | ELEVATED_BOTTOM_LEFT, 13},
    {ELEVATED_BOTTOM | ELEVATED_RIGHT | ELEVATED_BOTTOM_RIGHT, 13},
    {ELEVATED_BOTTOM | ELEVATED_RIGHT | ELEVATED_BOTTOM_RIGHT | ELEVATED_BOTTOM_LEFT, 13},
    {ELEVATED_BOTTOM | ELEVATED_RIGHT | ELEVATED_BOTTOM_RIGHT | ELEVATED_TOP_RIGHT, 13},
    {ELEVATED_BOTTOM | ELEVATED_RIGHT | ELEVATED_BOTTOM_RIGHT | ELEVATED_BOTTOM_LEFT | ELEVATED_TOP_RIGHT, 13},
    {ELEVATED_BOTTOM | ELEVATED_RIGHT | ELEVATED_BOTTOM_LEFT | ELEVATED_TOP_RIGHT, 13},
    {ELEVATED_BOTTOM | ELEVATED_BOTTOM_LEFT | ELEVATED_TOP_RIGHT, 13},

    {ELEVATED_BOTTOM | ELEVATED_LEFT, 11},
    {ELEVATED_BOTTOM | ELEVATED_LEFT | ELEVATED_TOP_LEFT, 11},
    {ELEVATED_BOTTOM | ELEVATED_LEFT | ELEVATED_BOTTOM_RIGHT, 11},
    {ELEVATED_BOTTOM | ELEVATED_LEFT | ELEVATED_BOTTOM_LEFT, 11},
    {ELEVATED_BOTTOM | ELEVATED_LEFT | ELEVATED_BOTTOM_LEFT | ELEVATED_TOP_LEFT, 11},
    {ELEVATED_BOTTOM | ELEVATED_LEFT | ELEVATED_BOTTOM_LEFT | ELEVATED_BOTTOM_RIGHT, 11},
    {ELEVATED_BOTTOM | ELEVATED_LEFT | ELEVATED_BOTTOM_LEFT | ELEVATED_TOP_LEFT | ELEVATED_BOTTOM_RIGHT, 11},
    {ELEVATED_BOTTOM | ELEVATED_LEFT | ELEVATED_TOP_LEFT | ELEVATED_BOTTOM_RIGHT, 11},
    {ELEVATED_BOTTOM | ELEVATED_LEFT | ELEVATED_BOTTOM_RIGHT | ELEVATED_TOP_LEFT, 11},

    //diagonal tiles - insert a block
    {ELEVATED_TOP_LEFT | ELEVATED_BOTTOM_RIGHT, 4},
    {ELEVATED_BOTTOM_LEFT | ELEVATED_TOP_RIGHT, 4},

    // special cases - elevate the tile
    {ELEVATED_RIGHT | ELEVATED_BOTTOM_LEFT, -1},
    {ELEVATED_RIGHT | ELEVATED_TOP_LEFT, -1},
    {ELEVATED_LEFT | ELEVATED_BOTTOM_RIGHT, -1},
    {ELEVATED_LEFT | ELEVATED_TOP_RIGHT, -1},
    {ELEVATED_TOP | ELEVATED_BOTTOM_LEFT, -1},
    {ELEVATED_TOP | ELEVATED_BOTTOM_RIGHT, -1},
    {ELEVATED_BOTTOM | ELEVATED_TOP_LEFT, -1},
    {ELEVATED_BOTTOM | ELEVATED_TOP_RIGHT, -1},

    {ELEVATED_LEFT | ELEVATED_RIGHT, -1},
    {ELEVATED_TOP | ELEVATED_BOTTOM, -1},

    // Missing
    // TOP LEFT - RIGHT
    // TOP LEFT - BOTTOM - RIGHT - BOTTOM RIGHT

};

void Resources::init()
{
  readINIFile();
  readTileListFile();
  readUITextureListFile();
  readUILayoutFile();
}

Point Resources::convertScreenToIsoCoordinates(const Point &screenCoordinates)
{
  return Engine::Instance().findCellAt(screenCoordinates);
}

Point Resources::convertIsoToScreenCoordinates(const Point &isoCoordinates, bool calcWithoutOffset)
{
  int x, y;

  int height = isoCoordinates.height;
  int heightOffset = 20;

  if (calcWithoutOffset)
  {
    x = static_cast<int>((_tileSize * _zoomLevel * isoCoordinates.x * 0.5) + (_tileSize * _zoomLevel * isoCoordinates.y * 0.5));
    y = static_cast<int>((_tileSize * _zoomLevel * isoCoordinates.x * 0.25) - (_tileSize * _zoomLevel * isoCoordinates.y * 0.25));
  }
  else
  {
    x = static_cast<int>((_tileSize * _zoomLevel * isoCoordinates.x * 0.5) + (_tileSize * _zoomLevel * isoCoordinates.y * 0.5) -
                         _cameraOffset.x);
    y = static_cast<int>((_tileSize * _zoomLevel * isoCoordinates.x * 0.25) - (_tileSize * _zoomLevel * isoCoordinates.y * 0.25) -
                         _cameraOffset.y);
  }

  if (height > 0)
  {
    y = static_cast<int>(y - ((_tileSize - heightOffset) * height * _zoomLevel));
  }

  return {x, y, 0, 0};
}

std::string Resources::getTileDataFromJSON(const std::string &tileType, int tileID, const std::string &attribute)
{
  for (json::iterator it = _json.begin(); it != _json.end(); ++it)
  {
    if (it.key() == tileType)
    {
      // more json stuff later...
    }
  }

  if (_json[tileType][std::to_string(tileID)]["filename"].is_null())
  {
    LOG(LOG_ERROR) << "Can't retrieve filename from " << settings.tileDataJSONFile << " for ID " << tileID;
    // Application should quit here.
  }
  std::string retrievedFileName = _json[tileType][std::to_string(tileID)]["filename"].get<std::string>();
  return retrievedFileName;
}

std::string Resources::getUISpriteDataFromJSON(const std::string &uiType, int uiSpriteID, const std::string &attribute)
{
  std::string retrievedFileName;

  for (json::iterator it = _uiTextureFile.begin(); it != _uiTextureFile.end(); ++it)
  {
    if (it.key() == uiType)
    {
      // more json stuff later...
    }
  }
  if (_uiTextureFile[uiType][std::to_string(uiSpriteID)][attribute].is_null())
  {

    if (attribute == "filename")
    {
      LOG(LOG_ERROR) << "Can't retrieve attribute " << attribute << " from " << settings.uiDataJSONFile << " for ID " << uiSpriteID;
    }
    return {};
  }
  else
  {
    retrievedFileName = _uiTextureFile[uiType][std::to_string(uiSpriteID)][attribute].get<std::string>();
  }
  
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

void Resources::readUILayoutFile()
{
  std::ifstream i(settings.uiLayoutJSONFile);
  if (i.fail())
  {
    LOG(LOG_ERROR) << "File " << settings.uiLayoutJSONFile << " does not exist! Cannot load settings from INI File!";
    // Application should quit here, without textureData we can't continue
    return;
  }

  // check if json file can be parsed
  _uiLayout = json::parse(i, nullptr, false);
  if (_uiLayout.is_discarded())
  {
    LOG(LOG_ERROR) << "Error parsing JSON File " << settings.uiLayoutJSONFile;
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
  {
    LOG(LOG_ERROR) << "Error parsing JSON File " << settings.uiDataJSONFile;
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
  {
    LOG(LOG_ERROR) << "Error parsing JSON File " << iniFileName;
  }

  settings.screenWidth = _iniFile["Graphics"]["Resolution"]["Width"].get<int>();
  settings.screenHeight = _iniFile["Graphics"]["Resolution"]["Height"].get<int>();
  settings.vSync = _iniFile["Graphics"]["VSYNC"].get<bool>();
  settings.fullScreen = _iniFile["Graphics"]["FullScreen"].get<bool>();
  settings.mapSize = _iniFile["Game"]["MapSize"].get<int>();
  settings.maxElevationHeight = _iniFile["Game"]["MaxElevationHeight"].get<int>();
  settings.uiDataJSONFile = _iniFile["ConfigFiles"]["UIDataJSONFile"].get<std::string>();
  settings.tileDataJSONFile = _iniFile["ConfigFiles"]["TileDataJSONFile"].get<std::string>();
  settings.uiLayoutJSONFile = _iniFile["ConfigFiles"]["UILayoutJSONFile"].get<std::string>();
  settings.playMusic = _iniFile["Audio"]["PlayMusic"].get<bool>();
  settings.playSoundEffects = _iniFile["Audio"]["PlaySoundEffects"].get<bool>();
  settings.audioChannels = _iniFile["Audio"]["AudioChannels"].get<int>();
  settings.musicVolume = _iniFile["Audio"]["MusicVolume"].get<int>();
  settings.soundEffectsVolume = _iniFile["Audio"]["SoundEffectsVolume"].get<int>();
}