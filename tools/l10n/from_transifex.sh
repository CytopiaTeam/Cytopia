#!/bin/sh
set -eu

cd tx
tx pull -a

# Moves po files from local tx repo to folder structure
cd translations/cytopia.cytopiapot

for file in *
do
	file="${file%.*}"
	echo "Now copying $file from Cytopia"
	mkdir -p ../../../languages/$file/
	cp $file.po ../../../languages/$file/Cytopia.po
done