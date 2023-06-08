include(codecov)

set(TESTS_PROJECT_NAME Cytopia_Tests)

include(${CMAKE_SOURCE_DIR}/external/catch2/Catch.cmake)

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
        engine/basics/Point.cxx
        engine/basics/Settings.cxx
        engine/ResourcesManager.cxx
        engine/EventManager.cxx
        engine/MapFunctions.cxx
        engine/UIManager.cxx
        engine/WindowManager.cxx
        game/PowerManager.cxx
        services/GameClock.cxx
        util/Meta.cxx
        util/MessageQueue.cxx
        util/LOG.cxx
        )

if (USE_ANGELSCRIPT)
    list(APPEND TEST_SOURCES
            scripting/ScriptEngine.cxx
            )
endif ()


# Generate source groups for use in IDEs
generate_source_groups(${TEST_SOURCES})

if (ENABLE_AUDIO)
    LIST(APPEND TEST_SOURCES ${CMAKE_CURRENT_LIST_DIR}/services/AudioMixer.cxx)
endif (ENABLE_AUDIO)

# Generate source groups for use in IDEs
generate_source_groups(${TEST_SOURCES})

add_executable(${TESTS_PROJECT_NAME} ${TEST_SOURCES})

if (ENABLE_PHC)
    target_precompile_headers(${TESTS_PROJECT_NAME} PRIVATE phc.hxx)
endif ()

target_include_directories(
        ${TESTS_PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/external/header_only
        src
        ${CMAKE_SOURCE_DIR}/external/catch2
        ${CMAKE_SOURCE_DIR}/src/engine
        ${CMAKE_SOURCE_DIR}/src/engine/GameObjects
        ${CMAKE_SOURCE_DIR}/src/engine/basics
        ${CMAKE_SOURCE_DIR}/src/engine/common
        ${CMAKE_SOURCE_DIR}/src/engine/map
        ${CMAKE_SOURCE_DIR}/src/events
        ${CMAKE_SOURCE_DIR}/src/layout
        ${CMAKE_SOURCE_DIR}/src/model
        ${CMAKE_SOURCE_DIR}/src/scripting
        ${CMAKE_SOURCE_DIR}/src/services
        ${CMAKE_SOURCE_DIR}/src/util
)

target_compile_definitions(${TESTS_PROJECT_NAME} PRIVATE ${_compile_definitions} TESTING_ENABLED)
target_link_libraries(
        ${TESTS_PROJECT_NAME} PRIVATE
        IMGUI
        PNG::PNG
        SDL2::SDL2
        SDL2_ttf::SDL2_ttf
        SDL2_image::SDL2_image
        libnoise::libnoise
        ZLIB::ZLIB
)

if (ENABLE_AUDIO)
    target_link_libraries(${TESTS_PROJECT_NAME} PRIVATE OpenAL::OpenAL VorbisFile::VorbisFile)
    target_compile_definitions(${TESTS_PROJECT_NAME} PRIVATE USE_AUDIO)
endif ()

if (USE_MICROPROFILE)
    target_link_libraries(${TESTS_PROJECT_NAME} PRIVATE Microprofile::Microprofile)
    target_compile_definitions(${TESTS_PROJECT_NAME} PRIVATE MICROPROFILE_ENABLED MICROPROFILE_GPU_TIMERS=0)
endif ()

if (USE_ANGELSCRIPT)
    target_include_directories(${TESTS_PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/external/as_add_on)
    target_link_libraries(${TESTS_PROJECT_NAME} PRIVATE Angelscript::angelscript)
    target_compile_definitions(${TESTS_PROJECT_NAME} PRIVATE USE_ANGELSCRIPT)
endif ()

# enable C++17
set_target_properties(
        ${TESTS_PROJECT_NAME} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

catch_discover_tests(${TESTS_PROJECT_NAME})

add_coverage(${TESTS_PROJECT_NAME})
coverage_evaluate()
