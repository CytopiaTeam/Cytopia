#!/usr/bin/env node

const fs = require("fs")
const pwd = process.cwd()


let TileDataRaw = fs.readFileSync(`${pwd}/data/resources/data/TileData.json`, "utf8")
let TileData = JSON.parse(TileDataRaw)

let failed = 0

TileData.forEach(e => {
    let ex = fs.existsSync(`${pwd}/data/${e.tiles.fileName}`)
    if (!ex){
        console.error(`File does not exist: ${e.tiles.fileName}`)
        failed = 1
    }
})

let UIDataRaw = fs.readFileSync(`${pwd}/data/resources/data/UIData.json`, "utf8")
let UIData = JSON.parse(UIDataRaw)

for (const [_, ent] of Object.entries(UIData)) {
    for (const [_, image] of Object.entries(ent)){
        let ex = fs.existsSync(`${pwd}/data/${image}`)
        if (!ex){
            console.error(`File does not exist: ${image}`)
            failed = 1
        }
    }
}


process.exit(failed)