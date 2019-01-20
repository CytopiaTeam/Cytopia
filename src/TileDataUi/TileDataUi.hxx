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
    void newItem();
    void deleteItem();
    void duplicateItem();

  private:  // methods
    void setup(Ui::TileSetDataUi &ui);
    void createActions();
    QTreeWidgetItem *newTreeRootItem(const TileData &tile);
    QTreeWidgetItem *newTreeItem(const TileData &tile);
    void addItem(const TileData &tile);
    void ensureUniqueId(TileData &tile);
    void writeToTileData(TileData &tile);
    void readFromTileData(const TileData &tile);
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
