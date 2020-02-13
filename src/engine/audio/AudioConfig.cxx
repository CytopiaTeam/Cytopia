#include "AudioConfig.hxx"

void from_json(const json &j, AudioConfig &config)
{
  j["Music"].get_to(config.Music);
  j["Sound"].get_to(config.Sound);
}

void from_json(const json &j, AudioConfig::SoundtrackConfiguration &config)
{
  j["path"].get_to(config.stereoFilePath);
  j["monopath"].get_to(config.monoFilePath);
  std::vector<string> triggers;
  j["triggers"].get_to(triggers);
  std::transform(triggers.begin(), triggers.end(), std::back_inserter(config.triggers),
                 [](const string &trigger) { return AudioTrigger::_from_string(trigger.c_str()); });
}
