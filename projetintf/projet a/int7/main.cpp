#include <QApplication>
#include <QStandardPaths>
#include <QDir>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataDir);

    a.setStyleSheet(R"(
        QWidget{
            font-family: 'Segoe UI', sans-serif;
            font-size: 12px;
            background-color: #e0f2fe;
            color: #0f172a;
        }
        QWidget#menuPrincipal {
            background-color: #0f172a;
        }
        QWidget#menuPrincipal QPushButton#btnMenuPrincipal {
            background: transparent;
            color: white;
            padding: 8px;
            text-align: left;
            border-radius: 6px;
            font-size: 13px;
            margin-bottom: 3px;
        }
        QWidget#menuPrincipal QPushButton#btnMenuPrincipal:hover {
            background: #2563eb;
        }
        QWidget#pageFrame {
            background: #1e3a8a;
            border-radius: 8px;
            padding: 10px;
            margin: 5px;
        }
        QWidget#contentFrame {
            background: white;
            border-radius: 6px;
            padding: 10px;
        }
        QLabel#pageTitle {
            color: white;
            font-size: 18px;
            font-weight: bold;
            padding: 8px 0;
            margin-bottom: 10px;
        }
        QLineEdit, QComboBox, QDateEdit, QSpinBox, QDoubleSpinBox {
            background: white;
            padding: 5px;
            border-radius: 5px;
            border: 1px solid #94a3b8;
            selection-background-color: #2563eb;
        }
        QTableWidget {
            background: white;
            border-radius: 6px;
            gridline-color: #e2e8f0;
            border: 1px solid #cbd5e1;
        }
        QHeaderView::section {
            background: #1e3a8a;
            color: white;
            padding: 5px;
            border: none;
            font-weight: bold;
        }
        QPushButton {
            background: #1e40af;
            color: white;
            padding: 6px 12px;
            border-radius: 5px;
            font-weight: bold;
        }
        QPushButton:hover {
            background: #2563eb;
        }
        QPushButton[text^="🗑️"] {
            background-color: #ef4444;
        }
        QPushButton[text^="🗑️"]:hover {
            background-color: #dc2626;
        }
        QLabel#fishtechTitle {
            color: white;
            font-size: 20px;
            font-weight: bold;
            padding: 15px 5px 5px 5px;
            background-color: #1e3a8a;
            border-radius: 8px;
            margin: 5px;
            qproperty-alignment: AlignCenter;
        }
        QPushButton#btnRetour {
            background-color: #475569;
            color: white;
            padding: 6px 12px;
            border-radius: 5px;
            font-weight: bold;
            border: none;
        }
        QPushButton#btnRetour:hover {
            background-color: #64748b;
        }
    )");

    MainWindow w;
    w.show();
    return a.exec();
}
