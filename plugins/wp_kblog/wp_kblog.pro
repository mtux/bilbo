SOURCES += wp_kblog.cpp

HEADERS += wp_kblog.h

TEMPLATE = lib

CONFIG += dll \
plugin \
debug
CONFIG -= release

TARGET = $$qtLibraryTarget(bilbo_wpkblog)

DESTDIR = ../

