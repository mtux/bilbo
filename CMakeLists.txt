project(bilbo)

cmake_minimum_required(VERSION 2.6)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/modules )

find_package(KDE4 REQUIRED)
find_package(KdepimLibs REQUIRED)

set(CMAKE_VERBOSE_MAKEFILE ON)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory( bilbokblog )
add_subdirectory( bilbo )
add_subdirectory( icons )
macro_optional_add_subdirectory( po )