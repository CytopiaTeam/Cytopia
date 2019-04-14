#include <QApplication>

#include <TileDataUi.hxx>

int main(int argc, char **argv)
{
  QApplication app(argc, argv);
  app.setWindowIcon(QIcon("resources/images/app_icons/tiledataeditor_icon_iso.png"));

  TileDataUi ui;
  if (!ui.loadFile("resources/data/TileData.json"))
    return -1;

  ui.show();

  return app.exec();
}
