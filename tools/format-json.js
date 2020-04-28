#!/usr/bin/env node
const fs = require("fs");

const fileList = [
    "../data/resources/settings.json",
    "../data/resources/data/AudioConfig.json",
    "../data/resources/data/TerrainGen.json",
    "../data/resources/data/TileData.json",
    "../data/resources/data/UIData.json",
    "../data/resources/data/UILayout.json"
];

function formatFile(file, index) {
    fs.readFile(file, "utf8", (err, content) => {
        if (err) throw err;
        fs.writeFile(file, JSON.stringify( JSON.parse(content), null, 4), (err) => {
            if (err) throw err;
            console.log(`formatting file ${file} done`);
        });
    });
}

fileList.forEach(formatFile);



