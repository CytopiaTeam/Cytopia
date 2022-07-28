#ifndef PROPERTY_WINDOW_HXX_
#define PROPERTY_WINDOW_HXX_

class Point;

struct PropertyWindow 
{
  static bool m_show;

  void drawProperty(const char *uname, uint32_t l, const char *title, const std::string &data);
  void drawProperty(const char *uname, uint32_t l, const char *title, int value);
  void showTileInfo(const Point &isoCoord);

  void showInfo(const Point &isoCoord);
};

#endif // PROPERTY_WINDOW_HXX_