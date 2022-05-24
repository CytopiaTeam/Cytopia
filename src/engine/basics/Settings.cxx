#include "Settings.hxx"

// #include "LOG.hxx"
#include "Exception.hxx"
#include "Constants.hxx"
#include "JsonSerialization.hxx"
#include "Filesystem.hxx"
#include "LOG.hxx"

#include <iomanip>

struct SaxNoException : public nlohmann::detail::json_sax_dom_parser<json>
{
    explicit SaxNoException(json& j) : nlohmann::detail::json_sax_dom_parser<json>(j, false)
    {}

    bool parse_error(std::size_t position, const std::string& last_token, const json::exception& ex) const
    {
      LOG(LOG_ERROR) << "parse error at input byte " << position;
      LOG(LOG_ERROR) << ex.what();
      LOG(LOG_ERROR) << "last read: \"" << last_token << "\"";
      return false;
    }
};

Settings::Settings() { readFile(); }

void Settings::readFile()
{
  std::string pathToDataDir;
  std::string pathToSettingsFile = CYTOPIA_DATA_DIR + (std::string)SETTINGS_FILENAME;

  if (!fs::fileExists(pathToSettingsFile))
  { // if there's no custom settings file in data dir, use the provided one
    pathToSettingsFile = fs::getBasePath() + (std::string)CYTOPIA_RESOURCES_DIR + (std::string)SETTINGS_FILENAME;
  }

  std::string jsonFile = fs::readFileAsString(pathToSettingsFile);
  json settingsJSONObject;
  SaxNoException sax(settingsJSONObject);

  bool parse_result = json::sax_parse(jsonFile, &sax);
  // check if json file can be parsed
  if (!parse_result)
    throw ConfigurationError(TRACE_INFO "Error parsing JSON File " + string{pathToSettingsFile});

  SettingsData data = settingsJSONObject;
  *this = data;

  // init the actual resolution with the desired resolution
  currentScreenWidth = screenWidth;
  currentScreenHeight = screenHeight;
#ifdef __ANDROID__
  subMenuButtonHeight *= 2;
  subMenuButtonWidth *= 2;
#endif
}

void Settings::writeFile()
{
  const json settingsJsonObject = *this;
  std::string pathToDataDir;
  if (CYTOPIA_DATA_DIR_BASE.empty())
  {
    LOG(LOG_ERROR) << "CYTOPIA_DATA_DIR_BASE is not set! Please report this issue on github. Falling back to cytopia base dir.";
    pathToDataDir = fs::getBasePath() + (std::string)CYTOPIA_RESOURCES_DIR;
  }
  else
  {
    pathToDataDir = CYTOPIA_DATA_DIR;
  }
  std::string pathToSettingsFile = pathToDataDir + (std::string)SETTINGS_FILENAME;
  fs::createDirectory(pathToDataDir);
  fs::writeStringToFile(pathToSettingsFile, settingsJsonObject.dump());
}
