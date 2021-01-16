#ifndef WINDOW_HXX_
#define WINDOW_HXX_

#include <string>
#include <SDL.h>
#include "../activity/iActivity.hxx"
#include "../renderer/iRenderer.hxx"
#include "../util/iShape.hxx"
#include "../view/CursorView.hxx"
#include "../events/UIEvents.hxx"
#include "../model/MouseState.hxx"
#include "../util/Exception.hxx"
#include "../util/PixelBuffer.hxx"
#include "../GameService.hxx"
#include "../model/SettingsModel.hxx"

using std::string;

class Window
{
public:
  Window(GameService::ServiceTuple context, const char *title, unsigned int width, unsigned int height, bool isFullScreen,
         const string &windowIcon);
  virtual ~Window();
  void setActivity(iActivityPtr &&);
  Rectangle getBounds() const noexcept;

  MouseState &getMouseState() noexcept;
  SettingsModel &getSettingsModel() noexcept;

private:
  /**
   * @todo Remove this when new ui is complete
   */
  friend class WindowManager;
  SDL_Window *m_Window;
  iRendererPtr m_Renderer;
  iActivityPtr m_Activity;
  CursorViewPtr m_Cursor;

  MouseState m_MouseState;
  SettingsModel m_SettingsModel;

  void resize();
  void redraw();

  /**
  * @brief Destroy old render and create new one.
  * @attention If window has been touched from other thread than UI, new render must be created.
  */
  void recreateRender();

  template <typename... Args> iActivityPtr fromActivityType(ActivityType, Args &&...);
  friend class Game;
};

#include "Window.inl.hxx"

#endif // WINDOW_HXX_
