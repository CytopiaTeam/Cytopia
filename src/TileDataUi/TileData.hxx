#ifndef _TILE_DATA_H_
#define _TILE_DATA_H_

#include <QObject>
#include <QMap>
#include <QString>
#include <QJsonObject>

#include <engine/basics/tileData.hxx>

class TileDataContainer : public QObject
{
  public:
    QString loadFile(const QString &fileName);
    bool saveFile();

    TileData *getTileData(const QString &id);

    using Map = QMap<QString, TileData>;
    Map::iterator begin() { return tileData.begin(); }
    Map::iterator end() { return tileData.end(); }

  private:
    void tileSetDataFromJson(TileSetData &data, const QJsonValue &value);
    QJsonObject tileSetDataToJson(const TileSetData &data);

  private:
    QString fileName;
    QMap<QString, TileData> tileData;
};

#endif
