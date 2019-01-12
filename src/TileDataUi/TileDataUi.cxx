#include <TileDataUi.hxx>

#include <QToolBar>
#include <QAction>
#include <QMap>
#include <QIcon>
#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>

//--------------------------------------------------------------------------------

TileDataUi::TileDataUi()
{
  splitter = new QSplitter(this);

  tree = new QTreeWidget;
  tree->header()->hide();
  connect(tree, &QTreeWidget::currentItemChanged, this, &TileDataUi::itemSelected);
  splitter->addWidget(tree);

  QWidget *w = new QWidget;
  ui.setupUi(w);
  splitter->addWidget(w);

  w = new QWidget;
  tilesSet.setupUi(w);
  setup(tilesSet);
  ui.tabWidget->addTab(w, tr("Tiles"));

  w = new QWidget;
  cornerSet.setupUi(w);
  setup(cornerSet);
  ui.tabWidget->addTab(w, tr("Corner"));

  w = new QWidget;
  slopeSet.setupUi(w);
  setup(slopeSet);
  ui.tabWidget->addTab(w, tr("Slope"));

  setCentralWidget(splitter);

  QToolBar *toolBar = new QToolBar(this);
  toolBar->setObjectName("ToolBar");
  addToolBar(toolBar);

  QAction *action = new QAction(tr("Save and Quit"), this);
  action->setIcon(QIcon::fromTheme("application-exit"));
  connect(action, &QAction::triggered, this, &TileDataUi::close);
  toolBar->addAction(action);

  action = new QAction(tr("Save"), this);
  action->setIcon(QIcon::fromTheme("document-save"));
  connect(action, &QAction::triggered, this, &TileDataUi::saveTileData);
  toolBar->addAction(action);

  QSettings settings("JimmySnails", "Cytopia");
  restoreGeometry(settings.value("main/geometry").toByteArray());
  restoreState(settings.value("main/windowState").toByteArray());
  splitter->restoreState(settings.value("main/splitter").toByteArray());
}

//--------------------------------------------------------------------------------

void TileDataUi::closeEvent(QCloseEvent *event)
{
  saveTileData();

  QSettings settings("JimmySnails", "Cytopia");
  settings.setValue("main/geometry", saveGeometry());
  settings.setValue("main/windowState", saveState());
  settings.setValue("main/splitter", splitter->saveState());

  QMainWindow::closeEvent(event);
}

//--------------------------------------------------------------------------------

void TileDataUi::setup(Ui::TileSetDataUi &ui)
{
  connect(ui.fileButton, &QPushButton::clicked, this,
          [ui]()
          {
            QString fileName = QFileDialog::getOpenFileName(ui.fileButton, tr("Select Image"),
                                                  ui.fileName->text(),
                                                  tr("Images (*.png)"));
            if ( !fileName.isEmpty() )
              ui.fileName->setText(QDir::current().relativeFilePath(fileName));
          });
}

//--------------------------------------------------------------------------------

bool TileDataUi::loadFile(const QString &fileName)
{
  QString error = tileContainer.loadFile(fileName);
  if ( !error.isEmpty() )
  {
    QMessageBox::critical(this, tr("Load Error"), tr("Could not load %1. %2").arg(fileName).arg(error));
    return false;
  }

  QMap<QString, QTreeWidgetItem *> types;
  for (const TileData &tile : tileContainer)
  {
    QString type(QString::fromStdString(tile.type));

    if ( !types.contains(type) )
    {
      QTreeWidgetItem *item = new QTreeWidgetItem(tree);
      item->setIcon(0, QIcon::fromTheme("folder"));
      item->setText(0, type);
      item->setExpanded(true);

      types.insert(item->text(0), item);
    }
  }

  for (const TileData &tile : tileContainer)
  {
    QString type(QString::fromStdString(tile.type));
    QTreeWidgetItem *root = types.value(type);

    QTreeWidgetItem *item = new QTreeWidgetItem(root);
    item->setIcon(0, QIcon::fromTheme("text-x-generic"));
    item->setText(0, QString::fromStdString(tile.title));
    item->setData(0, Qt::UserRole, QString::fromStdString(tile.id));
  }

  tree->resizeColumnToContents(0);
  tree->resize(tree->columnWidth(0), tree->height());

  return true;
}

//--------------------------------------------------------------------------------

void TileDataUi::itemSelected(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
  // store current form values into previous item
  if ( previous && previous->data(0, Qt::UserRole).isValid() )
  {
    TileData *tile = tileContainer.getTileData(previous->data(0, Qt::UserRole).toString());

    tile->id = ui.id->text().toStdString();
    tile->type = ui.type->text().toStdString();
    tile->title = ui.title->text().toStdString();
    tile->description = ui.description->toPlainText().toStdString();
    tile->author = ui.author->text().toStdString();

    tile->power = ui.powerProduction->value();
    tile->water = ui.waterProduction->value();
    tile->upkeepCost = ui.upkeepCost->value();
    tile->price = ui.buildCost->value();

    readTileSetDataWidget(tilesSet, tile->tiles);
    readTileSetDataWidget(cornerSet, tile->cornerTiles);
    readTileSetDataWidget(slopeSet, tile->slopeTiles);
  }

  // show current item values
  if ( !current || !current->data(0, Qt::UserRole).isValid() )
    return;

  const TileData *tile = tileContainer.getTileData(current->data(0, Qt::UserRole).toString());

  if ( !tile )
    return;

  ui.id->setText(QString::fromStdString(tile->id));
  ui.type->setText(QString::fromStdString(tile->type));
  ui.title->setText(QString::fromStdString(tile->title));
  ui.description->setPlainText(QString::fromStdString(tile->description));
  ui.author->setText(QString::fromStdString(tile->author));

  ui.powerProduction->setValue(tile->power);
  ui.waterProduction->setValue(tile->water);
  ui.upkeepCost->setValue(tile->upkeepCost);
  ui.buildCost->setValue(tile->price);

  fillTileSetDataWidget(tilesSet, tile->tiles);
  fillTileSetDataWidget(cornerSet, tile->cornerTiles);
  fillTileSetDataWidget(slopeSet, tile->slopeTiles);
}

//--------------------------------------------------------------------------------

void TileDataUi::fillTileSetDataWidget(const Ui::TileSetDataUi &ui, const TileSetData &data)
{
  ui.fileName->setText(QString::fromStdString(data.fileName));
  ui.width->setValue(data.clippingWidth);
  ui.height->setValue(data.clippingHeight);
  ui.count->setValue(data.count);
  ui.image->setPixmap(QPixmap(QString::fromStdString(data.fileName)));
}

//--------------------------------------------------------------------------------

void TileDataUi::readTileSetDataWidget(const Ui::TileSetDataUi &ui, TileSetData &data)
{
  data.fileName = ui.fileName->text().toStdString();
  data.clippingWidth = ui.width->value();
  data.clippingHeight = ui.height->value();
  data.count = ui.count->value();
}

//--------------------------------------------------------------------------------

void TileDataUi::saveTileData()
{
  itemSelected(nullptr, tree->currentItem());  // ensure current widget content is stored into TileData

  tileContainer.saveFile();
}

//--------------------------------------------------------------------------------
