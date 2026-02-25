QT       += core gui sql charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = projet
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
    connection.cpp \
    main.cpp \
    mainwindow.cpp \
    addedit_employeedialog.cpp \
    forgotpassworddialog.cpp

# int7 module sources (GestionQuai / GestionNavires / GestionCaptures / GestionUtilisateurs)
SOURCES += \
    int7/gestionquai.cpp \
    int7/gestionnavires.cpp \
    int7/gestioncaptures.cpp \
    int7/gestionutilisateurs.cpp

HEADERS += \
    connection.h \
    mainwindow.h \
    addedit_employeedialog.h \
    forgotpassworddialog.h

# Headers for int7 modules
HEADERS += \
    int7/gestionquai.h \
    int7/gestionnavires.h \
    int7/gestioncaptures.h \
    int7/gestionutilisateurs.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
