QT += quick
CONFIG += c++11

QMAKE_CXXFLAGS += \
    -Wno-unused-parameter \
    -Wno-unused-variable

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += \
    $$(INSTALL_ROOT)/include

HEADERS += \
    colors.h \
    prefix_opt.h \
    chat_base.h \
    contact_base.h \
    contact_model.h \
    message_base.h \
    message_model.h \
    thr.h \
    chat.h

SOURCES += \
    prefix_opt.cpp \
    easyloggingpp.cpp \
    main.cpp \
    colors.cpp \
    contact_model.cpp \
    message_model.cpp \
    thr.cpp \
    chat.cpp

RESOURCES += qml.qrc

LIBS += \
    -lc2c \
    -lcmsgpack \
    -lluajit-5.1 \
    -lleveldb \
    -lboost_system \
    -lboost_program_options \
    -lssl \
    -lcrypto \
    -lsodium \
    -L$$(INSTALL_ROOT)/lib

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /bin
else: unix:!android: target.path = /bin
!isEmpty(target.path): INSTALLS += target
