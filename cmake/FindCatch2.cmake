set(Catch2_FOUND TRUE)

set(CATCH2_INCLUDEDIR "${CMAKE_BINARY_DIR}/Catch2/")

file(MAKE_DIRECTORY "${CATCH2_INCLUDEDIR}/catch2")
file(DOWNLOAD "https://github.com/catchorg/Catch2/releases/download/v2.11.1/catch.hpp" "${CATCH2_INCLUDEDIR}/catch2/catch.hpp")
file(DOWNLOAD "https://cdn.statically.io/gh/catchorg/Catch2/v2.11.1/contrib/Catch.cmake" "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Catch.cmake")
file(DOWNLOAD "https://cdn.statically.io/gh/catchorg/Catch2/v2.11.1/contrib/CatchAddTests.cmake" "${CMAKE_CURRENT_SOURCE_DIR}/cmake/CatchAddTests.cmake")
file(DOWNLOAD "https://cdn.statically.io/gh/catchorg/Catch2/v2.11.1/contrib/ParseAndAddCatchTests.cmake" "${CMAKE_CURRENT_SOURCE_DIR}/cmake/ParseAndAddCatchTests.cmake")

