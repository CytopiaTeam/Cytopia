#include "Text.hxx"
#include <LOG.hxx>
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
  LOG(LOG_INFO) << "Translatable string: " << text;
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
  TTF_Font *font = TTF_OpenFont(Settings::instance().fontFileName.get().c_str(), m_fontSize);

  if (!font)
  {
    LOG(LOG_ERROR) << "Failed to load font!\n" << TTF_GetError();
    //@todo throw an error when the exception handling improvements have been merged
    return;
  }

  // destroy texture first to prevent memleaks
  if (m_texture)
    SDL_DestroyTexture(m_texture);

  SDL_Surface *textSurface = TTF_RenderText_Solid(font, text.c_str(), textColor);
  if (textSurface)
  {
    SDL_Rect _textRect{0, 0, 0, 0};

    m_texture = SDL_CreateTextureFromSurface(WindowManager::instance().getRenderer(), textSurface);

    // no surface exists but some shape has been drawn for that ui element
    SDL_QueryTexture(m_texture, nullptr, nullptr, &_textRect.w, &_textRect.h);

    _textRect.x = m_uiElementRect.x + (m_uiElementRect.w / 2) - (_textRect.w / 2);
    _textRect.y = m_uiElementRect.y + (m_uiElementRect.h / 2) - (_textRect.h / 2);
    m_uiElementRect = _textRect;

    if (!m_texture)
    {
      LOG(LOG_ERROR) << "Failed to create texture from text surface!\n" << SDL_GetError();
    }
    //Delete no longer needed surface
    SDL_FreeSurface(textSurface);
  }
  else
  {
    LOG(LOG_ERROR) << "Failed to create text surface!\n" << TTF_GetError();
  }

  TTF_CloseFont(font);
}
