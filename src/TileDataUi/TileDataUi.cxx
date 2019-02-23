#include <TileDataUi.hxx>

#include <QToolBar>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QMap>
#include <QIcon>
#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QHeaderView>
#include <QSignalBlocker>

#include <engine/basics/tileData.hxx>

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

  ui.id->setMaxLength(TD_ID_MAX_CHARS);
  ui.category->setMaxLength(TD_CATEGORY_MAX_CHARS);
  ui.title->setMaxLength(TD_TITLE_MAX_CHARS);
  ui.author->setMaxLength(TD_AUTHOR_MAX_CHARS);
  ui.buildCost->setRange(TD_PRICE_MIN, TD_PRICE_MAX);
  ui.upkeepCost->setRange(TD_UPKEEP_MIN, TD_UPKEEP_MAX);
  ui.powerProduction->setRange(TD_POWER_MIN, TD_POWER_MAX);
  ui.waterProduction->setRange(TD_WATER_MIN, TD_WATER_MAX);

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

  createActions();

  QSettings settings("JimmySnails", "Cytopia");
  restoreGeometry(settings.value("main/geometry").toByteArray());
  restoreState(settings.value("main/windowState").toByteArray());
  splitter->restoreState(settings.value("main/splitter").toByteArray());
}

//--------------------------------------------------------------------------------

void TileDataUi::createActions()
{
  QMenu *fileMenu = menuBar()->addMenu(tr("File"));
  QMenu *editMenu = menuBar()->addMenu(tr("Edit"));
  QMenu *helpMenu = menuBar()->addMenu(tr("Help"));

  QToolBar *toolBar = new QToolBar(this);
  toolBar->setObjectName("ToolBar");
  addToolBar(toolBar);

  // --- file menu ---
  QAction *action = new QAction(tr("Save"), this);
  action->setIcon(QIcon::fromTheme("document-save"));
  action->setShortcut(QKeySequence::Save);
  connect(action, &QAction::triggered, this, &TileDataUi::saveTileData);
  toolBar->addAction(action);
  fileMenu->addAction(action);

  action = new QAction(tr("Save and Quit"), this);
  action->setIcon(QIcon::fromTheme("application-exit"));
  action->setShortcut(QKeySequence::Quit);
  connect(action, &QAction::triggered, this, &TileDataUi::close);
  toolBar->addAction(action);
  fileMenu->addAction(action);

  // --- edit menu ---
  action = new QAction(tr("New Item"), this);
  action->setIcon(QIcon::fromTheme("document-new"));
  action->setShortcut(QKeySequence::New);
  connect(action, &QAction::triggered, this, &TileDataUi::newItem);
  toolBar->addAction(action);
  editMenu->addAction(action);

  action = new QAction(tr("Delete Item"), this);
  action->setIcon(QIcon::fromTheme("edit-delete"));
  action->setShortcut(QKeySequence::Cut);
  connect(action, &QAction::triggered, this, &TileDataUi::deleteItem);
  toolBar->addAction(action);
  editMenu->addAction(action);

  action = new QAction(tr("Duplicate Item"), this);
  action->setIcon(QIcon::fromTheme("edit-copy"));
  action->setShortcut(QKeySequence("CTRL+D"));
  connect(action, &QAction::triggered, this, &TileDataUi::duplicateItem);
  toolBar->addAction(action);
  editMenu->addAction(action);

  // --- help menu ---
  action = new QAction(tr("About Qt"), this);
  connect(action, &QAction::triggered, this, &QApplication::aboutQt);
  helpMenu->addAction(action);
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
            {
              ui.fileName->setText(QDir::current().relativeFilePath(fileName));
              QPixmap pix(fileName);
              ui.origImage->setPixmap(pix);
              ui.image->setPixmap(pix);
              ui.width->setValue(pix.width());
              ui.height->setValue(pix.height());
              ui.size1->setChecked(true);
            }
          });

  ui.origImage->hide();  // a hidden storage for the original sized pixmap

  connect(ui.buttonGroup, static_cast<void(QButtonGroup::*)(QAbstractButton *)>(&QButtonGroup::buttonClicked),
          this,
          [ui](QAbstractButton *button)
          {
            if ( !ui.origImage->pixmap() )
              return;

            QPixmap pix = *(ui.origImage->pixmap());

            if ( button == ui.size2 )
              pix = pix.transformed(QTransform().scale(2, 2));
            else if ( button == ui.size4 )
              pix = pix.transformed(QTransform().scale(4, 4));

            ui.image->setPixmap(pix);
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

  QMap<QString, QTreeWidgetItem *> categories;
  for (const TileData &tile : tileContainer)
  {
    QString category(QString::fromStdString(tile.category));

    if ( !categories.contains(category) )
    {
      QTreeWidgetItem *item = newTreeRootItem(tile);
      categories.insert(category, item);
    }
  }

  for (const TileData &tile : tileContainer)
  {
    QString category(QString::fromStdString(tile.category));
    QTreeWidgetItem *root = categories.value(category);

    root->addChild(newTreeItem(tile));
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
    QString id = previous->data(0, Qt::UserRole).toString();

    if ( tileContainer.hasTileData(id) )
    {
      TileData tile = tileContainer.getTileData(id);
      tileContainer.removeTileData(id);
      writeToTileData(tile);
      tileContainer.addTileData(tile);

      // could be different from what is seen in ui widgets since writeToTileData() adjusts
      QString tileCategory = QString::fromStdString(tile.category);
      QString tileId = QString::fromStdString(tile.id);

      // also update tree
      previous->setText(0, ui.title->text());
      previous->setData(0, Qt::UserRole, tileId);

      // if category changed, move item
      QTreeWidgetItem *root = previous->parent();
      if ( tileCategory != root->text(0) )
      {
        QSignalBlocker blocker(tree);  // avoid recursive call of itemSelected

        // different from current when called from saveTileData()
        QTreeWidgetItem *currentItem = tree->currentItem();

        int idx = root->indexOfChild(previous);
        QTreeWidgetItem *item = root->takeChild(idx);
        if ( root->childCount() == 0 )
        {
          if ( current == root )
            current = nullptr;

          delete root;
        }

        // find new category
        for (int i = 0; i < tree->topLevelItemCount(); i++)
        {
          if ( tree->topLevelItem(i)->text(0) == tileCategory )  // new parent found
          {
            tree->topLevelItem(i)->addChild(item);
            item = nullptr;
            break;
          }
        }

        if ( item )  // only if no toplevel category node found - create a new one
        {
          QTreeWidgetItem *root = newTreeRootItem(tile);
          root->addChild(item);
        }

        tree->setCurrentItem(currentItem);
      }
    }
  }

  if ( !current || !current->data(0, Qt::UserRole).isValid() )
    return;

  // show current item values
  TileData tile = tileContainer.getTileData(current->data(0, Qt::UserRole).toString());
  readFromTileData(tile);
}

//--------------------------------------------------------------------------------

void TileDataUi::ensureUniqueId(TileData &tile)
{
  if ( tile.category.empty() )
    tile.category = "unknown category";

  if ( tile.id.empty() )
    tile.id = tile.category + "1";

  QString id = QString::fromStdString(tile.id);

  if ( tileContainer.hasTileData(id) )  // non-unique; crete a new one
  {
    // find trailing number
    int i;
    for (i = id.length() - 1; id[i].isNumber() && (i > 0); i--) ;

    int num = id.mid(i + 1).toInt();
    QString newId;
    do
    {
      num++;
      newId = id.left(i + 1) + QString::number(num);
    }
    while ( tileContainer.hasTileData(newId) );

    tile.id = newId.toStdString();
  }
}

//--------------------------------------------------------------------------------

void TileDataUi::writeToTileData(TileData &tile)
{
  tile.id = ui.id->text().toStdString();
  tile.category = ui.category->text().toStdString();
  tile.title = ui.title->text().toStdString();
  tile.description = ui.description->toPlainText().toStdString();
  tile.author = ui.author->text().toStdString();

  tile.power = ui.powerProduction->value();
  tile.water = ui.waterProduction->value();
  tile.upkeepCost = ui.upkeepCost->value();
  tile.price = ui.buildCost->value();

  readTileSetDataWidget(tilesSet, tile.tiles);
  readTileSetDataWidget(cornerSet, tile.cornerTiles);
  readTileSetDataWidget(slopeSet, tile.slopeTiles);

  ensureUniqueId(tile);
  readFromTileData(tile);  // might have been changed in ensureUniqueId()
}

//--------------------------------------------------------------------------------

void TileDataUi::readFromTileData(const TileData &tile)
{
  ui.id->setText(QString::fromStdString(tile.id));
  ui.category->setText(QString::fromStdString(tile.category));
  ui.title->setText(QString::fromStdString(tile.title));
  ui.description->setPlainText(QString::fromStdString(tile.description));
  ui.author->setText(QString::fromStdString(tile.author));

  ui.powerProduction->setValue(tile.power);
  ui.waterProduction->setValue(tile.water);
  ui.upkeepCost->setValue(tile.upkeepCost);
  ui.buildCost->setValue(tile.price);

  fillTileSetDataWidget(tilesSet, tile.tiles);
  fillTileSetDataWidget(cornerSet, tile.cornerTiles);
  fillTileSetDataWidget(slopeSet, tile.slopeTiles);
}

//--------------------------------------------------------------------------------

void TileDataUi::fillTileSetDataWidget(const Ui::TileSetDataUi &ui, const TileSetData &data)
{
  ui.fileName->setText(QString::fromStdString(data.fileName));
  ui.width->setValue(data.clippingWidth);
  ui.height->setValue(data.clippingHeight);
  ui.count->setValue(data.count);
  QPixmap pix(QString::fromStdString(data.fileName));
  ui.image->setPixmap(pix);
  ui.origImage->setPixmap(pix);
  ui.size1->setChecked(true);
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
  if ( tree->currentItem() )
    itemSelected(nullptr, tree->currentItem());  // ensure current widget content is stored into TileData

  tileContainer.saveFile();
}

//--------------------------------------------------------------------------------

QTreeWidgetItem *TileDataUi::newTreeRootItem(const TileData &tile)
{
  QTreeWidgetItem *root = new QTreeWidgetItem(tree);
  root->setIcon(0, QIcon::fromTheme("folder"));
  root->setText(0, QString::fromStdString(tile.category));
  root->setExpanded(true);

  return root;
}

//--------------------------------------------------------------------------------

QTreeWidgetItem *TileDataUi::newTreeItem(const TileData &tile)
{
  QTreeWidgetItem *item = new QTreeWidgetItem;
  item->setIcon(0, QIcon::fromTheme("text-x-generic"));
  item->setText(0, QString::fromStdString(tile.title));
  item->setData(0, Qt::UserRole, QString::fromStdString(tile.id));

  return item;
}

//--------------------------------------------------------------------------------

void TileDataUi::newItem()
{
  TileData tile;

  if ( tree->currentItem() )  // ensure current widget content is stored into TileData
  {
    if ( tree->currentItem()->data(0, Qt::UserRole).isValid() )  // item, not root
    {
      QString id = tree->currentItem()->data(0, Qt::UserRole).toString();
      tile.category = tileContainer.getTileData(id).category;
    }
    else
      tile.category = tree->currentItem()->text(0).toStdString();  // root

    tree->setCurrentItem(nullptr);  // and select nothing (this calls itemSelected)
  }

  // get new unique id
  ensureUniqueId(tile);

  if ( tile.title.empty() )
    tile.title = "new " + tile.category;

  addItem(tile);
}

//--------------------------------------------------------------------------------

void TileDataUi::addItem(const TileData &tile)
{
  tileContainer.addTileData(tile);

  // show in tree
  QTreeWidgetItem *root = nullptr;
  for (int i = 0; i < tree->topLevelItemCount(); i++)
  {
    if ( tree->topLevelItem(i)->text(0) == QString::fromStdString(tile.category) )  // new parent found
    {
      root = tree->topLevelItem(i);
      break;
    }
  }

  if ( !root )
    root = newTreeRootItem(tile);

  QTreeWidgetItem *item = newTreeItem(tile);
  root->addChild(item);
  tree->setCurrentItem(item);
}

//--------------------------------------------------------------------------------

void TileDataUi::deleteItem()
{
  if ( !tree->currentItem() || !tree->currentItem()->data(0, Qt::UserRole).isValid() )
    return;

  QString id = tree->currentItem()->data(0, Qt::UserRole).toString();
  TileData tile = tileContainer.getTileData(id);

  QMessageBox::StandardButton ret =
    QMessageBox::question(this, tr("Delete Item"),
                          tr("Shall the item '%1/%2' be deleted?")
                            .arg(QString::fromStdString(tile.category))
                            .arg(QString::fromStdString(tile.title)));

  if ( ret == QMessageBox::No )
    return;

  tileContainer.removeTileData(id);
  QTreeWidgetItem *root = tree->currentItem()->parent();
  delete tree->currentItem();
  tree->setCurrentItem(nullptr);  // and select nothing

  if ( root->childCount() == 0 )
    delete root;
}

//--------------------------------------------------------------------------------

void TileDataUi::duplicateItem()
{
  if ( !tree->currentItem() || !tree->currentItem()->data(0, Qt::UserRole).isValid() )
    return;

  QString id = tree->currentItem()->data(0, Qt::UserRole).toString();
  TileData tile = tileContainer.getTileData(id);

  // get new unique id
  ensureUniqueId(tile);

  addItem(tile);
}

//--------------------------------------------------------------------------------
