#include "combobox.hxx"

#include "../../basics/log.hxx"

ComboBox::ComboBox(const SDL_Rect &uiElementRect)
    : UiElement(uiElementRect), m_dropDownRect(uiElementRect), m_buttonLabel(std::make_unique<Text>())
{
  m_dropDownRect.y = m_uiElementRect.y + m_uiElementRect.h;
  m_textField = std::make_unique<TextField>(m_dropDownRect);

  m_textField->addText("test");
  m_textField->addText("awesome element");
  m_textField->addText("one more element");

  m_buttonLabel->setText(m_textField->getTextFromID(0));

  //set menu to same height as textfield
  m_dropDownRect.h = m_textField->getUiElementRect().h;

  // Rect for the whole element
  m_wholeElementRect = m_uiElementRect;
  m_wholeElementRect.h = m_dropDownRect.h + m_uiElementRect.h;

  m_textField->setVisibility(false);

  // center the text in dropdown menu
  m_textField->textAlignment = TextFieldAlignment::CENTERED;

  centerTextLabel();
}

void ComboBox::draw()
{
  Uint8 bgColorFrameShade = 172;

  // draw the button frame
  if (m_uiElementRect.w != 0 && m_uiElementRect.h != 0)
  {
    drawButtonFrame(m_uiElementRect);
  }

  // draw the arrow
  SDL_Rect arrowRect;
  SDL_Color color{bgColorFrameShade, bgColorFrameShade, bgColorFrameShade};

  arrowRect.w = 18;
  arrowRect.h = 2;
  arrowRect.x = m_uiElementRect.x + (m_uiElementRect.w - arrowRect.w) - 9;
  arrowRect.y = m_uiElementRect.y + (m_uiElementRect.h / 2) - (arrowRect.w / 2) / arrowRect.h - 1;

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
    drawButtonFrame(m_dropDownRect, false);
    m_textField->draw();
  }
  m_buttonLabel->draw();
}

void ComboBox::setPosition(int x, int y)
{
  m_uiElementRect.x = x;
  m_uiElementRect.y = y;

  m_dropDownRect.x = x;
  m_dropDownRect.y = m_uiElementRect.y + m_uiElementRect.h;

  m_wholeElementRect.x = x;
  m_wholeElementRect.y = y;

  m_textField->setPosition(m_dropDownRect.x, m_dropDownRect.y);
  centerTextLabel();
}

void ComboBox::centerTextLabel()
{
  int x = m_uiElementRect.x + m_uiElementRect.w / 2 - m_buttonLabel->getUiElementRect().w / 2;
  int y = m_uiElementRect.y + m_uiElementRect.h / 2 - m_buttonLabel->getUiElementRect().h / 2;
  m_buttonLabel->setPosition(x, y);
}

bool ComboBox::isMouseOverHoverableArea(int x, int y) { return checkBoundaries(x, y); }

bool ComboBox::isMouseOver(int x, int y) { return checkBoundaries(x, y); }

bool ComboBox::checkBoundaries(int x, int y)
{
  if (m_isMenuOpened)
  {
    return x > m_wholeElementRect.x && x < (m_wholeElementRect.x + m_wholeElementRect.w) && y > m_wholeElementRect.y &&
           y < (m_wholeElementRect.y + m_wholeElementRect.h);
  }
  return x > m_uiElementRect.x && x < (m_uiElementRect.x + m_uiElementRect.w) && y > m_uiElementRect.y &&
         y < (m_uiElementRect.y + m_uiElementRect.h);
}

bool ComboBox::onMouseButtonUp(const SDL_Event &event)
{
  int x = event.button.x;
  int y = event.button.y;

  if (checkBoundaries(x, y))
  {
    // check if the mouse is over the button part of the combox box.
    if (x > m_uiElementRect.x && x < m_uiElementRect.x + m_uiElementRect.w && y > m_uiElementRect.y &&
        y < m_uiElementRect.y + m_uiElementRect.h)
    {
      // toggle the dropdown menu, it's textfield and set the buttonstate to hovering
      m_isMenuOpened = !m_isMenuOpened;
      m_textField->setVisibility(m_isMenuOpened);
      changeButtonState(BUTTONSTATE_HOVERING);
      return true;
    }

    // if the click event is outside the button, handle a click on the dropdown menu
    m_textField->onMouseButtonUp(event); //trigger TextField onMouseButtonUp event
    m_activeID = m_textField->selectedID;
    activeText = m_textField->getTextFromID(m_activeID);
    m_buttonLabel->setText(activeText);
    m_isMenuOpened = false;
    m_textField->setVisibility(false);
    return true;
  }
  return false;
}

bool ComboBox::onMouseButtonDown(const SDL_Event &event)
{
  if (checkBoundaries(event.button.x, event.button.y))
  {
    changeButtonState(BUTTONSTATE_DEFAULT);
    return true;
  }
  return false;
}

void ComboBox::onMouseLeave(const SDL_Event &event)
{
  m_textField->onMouseLeave(event);
  changeButtonState(BUTTONSTATE_DEFAULT);
}

void ComboBox::onMouseMove(const SDL_Event &event)
{
  int x = event.button.x;
  int y = event.button.y;

  if (checkBoundaries(x, y))
  {

    // if the mouse is moving over the button, handle the button alone
    if (x > m_uiElementRect.x && x < m_uiElementRect.x + m_uiElementRect.w && y > m_uiElementRect.y &&
        y < m_uiElementRect.y + m_uiElementRect.h)
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
      changeButtonState(BUTTONSTATE_DEFAULT);
      {
        m_textField->onMouseMove(event);
      }
    }
  }
}
