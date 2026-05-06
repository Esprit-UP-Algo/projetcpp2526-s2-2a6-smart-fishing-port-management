QT       += core gui sql printsupport charts network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    connection.cpp \
    main.cpp \
    navire.cpp \
    stats_dialog.cpp \
    history_dialog.cpp \
    interdit_dialog.cpp \
    chatbot_dialog.cpp

HEADERS += \
    connection.h \
    navire.h \
    stats_dialog.h \
    history_dialog.h \
    interdit_dialog.h \
    chatbot_dialog.h

FORMS += \
    navire.ui \
    chatbot_dialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
