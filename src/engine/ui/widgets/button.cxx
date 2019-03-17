#include "button.hxx"
#include "../../basics/log.hxx"

Button::Button(const SDL_Rect &uiElementRect) : UiElement(uiElementRect), m_rect(uiElementRect)
{
  m_buttonLabel = std::make_unique<Text>(uiElementRect);
}

void Button::draw()
{
  // if the button is initialized with no dimension, don't draw a frame.
  if (m_rect.h != 0 && m_rect.w != 0)
  {
    drawButtonFrame(m_rect);
  }

  if (m_drawFrame)
  {
    drawButtonFrame(m_uiElementRect);
  }

  m_buttonLabel->draw();

  // render the buttons texture if available
  renderTexture();
}

void Button::setText(const std::string &text) { m_buttonLabel->setText(text); }

bool Button::onMouseButtonUp(const SDL_Event &event)
{
  if (isMouseOver(event.button.x, event.button.y))
  {

    if (!elementData.isToggleButton)
    {
      changeButtonState(BUTTONSTATE_DEFAULT);
      clickSignal.emit();
      clickSignalString.emit(elementData.actionParameter, this);
    }
    else
    {
      if (!m_isMouseButtonDown)
      {
        changeButtonState(getButtonState() == BUTTONSTATE_CLICKED ? BUTTONSTATE_DEFAULT : BUTTONSTATE_CLICKED);
      }
      m_isMouseButtonDown = false;
      setCheckState(!m_checkState);
    }
    return true;
  }
  return false;
}

bool Button::onMouseButtonDown(const SDL_Event &event)
{
  if (isMouseOver(event.button.x, event.button.y))
  {
    if (!elementData.isToggleButton)
    {
      changeButtonState(BUTTONSTATE_CLICKED);
    }
    else
    {
      m_isMouseButtonDown = true;
    }
    return true;
  }
  return false;
}

void Button::onMouseEnter(const SDL_Event &event)
{
  if (event.button.button == SDL_BUTTON_LEFT && getButtonState() != BUTTONSTATE_CLICKED)
  {
    if (elementData.isToggleButton)
    {
      m_isMouseButtonDown = true;
    }
    changeButtonState(BUTTONSTATE_CLICKED);
  }
  else if (getButtonState() != BUTTONSTATE_HOVERING)
  {
    changeButtonState(BUTTONSTATE_HOVERING);
  }
}

void Button::onMouseLeave(const SDL_Event &)
{
  if (elementData.isToggleButton)
  {
    changeButtonState(m_checkState ? BUTTONSTATE_CLICKED : BUTTONSTATE_DEFAULT);
    m_isMouseButtonDown = false;
  }
  else if (getButtonState() != BUTTONSTATE_DEFAULT)
  {
    changeButtonState(BUTTONSTATE_DEFAULT);
  }
}

void Button::drawImageButtonFrame(bool drawFrame) { m_drawFrame = drawFrame; }

void Button::setCheckState(bool state)
{
  m_checkState = state;
  changeButtonState(m_checkState ? BUTTONSTATE_CLICKED : BUTTONSTATE_DEFAULT);
  //clickSignal.emit();
  clickSignalString.emit(elementData.actionParameter, this);
}


void Button::registerCallbackFunction(std::function<void()> const &cb) { clickSignal.connect(cb); }
void Button::registerCallbackFunction(std::function<void(const std::string &, UiElement* sender)> const &cb) { clickSignalString.connect(cb); }