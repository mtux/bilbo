include (KDE4Defaults)
include_directories( ${QT_INCLUDES} ${KDEPIMLIBS_INCLUDE_DIR} ${KDE4_INCLUDES} ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR}/bilbo/ )

SET(QT_USE_QTSQL 1)

add_subdirectory(htmlconvertors)

set(bilbo_SRCS
 main.cpp
 mainwindow.cpp
 toolbox.cpp
 addeditblog.cpp
 postentry.cpp
 dbman.cpp
 global.cpp
 bilbopost.cpp
 bilboblog.cpp
 backend.cpp
 entriescountdialog.cpp
 bilbomedia.cpp
 addcategory.cpp
 catcheckbox.cpp
 sendtoblogdialog.cpp
 waitwidget.cpp
 blogsettings.cpp
 uploadmediadialog.cpp
 syncuploader.cpp
 composer/bilbobrowser.cpp
 composer/htmleditor.cpp
 composer/stylegetter.cpp
 composer/medialistwidget.cpp
 composer/bilboeditor.cpp
 composer/multilinetextedit.cpp
 composer/dialogs/addeditlink.cpp
 composer/dialogs/addmediadialog.cpp
 composer/dialogs/addimagedialog.cpp
)

kde4_add_ui_files(
   bilbo_SRCS 
  toolboxbase.ui
  addeditblogbase.ui
  entriescountdialogbase.ui
  addcategorybase.ui
  settingsbase.ui
  editorsettingsbase.ui
  advancedsettingsbase.ui
  sendtoblogbase.ui
  waitwidgetbase.ui
  blogsettingsbase.ui
  uploadmediabase.ui
  composer/dialogs/addmediadialogbase.ui
  composer/dialogs/editimagebase.ui
  composer/dialogs/addeditlinkbase.ui
)

kde4_add_kcfg_files( bilbo_SRCS settings.kcfgc)
kde4_add_app_icon(choqok_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../icons/hi*-app-bilbo.png")
kde4_add_executable( bilbo ${bilbo_SRCS})

#target_link_libraries( bilbo ${KDE4_KDEUI_LIBS} ${QT_QTSQL_LIBRARY} ${QT_QTWEBKIT_LIBRARY} bilbokblog)

target_link_libraries( bilbo ${KDE4_KDEUI_LIBS} ${QT_QTSQL_LIBRARY} ${KDE4_KTEXTEDITOR_LIBS} ${KDE4_KHTML_LIBS} bilbokblog htmlconvertors )

# target_link_libraries( bilbo bilbomarkupbuilders ${KDE4_KBLOG_LIBS} ${KDE4_KDEUI_LIBS} ${QT_QTSQL_LIBRARY} ${QT_QTWEBKIT_LIBRARY})

install( TARGETS bilbo DESTINATION ${BIN_INSTALL_DIR} )


########### install files ###############

install( FILES ../bilbo.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )
install( FILES bilbo.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
install( FILES bilboui.rc  DESTINATION  ${DATA_INSTALL_DIR}/bilbo )
kde4_install_icons( ${ICON_INSTALL_DIR} )
