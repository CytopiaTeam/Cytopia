#ifndef MAP_SCENE_HXX_
#define MAP_SCENE_HXX_

#ifndef VERSION
#define VERSION 0
#endif

#include "Scenes.hxx"
#include "Image.hxx"

/** @brief initializes and displays the main menu
    * initializes and displays the main menu
    * @return true in case game has been quit, otherwise false.
    */
class SceneMap : public BaseScene {
public:
  void onBeforeStart() override;
  void onRender(float dt) override;
  void onEvent(const SDL_Event &event) override;
  void onUpdate() override;
  bool isActive() const override { return mainMenuLoop; }
  void onBeforeStop() override {};
  void onDestroy() override { delete this; }

  bool mainMenuLoop = true;
};

class SceneLoading : public BaseScene {
public:
  void onBeforeStart() override;
  void onRender(float dt) override;
  void onEvent(const SDL_Event &event) override;
  bool isActive() const override { return waiting4load; }
  void onBeforeStop() override;
  void onDestroy() override { delete this; }

  class Text *loadingText = nullptr;
  uint32_t frames = 0;
  bool waiting4load = true;
};

class SceneLoadingNew : public SceneLoading {
public:
  void onBeforeStart() override;
};


class SceneLoadingExist : public SceneLoading {
public:
  void onBeforeStart() override;
};
#endif // MAP_SCENE_HXX_