#ifndef MAP_SERIALIZER_HXX_
#define MAP_SERIALIZER_HXX_

#include <fstream>
#include "../map.hxx"
#include "../../ThirdParty/json.hxx"

using json = nlohmann::json;

void saveMap(Map* map, const std::string fileName)
{
  json j = json{ "mapNode", map->_mapNodes};

  std::ofstream file(fileName);
  file << j;
}

// JSON serializer for MapNode class
void to_json(json &j, const MapNode *m) { j = json{ { "_tileID", m->getTileID() },{ "_coordinates", m->getCoordinates() } }; }

// JSON serializer for Point struct
void to_json(json &j, const Point &point) { j = json{ { "x", point.x },{ "y", point.y },{ "z", point.z },{ "height", point.height } }; }

#endif
