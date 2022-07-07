#ifndef SCENES_HXX_
#define SCENES_HXX_

union SDL_Event;
struct BaseScene;

struct BaseSceneSelector {
  virtual void nextScene(BaseScene *) = 0;
  virtual ~BaseSceneSelector() = default;
};

struct BaseScene {
  virtual ~BaseScene() = default;
  virtual void onBeforeStart() = 0;
  virtual void onRender(float dt) = 0;
  virtual void onEvent(const SDL_Event &event) = 0;
  virtual void onUpdate() {}
  virtual bool isActive() const = 0;
  virtual void onBeforeStop() = 0;
  virtual void onDestroy() = 0;
  void nextScene(BaseScene* scene)
  {
    m_sceneSelector ? m_sceneSelector->nextScene(scene) : (void)0;
  }

  static BaseSceneSelector *m_sceneSelector;
};

#endif // SCENES_HXX_