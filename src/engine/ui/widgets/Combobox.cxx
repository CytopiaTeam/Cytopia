#include "Combobox.hxx"

#include "LOG.hxx"

ComboBox::ComboBox(const SDL_Rect &uiElementRect)
    : UIElement(uiElementRect), m_dropdownRect(uiElementRect), m_wholeElementRect(uiElementRect),
      m_dropdownMenu(std::make_unique<DropdownMenu>(m_dropdownRect)), m_selectedItem(std::make_unique<Text>())
{
  // the Dropdown frame starts directly beneath the button element of the combobox
  m_dropdownRect.y = m_uiElementRect.y + m_uiElementRect.h;
  m_dropdownMenu->setPosition(m_dropdownRect.x, m_dropdownRect.y);

  // ComboBox is collapsed when element is instantiated
  m_dropdownMenu->setVisibility(false);

  // set TextFields text alignment to center as default
  // REDUNDANT
  // m_dropdownMenu->textAlignment = TextFieldAlignment::CENTERED;
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

  // dropdown menu
  if (m_isMenuOpened)
  {
    drawButtonFrame(m_dropdownRect, false);
    m_dropdownMenu->draw();
  }
  m_selectedItem->draw();
}

void ComboBox::setPosition(int x, int y)
{
  // set the button elements position
  m_uiElementRect.x = x;
  m_uiElementRect.y = y;

  // reposition the dropdown menu frame
  m_dropdownRect.x = x;
  m_dropdownRect.y = m_uiElementRect.y + m_uiElementRect.h;

  // adjust the rect that represents the whole element
  m_wholeElementRect.x = x;
  m_wholeElementRect.y = y;

  // reposition the text field
  m_dropdownMenu->setPosition(m_dropdownRect.x, m_dropdownRect.y);

  // center the Label on the button element
  centerTextLabel();
}

void ComboBox::centerTextLabel() const
{
  int x = m_uiElementRect.x + m_uiElementRect.w / 2 - m_selectedItem->getUiElementRect().w / 2;
  int y = m_uiElementRect.y + m_uiElementRect.h / 2 - m_selectedItem->getUiElementRect().h / 2;
  m_selectedItem->setPosition(x, y);
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
  m_dropdownMenu->addItem(text);

  // if the added element is the active one, set the main text to the added elements text.
  if (m_activeID == static_cast<int>(m_dropdownMenu->count() - 1))
  {
    m_selectedItem->setText(m_dropdownMenu->getTextFromID(m_activeID));
    centerTextLabel();
  }

  //set dropdown frame to the same height as DropdownMenu
  m_dropdownRect.h = m_dropdownMenu->getUiElementRect().h;

  // increase the size of the rect that represents the whole element
  m_wholeElementRect.h = m_uiElementRect.h + m_dropdownRect.h;
}

void ComboBox::setActiveID(int ID)
{
  m_activeID = ID;
  m_selectedItem->setText(m_dropdownMenu->getTextFromID(m_activeID));
  clickSignalSender.emit(this);
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
      m_dropdownMenu->setVisibility(m_isMenuOpened);
      changeButtonState(BUTTONSTATE_HOVERING);
      return true;
    }

    // if the click event is outside the button, handle a click on the dropdown menu
    m_dropdownMenu->onMouseButtonUp(event); //trigger DropdownMenu onMouseButtonUp event
    m_activeID = m_dropdownMenu->selectedID;
    activeText = m_dropdownMenu->getTextFromID(m_activeID);
    m_selectedItem->setText(activeText);
    m_isMenuOpened = false;
    m_dropdownMenu->setVisibility(false);
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
  // m_dropdownMenu->onMouseLeave(event);
  m_dropdownMenu->hoveredID = -1;
  changeButtonState(BUTTONSTATE_DEFAULT);
}

void ComboBox::onMouseMove(const SDL_Event &event)
{
  int x = event.button.x;
  int y = event.button.y;

  // LOG(LOG_INFO) << "ComboBox text field: " << m_dropdownMenu->
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
      if (m_dropdownMenu->hoveredID != -1)
      {
        m_dropdownMenu->onMouseLeave(event);
      }
    }
    else
    {
      changeButtonState(BUTTONSTATE_DEFAULT);
      {
        m_dropdownMenu->onMouseMove(event);
      }
    }
  }
}

void ComboBox::registerCallbackFunction(std::function<void(UIElement *sender)> const &cb) { clickSignalSender.connect(cb); }
