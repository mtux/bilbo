SOURCES += main.cpp \
 mainwindow.cpp \
 toolbox.cpp \
 postproperties.cpp \
 bilboengine.cpp \
 systray.cpp \
 settings.cpp \
 addeditblog.cpp \
 postentry.cpp
TEMPLATE = app
CONFIG += warn_on \
	  thread \
          qt \
 debug
TARGET = bilbo
DESTDIR = ../bin
CONFIG -= release

FORMS += toolbox.ui \
post.ui \
addeditblog.ui
HEADERS += mainwindow.h \
toolbox.h \
postproperties.h \
bilboengine.h \
plugininterface.h \
systray.h \
settings.h \
addeditblog.h \
postentry.h
QT += webkit

