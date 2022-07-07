#ifndef MAIN_MENU_SCENE_HXX_
#define MAIN_MENU_SCENE_HXX_

#ifndef VERSION
#define VERSION 0
#endif

#include "Scenes.hxx"
#include "SDL_rect.h"

/** @brief initializes and displays the main menu
    * initializes and displays the main menu
    * @return true in case game has been quit, otherwise false.
    */

union SDL_Event;
struct SDL_Texture;

class SceneMainMenu : public BaseScene
{
public:
  void onBeforeStart() override;
  void onRender(float dt) override;
  void onEvent(const SDL_Event &event) override;
  bool isActive() const override { return m_isActive; }
  void onBeforeStop() override { /* do nothinh */ };
  void onDestroy() override;

  uint8_t logo_fade_opacity = 0;

  SDL_Texture *m_logoTex;
  SDL_Texture *m_discordTex;
  SDL_Texture *m_githubTex;
  SDL_Point m_logoTexSize;

  bool m_isActive = true;
  uint8_t m_opacity = 0;
};
#endif // MAIN_MENU_SCENE_HXX_