#include "textField.hxx"
#include "text.hxx"

TextField::TextField(const SDL_Rect &uiElementRect) : UiElement(uiElementRect)
{
  // initialize height with an offset of 4 so the frame doesn't overlap with the last text element
  // this elemenets height will be adjusted accordingly when the textField is filled.
  m_uiElementRect.h = 4;
  m_hoverRect = m_uiElementRect;
  m_hoverRect.x += 4;
  m_hoverRect.w -= 8;
}

void TextField::draw()
{
  if (isVisible())
  {
    if (hoveredID != -1)
    {
      drawSolidRect(m_hoverRect, SDL_Color({150, 150, 150}));
    }
    for (auto text : m_textElements)
    {
      text->draw();
    }
  }
}

void TextField::addText(const std::string &text)
{
  SDL_Rect textRect = m_uiElementRect;

  m_textElements.push_back(new Text(text));
  textRect.h = m_textElements.back()->getUiElementRect().h; // get height of text after instantiating
  textRect.y = (m_uiElementRect.y + count - 1) * textRect.h;

  // center text
  if (centerText)
  {
    textRect.x = m_uiElementRect.x + (m_uiElementRect.w / 2 - m_textElements.back()->getUiElementRect().w / 2);
  }
  else
  {
    textRect.x = m_uiElementRect.x;
  }

  m_textElementHeight = textRect.h;

  m_uiElementRect.h += m_textElementHeight;
  m_hoverRect.h = m_textElementHeight;

  m_textElements.back()->setPosition(textRect.x, textRect.y);
  count = static_cast<int>(m_textElements.size());
}

std::string TextField::getTextFromID(int id)
{
  if (id < count)
  {
    return m_textElements[id]->getUiElementData().text;
  }
  return "";
}

void TextField::onMouseButtonUp(const SDL_Event &event)
{
  if (!m_textElements.empty())
  {
    selectedID = ((m_textElementHeight + event.button.y - m_uiElementRect.y) / m_textElementHeight) - 1;
    // because of the -4 pixel offset that's been added in the constructor, the id would exceed the size of the vector, if the bottom of the dropdown is clicked

    if (selectedID >= count)
    {
      selectedID = count - 1;
    }
  }
}

void TextField::onMouseMove(const SDL_Event &event)
{
  // TODO: Calculation deviates by 4 pixel in height because of the frame
  if (hoveredID != ((m_textElementHeight + event.button.y - m_uiElementRect.y) / m_textElementHeight) - 1)
  {
    hoveredID = ((m_textElementHeight + event.button.y - m_uiElementRect.y) / m_textElementHeight) - 1;
    // because of the -4 pixel offset that's been added in the constructor, the id would exceed the size of the vector, if the bottom of the dropdown is clicked
    if (hoveredID >= count)
    {
      hoveredID = count - 1;
    }
    m_hoverRect.y = ((hoveredID)*m_textElementHeight) + m_uiElementRect.y;
  }
}

void TextField::onMouseLeave(const SDL_Event &)
{
  // reset the hovering
  hoveredID = -1;
}