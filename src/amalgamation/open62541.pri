# include pre-compiled open62541 library as a dependency
LIBS += -L$$PWD/../../build/
CONFIG(debug, debug|release) {
        LIBS += -lopen62541d
} else {
        LIBS += -lopen62541
}	
# include sockets dependency for windows
win32 {
	LIBS += -lws2_32 
}
# include open62541 header directory
INCLUDEPATH += $$PWD/
# include header to project
HEADERS += $$PWD/open62541.h

ua_encryption {
    # replace with openssl
    LIBS += -lcrypto -lssl
}
