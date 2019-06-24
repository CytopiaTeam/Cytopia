set(TESTS_PROJECT_NAME Cytopia_Tests)

include(Catch)

list(APPEND _include_directories
        ${CMAKE_SOURCE_DIR}/src/engine
        ${CMAKE_SOURCE_DIR}/src/engine/basics
        ${CMAKE_SOURCE_DIR}/src/engine/common
        ${CMAKE_SOURCE_DIR}/src/engine/GameObjects
        ${CMAKE_SOURCE_DIR}/src/engine/map
        ${CMAKE_SOURCE_DIR}/src/engine/ui/basics/
        ${CMAKE_SOURCE_DIR}/src/engine/ui/widgets
        ${CMAKE_SOURCE_DIR}/src/engine/ui/menuGroups
        ${CMAKE_SOURCE_DIR}/src/util
        )


FetchContent_Declare(
        codecov
        GIT_REPOSITORY https://github.com/RWTH-HPC/CMake-codecov.git
)

FetchContent_GetProperties(codecov)
if (NOT codecov_POPULATED)
    FetchContent_Populate(codecov)
endif ()

list(APPEND CMAKE_MODULE_PATH "${codecov_SOURCE_DIR}/cmake/")

find_package(codecov)

# We need to test our sources
SET(_TMP_SRC ${MAIN_EXE_SOURCE_FILES})
# We need to remove main()
LIST(REMOVE_ITEM _TMP_SRC main.cxx)

foreach (SRC_FILE IN LISTS _TMP_SRC)
    string(FIND ${SRC_FILE} ${CMAKE_SOURCE_DIR} _HAS_SRC_DIR)
    if (_HAS_SRC_DIR EQUAL "-1")
        LIST(APPEND TEST_SOURCES "${CMAKE_SOURCE_DIR}/src/${SRC_FILE}")
    else ()
        LIST(APPEND TEST_SOURCES "${SRC_FILE}")
    endif ()
endforeach ()

LIST(APPEND TEST_SOURCES
        main.cxx
        Example.cxx
        engine/ResourcesManager.cxx
        ui/widgets/Text.cxx
        engine/AudioMixer.cxx
        util/Meta.cxx
        util/Observer.cxx
        util/TransitiveModel.cxx
        )

# Generate source groups for use in IDEs
generate_source_groups(${TEST_SOURCES})

if (ENABLE_SDL2_MIXER)
    LIST(APPEND TEST_SOURCES ${CMAKE_CURRENT_LIST_DIR}/engine/AudioMixer.cxx)
endif (ENABLE_SDL2_MIXER)

# Generate source groups for use in IDEs
generate_source_groups(${TEST_SOURCES})

add_executable(${TESTS_PROJECT_NAME} ${TEST_SOURCES})

target_include_directories(${TESTS_PROJECT_NAME} PRIVATE ${_include_directories} src)
target_compile_definitions(${TESTS_PROJECT_NAME} PRIVATE ${_compile_definitions})
target_link_libraries(${TESTS_PROJECT_NAME} PRIVATE ${_link_libraries})

# enable C++17
set_target_properties(${TESTS_PROJECT_NAME} PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED YES
  CXX_EXTENSIONS NO)

catch_discover_tests(${TESTS_PROJECT_NAME})

add_coverage(${TESTS_PROJECT_NAME})
coverage_evaluate()

if (ENABLE_COVERAGE)
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/gcov_reports)
    add_custom_target(
            copy_coverage
            COMMAND find ${CMAKE_BINARY_DIR}/tests/CMakeFiles/Cytopia_Tests.dir/ -name '*.gcov' -exec cp {} ${CMAKE_BINARY_DIR}/gcov_reports "\;"
    )
endif ()
