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
  _menuRect.h = _textField->getSize().h;
  _textField->setVisibility(false);
}

void ComboBox::draw()
{
  Uint8 bgColor, bgColorFrame, bgColorFrameShade, bgColorBottomFrame, bgColorBottomFrameShade;

  bgColor = 128;
  bgColorFrame = 150;
  bgColorFrameShade = 172;
  bgColorBottomFrame = 106;
  bgColorBottomFrameShade = 84;

  // draw the button frame
  if (_comboBoxRect.w != 0 && _comboBoxRect.h != 0)
  {
    drawButtonFrame(_comboBoxRect);
  }

  // arrow

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

  drawText(_activeText, {255, 255, 255});

  //render the buttons texture if available
  renderTexture();
}

int ComboBox::getClickedID(int x, int y) { return _activeID; }

bool ComboBox::isHovering(int x, int y)
{

  bool isClicked = false;

  SDL_Rect boundaries = _comboBoxRect;
  ;

  isClicked = x > boundaries.x && x < boundaries.x + boundaries.w && y > boundaries.y && y < boundaries.y + boundaries.h;

  if (_isMenuOpened)
  {
    boundaries.y += _comboBoxRect.h;
    boundaries.h += _menuRect.h - _comboBoxRect.h;
  }

  return isClicked;
}

bool ComboBox::isMouseOver(int x, int y)
{
  bool isClicked = false;

  SDL_Rect boundaries = _comboBoxRect;

  if (_isMenuOpened)
  {
    boundaries.h += _menuRect.h;
  }

  isClicked = x > boundaries.x && x < boundaries.x + boundaries.w && y > boundaries.y && y < boundaries.y + boundaries.h;

  return isClicked;
}

void ComboBox::onMouseButtonUp(SDL_Event &event)
{
  SDL_Rect boundaries = _comboBoxRect;

  int x = event.button.x;
  int y = event.button.y;
  // clicked on the button
  if (x > boundaries.x && x < boundaries.x + boundaries.w && y > boundaries.y && y < boundaries.y + boundaries.h)
  {
    _isMenuOpened = !_isMenuOpened;
    _textField->setVisibility(!_textField->isVisible());
    changeButtonState(TextureManager::ButtonState::HOVERING);
    return;
  }

  if (_isMenuOpened)
  {
    _activeID = _textField->getSeletectedID(x, y);
    _activeText = _textField->getTextFromID(_activeID);
    _isMenuOpened = false;
    _textField->setVisibility(false);
  }
}

void ComboBox::onMouseButtonDown(SDL_Event &event) { changeButtonState(TextureManager::ButtonState::DEFAULT); }

void ComboBox::onMouseEnter(SDL_Event &event)
{
  bool isClicked = false;
  int x = event.button.x;
  int y = event.button.y;

  SDL_Rect boundaries = _comboBoxRect;
  ;

  isClicked = x > boundaries.x && x < boundaries.x + boundaries.w && y > boundaries.y && y < boundaries.y + boundaries.h;

  if (_isMenuOpened)
  {
    boundaries.y += _comboBoxRect.h;
    boundaries.h += _menuRect.h - _comboBoxRect.h;
  }

  //if (isClicked)
  changeButtonState(TextureManager::ButtonState::HOVERING);
}

void ComboBox::onMouseLeave(SDL_Event &event) { changeButtonState(TextureManager::ButtonState::DEFAULT); }