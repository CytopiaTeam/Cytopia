#include "button.hxx"

Button::Button(const SDL_Rect &uiElementRect) : UiElement(uiElementRect), _rect(uiElementRect)
{
  _buttonLabel = std::make_unique<Text>(uiElementRect);
}

void Button::draw()
{
  // if the button is initialized with no dimension, don't draw a frame.
  if (_rect.h != 0 && _rect.w != 0)
  {
    drawButtonFrame(_rect);
  }

  _buttonLabel->draw();

  // render the buttons texture if available
  renderTexture();
}

void Button::setText(const std::string &text) { _buttonLabel->setText(text); }

void Button::onMouseButtonUp(const SDL_Event &event)
{
  clickSignal.emit();

  if (!getParentID().empty())
  {
    toggleGroupSignal.emit(getParentID());
  }

  if (!isToggleButton())
  {
    changeButtonState(BUTTONSTATE_DEFAULT);
  }
  else
  {
    if (!_isMouseButtonDown)
    {
      changeButtonState(getButtonState() == BUTTONSTATE_CLICKED ? BUTTONSTATE_DEFAULT : BUTTONSTATE_CLICKED);
    }
    _isMouseButtonDown = false;
  }
}

void Button::onMouseButtonDown(const SDL_Event &event)
{
  if (!isToggleButton())
  {
    changeButtonState(BUTTONSTATE_CLICKED);
  }
  else
  {
    changeButtonState(getButtonState() == BUTTONSTATE_CLICKED ? BUTTONSTATE_DEFAULT : BUTTONSTATE_CLICKED);
    _isMouseButtonDown = true;
  }
}

void Button::onMouseEnter(const SDL_Event &event)
{
  if (event.button.button == SDL_BUTTON_LEFT)
  {
    if (!isToggleButton())
    {
      changeButtonState(BUTTONSTATE_CLICKED);
    }
    else
    {
      changeButtonState(getButtonState() == BUTTONSTATE_CLICKED ? BUTTONSTATE_DEFAULT : BUTTONSTATE_CLICKED);
      _isMouseButtonDown = true;
    }
  }
  else if (getButtonState() != BUTTONSTATE_CLICKED)
  {
    changeButtonState(BUTTONSTATE_HOVERING);
  }
}

void Button::onMouseLeave(const SDL_Event &event)
{
  if (isToggleButton() && event.button.button == SDL_BUTTON_LEFT)
  {
    changeButtonState(getButtonState() == BUTTONSTATE_DEFAULT ? BUTTONSTATE_CLICKED : BUTTONSTATE_DEFAULT);
  }
  else if (getButtonState() != BUTTONSTATE_DEFAULT)
  {
    changeButtonState(BUTTONSTATE_DEFAULT);
  }
}

void Button::registerCallbackFunction(std::function<void()> const &cb) { clickSignal.connect(cb); }
void Button::registerToggleUIFunction(std::function<void(const std::string &)> const &cb) { toggleGroupSignal.connect(cb); }
