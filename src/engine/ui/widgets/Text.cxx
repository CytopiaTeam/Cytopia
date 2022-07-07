#include "Text.hxx"
#include "LOG.hxx"
#include "Exception.hxx"
#include "Settings.hxx"
#include "Filesystem.hxx"

#ifdef USE_MOFILEREADER
#include "moFileReader.h"
#endif

#include <SDL_ttf.h>

Text::~Text()
{
  if (m_texture)
    SDL_DestroyTexture(m_texture);
}

void Text::draw()
{
  if (m_texture && isVisible())
  {
    renderTexture();
  }
}

void Text::setText(const std::string &text)
{
#ifdef USE_MOFILEREADER
  elementData.text = moFileLib::moFileReaderSingleton::GetInstance().Lookup(text.c_str());
#else
  elementData.text = text;
#endif
  if (!text.empty())
    createTextTexture(elementData.text, SDL_Color{255, 255, 255});
}

void Text::setFontSize(int fontSize)
{
  if (fontSize == m_fontSize)
    return;

  m_fontSize = fontSize;
  // call the setText function again with the current text
  setText(elementData.text);
}

void Text::createTextTexture(const std::string &text, const SDL_Color &textColor)
{
  string fontFName = fs::getBasePath() + Settings::instance().fontFileName.get();

  if (!fs::fileExists(fontFName))
    throw ConfigurationError(TRACE_INFO "File " + fontFName + " doesn't exist");

  TTF_Font *font = TTF_OpenFont(fontFName.c_str(), m_fontSize);

  if (!font)
    throw FontError(TRACE_INFO "Failed to load font " + fontFName + ": " + TTF_GetError());

  // destroy texture first to prevent memleaks
  if (m_texture)
    ResourcesManager::instance().destroyTexture(m_texture);

  SDL_Surface *textSurface = TTF_RenderText_Solid(font, text.c_str(), textColor);
  TTF_CloseFont(font);

  if (!textSurface)
    throw UIError(TRACE_INFO "Failed to create text surface: " + string{TTF_GetError()});

  m_uiElementRect.w = textSurface->w;
  m_uiElementRect.h = textSurface->h;

  ResourcesManager::instance().createTextureAsync({textSurface, /*destroySurface*/true, "Text::createTextTexture", [this] (auto *texture) {
    this->m_texture = texture;
  }});
}

