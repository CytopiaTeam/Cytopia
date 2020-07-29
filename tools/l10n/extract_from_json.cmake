file(GLOB_RECURSE json_files ../../data/resources/data/*.json)

set(TMP_pot "${CMAKE_BINARY_DIR}/TMP/TMP.pot")
set(final_pot "../../data/languages/Cytopia.pot")

set(pot_cont [==[
msgid ""
msgstr  ""
"Project-Id-Version: PACKAGE VERSION\n"
"Report-Msgid-Bugs-To: \n" 
"POT-Creation-Date: 00000000000000\n"
"PO-Revision-Date: YEAR-MO-DA HO:MI+ZONE\n"
"Last-Translator: FULL NAME <EMAIL@ADDRESS>\n"
"Language-Team: LANGUAGE <LL@li.org>\n"
"Language: \n" 
"MIME-Version: 1.0\n"
"Content-Type: text/plain; charset=CHARSET\n"
"Content-Transfer-Encoding: 8bit\n" 

]==])


foreach (_file IN LISTS json_files)
    file(READ ${_file} _file_cont)
    string(REGEX MATCHALL "\"(title|Text|TooltipText|category|subCategory|description)\": \"((\\\\\"|[^\"])*)\""
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