#!/bin/sh
set -eu

# extract strings preceded by a specific macro to a .pot file as a template for translations
cd ../../
find src/TileDataUi -iname "*.cxx" | xargs xgettext -odata/languages/TileDataUi.pot -ktr -k"_LC:1c,2" -c -s -j
