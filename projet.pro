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

# Include paths for organized project structure
INCLUDEPATH += \
    $$PWD/database \
    $$PWD/models \
    $$PWD/dialogs \
    $$PWD/modules/employeeManagement \
    $$PWD/modules/authentication \
    $$PWD/int7

# --- DATABASE LAYER ---
SOURCES += \
    database/connection.cpp

HEADERS += \
    database/connection.h \
    database/constants.h

# --- MODELS ---
HEADERS += \
    models/employe.h

# --- DIALOGS ---
SOURCES += \
    dialogs/addedit_employeedialog.cpp \
    dialogs/forgotpassworddialog.cpp

HEADERS += \
    dialogs/addedit_employeedialog.h \
    dialogs/forgotpassworddialog.h

# --- EMPLOYEE MANAGEMENT MODULE ---
SOURCES += \
    modules/employeeManagement/gestionutilisateurs.cpp

HEADERS += \
    modules/employeeManagement/gestionutilisateurs.h

# --- AUTHENTICATION MODULE ---
SOURCES += \
    modules/authentication/sessionmanager.cpp

HEADERS += \
    modules/authentication/sessionmanager.h

# --- MAIN APPLICATION ---
SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui

# --- INT7 MODULES ---
SOURCES += \
    int7/gestionquai.cpp \
    int7/gestionnavires.cpp \
    int7/gestioncaptures.cpp

HEADERS += \
    int7/gestionquai.h \
    int7/gestionnavires.h \
    int7/gestioncaptures.h \
    int7/dashboard.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
