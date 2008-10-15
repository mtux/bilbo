SOURCES += main.cpp \
 mainwindow.cpp \
 postentriy.cpp \
 toolbox.cpp \
 postproperties.cpp \
 bilboengine.cpp \
 systray.cpp
TEMPLATE = app
CONFIG += warn_on \
	  thread \
          qt \
 debug
TARGET = bilbo
DESTDIR = ../bin
CONFIG -= release

FORMS += toolbox.ui \
post.ui
HEADERS += mainwindow.h \
postentriy.h \
toolbox.h \
postproperties.h \
bilboengine.h \
plugininterface.h \
systray.h
QT += webkit

