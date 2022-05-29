# Load Conan
include(conan)
include(FindPkgConfig)

if (USE_PACKAGE_MANAGER)
    conan_add_remote(NAME rigs-of-rods-deps-V2
            URL https://artifactory.anotherfoxguy.com/artifactory/api/conan/rigs-of-rods
            VERIFY_SSL True
            )
endif ()

set(PKG_CONFIG_USE_CMAKE_PREFIX_PATH TRUE)

set(CONAN_BUILD_REQUIRES)
set(CONAN_REQUIRES)
set(CONAN_PACKAGE_OPTIONS)
set(CONAN_ONLY_DEBUG_RELEASE)
set(CONAN_CONFIG_OPTIONS)
set(CONAN_RESOLVE_LIST)

# Add a external dependency
# Example usage:
# add_external_lib( 
#   OGRE
#   ogre/1.12
#   OPTION_NAME ogre
#   SYMBOL OGRE
#   REQUIRED
#   FIND_PACKAGE
#   ALWAYS_ALLOW_CONAN_FALLBACK
#   PKG_CONFIG "ogre >= 1.11"
#   FIND_PACKAGE_OPTIONS COMPONENTS Bites Overlay Paging RTShaderSystem MeshLodGenerator Terrain
#   INTERFACE_NAME ogre::ogre
#   HAS_ONLY_DEBUG_RELEASE
#   CONAN_OPTIONS 
#        ogre:shared=True
# )

function(add_external_lib package conan_package_name)
    list(REMOVE_AT ARGV 0 1)
    better_parse_args(
            . REQUIRED FIND_PACKAGE ALWAYS_ALLOW_CONAN_FALLBACK HAS_ONLY_DEBUG_RELEASE BY_DEFAULT_DISABLED
            - OPTION_NAME SYMBOL INTERFACE_NAME CONAN_PKG_NAME
            + CONAN_OPTIONS FIND_PACKAGE_OPTIONS PKG_CONFIG
    )

    if (ARG_OPTION_NAME)
        set(option_name_base ${ARG_OPTION_NAME})
    else ()
        string(TOUPPER "${package}" option_name_base)
    endif ()

    if (ARG_ALWAYS_ALLOW_CONAN_FALLBACK)
        set(system_only OFF)
    else ()
        set(system_only ${${_PREFIX}FORCE_SYSTEM_DEPENDENCIES})
    endif ()

    if (ARG_FIND_PACKAGE_OPTIONS)
        set(ARG_FIND_PACKAGE ON)
    endif ()

    if (ARG_INTERFACE_NAME)
        set(interface_name ${ARG_INTERFACE_NAME})
    else ()
        set(interface_name "${package}::${package}")
    endif ()

    if (ARG_SYMBOL)
        set(symbol ${ARG_SYMBOL})
    else ()
        set(symbol ${option_name_base})
    endif ()

    # Generate CMake option
    set(option_name ${_PREFIX}USE_${option_name_base})
    set(option_desc "CONAN")

    if (NOT USE_PACKAGE_MANAGER)
        set(sysopt "SYSTEM")
        set(default "SYSTEM")
        set(option_desc "SYSTEM (forced)")
        set(system_only ON)
    elseif (ARG_PKG_CONFIG OR ARG_FIND_PACKAGE)
        set(sysopt "SYSTEM")
        string(PREPEND option_desc "SYSTEM (if available), ")
        set(default "${${_PREFIX}LIB_PREFERENCE}")
    else ()
        set(default "CONAN")
    endif ()

    if (ARG_BY_DEFAULT_DISABLED)
        set(default "OFF")
    endif()

    if (NOT ARG_REQUIRED)
        set(reqopt "OFF")
        string(APPEND option_desc ", OFF")
    endif ()

    cmd_option(${option_name}
            "Use ${option_name_base} library [${option_desc}]"
            "${default}"
            STRINGS ${sysopt} "CONAN" ${reqopt}
            )
    # Early bail out
    if (${option_name} STREQUAL "OFF")

        message(STATUS "${package} disabled")

        set(USE_${symbol} OFF CACHE INTERNAL "" FORCE)

        return()
    endif ()

    set(USE_${symbol} ON CACHE INTERNAL "" FORCE)

    if (TARGET "${package}")
        return()
    endif ()

    if (${option_name} STREQUAL "SYSTEM")
        if (ARG_PKG_CONFIG AND PKG_CONFIG_FOUND)
            pkg_check_modules(PKG_${package} ${ARG_PKG_CONFIG})

            if (PKG_${package}_FOUND)
                message(STATUS "Using '${package}' system library (Found by pkg_config)")

                # Create the target interface library
                add_library(${interface_name} INTERFACE IMPORTED GLOBAL)

                # Retrieve the package information
                get_package_interface(PKG_${package})

                # And add it to our target
                target_include_directories(${interface_name} INTERFACE ${INCLUDES})
                target_link_libraries(${interface_name} INTERFACE ${LIBRARIES})

                message(STATUS "Added inteface ${interface_name} ${INCLUDES} ${LIBRARIES}")
                return()
            endif ()
        endif ()

        if (ARG_FIND_PACKAGE OR NOT PKG_CONFIG_FOUND)
            if (ARG_REQUIRED)
                list(APPEND find_package_options REQUIRED)
            endif ()

            find_package(${package} ${find_package_options})

            if (${package}_FOUND)
                message(STATUS "Using '${package}' system library (Found by find_package)")
                return()
            endif ()
        endif ()

        if (system_only)
            if (ARG_REQUIRED)
                message(FATAL_ERROR "Failed to find the required system package ${package}")
            endif ()

            message(STATUS "\tFailed to find the system package ${package}")

            set(USE_${symbol} OFF CACHE INTERNAL "" FORCE)
            set(${option_name} "OFF")
            set_property(CACHE ${option_name} PROPERTY VALUE "OFF")

            return()
        else ()
            set(${option_name} "CONAN")
            set_property(CACHE ${option_name} PROPERTY VALUE "CONAN")
            message(STATUS "'${package}' not found on system, falling back to conan")
        endif ()
    endif ()

    if (ARG_CONAN_PKG_NAME)
        set(conan_package_name_only ${ARG_CONAN_PKG_NAME})
    else ()
        # We only want the name of the library
        string(REPLACE "/" ";" pkg_name_split ${conan_package_name})
        list(GET pkg_name_split 0 conan_package_name_only)
    endif ()

    list(APPEND CONAN_REQUIRES ${conan_package_name})
    list(APPEND CONAN_PACKAGE_OPTIONS ${ARG_CONAN_OPTIONS})
    list(APPEND CONAN_RESOLVE_LIST "${conan_package_name_only}|${interface_name}")

    if (ARG_HAS_ONLY_DEBUG_RELEASE)
        message(STATUS "${package} only has Debug and Release versions")
        list(APPEND CONAN_ONLY_DEBUG_RELEASE ${package})
    endif ()

    lift_var(CONAN_REQUIRES
            CONAN_PACKAGE_OPTIONS
            CONAN_RESOLVE_LIST
            CONAN_ONLY_DEBUG_RELEASE)

    message(STATUS "Adding Conan dependency ${package}")
endfunction()

function(_conan_install build_type)
    conan_cmake_configure(
            REQUIRES ${CONAN_REQUIRES}
            GENERATORS cmake_multi
            BUILD_REQUIRES ${CONAN_BUILD_REQUIRES}
            ${CONAN_CONFIG_OPTIONS}
            IMPORTS "bin, *.dll -> ${RUNTIME_OUTPUT_DIRECTORY} @ keep_path=False"
            IMPORTS "lib, *.dll -> ${RUNTIME_OUTPUT_DIRECTORY} @ keep_path=False"
            IMPORTS "lib, *.dylib -> ${RUNTIME_OUTPUT_DIRECTORY} @ keep_path=False"
            IMPORTS "lib, *.so* -> ${RUNTIME_OUTPUT_DIRECTORY} @ keep_path=False"
            OPTIONS ${CONAN_PACKAGE_OPTIONS}
    )

    # Hash the conanfile.txt to check if it has changed, so we can skip running conan
    set(hash_name "${build_type}_CONAN_HASH")
    file(MD5 "${CMAKE_CURRENT_BINARY_DIR}/conanfile.txt" conanfile_hash)

    if ("_${conanfile_hash}" STREQUAL "_${${hash_name}}" AND EXISTS "${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo_multi.cmake")
        message(STATUS "Conan up to date for ${build_type}, not running Conan")
        return()
    endif ()

    set(${hash_name} ${conanfile_hash} CACHE INTERNAL "MD5 hash of conanfile.txt for ${build_type}" FORCE)

    message(STATUS "Configuring packages for ${build_type}")

    conan_cmake_autodetect(settings BUILD_TYPE ${build_type})

    if (build_type MATCHES "MinSizeRel|RelWithDebInfo")
        message(STATUS "Release only libraries: ${CONAN_ONLY_DEBUG_RELEASE}")

        foreach (package ${CONAN_ONLY_DEBUG_RELEASE})
            list(APPEND settings "${package}:build_type=Release")
        endforeach ()
    endif ()

    message(STATUS
            "Executing Conan: \
        REQUIRES ${CONAN_REQUIRES}
        GENERATORS cmake_multi
        BUILD_REQUIRES ${CONAN_BUILD_REQUIRES}
        ${CONAN_CONFIG_OPTIONS}
        OPTIONS ${CONAN_PACKAGE_OPTIONS}
        SETTINGS ${settings}
    ")

    conan_cmake_install(PATH_OR_REFERENCE .
            BUILD missing
            SETTINGS ${settings}
            )
endfunction()

macro(resolve_conan_dependencies)
    if (USE_PACKAGE_MANAGER)
        if (MSVC OR XCODE AND NOT DEFINED CMAKE_BUILD_TYPE)
            foreach (TYPE ${CMAKE_CONFIGURATION_TYPES})
                _conan_install(${TYPE})
            endforeach ()
        else ()
            _conan_install(${CMAKE_BUILD_TYPE})
        endif ()

        list(REMOVE_DUPLICATES CONAN_REQUIRES)

        set(CONAN_CMAKE_SILENT_OUTPUT TRUE)
        include(${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo_multi.cmake)
        conan_define_targets()

        foreach (package_raw ${CONAN_RESOLVE_LIST})
            string(REPLACE "|" ";" package_list ${package_raw})
            list(GET package_list 0 package)
            list(GET package_list 1 target_name)

            # Alias the CONAN_PKG target to the propper target name
            set(_curr_conan_target "CONAN_PKG::${package}")
            if (TARGET ${_curr_conan_target})
                set_target_properties(${_curr_conan_target} PROPERTIES IMPORTED_GLOBAL TRUE)
                add_library(${target_name} ALIAS ${_curr_conan_target})
            else ()
                message(WARNING "Conan target ${_curr_conan_target} not found")
            endif ()
        endforeach ()

    endif (USE_PACKAGE_MANAGER)

    file(GLOB dependency_helpers "${CMAKE_SOURCE_DIR}/cmake/dependencies-helpers/*.cmake")
    foreach (f ${dependency_helpers})
        include(${f})
    endforeach ()
endmacro()

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