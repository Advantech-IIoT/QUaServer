TEMPLATE = lib
CONFIG  += debug_and_release build_all
CONFIG  += staticlib
CONFIG  -= app_bundle
CONFIG  -= qt

QMAKE_CFLAGS += -std=c99

DESTDIR  = $$PWD/../../build

CONFIG(debug, debug|release) {
	OBJECTS_DIR = $$DESTDIR/debug
    TARGET = open62541d
} else {
	OBJECTS_DIR = $$DESTDIR/release
    TARGET = open62541
}	

# generate and copy amalgamation if not exists
include($$PWD/open62541amalgamation.pri)

INCLUDEPATH += $$PWD

HEADERS += $$PWD/open62541.h
SOURCES += $$PWD/open62541.c

ua_encryption {
    DEFINES += UA_ENABLE_ENCRYPTION
}
