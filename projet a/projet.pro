QT += core gui widgets sql charts serialport printsupport network multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = projet
TEMPLATE = app

# Warnings pour APIs dépréciées
DEFINES += QT_DEPRECATED_WARNINGS

# Standard C++17
CONFIG += c++17

# ─────────────────────────────────────────
# ODBC Configuration for Windows
# ─────────────────────────────────────────
win32 {
    # ODBC Windows libraries (no OCI needed for ODBC connections)
    LIBS += -lodbc32 \
            -lodbccp32
}

# ─────────────────────────────────────────
# Sources
# ─────────────────────────────────────────
SOURCES += \
    affectation.cpp \
    arduino.cpp \
    connection.cpp \
    main.cpp \
    mainwindow.cpp \
    planning.cpp \
    quai.cpp \
    qrcode.cpp \
    addedit_employeedialog.cpp \
    forgotpassworddialog.cpp \
    int7/dashboard.cpp \
    int7/planningwidget.cpp \
    int7/gestionquai.cpp \
    int7/gestionnavires.cpp \
    int7/navire_stats_dialog.cpp \
    int7/navire_history_dialog.cpp \
    int7/navire_interdit_dialog.cpp \
    int7/navire_chatbot_dialog.cpp \
    int7/gestioncaptures.cpp \
    int7/gestionutilisateurs.cpp \
    int7/TunisiaMapWidget.cpp \
    int7/BarriereWidget.cpp

# ─────────────────────────────────────────
# Headers
# ─────────────────────────────────────────
HEADERS += \
    AppTheme.h \
    AppStyle.h \
    settingsdialog.h \
    affectation.h \
    arduino.h \
    connection.h \
    mainwindow.h \
    planning.h \
    quai.h \
    qrcode.hpp \
    addedit_employeedialog.h \
    forgotpassworddialog.h \
    int7/planningwidget.h \
    int7/gestionquai.h \
    int7/gestionnavires.h \
    int7/navire_stats_dialog.h \
    int7/navire_history_dialog.h \
    int7/navire_interdit_dialog.h \
    int7/navire_chatbot_dialog.h \
    int7/gestioncaptures.h \
    int7/gestionutilisateurs.h \
    int7/dashboard.h \
    int7/dashboard_widgets.h \
    int7/TunisiaMapWidget.h \
    int7/BarriereWidget.h

# ─────────────────────────────────────────
# Forms
# ─────────────────────────────────────────
FORMS += \
    mainwindow.ui

# ─────────────────────────────────────────
# Déploiement
# ─────────────────────────────────────────
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# ─────────────────────────────────────────
# Ressources (optionnel - décommenter si besoin)
# ─────────────────────────────────────────
RESOURCES += resources.qrc