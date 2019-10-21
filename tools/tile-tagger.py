#!/usr/bin/env python3

import json
from pathlib import Path

base_path = Path(__file__).parent
tile_data_path = (base_path / "../data/resources/data/TileData.json").resolve()

removed_words = ["a", "an", "and", "be", "become", "but", "by", "can", "do", "does", "doesn't", "from", "has", "have", "i", "in", "into", "is", "it", "for", "much", "not", "of", "since", "that", "the", "they", "to", "was", "with", "you"]

with open(tile_data_path, "r") as data_file:
    tile_data = json.load(data_file)

for tile in tile_data:
    tag = tile["description"].lower().split()

    # Removing forbidden words
    tag = list(filter(lambda x: x not in removed_words, tag))
    # Removing symbols
    tag = list(map(lambda x: ''.join(i for i in x if i.isalpha()), tag))
    # Removing empty strings
    tag = list(filter(lambda x: x != '', tag))

    tile["tag"] = tag

with open(tile_data_path, "w") as tagged_file:
    json.dump(tile_data, tagged_file)
