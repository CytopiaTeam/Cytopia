#include "UIElement.hxx"
#include "../../basics/LOG.hxx"

void UIElement::draw()
{
  if (m_texture)
  {
    renderTexture();
  }
}

void UIElement::setTextureID(const std::string &textureID)
{
  elementData.textureID = textureID;
  SDL_Texture *texture = ResourcesManager::instance().getUITexture(textureID);
  if (texture)
  {
    m_texture = texture;
  }
  SDL_QueryTexture(m_texture, nullptr, nullptr, &m_uiElementRect.w, &m_uiElementRect.h);
}

void UIElement::changeButtonState(int state)
{
  if (m_buttonState != state && !elementData.textureID.empty())
  {
    changeTexture(ResourcesManager::instance().getUITexture(elementData.textureID, state));
  }
  m_buttonState = state;
}

void UIElement::renderTexture() const
{
  if (m_texture)
  {
    SDL_RenderCopy(m_renderer, m_texture, nullptr, &m_uiElementRect);
  }
}

bool UIElement::isMouseOver(int x, int y)
{
  return x > m_uiElementRect.x && x < m_uiElementRect.x + m_uiElementRect.w && y > m_uiElementRect.y &&
         y < m_uiElementRect.y + m_uiElementRect.h;
}

bool UIElement::isMouseOverHoverableArea(int x, int y)
{
  return x > m_uiElementRect.x && x < m_uiElementRect.x + m_uiElementRect.w && y > m_uiElementRect.y &&
         y < m_uiElementRect.y + m_uiElementRect.h;
}

void UIElement::setText(const std::string &text)
{
  elementData.text = text;
  createTextTexture(elementData.text, SDL_Color{255, 255, 255});
}

void UIElement::createTextTexture(const std::string &text, const SDL_Color &textColor)
{
  m_font = TTF_OpenFont("resources/fonts/arcadeclassics.ttf", 20);

  if (!m_font)
  {
    LOG(LOG_ERROR) << "Failed to load font!\n" << TTF_GetError();
  }

  // destroy texture first to prevent memleaks
  if (m_texture)
    SDL_DestroyTexture(m_texture);

  SDL_Surface *textSurface = TTF_RenderText_Solid(m_font, text.c_str(), textColor);
  if (textSurface)
  {
    SDL_Rect _textRect{0, 0, 0, 0};

    m_texture = SDL_CreateTextureFromSurface(m_renderer, textSurface);

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

  TTF_CloseFont(m_font);
}

void UIElement::drawTextFrame() const
{
  if (m_uiElementRect.w != 0 && m_uiElementRect.h != 0)
  {
    drawSolidRect(m_uiElementRect, SDL_Color{150, 150, 150});
    drawSolidRect({m_uiElementRect.x - 2, m_uiElementRect.y - 2, m_uiElementRect.w + 1, m_uiElementRect.h + 1},
                  SDL_Color{128, 128, 128});
  }
}

void UIElement::drawSolidRect(SDL_Rect rect, const SDL_Color &color) const
{
  SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, SDL_ALPHA_OPAQUE);
  SDL_RenderFillRect(m_renderer, &rect);
}

void UIElement::drawLine(int x1, int y1, int x2, int y2, const SDL_Color &color) const
{
  SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, SDL_ALPHA_OPAQUE);
  SDL_RenderDrawLine(m_renderer, x1, y1, x2, y2);
}

void UIElement::drawButtonFrame(SDL_Rect rect, bool isHighlightable)
{
  Uint8 bgColor, bgColorFrame, bgColorFrameShade, bgColorBottomFrame, bgColorBottomFrameShade;

  if (getButtonState() == BUTTONSTATE_CLICKED && isHighlightable)
  {
    bgColor = 128;
    bgColorFrame = 106;
    bgColorFrameShade = 84;
    bgColorBottomFrame = 150;
    bgColorBottomFrameShade = 172;
  }
  else if ((getButtonState() == BUTTONSTATE_HOVERING) && isHighlightable)
  {
    bgColor = 228;
    bgColorFrame = 250;
    bgColorFrameShade = 255;
    bgColorBottomFrame = 206;
    bgColorBottomFrameShade = 184;
  }
  else
  {
    bgColor = 128;
    bgColorFrame = 150;
    bgColorFrameShade = 172;
    bgColorBottomFrame = 106;
    bgColorBottomFrameShade = 84;
  }

  drawSolidRect(rect, SDL_Color{bgColorFrameShade, bgColorFrameShade, bgColorFrameShade});
  drawSolidRect(SDL_Rect{rect.x + 2, rect.y + 2, rect.w - 4, rect.h - 4}, SDL_Color{bgColorFrame, bgColorFrame, bgColorFrame});
  // background
  drawSolidRect(SDL_Rect{rect.x + 4, rect.y + 4, rect.w - 8, rect.h - 8}, SDL_Color{bgColor, bgColor, bgColor});
  // bottom frame
  drawSolidRect(SDL_Rect{rect.x + 4, (rect.y + rect.h) - 4, rect.w - 4, 4},
                SDL_Color{bgColorBottomFrame, bgColorBottomFrame, bgColorBottomFrame});
  drawSolidRect(SDL_Rect{(rect.x + rect.w) - 4, rect.y + 4, 4, rect.h - 4},
                SDL_Color{bgColorBottomFrame, bgColorBottomFrame, bgColorBottomFrame});
  // bottom frame shade
  drawSolidRect(SDL_Rect{rect.x + 2, (rect.y + rect.h) - 2, rect.w - 2, 2},
                SDL_Color{bgColorBottomFrameShade, bgColorBottomFrameShade, bgColorBottomFrameShade});
  drawSolidRect(SDL_Rect{(rect.x + rect.w) - 2, rect.y + 2, 2, rect.h - 2},
                SDL_Color{bgColorBottomFrameShade, bgColorBottomFrameShade, bgColorBottomFrameShade});
}

void UIElement::drawFrame(SDL_Rect rect) const
{
  const Uint8 bgColor = 128;
  const Uint8 bgColorFrame = 150;
  const Uint8 bgColorFrameShade = 172;

  drawSolidRect(rect, SDL_Color{bgColorFrame, bgColorFrame, bgColorFrame});
  drawSolidRect(SDL_Rect{rect.x + 2, rect.y + 2, rect.w - 4, rect.h - 4},
                SDL_Color{bgColorFrameShade, bgColorFrameShade, bgColorFrameShade});
  drawSolidRect(SDL_Rect{rect.x + 4, rect.y + 4, rect.w - 8, rect.h - 8}, SDL_Color{bgColorFrame, bgColorFrame, bgColorFrame});
  drawSolidRect(SDL_Rect{rect.x + 6, rect.y + 6, rect.w - 12, rect.h - 12}, SDL_Color{bgColor, bgColor, bgColor});
}
