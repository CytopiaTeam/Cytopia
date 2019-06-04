#ifndef _TileDataUI_H_
#define _TileDataUI_H_

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QSplitter>
#include <QtCore/QJsonObject>

#include <ui_TileDataUi.h>
#include <ui_TileSetDataUi.h>

#include "TileDataContainer.hxx"

class TileDataUI : public QMainWindow
{
  Q_OBJECT

public:
  TileDataUI();

  bool loadFile(const QString &name);

protected:
  void closeEvent(QCloseEvent *event) override;

private slots:
  void itemSelected(QTreeWidgetItem *current, QTreeWidgetItem *previous);
  void saveTileData();
  void newItem();
  void deleteItem();
  void duplicateItem();

private: // methods
  void setup(Ui::TileSetDataUI &ui);
  void createActions();
  QTreeWidgetItem *newTreeRootItem(const TileData &tile);
  QTreeWidgetItem *newTreeItem(const TileData &tile);
  void addItem(const TileData &tile);
  void ensureUniqueId(TileData &tile);
  void writeToTileData(TileData &tile);
  void readFromTileData(const TileData &tile);
  void fillTileSetDataWidget(const Ui::TileSetDataUI &ui, const TileSetData &data);
  void readTileSetDataWidget(const Ui::TileSetDataUI &ui, TileSetData &data);
  QJsonObject tileSetDataToJson(const TileSetData &data);

private: // members
  TileDataContainer tileContainer;
  QTreeWidget *tree;
  QSplitter *splitter;
  Ui::TileDataUI ui;
  Ui::TileSetDataUI tilesSet;
  Ui::TileSetDataUI cornerSet;
  Ui::TileSetDataUI slopeSet;
};

#endif
