#include "mapNode.hxx"
#include "../basics/log.hxx"

MapNode::MapNode(Point isoCoordinates) : _isoCoordinates(std::move(isoCoordinates))
{
  _sprite = std::make_unique<Sprite>(_isoCoordinates);
  updateTexture();
}

void MapNode::increaseHeight()
{
  int height = _isoCoordinates.height;

  if (height < _maxHeight)
  {
    _isoCoordinates.height++;
    _sprite->setCoordinates(_isoCoordinates);
  }
}

void MapNode::decreaseHeight()
{
  int height = _isoCoordinates.height;

  if (height > 0)
  {
    _isoCoordinates.height--;
    _sprite->setCoordinates(_isoCoordinates);
  }
}

void MapNode::render() const { _sprite->render(); }

void MapNode::setBitmask(unsigned char elevationBitmask, unsigned char tileIDBitmask)
{
  _elevationBitmask = elevationBitmask;
  _tileIDBitmask = tileIDBitmask;
  updateTexture();
}

void MapNode::setTileID(const std::string &tileID)
{
  _previousTileID = _tileID;
  _tileID = tileID;
  updateTexture();
}

void MapNode::updateTexture()
{
  SDL_Rect clipRect = {0, 0, 0, 0};

  _tileData = TileManager::instance().getTileData(_tileID);
  _tileMap = TileMap::DEFAULT;

  if (_tileData)
  {
    // only calculate orientation for textures that adjust themselves according to elevation / other tiles of the same id
    if (_tileData->category == "terrain" || _tileData->category == "road" || _tileData->category == "water")
    {
      _orientation = TileManager::instance().caluclateSlopeOrientation(_elevationBitmask);

      if (_orientation == TileSlopes::DEFAULT_ORIENTATION)
      {
        if (_tileData->category != "terrain")
        {
          _orientation = TileManager::instance().caluclateTileOrientation(_tileIDBitmask);
        }
      }
      else
      {
        _tileMap = TileMap::SLOPES;
      }
    }
    else
    {
      _orientation = TileList::TILE_DEFAULT_ORIENTATION;
    }

    switch (_tileMap)
    {
    case TileMap::DEFAULT:
      clipRect.x = _tileData->tiles.clippingWidth * static_cast<int>(_orientation);
      _clippingWidth = _tileData->tiles.clippingWidth;
      if (_tileID == "terrain")
      {
        clipRect.x = 0;
        _sprite->setClipRect({0, 0, _tileData->tiles.clippingWidth, _tileData->tiles.clippingHeight});
      }
      else
      {
        _sprite->setClipRect({clipRect.x, 0, _tileData->tiles.clippingWidth, _tileData->tiles.clippingHeight});
      }
      _spriteCount = _tileData->tiles.count;
      break;
    case TileMap::CORNERS:
      _clippingWidth = _tileData->cornerTiles.clippingWidth;
      clipRect.x = _tileData->cornerTiles.clippingWidth * static_cast<int>(_orientation);
      _sprite->setClipRect({clipRect.x, 0, _tileData->cornerTiles.clippingWidth, _tileData->cornerTiles.clippingHeight});
      _spriteCount = _tileData->cornerTiles.count;
      break;
    case TileMap::SLOPES:
      _clippingWidth = _tileData->slopeTiles.clippingWidth;
      clipRect.x = _tileData->slopeTiles.clippingWidth * static_cast<int>(_orientation);
      _sprite->setClipRect({clipRect.x, 0, _tileData->slopeTiles.clippingWidth, _tileData->slopeTiles.clippingHeight});
      _spriteCount = _tileData->slopeTiles.count;
      break;
    }
    if (clipRect.x >=  static_cast<int>(_spriteCount) * _clippingWidth)
    {
      _tileID = _previousTileID;
      updateTexture();
    }
    _sprite->setSpriteCount(_spriteCount);
    _sprite->setTexture(TileManager::instance().getTexture(_tileID, _tileMap));
  }
  else
  {
    LOG(LOG_ERROR) << "Could not retrieve TileData Information for TileID " << _tileID;
  }
}

void MapNode::setCoordinates(const Point &newIsoCoordinates)
{
  _isoCoordinates = newIsoCoordinates;
  _sprite->setCoordinates(_isoCoordinates);
}
