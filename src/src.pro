SOURCES += main.cpp \
 mainwindow.cpp \
 toolbox.cpp \
 systray.cpp \
 settings.cpp \
 addeditblog.cpp \
 postentry.cpp \
 bilboeditor.cpp \
 htmlexporter.cpp \
 addeditlink.cpp \
 dbman.cpp \
 global.cpp \
 bilbopost.cpp \
 bilboblog.cpp \
 backend.cpp \
 entriescountdialog.cpp \
 multilinetextedit.cpp
TEMPLATE = app
CONFIG += warn_on \
	  thread \
          qt \
 debug
TARGET = bilbo
DESTDIR = ../bin
CONFIG -= release

FORMS += toolbox.ui \
addeditblog.ui \
 addeditlink.ui \
 entriescount.ui
HEADERS += mainwindow.h \
toolbox.h \
systray.h \
settings.h \
addeditblog.h \
postentry.h \
 bilboeditor.h \
 htmlexporter.h \
 addeditlink.h \
 dbman.h \
 global.h \
 constants.h \
 bilbopost.h \
 bilboblog.h \
 backend.h \
 entriescountdialog.h \
 multilinetextedit.h
QT += webkit \
 sql

RESOURCES += editor.qrc

INCLUDEPATH += /usr/lib/kde4/include/

LIBS += -L/usr/lib/kde4/lib \
  -lkdecore \
  -lkblog

