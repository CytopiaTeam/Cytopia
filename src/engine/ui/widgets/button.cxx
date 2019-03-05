#include "button.hxx"
#include "../../basics/log.hxx"

Button::Button(const SDL_Rect &uiElementRect) : UiElement(uiElementRect), _rect(uiElementRect)
{
  m_buttonLabel = std::make_unique<Text>(uiElementRect);
}

void Button::draw()
{
  // if the button is initialized with no dimension, don't draw a frame.
  if (_rect.h != 0 && _rect.w != 0)
  {
    drawButtonFrame(_rect);
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

void Button::onMouseButtonUp(const SDL_Event &)
{
  clickSignal.emit();
  clickSignalString.emit(elementData.actionParameter);

  if (!elementData.isToggleButton)
  {
    changeButtonState(BUTTONSTATE_DEFAULT);
  }
  else
  {
    if (!m_isMouseButtonDown)
    {
      changeButtonState(getButtonState() == BUTTONSTATE_CLICKED ? BUTTONSTATE_DEFAULT : BUTTONSTATE_CLICKED);
    }
    m_isMouseButtonDown = false;
    m_isButtonToggled = !m_isButtonToggled;
  }
}

void Button::onMouseButtonDown(const SDL_Event &)
{
  if (!elementData.isToggleButton)
  {
    changeButtonState(BUTTONSTATE_CLICKED);
  }
  else
  {
    changeButtonState(getButtonState() == BUTTONSTATE_CLICKED ? BUTTONSTATE_DEFAULT : BUTTONSTATE_CLICKED);
    m_isMouseButtonDown = true;
  }
}

void Button::onMouseEnter(const SDL_Event &event)
{
  if (event.button.button == SDL_BUTTON_LEFT)
  {
    if (elementData.isToggleButton)
    {
      m_isMouseButtonDown = true;
    }
    changeButtonState(BUTTONSTATE_CLICKED);
  }
  else
  {
    changeButtonState(BUTTONSTATE_HOVERING);
  }
}

void Button::onMouseLeave(const SDL_Event &)
{
  if (elementData.isToggleButton)
  {
    changeButtonState(m_isButtonToggled ? BUTTONSTATE_CLICKED : BUTTONSTATE_DEFAULT);
    m_isMouseButtonDown = false;
  }
  else
  {
    changeButtonState(BUTTONSTATE_DEFAULT);
  }
}

void Button::drawImageButtonFrame(bool drawFrame) { m_drawFrame = drawFrame; }

void Button::registerCallbackFunction(std::function<void()> const &cb) { clickSignal.connect(cb); }
void Button::registerCallbackFunction(std::function<void(const std::string &)> const &cb) { clickSignalString.connect(cb); }

