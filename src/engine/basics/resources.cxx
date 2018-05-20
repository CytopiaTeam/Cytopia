#include "resources.hxx"
#include "../engine.hxx"

// Instantiate static variables
SDL_Renderer* Resources::_renderer = nullptr;
SDL_Window* Resources::_window = nullptr;
float Resources::_zoomLevel = 1.0;
Point Resources::_cameraOffset;
const int Resources::_TILE_SIZE = 32;
int Resources::_terrainEditMode = Resources::NO_TERRAIN_EDIT;
json Resources::_json;
json Resources::_iniFile;
Resources::Settings Resources::settings;


void Resources::init()
{
  readINIFile();
  readTileListFile();
}

SDL_Renderer* Resources::getRenderer()
{
  return _renderer;
}

void Resources::setRenderer(SDL_Renderer* renderer)
{
  _renderer = renderer;
}

SDL_Window* Resources::getWindow()
{
  return _window;
}

void Resources::setWindow(SDL_Window* window)
{
  _window = window;
}

Point Resources::getCameraOffset()
{
  return _cameraOffset;
}

void Resources::setCameraOffset(Point cameraOffset)
{
  _cameraOffset = cameraOffset;
}

float Resources::getZoomLevel()
{
  return _zoomLevel;
}

void Resources::setZoomLevel(float zoomLevel)
{
  _zoomLevel = zoomLevel;
}

int Resources::getTileSize()
{
  return _TILE_SIZE;
}

Point Resources::convertScreenToIsoCoordinates(Point screenCoordinates)
{
  return Engine::Instance().findCellAt(screenCoordinates);
}

Point Resources::convertIsoToScreenCoordinates(Point isoCoordinates, bool calcWithoutOffset)
{
  int x, y;

  int height = isoCoordinates.getHeight();
  int heightOffset = 20;

  if (calcWithoutOffset)
  {
    x = static_cast<int>((_TILE_SIZE * _zoomLevel * isoCoordinates.getX() * 0.5)  + (_TILE_SIZE * _zoomLevel * isoCoordinates.getY() * 0.5));
    y = static_cast<int>((_TILE_SIZE * _zoomLevel * isoCoordinates.getX() * 0.25) - (_TILE_SIZE * _zoomLevel * isoCoordinates.getY() * 0.25));
  }
  else
  {
    x = static_cast<int>((_TILE_SIZE * _zoomLevel * isoCoordinates.getX() * 0.5)  + (_TILE_SIZE * _zoomLevel * isoCoordinates.getY() * 0.5)  - _cameraOffset.getX());
    y = static_cast<int>((_TILE_SIZE * _zoomLevel * isoCoordinates.getX() * 0.25) - (_TILE_SIZE * _zoomLevel * isoCoordinates.getY() * 0.25) - _cameraOffset.getY());
  }
  
  if (height > 0)
  {
    y = static_cast<int>(y - ((_TILE_SIZE - heightOffset) * height * _zoomLevel));
  }
  return Point (x, y);
}

void Resources::setTerrainEditMode(int editMode)
{
  _terrainEditMode = editMode;
}

int Resources::getTerrainEditMode()
{
  return _terrainEditMode;
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

  std::ofstream myJsonFile("resources/tileList.json");
  if (myJsonFile.is_open())
  {

  myJsonFile << std::setw(4) << tileIDJSON << std::endl;
  myJsonFile.close();
  }
  else
  {
    LOG(LOG_ERROR) << "Couldn't write file \"resources/tileList.json\"";
  }
}

std::string Resources::getTileDataFromJSON(std::string tileType, int tileID, std::string attribute)
{
  for (json::iterator it = _json.begin(); it != _json.end(); ++it) 
  {
    if ( it.key() == tileType ) 
    {
      // more json stuff later...
    }
  }
  std::string retrievedFileName = _json[tileType][std::to_string(tileID)]["filename"].get<std::string>();
  return retrievedFileName;
}

void Resources::readTileListFile()
{
  std::string tileListFile = "resources/tileList.json";
  std::ifstream i(tileListFile);
  if (i.fail())
  {
    LOG(LOG_ERROR) << "File " << tileListFile << " does not exist! Cannot load settings from INI File!";
    // Application should quit here, without settings from the tileList file we can't continue
    return;
  }

  i >> _json;
}

// Ini File Handling

void Resources::generateINIFile()
{
  json iniFile;

  iniFile["Graphics"]["Resolution"]["Width"] = 800;
  iniFile["Graphics"]["Resolution"]["Height"] = 600;
  iniFile["Graphics"]["VSYNC"] = false;
  iniFile["Graphics"]["FullScreen"] = false;
  iniFile["Game"]["MapSize"] = 32;

  std::ofstream myJsonFile("resources/settings.json");

  if (myJsonFile.is_open())
  {
    myJsonFile << std::setw(4) << iniFile << std::endl;
    myJsonFile.close();
  }
  else
  {
    printf("ERROR: Couldn't write file \"resources/iniFile.json\"");
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

  i >> _iniFile;

  settings.screenWidth = _iniFile["Graphics"]["Resolution"]["Width"].get<int>();
  settings.screenHeight = _iniFile["Graphics"]["Resolution"]["Height"].get<int>();
  settings.vSync = _iniFile["Graphics"]["VSYNC"].get<bool>();
  settings.fullScreen = _iniFile["Graphics"]["FullScreen"].get<bool>();
  settings.mapSize = _iniFile["Game"]["MapSize"].get<int>();

}