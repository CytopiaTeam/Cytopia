#ifndef POPUP_WINDOW_HXX_
#define POPUP_WINDOW_HXX_

#include "UIManager.hxx"
#include <vector>
#include <functional>

struct PopupWindow : public GameMenu
{
  enum Result {
    e_none,
    e_ok,
    e_yes,
    e_no,
    e_cancel
  };

  struct Button {
    std::string text;
    uint32_t id;
  };

  template<class ... Args>
  PopupWindow(const std::string &text, Args&&...args) : m_text(text) {
    Button btns[] = {args...};
    for (auto &btn : btns)
      m_buttons.push_back(btn);
  }

  void draw() const override;
  void close() const;
  uint32_t result() const { return m_result; }

private:
  std::vector<Button> m_buttons;
  std::string m_text;
  mutable uint32_t m_result;
};

struct PopupWindowOk : public PopupWindow
{
  PopupWindowOk(const std::string &text, const std::string &btnText, std::function<void()> cb) : PopupWindow(text, Button{btnText, e_ok}), m_callback(cb)
  {
  }

  void draw() const override {
    PopupWindow::draw();

    if (result() == e_ok) {
      m_callback();
      close();
    }
  }

  std::function<void()> m_callback;
};

struct PopupWindowYesNo : public PopupWindow
{
  PopupWindowYesNo(const std::string &text,
                   const std::string &yes, std::function<void()> cbYes,
                   const std::string &no, std::function<void()> cbNo) 
                      : PopupWindow(text, Button{yes, e_yes}, Button{no, e_no}), m_yesCb(cbYes), m_noCb(cbNo) {}

  void draw() const override {
    PopupWindow::draw();

    if (result() == e_yes) {
      m_yesCb();
      close();
    } else if (result() == e_no) {
      m_noCb();
      close();
    }
  }

  std::function<void()> m_yesCb;
  std::function<void()> m_noCb;
};

#endif // POPUP_WINDOW_HXX_