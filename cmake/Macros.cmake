macro(get_sub_dirs result curdir)
    FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
    SET(dirlist "")
    FOREACH (child ${children})
        IF (IS_DIRECTORY ${curdir}/${child})
            LIST(APPEND dirlist ${child})
        ENDIF ()
    ENDFOREACH ()
    SET(${result} ${dirlist})
endmacro(get_sub_dirs)

function(recursive_zip_folder in_dir out_dir)
    get_filename_component(FROM_DIR_NAME ${in_dir} NAME)
    set(TMP_FILE_DIR ${CMAKE_BINARY_DIR}/tmp/${name})
    get_sub_dirs(SUB_DIRS "${in_dir}")
    file(MAKE_DIRECTORY ${out_dir})

    foreach (ZIP_DIR ${SUB_DIRS})
        if ("${ZIP_DIR}" STREQUAL ".git")
            continue()
        endif ()

        file(GLOB_RECURSE ZIP_FILES LIST_DIRECTORIES TRUE "${in_dir}/${ZIP_DIR}/*")

        set(ZIP_FILES_TXT "")
        set(ZIP_FILES_DEP "")
        foreach (ZIP_FILE ${ZIP_FILES})
            file(RELATIVE_PATH REL_PATH "${in_dir}/${ZIP_DIR}" "${ZIP_FILE}")
            set(ZIP_FILES_TXT "${ZIP_FILES_TXT}${REL_PATH}\n")
            if (NOT IS_DIRECTORY ${ZIP_FILE})
                list(APPEND ZIP_FILES_DEP ${ZIP_FILE})
            endif ()
        endforeach ()
        file(WRITE "${TMP_FILE_DIR}/${ZIP_DIR}-filelist.txt" ${ZIP_FILES_TXT})

        set(ZIP_NAME "${out_dir}/${ZIP_DIR}.zip")
        add_custom_command(
                OUTPUT ${ZIP_NAME}
                DEPENDS ${ZIP_FILES_DEP}
                COMMAND ${CMAKE_COMMAND} -E tar c ${ZIP_NAME} --format=zip --files-from="${TMP_FILE_DIR}/${ZIP_DIR}-filelist.txt"
                WORKING_DIRECTORY "${in_dir}/${ZIP_DIR}"
        )
        list(APPEND ALL_ZIPS ${ZIP_NAME})
    endforeach ()

    add_custom_target(
            zip_folder_${FROM_DIR_NAME} ALL
            DEPENDS ${ALL_ZIPS}
    )
    set_property(TARGET zip_folder_${FROM_DIR_NAME} PROPERTY FOLDER "Scripts")
endfunction(recursive_zip_folder)

function(fast_copy FROM_DIR TO_DIR)
    file(GLOB_RECURSE files "${FROM_DIR}/*")
    get_filename_component(FROM_DIR_NAME ${FROM_DIR} NAME)

    foreach (file IN LISTS files)
        file(RELATIVE_PATH output_file ${FROM_DIR} ${file})
        set(dest "${TO_DIR}/${output_file}")

        add_custom_command(
                COMMENT "Copying ${output_file}"
                OUTPUT ${dest}
                DEPENDS ${file}
                COMMAND ${CMAKE_COMMAND} -E copy ${file} ${dest}
        )
        list(APPEND ALL_FILES ${dest})
    endforeach ()

    add_custom_target(
            fast_copy_${FROM_DIR_NAME} ALL
            DEPENDS ${ALL_FILES}
    )
    set_property(TARGET fast_copy_${FROM_DIR_NAME} PROPERTY FOLDER "Scripts")
endfunction()

function(extract_pot source_files_list)
    set(FILELIST ${CMAKE_BINARY_DIR}/tmp/xgettext-filelist.txt)
    set(ROR_POT "${CMAKE_SOURCE_DIR}/languages/ror.pot")

    list(JOIN source_files_list "\n" source_files)
    file(WRITE "${FILELIST}" "${source_files}")

    add_custom_target(
            extract_pot
            COMMENT "Extract pot"
            OUTPUT ${ROR_POT}
            COMMAND xgettext -i -F -k_L -k_LC:1c,2 -c -o${ROR_POT} -f ${FILELIST}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    set_property(TARGET extract_pot PROPERTY FOLDER "Scripts")
endfunction()

function(compile_mo)
    file(GLOB_RECURSE potfiles ${CMAKE_SOURCE_DIR}/languages/*/*.po CONFIGURE_DEPENDS)

    foreach (file IN LISTS potfiles)
        get_filename_component(dir ${file} DIRECTORY)
        get_filename_component(filename ${file} NAME_WE)
        get_filename_component(name "${dir}" NAME)
        set(out "${dir}/${filename}.mo")
        add_custom_command(
                COMMENT "Compiling ${name}"
                OUTPUT ${out}
                DEPENDS ${file}
                COMMAND msgfmt -o${out} ${file}
                WORKING_DIRECTORY ${dir}
        )
        list(APPEND MO_FILES ${out})
    endforeach ()

    add_custom_target(
            compile_mo
            DEPENDS ${MO_FILES}
    )
    set_property(TARGET compile_mo PROPERTY FOLDER "Scripts")
endfunction()

# Argument parser helper. This may look like magic, but it is pretty simple:
# - Call this at the top of a function
# - It takes three "list" arguments: `.`, `-` and `+`.
# - The `.` arguments specify the "option/boolean" values to parse out.
# - The `-` arguments specify the one-value arguments to parse out.
# - The `+` argumenst specify mult-value arguments to parse out.
# - Specify `-nocheck` to disable warning on unparse arguments.
# - Parse values are prefixed with `ARG`
#
# This macro makes use of some very horrible aspects of CMake macros:
# - Values appear the caller's scope, so no need to set(PARENT_SCOPE)
# - The ${${}ARGV} eldritch horror evaluates to the ARGV *OF THE CALLER*, while
#   ${ARGV} evaluates to the macro's own ARGV value. This is because ${${}ARGV}
#   inhibits macro argument substitution. It is painful, but it makes this magic
#   work.
macro(better_parse_args)
    cmake_parse_arguments(_ "-nocheck;-hardcheck" "" ".;-;+" "${ARGV}")
    set(__arglist "${${}ARGV}")
    better_parse_arglist("${__.}" "${__-}" "${__+}")
endmacro()

macro(better_parse_arglist opt args list_args)
    cmake_parse_arguments(ARG "${opt}" "${args}" "${list_args}" "${__arglist}")
    if (NOT __-nocheck)
        foreach (arg IN LISTS ARG_UNPARSED_ARGUMENTS)
            message(WARNING "Unknown argument: ${arg}")
        endforeach ()
        if (__-hardcheck AND NOT ("${ARG_UNPARSED_ARGUMENTS}" STREQUAL ""))
            message(FATAL_ERROR "Unknown arguments provided.")
        endif ()
    endif ()
endmacro()


macro(lift_var)
    foreach (varname IN ITEMS ${ARGN})
        set("${varname}" "${${varname}}" PARENT_SCOPE)
    endforeach ()
endmacro()

# Taken from wxWidgets
#
# cmd_option(<name> <desc> [default] [STRINGS strings])
# The default is ON if third parameter isn't specified
function(cmd_option name desc)
    cmake_parse_arguments(OPTION "" "" "STRINGS" ${ARGN})

    if (ARGC EQUAL 2)
        if (OPTION_STRINGS)
            list(GET OPTION_STRINGS 1 default)
        else ()
            set(default ON)
        endif ()
    else ()
        set(default ${OPTION_UNPARSED_ARGUMENTS})
    endif ()

    if (OPTION_STRINGS)
        set(cache_type STRING)
    else ()
        set(cache_type BOOL)
    endif ()

    set(${name} "${default}" CACHE ${cache_type} "${desc}")
    if (OPTION_STRINGS)
        set_property(CACHE ${name} PROPERTY STRINGS ${OPTION_STRINGS})

        # Check valid value
        set(value_is_valid FALSE)
        set(avail_values)
        foreach (opt ${OPTION_STRINGS})
            if (${name} STREQUAL opt)
                set(value_is_valid TRUE)
                break()
            endif ()
            string(APPEND avail_values " ${opt}")
        endforeach ()
        if (NOT value_is_valid)
            message(FATAL_ERROR "Invalid value \"${${name}}\" for option ${name}. Valid values are: ${avail_values}")
        endif ()
    endif ()

    set(${name} "${${name}}" PARENT_SCOPE)
endfunction()

# Helper to retrieve the settings returned from pkg_check_modules()
macro(get_package_interface package)
    set(INCLUDES ${${package}_INCLUDE_DIRS})

    set(LINKDIRS ${${package}_LIBDIR})

    # We resolve the full path of each library to ensure the
    # correct one is referenced while linking
    foreach (lib ${${package}_LIBRARIES})
        find_library(LIB_${lib} ${lib} HINTS ${LINKDIRS})
        list(APPEND LIBRARIES ${LIB_${lib}})
    endforeach ()
endmacro()

macro(find_with_pkg package interface_name PKG_CONFIG)
    pkg_check_modules(${package} ${PKG_CONFIG})

    if (${package}_FOUND)
        message(STATUS "Using '${package}' system library (Found by pkg_config)")

        # Create the target interface library
        add_library(${interface_name} INTERFACE IMPORTED GLOBAL)

        # Retrieve the package information
        get_package_interface(${package})

        # And add it to our target
        target_include_directories(${interface_name} INTERFACE ${INCLUDES})
        target_link_libraries(${interface_name} INTERFACE ${LIBRARIES})

        message(STATUS "Added inteface ${interface_name} ${INCLUDES} ${LIBRARIES}")
    endif ()
endmacro()