TEMPLATE = app
CONFIG += console static
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c

LIBS	+= -lzstd
