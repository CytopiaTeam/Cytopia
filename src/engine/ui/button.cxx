#include "button.hxx"

Button::Button(const SDL_Rect &uiElementRect) : UiElement(uiElementRect), rect(uiElementRect) {}

void Button::draw()
{

  if (rect.h != 0 && rect.w != 0)
  {
    drawButtonFrame(rect);
  }

  // render the buttons texture if available
  renderTexture();
}

void Button::onMouseButtonUp(const SDL_Event &event)
{
  clickSignal.emit();

  if (getParentID() != "")
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
  if (!isToggleButton())
  {
    if (event.button.button == SDL_BUTTON_LEFT)
    {
      changeButtonState(BUTTONSTATE_CLICKED);
    }
    else
    {
      changeButtonState(BUTTONSTATE_HOVERING);
    }
  }
  else
  {
    if (event.button.button == SDL_BUTTON_LEFT)
    {
      changeButtonState(getButtonState() == BUTTONSTATE_CLICKED ? BUTTONSTATE_DEFAULT : BUTTONSTATE_CLICKED);
      _isMouseButtonDown = true;
    }
  }
}

void Button::onMouseLeave(const SDL_Event &event)
{
  if (!isToggleButton())
  {
    changeButtonState(BUTTONSTATE_DEFAULT);
  }
  else
  {
    if (event.button.button == SDL_BUTTON_LEFT)
    {
      changeButtonState(getButtonState() == BUTTONSTATE_DEFAULT ? BUTTONSTATE_CLICKED : BUTTONSTATE_DEFAULT);
    }
  }
}

void Button::registerFunction(std::function<void()> const &cb) { clickSignal.connect(cb); }
void Button::registerToggleUIFunction(std::function<void(const std::string &)> const &cb) { toggleGroupSignal.connect(cb); }
