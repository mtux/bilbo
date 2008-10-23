

TEMPLATE = lib

CONFIG += dll \
plugin \
debug
CONFIG -= release


DESTDIR = ../

INCLUDEPATH += /usr/lib/kde4/include/ \
  ../../src/

HEADERS += wp_kblog.h

SOURCES += wp_kblog.cpp

TARGET = $$qtLibraryTarget(bilbo_wpkblog)

LIBS += -L/usr/lib/kde4/lib/ \
  -lkdecore \
  -lkblog

