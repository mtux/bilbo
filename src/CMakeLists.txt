include (KDE4Defaults)
include_directories( ${KDE4_INCLUDES})
FIND_PACKAGE(Qt4) 
SET(QT_USE_QTSQL 1) 
SET(QT_USE_QTWEBKIT 1)
# INCLUDE(${QT_USE_FILE}) 
# macro_optional_find_package(Kipi)${QT_INCLUDES} 

# if(KIPI_FOUND)
#  set(plugins_SRCS
#     ${CMAKE_CURRENT_SOURCE_DIR}/plugins/setupplugins.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/plugins/interface.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/plugins/imagecollection.cpp
#     )
#  set(KIPI_LIBS kipi)
# endif(KIPI_FOUND)

# macro_bool_to_01(KIPI_FOUND HASKIPI)
# configure_file(config-kpa-kipi.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-kpa-kipi.h)

set(bilbo_SRCS
 main.cpp
 mainwindow.cpp
 toolbox.cpp
 systray.cpp
 addeditblog.cpp
 postentry.cpp
 bilboeditor.cpp
 htmlexporter.cpp
 addeditlink.cpp
 dbman.cpp
 global.cpp
 bilbopost.cpp
 bilboblog.cpp
 backend.cpp
 entriescountdialog.cpp
 multilinetextedit.cpp
 addimagedialog.cpp
 bilbomedia.cpp
 addcategory.cpp
)

kde4_add_ui_files(
   bilbo_SRCS 
  toolboxbase.ui
  addeditblogbase.ui
  addeditlinkbase.ui
  entriescountdialogbase.ui
  addimagedialogbase.ui
  addcategorybase.ui
  settingsbase.ui
)

kde4_add_kcfg_files( bilbo_SRCS settings.kcfgc)
kde4_add_executable( bilbo ${bilbo_SRCS})

target_link_libraries( bilbo ${KDE4_KBLOG_LIBS} ${KDE4_KDEUI_LIBS} ${QT_QTSQL_LIBRARY} ${QT_QTWEBKIT_LIBRARY})

install( TARGETS bilbo DESTINATION ${BIN_INSTALL_DIR} )


########### install files ###############
#//TODO: Add
install( FILES ../bilbo.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )
install( FILES bilbo.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
install( FILES bilboui.rc  DESTINATION  ${DATA_INSTALL_DIR}/bilbo )
kde4_install_icons( ${ICON_INSTALL_DIR} )
