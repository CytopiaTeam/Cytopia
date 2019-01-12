#ifndef _TileDataUi_H_
#define _TileDataUi_H_

#include <QMainWindow>
#include <QTreeWidget>
#include <QSplitter>
#include <QJsonObject>

#include <ui_TileDataUi.h>
#include <ui_TileSetDataUi.h>

#include <TileData.hxx>

class TileDataUi : public QMainWindow
{
  Q_OBJECT

  public:
    TileDataUi();

    bool loadFile(const QString &name);

  protected:
    void closeEvent(QCloseEvent *event) override;

  private slots:
    void itemSelected(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void saveTileData();

  private:  // methods
    void setup(Ui::TileSetDataUi &ui);
    void fillTileSetDataWidget(const Ui::TileSetDataUi &ui, const TileSetData &data);
    void readTileSetDataWidget(const Ui::TileSetDataUi &ui, TileSetData &data);
    QJsonObject tileSetDataToJson(const TileSetData &data);

  private:  // members
    TileDataContainer tileContainer;
    QTreeWidget *tree;
    QSplitter *splitter;
    Ui::TileDataUi ui;
    Ui::TileSetDataUi tilesSet;
    Ui::TileSetDataUi cornerSet;
    Ui::TileSetDataUi slopeSet;
};

#endif
