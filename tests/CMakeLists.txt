set(TESTS_PROJECT_NAME Cytopia_Tests)

include(Catch)
include(CTest)

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
        util/Observer.cxx
        util/TransitiveModel.cxx
        )

if (ENABLE_SDL2_MIXER)
    LIST(APPEND TEST_SOURCES ${CMAKE_CURRENT_LIST_DIR}/engine/AudioMixer.cxx)
endif (ENABLE_SDL2_MIXER)

# Generate source groups for use in IDEs
generate_source_groups(${TEST_SOURCES})

add_executable(${TESTS_PROJECT_NAME} ${TEST_SOURCES})

# enable C++14
target_compile_features(${TESTS_PROJECT_NAME} PRIVATE cxx_std_14)

target_include_directories(${TESTS_PROJECT_NAME} PRIVATE ${_include_directories} src)
target_compile_definitions(${TESTS_PROJECT_NAME} PRIVATE ${_compile_definitions})
target_link_libraries(${TESTS_PROJECT_NAME} PRIVATE ${_link_libraries})

catch_discover_tests(${TESTS_PROJECT_NAME})
