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
            font-size: 13px;
            background-color: #e0f2fe;
            color: #0f172a;
        }
        QWidget#menuPrincipal {
            background-color: #0f172a;
        }
        QWidget#menuPrincipal QPushButton#btnMenuPrincipal {
            background: transparent;
            color: white;
            padding: 12px;
            text-align: left;
            border-radius: 8px;
            font-size: 14px;
            margin-bottom: 5px;
        }
        QWidget#menuPrincipal QPushButton#btnMenuPrincipal:hover {
            background: #2563eb;
        }
        QWidget#pageFrame {
            background: #1e3a8a;
            border-radius: 12px;
            padding: 20px;
            margin: 10px;
        }
        QWidget#contentFrame {
            background: white;
            border-radius: 8px;
            padding: 20px;
        }
        QLabel#pageTitle {
            color: white;
            font-size: 22px;
            font-weight: bold;
            padding: 15px 0;
            margin-bottom: 20px;
        }
        QLineEdit, QComboBox, QDateEdit, QSpinBox, QDoubleSpinBox {
            background: white;
            padding: 8px;
            border-radius: 8px;
            border: 1px solid #94a3b8;
            selection-background-color: #2563eb;
        }
        QTableWidget {
            background: white;
            border-radius: 10px;
            gridline-color: #e2e8f0;
            border: 1px solid #cbd5e1;
        }
        QHeaderView::section {
            background: #1e3a8a;
            color: white;
            padding: 8px;
            border: none;
            font-weight: bold;
        }
        QPushButton {
            background: #1e40af;
            color: white;
            padding: 10px 20px;
            border-radius: 8px;
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
            font-size: 24px;
            font-weight: bold;
            padding: 20px 10px 10px 10px;
            background-color: #1e3a8a;
            border-radius: 10px;
            margin: 10px;
            qproperty-alignment: AlignCenter;
        }
        QPushButton#btnRetour {
            background-color: #475569;
            color: white;
            padding: 10px 20px;
            border-radius: 8px;
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
