set(SOURCE_FILES
        main.cxx
        Game.{hxx,cxx}
        Game.inl.hxx
        util/LOG.{hxx,cxx}
        util/LOG.inl.hxx
        util/filesystem.hxx
        util/IEquatable.hxx
        util/IEquatable.inl.hxx
        util/Singleton.hxx
        util/Meta.hxx
        util/Exception.{hxx,cxx}
        util/Range.hxx
        util/Range.inl.hxx
        util/Rectangle.{hxx,cxx}
        util/ZoomFactor.hxx
        util/ZoomFactor.inl.hxx
        util/iShape.{hxx,cxx}
        util/ZipRange.hxx
        util/ZipRange.inl.hxx
        util/Point.{hxx,cxx}
        util/Color.{hxx,cxx}
        util/SDLAdapter.{hxx,cxx}
        events/UIEvents.hxx
        events/MouseEvents.hxx
        GameService.{hxx,cxx}
        GameService.inl.hxx
        services/Randomizer.{hxx,cxx}
        services/GameClock.{hxx,cxx}
        services/GameClock.inl.hxx
        services/ResourceManager.{hxx,cxx}
        services/ResourceManager.inl.hxx
        services/MouseController.{hxx,cxx}
        engine/MessageQueue.hxx
        engine/MessageQueue.inl.hxx
        engine/basics/Camera.{hxx,cxx}
        engine/basics/isoMath.{hxx,cxx}
        engine/basics/mapEdit.{hxx,cxx}
        engine/basics/Settings.{hxx,cxx}
        engine/basics/GameStates.{hxx,cxx}
        engine/basics/signal.hxx
        engine/basics/Timer.{hxx,cxx}
        engine/basics/utils.{hxx,cxx}
        engine/common/Constants.hxx
        engine/common/enums.hxx
        engine/common/JsonSerialization.hxx
        engine/GameObjects/MapNode.{hxx,cxx}
        engine/map/MapLayers.{hxx,cxx}
        engine/map/TerrainGenerator.{hxx,cxx}
        engine/ui/basics/UIElement.{hxx,cxx}
        engine/ui/basics/ButtonGroup.{hxx,cxx}
        engine/ui/basics/Layout.{hxx,cxx}
        engine/ui/widgets/Bar.{hxx,cxx}
        engine/ui/widgets/Button.{hxx,cxx}
        engine/ui/widgets/Combobox.{hxx,cxx}
        engine/ui/widgets/Checkbox.{hxx,cxx}
        engine/ui/widgets/Frame.{hxx,cxx}
        engine/ui/widgets/Image.{hxx,cxx}
        engine/ui/widgets/Slider.{hxx,cxx}
        engine/ui/widgets/Text.{hxx,cxx}
        engine/ui/widgets/TextField.{hxx,cxx}
        engine/ui/widgets/Tooltip.{hxx,cxx}
        engine/ui/menuGroups/IMenuGroup.hxx
        engine/Engine.{hxx,cxx}
        engine/EventManager.{hxx,cxx}
        engine/Map.{hxx,cxx}
        engine/Sprite.{hxx,cxx}
        engine/ResourcesManager.{hxx,cxx}
        engine/TileManager.{hxx,cxx}
        engine/UIManager.{hxx,cxx}
        engine/WindowManager.{hxx,cxx}
        activity/iActivity.{hxx,cxx}
        activity/iActivity.inl.hxx
        renderer/iRenderer.{hxx,cxx}
        controller/iController.{hxx,cxx}
        controller/iMouseHandler.{hxx,cxx}
        layout/iLayout.{hxx,cxx}
        layout/iLayout.inl.hxx
        model/iModel.{hxx,cxx}
        view/iView.{hxx,cxx} 
        )

list(APPEND _include_directories
        engine
        engine/basics
        engine/common
        engine/GameObjects
        engine/map
        engine/ui/basics/
        engine/ui/widgets
        engine/ui/menuGroups
        util
        )

if (ENABLE_AUDIO)
    list(APPEND SOURCE_FILES
            engine/audio/Soundtrack.{hxx,cxx}
            services/AudioMixer.{hxx,cxx}
            events/AudioEvents.hxx
            )
endif ()

if (ENABLE_ANGELSCRIPT)
    list(APPEND SOURCE_FILES
            ${CMAKE_SOURCE_DIR}/external/as_add_on/scriptbuilder/scriptbuilder.cpp
            ${CMAKE_SOURCE_DIR}/external/as_add_on/scriptbuilder/scriptbuilder.h
            ${CMAKE_SOURCE_DIR}/external/as_add_on/scriptstdstring/scriptstdstring.h
            ${CMAKE_SOURCE_DIR}/external/as_add_on/scriptstdstring/scriptstdstring.cpp
            Scripting/ScriptEngine.{hxx,cxx}
            )
endif ()

if (ENABLE_MICROPROFILE)
    list(APPEND SOURCE_FILES
            ${microprofile_SOURCE_DIR}/microprofile.h
            ${microprofile_SOURCE_DIR}/microprofile_html.h
            ${microprofile_SOURCE_DIR}/microprofile.cpp
            )
endif (ENABLE_MICROPROFILE)

# Expand file extensions (i.e. path/to/file.{h,cpp} becomes path/to/file.h;path/to/file.cpp)
expand_file_extensions(SOURCE_FILES ${SOURCE_FILES})

# Generate source groups for use in IDEs
generate_source_groups(${SOURCE_FILES})

set(MAIN_EXE_SOURCE_FILES "${SOURCE_FILES}" PARENT_SCOPE)

if (MSVC)
    set(SOURCE_FILES ${SOURCE_FILES} ${CMAKE_SOURCE_DIR}/src/Windows/icon_cytopia.rc)
endif ()

if (APPLE)
    file(GLOB_RECURSE MAC_RESOURCE_FILES ${CMAKE_BINARY_DIR}/resources/*)

    # not working now, we need a special icon file for apple
    set(ICON_PATH ${CMAKE_SOURCE_DIR}/data/resources/images/app_icons/cytopia_icon_iso.icns)
    set(MACOSX_BUNDLE_ICON_FILE cytopia_icon_iso.icns)

    # set each file as resources file for the app image.
    foreach (resources_file ${MAC_RESOURCE_FILES})
        get_filename_component(buildDirRelFilePath ${resources_file} DIRECTORY)
        string(REPLACE "${CMAKE_SOURCE_DIR}/" "" buildDirRelFilePath ${buildDirRelFilePath})
        set_source_files_properties(${resources_file} PROPERTIES MACOSX_PACKAGE_LOCATION "/Resources/${buildDirRelFilePath}")
    endforeach ()
    # set_source_files_properties(${PROJECT_SOURCE_DIR}/Cytopia_Resources/music  PROPERTIES MACOSX_PACKAGE_LOCATION Resources/resources/audio/music)
    set_source_files_properties(${ICON_PATH} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    set_source_files_properties(${myApp_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

    add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${SOURCE_FILES} ${ICON_PATH} ${MAC_RESOURCE_FILES})
else ()
    # create a new executable with the same name as the project
    add_executable(${PROJECT_NAME} ${SOURCE_FILES})
endif ()

# enable C++17
set_target_properties(${PROJECT_NAME} PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED YES
  CXX_EXTENSIONS NO)

list(APPEND _compile_definitions VERSION="${PROJECT_NAME} ${PROJECT_VERSION} - Urban Update")


target_include_directories(${PROJECT_NAME} PRIVATE ${_include_directories})
target_compile_definitions(${PROJECT_NAME} PRIVATE ${_compile_definitions})
target_link_libraries(${PROJECT_NAME} PRIVATE ${_link_libraries})

# copy the resources to the compiled directory

add_custom_target(
        copy_resources
        ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/data/resources/ $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources/ 
        COMMENT "Copy the resources to the compiled directory"
        VERBATIM
)

if (MSVC)
    # set startup project for Visual Studio Builds
    set_property(
            DIRECTORY ${PROJECT_SOURCE_DIR}
            PROPERTY
            VS_STARTUP_PROJECT ${PROJECT_NAME}
    )
    # set Visual Studio working directory to the Configuration directory
    set_target_properties(
            ${PROJECT_NAME}
            PROPERTIES
            VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/$(Configuration)"
    )
endif ()

install(TARGETS ${PROJECT_NAME} DESTINATION .)
if (NOT APPLE)
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/data/resources/ DESTINATION resources)
endif ()

install(FILES ${CMAKE_SOURCE_DIR}/ReleaseNotes.txt ${CMAKE_SOURCE_DIR}/credits.txt DESTINATION .)

if (APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE TRUE RESOURCE TRUE)
endif ()
