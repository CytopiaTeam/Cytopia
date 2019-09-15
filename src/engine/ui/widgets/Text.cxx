#include "Text.hxx"
#include "LOG.hxx"
#include "Exception.hxx"
#include "Settings.hxx"

#ifdef USE_MOFILEREADER
#include "moFileReader.h"
#endif

#include <SDL_ttf.h>

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
  createTextTexture(elementData.text, SDL_Color{255, 255, 255});
}

void Text::setFontSize(int fontSize)
{
  m_fontSize = fontSize;
  // call the setText function again with the current text
  setText(elementData.text);
}

void Text::createTextTexture(const std::string &text, const SDL_Color &textColor)
{
  LOG(LOG_DEBUG) << "Creating a Text texture";
  string fontFName = SDL_GetBasePath() + Settings::instance().fontFileName.get(); 
  
  /* @todo: Remove comment once we have support for the filesystem library
  if(!fs::is_regular_file(fontFName))
    throw ConfigurationError(TRACE_INFO "File " + fontFName.string() + " doesn't exist");
  */

  TTF_Font *font = TTF_OpenFont(fontFName.c_str(), m_fontSize);

  if (!font)
    throw FontError(TRACE_INFO "Failed to load font " + fontFName + ": " + TTF_GetError());

  // destroy texture first to prevent memleaks
  if (m_texture)
    SDL_DestroyTexture(m_texture);

  SDL_Surface *textSurface = TTF_RenderText_Solid(font, text.c_str(), textColor);
  TTF_CloseFont(font);
  
  if(!textSurface)
    throw UIError(TRACE_INFO "Failed to create text surface: " + string{TTF_GetError()});
  
  SDL_Rect _textRect{0, 0, 0, 0};

  m_texture = SDL_CreateTextureFromSurface(WindowManager::instance().getRenderer(), textSurface);

  // no surface exists but some shape has been drawn for that ui element
  SDL_QueryTexture(m_texture, nullptr, nullptr, &_textRect.w, &_textRect.h);

  _textRect.x = m_uiElementRect.x + (m_uiElementRect.w / 2) - (_textRect.w / 2);
  _textRect.y = m_uiElementRect.y + (m_uiElementRect.h / 2) - (_textRect.h / 2);
  m_uiElementRect = _textRect;

  /* Delete no longer needed surface */
  SDL_FreeSurface(textSurface);

  if (!m_texture)
    throw UIError(TRACE_INFO "Failed to create texture from text surface: " + string{SDL_GetError()});

}
