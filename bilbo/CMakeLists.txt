include (KDE4Defaults)
include_directories( ${KDEPIMLIBS_INCLUDE_DIR} ${KDE4_INCLUDES} ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR}/bilbo/)

SET(QT_USE_QTSQL 1)
SET(QT_USE_QTWEBKIT 1)

# TODO uncomment this line for using bilbomarkupbuilders library.
# add_subdirectory(bilbomarkupbuilders)

set(bilbo_SRCS
 htmleditor.cpp
 stylegetter.cpp
 bilbocssparser.cpp
 bilbohtmlparser.cpp
 bilbotexthtmlimporter.cpp
 medialistwidget.cpp
 main.cpp
 mainwindow.cpp
 toolbox.cpp
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
 addmediadialog.cpp
 addimagedialog.cpp
 bilbomedia.cpp
 addcategory.cpp
 catcheckbox.cpp
 sendtoblogdialog.cpp
 waitwidget.cpp
 blogsettings.cpp
 uploadmediadialog.cpp
)

kde4_add_ui_files(
   bilbo_SRCS 
  toolboxbase.ui
  addeditblogbase.ui
  addeditlinkbase.ui
  entriescountdialogbase.ui
  addmediadialogbase.ui
  editimagebase.ui
  addcategorybase.ui
  settingsbase.ui
  editorsettingsbase.ui
  advancedsettingsbase.ui
  sendtoblogbase.ui
  waitwidgetbase.ui
  blogsettingsbase.ui
  uploadmediabase.ui
)

kde4_add_kcfg_files( bilbo_SRCS settings.kcfgc)
kde4_add_executable( bilbo ${bilbo_SRCS})

#target_link_libraries( bilbo ${KDE4_KDEUI_LIBS} ${QT_QTSQL_LIBRARY} ${QT_QTWEBKIT_LIBRARY} bilbokblog)

target_link_libraries( bilbo ${KDE4_KDEUI_LIBS} ${QT_QTSQL_LIBRARY} ${QT_QTWEBKIT_LIBRARY} ${KDE4_KTEXTEDITOR_LIBS} bilbokblog)

# target_link_libraries( bilbo bilbomarkupbuilders ${KDE4_KBLOG_LIBS} ${KDE4_KDEUI_LIBS} ${QT_QTSQL_LIBRARY} ${QT_QTWEBKIT_LIBRARY})

install( TARGETS bilbo DESTINATION ${BIN_INSTALL_DIR} )


########### install files ###############

install( FILES ../bilbo.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )
install( FILES bilbo.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
install( FILES bilboui.rc  DESTINATION  ${DATA_INSTALL_DIR}/bilbo )
kde4_install_icons( ${ICON_INSTALL_DIR} )
