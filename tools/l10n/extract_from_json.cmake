file(GLOB_RECURSE json_files ../../data/resources/data/*.json)

set(TMP_pot "${CMAKE_BINARY_DIR}/TMP/TMP.pot")
set(final_pot "../../data/languages/Cytopia.pot")

set(pot_cont "\
msgid \"\"\n \
msgstr \"\" \n\
\"Project-Id-Version: PACKAGE VERSION\\n\" \n\
\"Report-Msgid-Bugs-To: \\n\" \n\
\"POT-Creation-Date: 2019-06-13 08:32+0000\\n\" \n\
\"PO-Revision-Date: YEAR-MO-DA HO:MI+ZONE\\n\" \n\
\"Last-Translator: FULL NAME <EMAIL@ADDRESS>\\n\"\n\
\"Language-Team: LANGUAGE <LL@li.org>\\n\" \n\
\"Language: \\n\" \n\
\"MIME-Version: 1.0\\n\" \n\
\"Content-Type: text/plain; charset=CHARSET\\n\" \n\
\"Content-Transfer-Encoding: 8bit\\n\" \n\n")


foreach (_file IN LISTS json_files)
    file(READ ${_file} _file_cont)
    string(REGEX MATCHALL "\"(title|Text|TooltipText|category|description)\": \"((\\\\\"|[^\"])*)\""
            reg_match ${_file_cont})
    message("Extracting strings from ${_file}")
    foreach (_line IN LISTS reg_match)
        string(REGEX MATCH "\"[A-Za-z0-9_]*\": \"(.*)\"" _ ${_line})
        set(pot_cont ${pot_cont} "\
msgid \"${CMAKE_MATCH_1}\" \n\
msgstr \"\"\n\n")
    endforeach ()
endforeach ()

file(WRITE ${TMP_pot} ${pot_cont})

message("Extracting strings from source files")
execute_process(COMMAND bash "-c" "find src -iname '*.cxx' -not -path '*/TileDataUi/*' | xargs xgettext -o${TMP_pot} -ksetText -c -s -j"
                WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/../..")

message("Cleaning up")
execute_process(COMMAND msguniq -i -s -o${final_pot} ${TMP_pot}
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR})