set(TESTS_PROJECT_NAME Cytopia_Tests)

include(CTest)
include(Catch)

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
SET(TEST_SOURCES ${SOURCE_FILES})
# We need to remove main()
LIST(REMOVE_ITEM TEST_SOURCES main.cxx)

LIST(APPEND TEST_SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/main.cxx
        ${CMAKE_CURRENT_LIST_DIR}/Example.cxx
        ${CMAKE_CURRENT_LIST_DIR}/engine/ResourcesManager.cxx
        )

# Generate source groups for use in IDEs
generate_source_groups(${TEST_SOURCES})

if (ENABLE_SDL2_MIXER)
    LIST(APPEND TEST_SOURCES ${CMAKE_CURRENT_LIST_DIR}/engine/AudioMixer.cxx)
endif (ENABLE_SDL2_MIXER)

add_executable(${TESTS_PROJECT_NAME} ${TEST_SOURCES})

# enable C++14
target_compile_features(${TESTS_PROJECT_NAME} PRIVATE cxx_std_14)

target_include_directories(${TESTS_PROJECT_NAME} PRIVATE ${_include_directories} src)
target_compile_definitions(${TESTS_PROJECT_NAME} PRIVATE ${_compile_definitions})
target_link_libraries(${TESTS_PROJECT_NAME} PRIVATE ${_link_libraries})

catch_discover_tests(${TESTS_PROJECT_NAME})

add_coverage(${TESTS_PROJECT_NAME})
coverage_evaluate()

if (ENABLE_COVERAGE)
    file(GLOB_RECURSE GCOV_REPS LIST_DIRECTORIES FALSE ${CMAKE_BINARY_DIR}/CMakeFiles/Cytopia_Tests.dir/*.gcov)
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/gcov_reports)
    add_custom_target(
            copy_coverage
            COMMAND ${CMAKE_COMMAND} -E copy ${GCOV_REPS} ${CMAKE_BINARY_DIR}/gcov_reports
            VERBATIM
    )
endif ()
