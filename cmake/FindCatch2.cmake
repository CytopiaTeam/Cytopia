set(Catch2_FOUND TRUE)

set(CATCH2_VERSION "v2.13.4")
set(CATCH2_INCLUDEDIR "${CMAKE_BINARY_DIR}/catch-${CATCH2_VERSION}")
list(APPEND CMAKE_MODULE_PATH "${CATCH2_INCLUDEDIR}")

if (NOT EXISTS "${CATCH2_INCLUDEDIR}/catch.hpp")
    file(MAKE_DIRECTORY "${CATCH2_INCLUDEDIR}")
    message(STATUS "Downloading catch.hpp from https://github.com/catchorg/Catch2/")
    file(DOWNLOAD "https://github.com/catchorg/Catch2/releases/download/${CATCH2_VERSION}/catch.hpp" "${CATCH2_INCLUDEDIR}/catch.hpp")
    file(DOWNLOAD "https://cdn.statically.io/gh/catchorg/Catch2/${CATCH2_VERSION}/contrib/Catch.cmake" "${CATCH2_INCLUDEDIR}/Catch.cmake")
    file(DOWNLOAD "https://cdn.statically.io/gh/catchorg/Catch2/${CATCH2_VERSION}/contrib/CatchAddTests.cmake" "${CATCH2_INCLUDEDIR}/CatchAddTests.cmake")
    file(DOWNLOAD "https://cdn.statically.io/gh/catchorg/Catch2/${CATCH2_VERSION}/contrib/ParseAndAddCatchTests.cmake" "${CATCH2_INCLUDEDIR}/ParseAndAddCatchTests.cmake")
endif ()

add_library(Catch2::Catch2 INTERFACE IMPORTED)
set_target_properties(Catch2::Catch2 PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CATCH2_INCLUDEDIR}")