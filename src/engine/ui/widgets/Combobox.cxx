#include "Combobox.hxx"

#include "LOG.hxx"
#include "Text.hxx"

ComboBox::ComboBox(const SDL_Rect &uiElementRect)
    : UIElement(uiElementRect), m_dropdownRect(uiElementRect), m_wholeElementRect(uiElementRect),
      m_highlightingRect(m_dropdownRect), m_selectedItem(std::make_unique<Text>())
{
  // initialize height with an offset of 4 so the frame doesn't overlap with the last text element
  // this elements height will be adjusted accordingly when the textField is filled.
  m_dropdownRect.h = 4;
  m_highlightingRect.x += 4;
  m_highlightingRect.w -= 8;
  // the Dropdown frame starts directly beneath the button element of the combobox
  m_dropdownRect.y = m_uiElementRect.y + m_uiElementRect.h;
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
    if (m_hoveredID != -1)
    {
      drawSolidRect(m_highlightingRect, SDL_Color({150, 150, 150}));
    }
    for (const auto &text : m_items)
    {
      text->draw();
    }
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
  m_dropdownRect.x = x;
  m_dropdownRect.y = y;
  m_highlightingRect.x = x + 4;

  int currentElement = 0;
  for (const auto &text : m_items)
  {
    //SDL_Rect textRect;
    switch (textAlignment)
    {
    case TextFieldAlignment::RIGHT:
      x = m_dropdownRect.x + m_dropdownRect.w - text->getUiElementRect().w;
      break;
    case TextFieldAlignment::CENTERED:
      x = m_dropdownRect.x + (m_dropdownRect.w / 2 - text->getUiElementRect().w / 2);
      break;
    case TextFieldAlignment::LEFT:
      // for LEFT alignment, we use the same values as the dropdownRect
      x = m_dropdownRect.x + m_dropdownRect.w;
      break;
    }
    y = m_dropdownRect.y + (m_itemHeight * currentElement++);
    text->setPosition(x, y);
  }

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
  SDL_Rect textRect = m_dropdownRect;

  Text *label = new Text();
  label->setText(text);
  textRect.h = label->getUiElementRect().h; // get height of text after instantiating
  textRect.y = static_cast<int>(m_dropdownRect.y + count() * textRect.h);

  // center text
  if (textAlignment == TextFieldAlignment::CENTERED)
  {
    textRect.x = m_dropdownRect.x + (m_dropdownRect.w / 2 - label->getUiElementRect().w / 2);
  }

  m_itemHeight = textRect.h;

  m_dropdownRect.h += m_itemHeight;
  m_highlightingRect.h = m_itemHeight;

  label->setPosition(textRect.x, textRect.y);
  m_items.emplace_back(label);

  // if the added element is the active one, set the main text to the added elements text.
  if (m_activeID == static_cast<int>(count() - 1))
  {
    m_selectedItem->setText(getTextFromID(m_activeID));
    centerTextLabel();
  }

  // increase the size of the rect that represents the whole element
  m_wholeElementRect.h = m_uiElementRect.h + m_dropdownRect.h;
}

void ComboBox::setActiveID(int ID)
{
  m_activeID = ID;
  m_selectedItem->setText(getTextFromID(ID));
  clickSignalSender.emit(this);
}

void ComboBox::clear() 
{ 
  m_items.clear();
  // m_selectedItem = nullptr;
}

std::string ComboBox::getTextFromID(int id) const
{
  if (id < static_cast<int>(count()) && static_cast<int>(count()) > 0 && id >= 0)
  {
    return m_items[id]->getUiElementData().text;
  }
  return "";
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
      // toggle the dropdown menu and set the buttonState to hovering
      m_isMenuOpened = !m_isMenuOpened;
      changeButtonState(BUTTONSTATE_HOVERING);
      return true;
    }

    // if the click event is outside the button, handle a click on the dropdown menu
    if (!m_items.empty())
    {
      setActiveID(((m_itemHeight + event.button.y - m_dropdownRect.y) / m_itemHeight) - 1);
      // because of the -4 pixel offset that's been added in the constructor, the id would exceed the size of the vector, if the bottom of the dropdown is clicked

      if (m_activeID >= static_cast<int>(count()))
      {
        setActiveID(static_cast<int>(count() - 1));
      }
    }
    m_isMenuOpened = false;
    centerTextLabel();
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
  m_hoveredID = -1;
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
      m_hoveredID = -1;
    }
    else
    {
      changeButtonState(BUTTONSTATE_DEFAULT);
      m_hoveredID = ((m_itemHeight + event.button.y - m_dropdownRect.y) / m_itemHeight) - 1;
      // because of the -4 pixel offset that's been added in the constructor, the id would exceed the size of the vector, if the bottom of the dropdown is clicked
      if (m_hoveredID >= static_cast<int>(count()))
      {
        m_hoveredID = static_cast<int>(count() - 1);
      }
      m_highlightingRect.y = (m_hoveredID * m_itemHeight) + m_dropdownRect.y;
    }
  }
}

std::string ComboBox::getActiveText() const
{
  if (m_selectedItem && count() > 0) // the count check is a temporary solution
    return m_selectedItem->getUiElementData().text;
  return "";
}

void ComboBox::registerCallbackFunction(std::function<void(UIElement *sender)> const &cb) { clickSignalSender.connect(cb); }
