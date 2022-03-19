file(GLOB_RECURSE potfiles ../../data/languages/*/*.po)

foreach (file IN LISTS potfiles)
    get_filename_component(dir ${file} DIRECTORY)
    get_filename_component(filename ${file} NAME_WE)
    get_filename_component(name "${dir}" NAME)
    message("Compiling ${name}")
    execute_process(COMMAND msgfmt -o "${filename}.mo" ${file} WORKING_DIRECTORY ${dir})
endforeach ()
