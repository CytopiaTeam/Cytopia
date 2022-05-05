#include "Button.hxx"
#include "LOG.hxx"

Button::Button(const SDL_Rect &uiElementRect) : UIElement(uiElementRect), m_frameRect(uiElementRect)
{
  m_buttonLabel = std::make_unique<Text>();
  centerTextLabel();
}

void Button::draw()
{
  // if the button is initialized with no dimension, don't draw a frame.
  if (m_frameRect.h != 0 && m_frameRect.w != 0)
  {
    drawButtonFrame(m_frameRect);
  }

  if (m_drawFrame)
  {
    drawButtonFrame(m_uiElementRect);
  }

  m_buttonLabel->draw();

  // render the buttons texture if available
  renderTexture();
}

void Button::setText(const std::string &text)
{
  m_buttonLabel->setText(text);
  elementData.text = text;
  centerTextLabel();
}

bool Button::onMouseButtonUp(const SDL_Event &event)
{
  if (isMouseOver(event.button.x, event.button.y))
  {

    if (!isToggleButton)
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
    if (!isToggleButton)
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
    if (isToggleButton)
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
  if (isToggleButton)
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
  clickSignal.emit();
  clickSignalSender.emit(this);
  clickSignalString.emit(elementData.actionParameter, this);
}

void Button::setPosition(int x, int y)
{
  m_uiElementRect.x = x;
  m_uiElementRect.y = y;
  m_frameRect.x = x;
  m_frameRect.y = y;
  centerTextLabel();
}

void Button::centerTextLabel() const
{
  int x = m_uiElementRect.x + m_uiElementRect.w / 2 - m_buttonLabel->getUiElementRect().w / 2;
  int y = m_uiElementRect.y + m_uiElementRect.h / 2 - m_buttonLabel->getUiElementRect().h / 2;

  m_buttonLabel->setPosition(x, y);
}

void Button::registerCallbackFunction(std::function<void()> const &cb) { clickSignal.connect(cb); }
void Button::registerCallbackFunction(std::function<void(UIElement *sender)> const &cb) { clickSignalSender.connect(cb); }
void Button::registerCallbackFunction(std::function<void(const std::string &, UIElement *sender)> const &cb)
{
  clickSignalString.connect(cb);
}
