#ifndef CONSTANTS_HXX_
#define CONSTANTS_HXX_

// PATHS
#ifdef __linux__
const std::string CYTOPIA_DATA_DIR_BASE = getenv("XDG_DATA_HOME") + (std::string) "/Cytopia/";
#elif _WIN32
const std::string CYTOPIA_DATA_DIR_BASE = getenv("APPDATA") + (std::string) "/Cytopia/";
#elif __APPLE__
const std::string CYTOPIA_DATA_DIR_BASE = getenv("HOME") + (std::string) "/Cytopia/";
#else
// TODO: Define mobile platform dirs
const std::string CYTOPIA_DATA_DIR_BASE = "";
#endif

const std::string CYTOPIA_DATA_DIR = CYTOPIA_DATA_DIR_BASE + (std::string) "/Cytopia/";

const std::string SETTINGS_FILENAME = "settings.json";
const std::string CYTOPIA_RESOURCES_DIR = "resources/";

constexpr const char SETTINGS_FILE_NAME[] = "resources/settings.json";
constexpr const unsigned int SAVEGAME_VERSION = 4;
constexpr const char TERRAINGEN_DATA_FILE_NAME[] = "resources/data/TerrainGen.json";

#endif
