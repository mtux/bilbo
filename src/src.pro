SOURCES += main.cpp \
 mainwindow.cpp \
 toolbox.cpp \
 postproperties.cpp \
 bilboengine.cpp \
 systray.cpp \
 settings.cpp \
 addeditblog.cpp \
 postentry.cpp \
 bilboeditor.cpp \
 htmlexporter.cpp \
 addeditlink.cpp
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
 addeditlink.ui
HEADERS += mainwindow.h \
toolbox.h \
postproperties.h \
bilboengine.h \
plugininterface.h \
systray.h \
settings.h \
addeditblog.h \
postentry.h \
 bilboeditor.h \
 htmlexporter.h \
 addeditlink.h
QT += webkit

RESOURCES += editor.qrc

