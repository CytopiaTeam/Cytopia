#include "Localization.hxx"
#include "Filesystem.hxx"
#include "../util/LOG.hxx"

using std::string;

Localization ::Localization() : m_MoFileReader{}, m_IsInitilaized{false} {}

void Localization::load(const string &language)
{
  m_MoFileReader.ClearTable();
  string moFilePath = fs::getBasePath() + "languages/" + language + "/Cytopia.mo";

  if (m_MoFileReader.ReadFile(moFilePath.c_str()) == moFileLib::moFileReader::EC_SUCCESS)
  {
    LOG(LOG_INFO) << "Localization: Loaded MO file " << moFilePath;
    m_IsInitilaized = true;
  }
  else
  {
    LOG(LOG_ERROR) << "Localization: Failed to load MO file " << moFilePath;
  }
}

string Localization::translateText(const string &text) const { return translateText(text.c_str()); }

string Localization::translateText(const char *text) const { return m_IsInitilaized ? m_MoFileReader.Lookup(text) : text; }
