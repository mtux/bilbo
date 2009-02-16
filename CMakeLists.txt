project(bilbo)

find_package(KDE4 REQUIRED)
find_package(KdepimLibs REQUIRED)

set(CMAKE_VERBOSE_MAKEFILE ON)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# add_definitions(-DQT3_SUPPORT)
# add_definitions(-DQT3_SUPPORT_WARNINGS)

add_subdirectory( src )
add_subdirectory( icons )
