#include "CursorView.hxx"
#include "../util/Filesystem.hxx"
#include "../util/PixelBuffer.hxx"
#include "../util/ZipRange.hxx"

CursorView::CursorView()
{
  /* Load all cursors */
  std::string path = fs::getBasePath();
  path += "resources/images/ui/general/cursors.png";
  PixelBuffer pb = fs::readPNGFile(path);
  pb.scale(2.f);
  int swidth = pb.bounds().width(), sheight = pb.bounds().height();
  swidth /= 6; // Currently support 6 different cursors
  std::array cropsFrom = {
    Rectangle{0*swidth, 0, 1*swidth-1, sheight-1},
    Rectangle{1*swidth, 0, 2*swidth-1, sheight-1},
    Rectangle{2*swidth, 0, 3*swidth-1, sheight-1},
    Rectangle{3*swidth, 0, 4*swidth-1, sheight-1},
    Rectangle{4*swidth, 0, 5*swidth-1, sheight-1},
    Rectangle{5*swidth, 0, 6*swidth-1, sheight-1}
  };
  Rectangle rto {0, 0, swidth-1, sheight-1};
  Rectangle rscaled {
    0, 0, 
    ((rto.width() * CursorScaleSize::value) >> 16) - 1,
    ((rto.height() * CursorScaleSize::value) >> 16) - 1
  };
  /**
   *  @todo: Maybe this should be configurable instead of hardcoded...
   */
  std::array hot_xs = {1, 3, 5, 5, 5, 0};
  std::array hot_ys = {1, 1, 8, 10, 7, 0};
  auto range = ZipRange{CursorType::_values(), hot_xs, hot_ys, cropsFrom};
  for(auto [type, hotx, hoty, rfrom] : range)
  {
    PixelBuffer cursor_img {pb};
    cursor_img.crop(std::move(rfrom));
    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(
        cursor_img.data(), 
        cursor_img.bounds().width(), 
        cursor_img.bounds().height(), 
        32, 
        4 * cursor_img.bounds().width(),
        0xff000000,
        0x00ff0000,
        0x0000ff00,
        0x000000ff
        );
    CursorPtr cursor { SDL_CreateColorCursor(surface, hotx, hoty) };
    m_Cursors[type._to_integral()] = std::move(cursor);
  }
}

void CursorView::update(Notification notif) noexcept
{
  CursorType type = std::get<MouseStateModel>(notif).cursorType;
  SDL_Cursor* cursor = m_Cursors[type._to_integral()].get();
  LOG(LOG_DEBUG) << "Setting cursor";
  SDL_SetCursor(cursor);
}

void CursorView::CursorDeleter::operator()(SDL_Cursor * ptr) const noexcept
{ SDL_FreeCursor(ptr); }
