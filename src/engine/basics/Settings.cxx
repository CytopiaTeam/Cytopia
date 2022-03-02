#include "Settings.hxx"

#include "LOG.hxx"
#include "Exception.hxx"
#include "Constants.hxx"
#include "JsonSerialization.hxx"
#include "Filesystem.hxx"

#include <iomanip>

Settings::Settings() { readFile(); }

Settings::~Settings()
{
  LOG(LOG_DEBUG) << "Destroying Setttings";
}

void Settings::readFile()
{
  std::string jsonFile = fs::readFileAsString(SETTINGS_FILE_NAME);
  const json _settingsJSONObject = json::parse(jsonFile, nullptr, false);

  // check if json file can be parsed
  if (_settingsJSONObject.is_discarded())
    throw ConfigurationError(TRACE_INFO "Error parsing JSON File " + string{SETTINGS_FILE_NAME});

  SettingsData data = _settingsJSONObject;
  *this = data;

#ifdef __ANDROID__
  subMenuButtonHeight *= 2;
  subMenuButtonWidth *= 2;
#endif
}

void Settings::writeFile()
{
  const json settingsJsonObject = *this;
  fs::writeStringToFile(SETTINGS_FILE_NAME, settingsJsonObject.dump(2));
}

void Settings::parse_args(int argc, char** argv) {
  if(argc == 1) {
    return;
  }
  json patch;
  ++argv;
  while(argc --> 1) {
    if(argv[0][0] != '-' || argv[0][1] != '-') {
      throw ConfigurationError{TRACE_INFO "Invalid argument: " + std::string(argv[0]) };
    }
    std::string key_str(argv[0] + 2);
    std::replace(key_str.begin(), key_str.end(), '.', '/');
    key_str = "/" + key_str;
    json::json_pointer key(key_str);
    --argc;
    ++argv;
    if(argc == 0) {
      throw ConfigurationError{TRACE_INFO "Missing parameter value for: " + key_str };
    }
    std::string value(argv[0]);
    patch[key] = json::parse(value, nullptr, false);
    // Allow strings
    if(patch[key].is_discarded()) {
      patch[key] = value;
    }
  }
  /**
   *  @todo This is a lazy implementation... We probably should get rid of singleton 
   *        and parse arguments in the constructor instead
   */
  json js = *this;
  js.merge_patch(patch);
  SettingsData data = js;
  *this = data;
}

int Settings::getDefaultWindowWidth() const noexcept
{
  return displayModes.at(defaultDisplayMode)[0];
}
  
int Settings::getDefaultWindowHeight() const noexcept
{
  return displayModes.at(defaultDisplayMode)[1];
}
