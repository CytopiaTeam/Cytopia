set(TESTS_PROJECT_NAME Cytopia_Tests)

include(CTest)
include(Catch)


# We need to test our sources
SET(TEST_SOURCES ${SOURCE_FILES})
# We need to remove main()
LIST(REMOVE_ITEM TEST_SOURCES main.cxx)

LIST(APPEND TEST_SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/main.cxx
        ${CMAKE_CURRENT_LIST_DIR}/Example.cxx
        ${CMAKE_CURRENT_LIST_DIR}/engine/ResourcesManager.cxx
        ${CMAKE_CURRENT_LIST_DIR}/util/Observer.cxx
        ${CMAKE_CURRENT_LIST_DIR}/util/TransitiveModel.cxx
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
