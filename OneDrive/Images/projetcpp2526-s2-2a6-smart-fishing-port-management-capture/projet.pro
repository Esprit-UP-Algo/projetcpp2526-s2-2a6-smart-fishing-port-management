QT       += core gui sql charts serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


TARGET = Atelier_Connexion
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += c++11

SOURCES += \
    connection.cpp \
    main.cpp \
    mainwindow.cpp \
    addedit_employeedialog.cpp \
    addedit_capturedialog.cpp \
    gestionquai.cpp \
    addedit_shipdialog.cpp \
    capturesdialog.cpp \
    fishingzonesdialog.cpp \
    forgotpassworddialog.cpp \
    navire.cpp

HEADERS += \
    connection.h \
    mainwindow.h \
    \
    addedit_employeedialog.h \
    addedit_capturedialog.h \
    gestionquai.h \
    addedit_shipdialog.h \
    capturesdialog.h \
    fishingzonesdialog.h \
    forgotpassworddialog.h \
    navire.h



FORMS += \
    mainwindow.ui \
    navire.ui
RESOURCES += resources.qrc

contains(CONFIG, webengine) {
    QT += webenginewidgets
    DEFINES += HAVE_QT_WEBENGINE
    SOURCES += \
        fishingzones3ddialog.cpp
    HEADERS += \
        fishingzones3ddialog.h
    message("Building with Qt WebEngine support (CONFIG+=webengine).")
} else {
    message("Building without Qt WebEngine; 3D map disabled. Use CONFIG+=webengine to enable.")
}

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32 {
    CONFIG(debug, debug|release) {
        QMAKE_POST_LINK -= debug/$$TARGET.exe --check-db
        QMAKE_POST_LINK += $$quote(cmd /C \"\"debug\\$$TARGET.exe\" --check-db\")
    } else:CONFIG(release, debug|release) {
        QMAKE_POST_LINK -= release/$$TARGET.exe --check-db
        QMAKE_POST_LINK += $$quote(cmd /C \"\"release\\$$TARGET.exe\" --check-db\")
    }
}
