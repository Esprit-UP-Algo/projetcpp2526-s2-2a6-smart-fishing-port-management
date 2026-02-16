#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setStyleSheet(R"(
        QWidget{
            font-family: 'Segoe UI', sans-serif;
            font-size: 13px;
            background-color: #e0f2fe;
            color: #0f172a;
        }

        /* --- MENU GAUCHE --- */
        QWidget#menu{
            background-color:#0f172a;
        }

        QLabel#title{
            background-color:#1e3a8a;
            color:white;
            font-size:20px;
            font-weight:bold;
            padding:15px;
            border-radius:10px;
            margin-bottom: 20px;
        }

        QWidget#menu QPushButton{
            background:transparent;
            color:white;
            padding:12px;
            text-align:left;
            border-radius:8px;
            font-size: 14px;
            margin-bottom: 5px;
        }

        QWidget#menu QPushButton:hover{
            background:#2563eb;
        }

        /* --- CADRE BLEU POUR LES PAGES --- */
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

        /* --- TITRES DES PAGES --- */
        QLabel#pageTitle {
            color: white;
            font-size: 22px;
            font-weight: bold;
            padding: 15px 0;
            margin-bottom: 20px;
        }

        /* --- CONTROLES FORMULAIRE --- */
        QLineEdit, QComboBox, QDateEdit, QSpinBox, QDoubleSpinBox{
            background:white;
            padding:8px;
            border-radius:8px;
            border:1px solid #94a3b8;
            selection-background-color: #2563eb;
        }

        /* --- TABLEAUX --- */
        QTableWidget{
            background:white;
            border-radius:10px;
            gridline-color: #e2e8f0;
            border: 1px solid #cbd5e1;
        }

        QHeaderView::section{
            background:#1e3a8a;
            color:white;
            padding:8px;
            border: none;
            font-weight: bold;
        }

        /* --- BOUTONS ACTIONS --- */
        QPushButton{
            background:#1e40af;
            color:white;
            padding:10px 20px;
            border-radius:8px;
            font-weight: bold;
        }

        QPushButton:hover{
            background:#2563eb;
        }

        /* Bouton simulation spécial */
        QPushButton#btnSimul {
            color: #a5f3fc;
        }

        /* Bouton PDF spécial */
        QPushButton#btnPDF {
            color: #fbbf24;
        }

        /* Spécifique bouton supprimer */
        QPushButton[text^="🗑️"] {
            background-color: #ef4444;
        }
        QPushButton[text^="🗑️"]:hover {
            background-color: #dc2626;
        }

        /* Style pour les labels de résultats */
        QLabel#resultatSimulation {
            background: white;
            border: 2px solid #0891b2;
            border-radius: 10px;
            padding: 15px;
            margin: 10px;
        }

        /* Style pour Fishtech en haut */
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
    )");

    MainWindow w;
    w.show();
    return a.exec();
}

