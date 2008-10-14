SOURCES += main.cpp
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
