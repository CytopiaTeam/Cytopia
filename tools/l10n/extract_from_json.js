#!/usr/bin/env node

const fs = require("fs")
const child_process = require('child_process')
const path = require('path')

const potPath = `${__dirname}/../../data/languages/Cytopia.pot`
const TMPpotPath = `${__dirname}/TMP.pot`

var PotContent = ""

let TileDataRaw = fs.readFileSync(`${__dirname}/../../data/resources/data/TileData.json`, "utf8")
let TileData = JSON.parse(TileDataRaw)

let categories = []
let subCategories = []

console.log("Extracting data from TileData.json")

TileData.forEach(e => {
    if (!e.title.includes("TODO"))
        AddPotContent(e.title, `This is a title for an object with the description: ${e.description}`)
    if (!e.description.includes("TODO"))
        AddPotContent(e.description, `This is a description for an object titled: ${e.title}`)

    if (!categories.includes(e.category))
        categories.push(e.category)
    if (!subCategories.includes(e.subCategory))
        subCategories.push(e.subCategory)
})

console.log("Adding categories")

categories.forEach(e => {
    AddPotContent(e, `This is a category`)
})

subCategories.forEach(e => {
    AddPotContent(e, `This is a subCategory`)
})


console.log("Extracting data from UILayout.json")

let UILayoutRaw = fs.readFileSync(`${__dirname}/../../data/resources/data/UILayout.json`, "utf8")
let UILayout = JSON.parse(UILayoutRaw)

for (const [key, value] of Object.entries(UILayout.UiElements)) {
    value.forEach(e => {
        let desc = null
        if (e.ID)
            desc = `This is a UiElement with the ID: ${e.ID}`
        else if (e.Action && e.Action != "ToggleVisibilityOfGroup")
            desc = `This is a UiElement with the Action: ${e.Action}`
        else if (e.Type)
            desc = `This is a UiElement with of the type: ${e.Type}`

        if (e.Text)
            AddPotContent(e.Text, desc)
        if (e.TooltipText)
            AddPotContent(e.TooltipText, desc)
    })
}

fs.writeFileSync(TMPpotPath, PotContent)

console.log("Extracting data from .cxx files")

for (const f of getFiles(`${__dirname}/../../src/`)) {
    if (f.endsWith('.cxx')) {
        let cmd = `xgettext -o${TMPpotPath} -ksetText -c -s -j ${f}`
        child_process.execSync(cmd)
    }
}

console.log("Cleaning up")
let cmd = `msguniq -i -s -o${potPath} ${TMPpotPath}`
child_process.execSync(cmd)

console.log("Done!")


// ############################
// Helpers
// ############################


function AddPotContent(content, comment = null) {
    if (!content)
        return

    PotContent += comment ? `
#. ${comment}
msgid "${content}"
msgstr ""

`: `
msgid "${content}"
msgstr ""

`
}


function* getFiles(dir) {
    const dirents = fs.readdirSync(dir, { withFileTypes: true });
    for (const dirent of dirents) {
        const res = path.resolve(dir, dirent.name);
        if (dirent.isDirectory()) {
            yield* getFiles(res);
        } else {
            yield res;
        }
    }
}