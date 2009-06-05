
find_package(Boost REQUIRED)

# add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})

include_directories( ${QT_INCLUDES} ${Boost_INCLUDE_DIR} ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR}/bilbokblog/ ${KDE4_INCLUDES} ${KDEPIMLIBS_INCLUDE_DIR})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/modules )
add_subdirectory( tests )

########### next target ###############

set(bilbokblog_LIB_SRCS
   blog.cpp
   blogpost.cpp
   blogcomment.cpp
   blogmedia.cpp
   blogger1.cpp
   gdata.cpp
   # livejournal.cpp
   metaweblog.cpp
   movabletype.cpp
   wordpressbuggy.cpp
   )


kde4_add_library(bilbokblog SHARED ${bilbokblog_LIB_SRCS})

target_link_libraries(bilbokblog kxmlrpcclient syndication ${KDE4_KIO_LIBS} kcal )

set_target_properties(bilbokblog PROPERTIES VERSION 4.2.2 SOVERSION 4 )

install(TARGETS bilbokblog EXPORT kdepimlibsLibraryTargets ${INSTALL_TARGETS_DEFAULT_ARGS} )

########### install files ##############

# install( FILES blog.h blogpost.h blogcomment.h blogmedia.h blogger1.h gdata.h metaweblog.h movabletype.h wordpressbuggy.h 
#          DESTINATION ${INCLUDE_INSTALL_DIR} COMPONENT Devel)
