#include "textField.hxx"
#include "text.hxx"

TextField::TextField(const SDL_Rect &uiElementRect) : UiElement(uiElementRect), m_highlightingRect(uiElementRect)
{
  // initialize height with an offset of 4 so the frame doesn't overlap with the last text element
  // this elemenets height will be adjusted accordingly when the textField is filled.
  m_uiElementRect.h = 4;
  m_highlightingRect = m_uiElementRect;
  m_highlightingRect.x += 4;
  m_highlightingRect.w -= 8;
}

void TextField::draw()
{
  if (isVisible())
  {
    if (hoveredID != -1)
    {
      drawSolidRect(m_highlightingRect, SDL_Color({150, 150, 150}));
    }
    for (const auto &text : m_textElements)
    {
      text->draw();
    }
  }
}

void TextField::addText(const std::string &text)
{
  SDL_Rect textRect = m_uiElementRect;

  Text *label = new Text(text);
  textRect.h = label->getUiElementRect().h; // get height of text after instantiating
  textRect.y = static_cast<int>(m_uiElementRect.y + m_count * textRect.h);

  // center text
  if (centerText)
  {
    textRect.x = m_uiElementRect.x + (m_uiElementRect.w / 2 - label->getUiElementRect().w / 2);
  }
  else
  {
    textRect.x = m_uiElementRect.x;
  }

  m_textElementHeight = textRect.h;

  m_uiElementRect.h += m_textElementHeight;
  m_highlightingRect.h = m_textElementHeight;

  label->setPosition(textRect.x, textRect.y);
  m_textElements.emplace_back(label);
  m_count = static_cast<int>(m_textElements.size());
}

std::string TextField::getTextFromID(int id) const
{
  if (id < static_cast<int>(m_count) && static_cast<int>(m_count) > 0 && id >= 0)
  {
    return m_textElements[id]->getUiElementData().text;
  }
  return "";
}

void TextField::setPosition(int x, int y)
{
  m_uiElementRect.x = x;
  m_uiElementRect.y = y;
  m_highlightingRect.x = x + 4;

  int currentElement = 0;
  for (const auto &text : m_textElements)
  {
    //SDL_Rect textRect;
    switch (textAlignment)
    {
    case TextFieldAlignment::LEFT:
      // for LEFT alignemnt, we use the same values as the uiElementRect
      break;
    case TextFieldAlignment::RIGHT:
      x = m_uiElementRect.x + m_uiElementRect.w - text->getUiElementRect().w;
      break;
    case TextFieldAlignment::CENTERED:
      x = m_uiElementRect.x + (m_uiElementRect.w / 2 - text->getUiElementRect().w / 2);
      break;
    }
    y = m_uiElementRect.y + (m_textElementHeight * currentElement++);
    text->setPosition(x, y);
    y += currentElement;
  }
}

bool TextField::onMouseButtonUp(const SDL_Event &event)
{
  if (isMouseOver(event.button.x, event.button.y))
  {
    if (!m_textElements.empty())
    {
      selectedID = ((m_textElementHeight + event.button.y - m_uiElementRect.y) / m_textElementHeight) - 1;
      // because of the -4 pixel offset that's been added in the constructor, the id would exceed the size of the vector, if the bottom of the dropdown is clicked

      if (selectedID >= static_cast<int>(m_count))
      {
        selectedID = static_cast<int>(m_count - 1);
      }
    }
    return true;
  }
  return false;
}

void TextField::onMouseMove(const SDL_Event &event)
{
  // TODO: Calculation deviates by 4 pixel in height because of the frame
  if (hoveredID != ((m_textElementHeight + event.button.y - m_uiElementRect.y) / m_textElementHeight) - 1)
  {
    hoveredID = ((m_textElementHeight + event.button.y - m_uiElementRect.y) / m_textElementHeight) - 1;
    // because of the -4 pixel offset that's been added in the constructor, the id would exceed the size of the vector, if the bottom of the dropdown is clicked
    if (hoveredID >= static_cast<int>(m_count))
    {
      hoveredID = static_cast<int>(m_count - 1);
    }
    m_highlightingRect.y = ((hoveredID)*m_textElementHeight) + m_uiElementRect.y;
  }
}

void TextField::onMouseLeave(const SDL_Event &)
{
  // reset the hovering
  hoveredID = -1;
}