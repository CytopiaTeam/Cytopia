#!/usr/bin/env node
const fs = require("fs");

const fileList = [
    `${__dirname}/data/resources/settings.json`,
    `${__dirname}/data/resources/data/AudioConfig.json`,
    `${__dirname}/data/resources/data/TerrainGen.json`,
    `${__dirname}/data/resources/data/TileData.json`,
    `${__dirname}/data/resources/data/UIData.json`,
    `${__dirname}/data/resources/data/UILayout.json`
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
