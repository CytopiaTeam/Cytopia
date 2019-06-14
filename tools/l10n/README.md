Basically these steps are required to get up to date translations:
- dev only: Extract \*.pot template from code, upload to Transifex (see below)
- wait for community to translate
- download \<lang.po\> for desired translations
- compile \<lang.mo\> for these translations (see below)
- put \<lang\>.mo in ```bin/languages/```

### Update source file on Transifex
The source file (\*.pot) on Transifex serves as the base for all translations and
should be updated whenever new strings which need translation have been added to the source.

```bash
# extract new translations template
extract_pot.sh
cmake -P extract_from_json.cmake
# now update the source file on Transifex:
# Cytopia -> more project options -> resource -> *.pot -> update
```

**Note:** The Cytopia_Json.pot contains duplicate keys.
These can be removed by running `msguniq -o Cytopia.pot Cytopia.pot`

### Compile *.mo files from Transifex
A \*.mo file is a translation for a given language in binary form.  
Cytopia needs \*.mo files for the translations to appear ingame.

```bash
# setup local Transifex repo, pull all translations
setup_transifex.sh
# copy translations from local Transifex repo to languages folder
from_transifex.sh
# compiles *.mo files for all languages where translations are available
compile_mo.sh
# The languages folder can now be put in the bin folder (same level as the executable).
```
