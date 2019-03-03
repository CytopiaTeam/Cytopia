#include "combobox.hxx"

ComboBox::ComboBox(const SDL_Rect &uiElementRect)
    : UiElement(uiElementRect), _comboBoxRect(uiElementRect), _menuRect(uiElementRect)
//, _textField(new TextField(uiElementRect))
{
  _menuRect.y = _comboBoxRect.y + _comboBoxRect.h;

  _textField = std::make_unique<TextField>(_menuRect);

  _textField->addText("test");
  _textField->addText("awesome element");
  _textField->addText("one more element");

  _activeText = _textField->getTextFromID(0);

  //set menu to same height as textfield
  _menuRect.h = _textField->getUiElementRect().h;
  _textField->setVisibility(false);
}

void ComboBox::draw()
{
  Uint8 bgColorFrameShade = 172;

  // draw the button frame
  if (_comboBoxRect.w != 0 && _comboBoxRect.h != 0)
  {
    drawButtonFrame(_comboBoxRect);
  }

  // draw the arrow
  SDL_Rect arrowRect;
  SDL_Color color = {bgColorFrameShade, bgColorFrameShade, bgColorFrameShade};

  arrowRect.w = 18;
  arrowRect.h = 2;
  arrowRect.x = _comboBoxRect.x + (_comboBoxRect.w - arrowRect.w) - 9;
  arrowRect.y = _comboBoxRect.y + (_comboBoxRect.h / 2) - (arrowRect.w / 2) / arrowRect.h - 1;

  for (; arrowRect.w > 1; arrowRect.w -= 2)
  {
    drawSolidRect(arrowRect, color);
    arrowRect.x++;
    arrowRect.y++;
  }
  drawSolidRect(arrowRect, color);

  // drowpdown menu
  if (_isMenuOpened)
  {
    drawButtonFrame(_menuRect, false);
    _textField->draw();
  }

  createTextTexture(_activeText, {255, 255, 255});

  //render the buttons texture if available
  renderTexture();
}

bool ComboBox::isMouseOverHoverableArea(int x, int y)
{
  SDL_Rect boundaries = _comboBoxRect;

  if (_isMenuOpened)
  {
    boundaries.h += _menuRect.h;
  }

  return x > boundaries.x && x < boundaries.x + boundaries.w && y > boundaries.y && y < boundaries.y + boundaries.h;
}

bool ComboBox::isMouseOver(int x, int y)
{
  SDL_Rect boundaries = _comboBoxRect;

  if (_isMenuOpened)
  {
    boundaries.h += _menuRect.h;
  }

  return x > boundaries.x && x < boundaries.x + boundaries.w && y > boundaries.y && y < boundaries.y + boundaries.h;
}

void ComboBox::onMouseButtonUp(const SDL_Event &event)
{
  int x = event.button.x;
  int y = event.button.y;

  if (x > _comboBoxRect.x && x < _comboBoxRect.x + _comboBoxRect.w && y > _comboBoxRect.y &&
      y < _comboBoxRect.y + _comboBoxRect.h)
  {
    _isMenuOpened = !_isMenuOpened;
    _textField->setVisibility(!_textField->isVisible());
    changeButtonState(BUTTONSTATE_HOVERING);
    return;
  }

  if (_isMenuOpened)
  {
    _textField->onMouseButtonUp(event); //trigger TextField onMouseButtonUp event
  }

  if (_isMenuOpened)
  {
    _activeID = _textField->getSelectedID();
    _activeText = _textField->getTextFromID(_activeID);
    _isMenuOpened = false;
    _textField->setVisibility(false);
  }
}

void ComboBox::onMouseButtonDown(const SDL_Event &) { changeButtonState(BUTTONSTATE_DEFAULT); }

void ComboBox::onMouseLeave(const SDL_Event &event)
{
  _textField->onMouseLeave(event);
  changeButtonState(BUTTONSTATE_DEFAULT);
}

void ComboBox::onMouseMove(const SDL_Event &event)
{
  int x = event.button.x;
  int y = event.button.y;

  if (x > _comboBoxRect.x && x < _comboBoxRect.x + _comboBoxRect.w && y > _comboBoxRect.y &&
      y < _comboBoxRect.y + _comboBoxRect.h)
  {
    if (getButtonState() != BUTTONSTATE_HOVERING)
    {
      changeButtonState(BUTTONSTATE_HOVERING);
    }
    if (_textField->getHoveredID() != -1)
    {
      _textField->onMouseLeave(event);
    }
  }
  else
  {
    if (_isMenuOpened)
    {
      changeButtonState(BUTTONSTATE_DEFAULT);
      {
        _textField->onMouseMove(event);
      }
    }
  }
}
