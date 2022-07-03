#ifndef PAUSEMENU_HXX_
#define PAUSEMENU_HXX_

#include "UIManager.hxx"

struct PauseMenu : public GameMenu
{
  virtual void draw() const override;
};

#endif // PAUSEMENU_HXX_