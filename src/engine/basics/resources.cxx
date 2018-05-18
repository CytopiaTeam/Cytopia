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


Resources::Resources()
{

}

Resources::~Resources()
{

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

Point Resources::convertScreenToIsoCoordinates(Point screenCoordinates, bool calcWithoutOffset)
{
  Point isoCoordinates;

  if (calcWithoutOffset)
  {
    isoCoordinates.setX((screenCoordinates.getX() + 2.0 * (screenCoordinates.getY())) / (_TILE_SIZE * _zoomLevel) - 1.5);
    isoCoordinates.setY((screenCoordinates.getX() - 2.0 * (screenCoordinates.getY())) / (_TILE_SIZE * _zoomLevel) + 1.5);
  }
  else
  {
    isoCoordinates = Engine::Instance().findCellAt(screenCoordinates);
  }
  return isoCoordinates;
}

Point Resources::convertIsoToScreenCoordinates(Point isoCoordinates, bool calcWithoutOffset)
{
  int x, y;

  int height = isoCoordinates.getHeight();
  int heightOffset = 20;

  if (calcWithoutOffset)
  {
    x = (_TILE_SIZE * _zoomLevel * isoCoordinates.getX() * 0.5)  + (_TILE_SIZE * _zoomLevel * isoCoordinates.getY() * 0.5);
    y = (_TILE_SIZE * _zoomLevel * isoCoordinates.getX() * 0.25) - (_TILE_SIZE * _zoomLevel * isoCoordinates.getY() * 0.25);
  }
  else
  {
    x = (_TILE_SIZE * _zoomLevel * isoCoordinates.getX() * 0.5)  + (_TILE_SIZE * _zoomLevel * isoCoordinates.getY() * 0.5)  - _cameraOffset.getX();
    y = (_TILE_SIZE * _zoomLevel * isoCoordinates.getX() * 0.25) - (_TILE_SIZE * _zoomLevel * isoCoordinates.getY() * 0.25) - _cameraOffset.getY();
  }
  
  if (height > 0)
  {
    y = (y - ((_TILE_SIZE - heightOffset) * height * _zoomLevel));
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
    printf("ERROR: Couldn't write file \"resources/tileList.json\"");
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

void Resources::readJSONFile()
{
  std::ifstream i("resources/tileList.json");
  i >> _json;
}


