#include "windowManager.hxx"

#include "basics/log.hxx"
#include "basics/settings.hxx"

#include <SDL_image.h>

WindowManager::WindowManager()
{
  m_window = SDL_CreateWindow(m_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              Settings::instance().settings.screenWidth, Settings::instance().settings.screenHeight, 0);
  if (m_window == nullptr)
  {
    LOG(LOG_ERROR) << "Failed to Init SDL\nSDL Error:" << SDL_GetError();
  }

  m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
  if (m_renderer == nullptr)
  {
    LOG(LOG_ERROR) << "Failed to create Renderer!\nSDL Error:" << SDL_GetError();
  }

  SDL_Surface *icon = IMG_Load(m_windowIcon.c_str());

  if (icon)
  {
    SDL_SetWindowIcon(m_window, icon);
    SDL_FreeSurface(icon);
  }
  else
  {

    LOG(LOG_ERROR) << "Could not load icon " << m_windowIcon << "\nSDL_IMAGE Error: " << IMG_GetError();
  }

  m_numOfDisplays = SDL_GetNumVideoDisplays();
  initializeScreenResolutions();
  setFullScreenMode(static_cast<FULLSCREEN_MODE>(Settings::instance().settings.fullScreenMode));
}

WindowManager::~WindowManager()
{
  SDL_DestroyRenderer(m_renderer);
  SDL_DestroyWindow(m_window);
}

void WindowManager::toggleFullScreen() const
{
  Settings::instance().settings.fullScreen = !Settings::instance().settings.fullScreen;

  if (Settings::instance().settings.fullScreen)
  {
    SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN);
  }
  else
  {
    SDL_SetWindowFullscreen(m_window, 0);
  }
}

void WindowManager::setFullScreenMode(FULLSCREEN_MODE mode)
{
  Settings::instance().settings.fullScreenMode = static_cast<int>(mode);

  // reset the actual resolution back to the desired resolution
  Settings::instance().settings.currentScreenHeight = Settings::instance().settings.screenHeight;
  Settings::instance().settings.currentScreenWidth = Settings::instance().settings.screenWidth;
  switch (mode)
  {
  case FULLSCREEN_MODE::WINDOWED:
    SDL_SetWindowFullscreen(m_window, 0);
    break;
  case FULLSCREEN_MODE::FULLSCREEN:
    SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN);
    break;
  case FULLSCREEN_MODE::BORDERLESS:
    SDL_DisplayMode desktopScreenMode;
    if (SDL_GetDesktopDisplayMode(0, &desktopScreenMode) != 0)
    {
      LOG() << "SDL_GetDesktopDisplayMode failed: " << SDL_GetError();
    }
    // set the actual resolution to the desktop resolution for borderless
    Settings::instance().settings.currentScreenHeight = desktopScreenMode.h;
    Settings::instance().settings.currentScreenWidth = desktopScreenMode.w;

    // As a workaround, need to swtich back into windowed mode
    // before changing the display mode, then back to full screen
    // mode.
    SDL_SetWindowFullscreen(m_window, 0);
    SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    break;
  }
}

void WindowManager::setWindowTitle(const std::string &title)
{
  m_title = title;
  SDL_SetWindowTitle(m_window, m_title.c_str());
}

void WindowManager::initializeScreenResolutions()
{
  if (m_activeDisplay > m_numOfDisplays)
  {
    LOG(LOG_ERROR) << "There is no display with number " << m_activeDisplay << " - Resetting to display 0";
  }

  // get the number of different screen modes
  for (int modeIndex = 0; modeIndex <= SDL_GetNumDisplayModes(m_activeDisplay); modeIndex++)
  {
    SDL_DisplayMode *mode = new SDL_DisplayMode{SDL_PIXELFORMAT_UNKNOWN, 0, 0, 0, 0};

    if (SDL_GetDisplayMode(m_activeDisplay, modeIndex, mode) == 0)
    {
      m_resolutions.push_back(mode);
    }
  }
}

void WindowManager::setScreenResolution(int mode)
{
  // check if the desired mode exists first
  if (mode > 0 && mode < m_resolutions.size() && m_resolutions[mode])
  {
    Settings::instance().settings.screenWidth = m_resolutions[mode]->w;
    Settings::instance().settings.screenHeight = m_resolutions[mode]->h;

    // update the actual resolution
    Settings::instance().settings.currentScreenWidth = Settings::instance().settings.screenWidth;
    Settings::instance().settings.currentScreenHeight = Settings::instance().settings.screenHeight;

    switch (static_cast<FULLSCREEN_MODE>(Settings::instance().settings.fullScreenMode))
    {
    case FULLSCREEN_MODE::FULLSCREEN:
      SDL_SetWindowDisplayMode(m_window, m_resolutions[mode]);
      // workaround. After setting Display Resolution in fullscreen, it won't work until disabling / enabling Fullscreen again.
      SDL_SetWindowFullscreen(m_window, 0);
      SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN);
      break;
    case FULLSCREEN_MODE::WINDOWED:
      SDL_SetWindowSize(m_window, m_resolutions[mode]->w, m_resolutions[mode]->h);
      SDL_SetWindowPosition(m_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
      break;
    case FULLSCREEN_MODE::BORDERLESS:
      // do nothing for borderless fullscreen, it's always the screensize
      break;
    }
  }
  else
  {
    LOG(LOG_ERROR) << "Cannot set screen mode " << mode;
  }
}