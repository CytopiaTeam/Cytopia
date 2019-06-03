set(TARGET TileDataEditor)

set(SOURCES
        main.cxx
        TileDataContainer.cxx
        TileDataUI.cxx
        )

if (MSVC)
    set(SOURCES ${SOURCES} ${PROJECT_SOURCE_DIR}/build/msvc/icon_tiledataeditor.rc)
endif ()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

include_directories(..)

add_executable(${TARGET} ${SOURCES})

set_property(TARGET ${TARGET} PROPERTY POSITION_INDEPENDENT_CODE ON)

target_compile_features(
        ${TARGET}
        PRIVATE
        # enable C++11
        cxx_std_11
)
target_compile_options(${TARGET} PRIVATE -fPIC)

target_link_libraries(${TARGET} Qt5::Widgets)

install(TARGETS ${TARGET} DESTINATION .)