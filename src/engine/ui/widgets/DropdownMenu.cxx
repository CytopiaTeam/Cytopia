#include "DropdownMenu.hxx"
#include "Text.hxx"

DropdownMenu::DropdownMenu(const SDL_Rect &uiElementRect) : UIElement(uiElementRect), m_highlightingRect(uiElementRect)
{
  // initialize height with an offset of 4 so the frame doesn't overlap with the last text element
  // this elements height will be adjusted accordingly when the textField is filled.
  m_uiElementRect.h = 4;
  m_highlightingRect = m_uiElementRect;
  m_highlightingRect.x += 4;
  m_highlightingRect.w -= 8;
}

void DropdownMenu::draw()
{
  if (isVisible())
  {
    if (hoveredID != -1)
    {
      drawSolidRect(m_highlightingRect, SDL_Color({150, 150, 150}));
    }
    for (const auto &text : m_items)
    {
      text->draw();
    }
  }
}

void DropdownMenu::addItem(const std::string &text)
{
  SDL_Rect textRect = m_uiElementRect;

  Text *label = new Text();
  label->setText(text);
  textRect.h = label->getUiElementRect().h; // get height of text after instantiating
  textRect.y = static_cast<int>(m_uiElementRect.y + m_numItems * textRect.h);

  // center text
  if (centerText)
  {
    textRect.x = m_uiElementRect.x + (m_uiElementRect.w / 2 - label->getUiElementRect().w / 2);
  }

  m_itemHeight = textRect.h;

  m_uiElementRect.h += m_itemHeight;
  m_highlightingRect.h = m_itemHeight;

  label->setPosition(textRect.x, textRect.y);
  m_items.emplace_back(label);
  m_numItems = static_cast<int>(m_items.size());
}

std::string DropdownMenu::getTextFromID(int id) const
{
  if (id < static_cast<int>(m_numItems) && static_cast<int>(m_numItems) > 0 && id >= 0)
  {
    return m_items[id]->getUiElementData().text;
  }
  return "";
}

void DropdownMenu::setPosition(int x, int y)
{
  m_uiElementRect.x = x;
  m_uiElementRect.y = y;
  m_highlightingRect.x = x + 4;

  int currentElement = 0;
  for (const auto &text : m_items)
  {
    //SDL_Rect textRect;
    switch (textAlignment)
    {
    case TextFieldAlignment::RIGHT:
      x = m_uiElementRect.x + m_uiElementRect.w - text->getUiElementRect().w;
      break;
    case TextFieldAlignment::CENTERED:
      x = m_uiElementRect.x + (m_uiElementRect.w / 2 - text->getUiElementRect().w / 2);
      break;
    default: //AKA TextFieldAlignment::LEFT
      // for LEFT alignment, we use the same values as the uiElementRect
      break;
    }
    y = m_uiElementRect.y + (m_itemHeight * currentElement++);
    text->setPosition(x, y);
  }
}

bool DropdownMenu::onMouseButtonUp(const SDL_Event &event)
{
  if (isMouseOver(event.button.x, event.button.y))
  {
    if (!m_items.empty())
    {
      selectedID = ((m_itemHeight + event.button.y - m_uiElementRect.y) / m_itemHeight) - 1;
      // because of the -4 pixel offset that's been added in the constructor, the id would exceed the size of the vector, if the bottom of the dropdown is clicked

      if (selectedID >= static_cast<int>(m_numItems))
      {
        selectedID = static_cast<int>(m_numItems - 1);
      }
    }
    return true;
  }
  return false;
}

void DropdownMenu::onMouseMove(const SDL_Event &event)
{
  // TODO: Calculation deviates by 4 pixel in height because of the frame
  if (hoveredID != ((m_itemHeight + event.button.y - m_uiElementRect.y) / m_itemHeight) - 1)
  {
    hoveredID = ((m_itemHeight + event.button.y - m_uiElementRect.y) / m_itemHeight) - 1;
    // because of the -4 pixel offset that's been added in the constructor, the id would exceed the size of the vector, if the bottom of the dropdown is clicked
    if (hoveredID >= static_cast<int>(m_numItems))
    {
      hoveredID = static_cast<int>(m_numItems - 1);
    }
    m_highlightingRect.y = ((hoveredID)*m_itemHeight) + m_uiElementRect.y;
  }
}

void DropdownMenu::onMouseLeave(const SDL_Event &)
{
  // reset the hovering
  hoveredID = -1;
}