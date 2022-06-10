#ifndef SETTINGSMENU_HXX_
#define SETTINGSMENU_HXX_

#include "UIManager.hxx"

struct SettingsMenu : public GameMenu
{
  SettingsMenu();
  ~SettingsMenu();
  virtual void draw() const override;
};

#endif // SETTINGSMENU_HXX_