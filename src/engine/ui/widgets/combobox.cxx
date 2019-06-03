#include "combobox.hxx"

#include "../../basics/log.hxx"

ComboBox::ComboBox(const SDL_Rect &uiElementRect)
    : UiElement(uiElementRect), m_dropDownRect(uiElementRect), m_wholeElementRect(uiElementRect),
      m_buttonLabel(std::make_unique<Text>())
{
  // the Dropdown frame starts directly beneath the button element of the combobox
  m_dropDownRect.y = m_uiElementRect.y + m_uiElementRect.h;
  m_textField = std::make_unique<TextField>(m_dropDownRect);

  // ComboBox is collapsed when element is instantiated
  m_textField->setVisibility(false);

  // set TextFields text alignment to center as default
  m_textField->textAlignment = TextFieldAlignment::CENTERED;
}

void ComboBox::draw()
{
  const Uint8 bgColorFrameShade = 172;

  // draw the button frame
  if (m_uiElementRect.w != 0 && m_uiElementRect.h != 0)
  {
    drawButtonFrame(m_uiElementRect);
  }

  // draw the arrow
  SDL_Rect arrowRect;
  const SDL_Color color{bgColorFrameShade, bgColorFrameShade, bgColorFrameShade};

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

  // dropDown menu
  if (m_isMenuOpened)
  {
    drawButtonFrame(m_dropDownRect, false);
    m_textField->draw();
  }
  m_buttonLabel->draw();
}

void ComboBox::setPosition(int x, int y)
{
  // set the button elements position
  m_uiElementRect.x = x;
  m_uiElementRect.y = y;

  // reposition the dropdown menu frame
  m_dropDownRect.x = x;
  m_dropDownRect.y = m_uiElementRect.y + m_uiElementRect.h;

  // adjust the rect that represents the whole element
  m_wholeElementRect.x = x;
  m_wholeElementRect.y = y;

  // reposition the text field
  m_textField->setPosition(m_dropDownRect.x, m_dropDownRect.y);

  // center the Label on the button element
  centerTextLabel();
}

void ComboBox::centerTextLabel() const
{
  int x = m_uiElementRect.x + m_uiElementRect.w / 2 - m_buttonLabel->getUiElementRect().w / 2;
  int y = m_uiElementRect.y + m_uiElementRect.h / 2 - m_buttonLabel->getUiElementRect().h / 2;
  m_buttonLabel->setPosition(x, y);
}

bool ComboBox::isMouseOverHoverableArea(int x, int y) { return checkBoundaries(x, y); }

bool ComboBox::isMouseOver(int x, int y) { return checkBoundaries(x, y); }

bool ComboBox::checkBoundaries(int x, int y) const
{
  if (m_isMenuOpened)
  {
    return x > m_wholeElementRect.x && x < (m_wholeElementRect.x + m_wholeElementRect.w) && y > m_wholeElementRect.y &&
           y < (m_wholeElementRect.y + m_wholeElementRect.h);
  }
  return x > m_uiElementRect.x && x < (m_uiElementRect.x + m_uiElementRect.w) && y > m_uiElementRect.y &&
         y < (m_uiElementRect.y + m_uiElementRect.h);
}

void ComboBox::addElement(const std::string &text)
{
  m_textField->addText(text);

  // if the added element is the active one, set the main text to the added elements text.
  if (m_activeID == static_cast<int>(m_textField->count() - 1))
  {
    m_buttonLabel->setText(m_textField->getTextFromID(m_activeID));
    centerTextLabel();
  }

  //set dropdown frame to the same height as textfield
  m_dropDownRect.h = m_textField->getUiElementRect().h;

  // increase the size of the rect that represents the whole element
  m_wholeElementRect.h = m_uiElementRect.h + m_dropDownRect.h;
}

void ComboBox::setActiveID(int ID)
{
  m_activeID = ID;
  m_buttonLabel->setText(m_textField->getTextFromID(m_activeID));
}

bool ComboBox::onMouseButtonUp(const SDL_Event &event)
{
  int x = event.button.x;
  int y = event.button.y;

  if (checkBoundaries(x, y))
  {
    // check if the mouse is over the button part of the combobox box.
    if (x > m_uiElementRect.x && x < m_uiElementRect.x + m_uiElementRect.w && y > m_uiElementRect.y &&
        y < m_uiElementRect.y + m_uiElementRect.h)
    {
      // toggle the dropdown menu, it's textfield and set the buttonState to hovering
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
    centerTextLabel();
    clickSignalSender.emit(this);
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

void ComboBox::registerCallbackFunction(std::function<void(UiElement *sender)> const &cb) { clickSignalSender.connect(cb); }
