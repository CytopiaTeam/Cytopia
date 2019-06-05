#include "TileDataContainer.hxx"

#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>

//--------------------------------------------------------------------------------

bool TileDataContainer::hasTileData(const QString &id) const { return tileData.contains(id); }

//--------------------------------------------------------------------------------

TileData TileDataContainer::getTileData(const QString &id) const
{
  if (!tileData.contains(id))
    return TileData();

  return tileData[id];
}

//--------------------------------------------------------------------------------

QString TileDataContainer::loadFile(const QString &theFileName)
{
  fileName = theFileName;
  tileData.clear();

  QFile file(fileName);
  if (!file.exists())
    return QString();

  if (!file.open(QIODevice::ReadOnly))
    return file.errorString();

  QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
  if (doc.isNull() || !doc.isArray())
    return tr("Illegal file content");

  for (const QJsonValue &value : doc.array())
  {
    QJsonObject obj = value.toObject();

    TileData tile;
    QString id = obj.value("id").toString();
    tile.id = id.toStdString();
    tile.category = obj.value("category").toString().toStdString();
    tile.title = obj.value("title").toString().toStdString();
    tile.description = obj.value("description").toString().toStdString();
    tile.author = obj.value("author").toString().toStdString();
    tile.price = obj.value("price").toInt();
    tile.upkeepCost = obj.value("upkeep cost").toInt();
    tile.power = obj.value("power").toInt();
    tile.water = obj.value("water").toInt();

    tileSetDataFromJson(tile.tiles, obj.value("tiles"));
    tileSetDataFromJson(tile.cornerTiles, obj.value("cornerTiles"));
    tileSetDataFromJson(tile.slopeTiles, obj.value("slopeTiles"));

    tileData.insert(id, tile);
  }

  return QString();
}

//--------------------------------------------------------------------------------

void TileDataContainer::tileSetDataFromJson(TileSetData &data, const QJsonValue &value)
{
  QJsonObject obj = value.toObject();

  data.fileName = obj.value("fileName").toString().toStdString();
  data.count = obj.value("count").toInt();
  data.clippingWidth = obj.value("clip_width").toInt();
  data.clippingHeight = obj.value("clip_height").toInt();
}

//--------------------------------------------------------------------------------

bool TileDataContainer::saveFile()
{
  QJsonDocument doc;
  QJsonArray array;

  for (const TileData &tile : tileData)
  {
    QJsonObject obj;
    obj.insert("id", QString::fromStdString(tile.id));
    obj.insert("category", QString::fromStdString(tile.category));
    obj.insert("title", QString::fromStdString(tile.title));
    obj.insert("description", QString::fromStdString(tile.description));
    obj.insert("author", QString::fromStdString(tile.author));
    obj.insert("price", tile.price);
    obj.insert("upkeep cost", tile.upkeepCost);
    obj.insert("power", tile.power);
    obj.insert("water", tile.water);

    if (!tile.tiles.fileName.empty())
      obj.insert("tiles", tileSetDataToJson(tile.tiles));

    if (!tile.cornerTiles.fileName.empty())
      obj.insert("cornerTiles", tileSetDataToJson(tile.cornerTiles));

    if (!tile.slopeTiles.fileName.empty())
      obj.insert("slopeTiles", tileSetDataToJson(tile.slopeTiles));

    array.append(obj);
  }

  doc.setArray(array);

  QFile file(fileName);
  if (!file.open(QIODevice::WriteOnly))
  {
    // error handling
    return false;
  }
  file.write(doc.toJson());
  return true;
}

//--------------------------------------------------------------------------------

QJsonObject TileDataContainer::tileSetDataToJson(const TileSetData &data)
{
  QJsonObject obj;

  obj.insert("fileName", QString::fromStdString(data.fileName));
  obj.insert("count", data.count);
  obj.insert("clip_width", data.clippingWidth);
  obj.insert("clip_height", data.clippingHeight);

  return obj;
}

//--------------------------------------------------------------------------------

void TileDataContainer::removeTileData(const QString &id) { tileData.remove(id); }

//--------------------------------------------------------------------------------

void TileDataContainer::addTileData(const TileData &tile) { tileData.insert(QString::fromStdString(tile.id), tile); }

//--------------------------------------------------------------------------------
