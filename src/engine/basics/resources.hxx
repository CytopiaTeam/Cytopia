#ifndef RESOURCES_HXX_
#define RESOURCES_HXX_

#include <fstream>

#include "SDL2/SDL.h"
#include "point.hxx"
#include "../../ThirdParty/json.hxx"
#include "log.hxx"

// Namespace from json library - https://github.com/nlohmann/json
using json = nlohmann::json;

//forward declare Engine class (prevent cross includes)
class Engine;

class Resources
{
public:
  static void init();
  static SDL_Renderer* getRenderer();
  static void setRenderer(SDL_Renderer* renderer);
  static SDL_Window* getWindow();
  static void setWindow(SDL_Window* window);

  static float getZoomLevel();
  static void setZoomLevel(float zoomLevel);
  static Point getCameraOffset();
  static void setCameraOffset(Point cameraOffset);

  static int getTileSize();

  // Public functions
  static Point convertScreenToIsoCoordinates(Point screenCoordinates);
  static Point convertIsoToScreenCoordinates(Point isoCoordinates, bool calcWithoutOffset = false);

  static void setTerrainEditMode(int editMode);
  static int getTerrainEditMode();

  // JSON Functions
  static void generateJSONFile();
  static std::string getTileDataFromJSON(std::string tileType, int tileID, std::string attributes);
  static void readTileListFile();

  static void generateINIFile();

  enum terrainEditMode : int
  {
    NO_TERRAIN_EDIT,
    TERRAIN_RAISE,
    TERRAIN_LOWER
  };

  static int _terrainEditMode;

  typedef struct
  {
    int screenWidth;
    int screenHeight;
    bool vSync;
    bool fullScreen;
    int mapSize;
  }Settings;

  static Settings settings;

private:
  Resources() { };
  ~Resources() { };

  static SDL_Renderer* _renderer;
  static SDL_Window* _window;

  static float _zoomLevel;
  static Point _cameraOffset;

  static const int _TILE_SIZE;

  static json _json;
  static json _iniFile;

  static void readINIFile();

};

#endif

