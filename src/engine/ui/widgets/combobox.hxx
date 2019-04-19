#ifndef COMBOBOX_HXX_
#define COMBOBOX_HXX_

#include <memory>

#include "../basics/uiElement.hxx"
#include "textField.hxx"

class ComboBox : public UiElement
{
public:
  ComboBox(const SDL_Rect &uiElementRect);
  ~ComboBox() override = default;

  bool onMouseButtonUp(const SDL_Event &event) override;
  bool onMouseButtonDown(const SDL_Event &event) override;
  void onMouseLeave(const SDL_Event &event) override;
  void onMouseMove(const SDL_Event &event) override;

  void draw() override;

  void setPosition(int x, int y) override;

  bool isMouseOver(int x, int y) override;
  bool isMouseOverHoverableArea(int x, int y) override;

  void addElement(const std::string &text);

  /** \brief Get ID of selected element
  * returns the ID of the selected element in the comboBox
  * @return selected ID
  */
  int getActiveID() const { return m_activeID; };
  void setActiveID(int ID);

  size_t count() const { return m_textField->count(); };
  std::string activeText;

  void registerCallbackFunction(std::function<void(UiElement *sender)> const &cb) override;

  void clear() { m_textField->clear(); }

private:
  int m_activeID = 0;
  SDL_Rect m_dropDownRect;     // represents the dropdownMenu
  SDL_Rect m_wholeElementRect; // represents the whole UiElement including the opened menu

  std::unique_ptr<TextField> m_textField;

  bool m_isMenuOpened = false;

  std::unique_ptr<Text> m_buttonLabel;

  void centerTextLabel();

  bool checkBoundaries(int x, int y);

  // Signals
  Signal::Signal<void(UiElement *sender)> clickSignalSender;
};

#endif
