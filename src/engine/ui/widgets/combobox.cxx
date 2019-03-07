#include "combobox.hxx"

ComboBox::ComboBox(const SDL_Rect &uiElementRect)
    : UiElement(uiElementRect), m_comboBoxRect(uiElementRect), m_menuRect(uiElementRect)
{
  m_menuRect.y = m_comboBoxRect.y + m_comboBoxRect.h;

  m_textField = std::make_unique<TextField>(m_menuRect);

  m_textField->addText("test");
  m_textField->addText("awesome element");
  m_textField->addText("one more element");

  activeText = m_textField->getTextFromID(0);

  //set menu to same height as textfield
  m_menuRect.h = m_textField->getUiElementRect().h;
  m_textField->setVisibility(false);
}

void ComboBox::draw()
{
  Uint8 bgColorFrameShade = 172;

  // draw the button frame
  if (m_comboBoxRect.w != 0 && m_comboBoxRect.h != 0)
  {
    drawButtonFrame(m_comboBoxRect);
  }

  // draw the arrow
  SDL_Rect arrowRect;
  SDL_Color color {bgColorFrameShade, bgColorFrameShade, bgColorFrameShade};

  arrowRect.w = 18;
  arrowRect.h = 2;
  arrowRect.x = m_comboBoxRect.x + (m_comboBoxRect.w - arrowRect.w) - 9;
  arrowRect.y = m_comboBoxRect.y + (m_comboBoxRect.h / 2) - (arrowRect.w / 2) / arrowRect.h - 1;

  for (; arrowRect.w > 1; arrowRect.w -= 2)
  {
    drawSolidRect(arrowRect, color);
    arrowRect.x++;
    arrowRect.y++;
  }
  drawSolidRect(arrowRect, color);

  // drowpdown menu
  if (m_isMenuOpened)
  {
    drawButtonFrame(m_menuRect, false);
    m_textField->draw();
  }

  createTextTexture(activeText, {255, 255, 255});

  //render the buttons texture if available
  renderTexture();
}

bool ComboBox::isMouseOverHoverableArea(int x, int y)
{
  SDL_Rect boundaries = m_comboBoxRect;

  if (m_isMenuOpened)
  {
    boundaries.h += m_menuRect.h;
  }

  return x > boundaries.x && x < boundaries.x + boundaries.w && y > boundaries.y && y < boundaries.y + boundaries.h;
}

bool ComboBox::isMouseOver(int x, int y)
{
  SDL_Rect boundaries = m_comboBoxRect;

  if (m_isMenuOpened)
  {
    boundaries.h += m_menuRect.h;
  }

  return x > boundaries.x && x < boundaries.x + boundaries.w && y > boundaries.y && y < boundaries.y + boundaries.h;
}

void ComboBox::onMouseButtonUp(const SDL_Event &event)
{
  int x = event.button.x;
  int y = event.button.y;

  if (x > m_comboBoxRect.x && x < m_comboBoxRect.x + m_comboBoxRect.w && y > m_comboBoxRect.y &&
      y < m_comboBoxRect.y + m_comboBoxRect.h)
  {
    m_isMenuOpened = !m_isMenuOpened;
    m_textField->setVisibility(!m_textField->isVisible());
    changeButtonState(BUTTONSTATE_HOVERING);
    return;
  }

  if (m_isMenuOpened)
  {
    m_textField->onMouseButtonUp(event); //trigger TextField onMouseButtonUp event
  }

  if (m_isMenuOpened)
  {
    activeID = m_textField->selectedID;
    activeText = m_textField->getTextFromID(activeID);
    m_isMenuOpened = false;
    m_textField->setVisibility(false);
  }
}

void ComboBox::onMouseButtonDown(const SDL_Event &) { changeButtonState(BUTTONSTATE_DEFAULT); }

void ComboBox::onMouseLeave(const SDL_Event &event)
{
  m_textField->onMouseLeave(event);
  changeButtonState(BUTTONSTATE_DEFAULT);
}

void ComboBox::onMouseMove(const SDL_Event &event)
{
  int x = event.button.x;
  int y = event.button.y;

  if (x > m_comboBoxRect.x && x < m_comboBoxRect.x + m_comboBoxRect.w && y > m_comboBoxRect.y &&
      y < m_comboBoxRect.y + m_comboBoxRect.h)
  {
    if (getButtonState() != BUTTONSTATE_HOVERING)
    {
      changeButtonState(BUTTONSTATE_HOVERING);
    }
    if (m_textField->hoveredID != -1)
    {
      m_textField->onMouseLeave(event);
    }
  }
  else
  {
    if (m_isMenuOpened)
    {
      changeButtonState(BUTTONSTATE_DEFAULT);
      {
        m_textField->onMouseMove(event);
      }
    }
  }
}
