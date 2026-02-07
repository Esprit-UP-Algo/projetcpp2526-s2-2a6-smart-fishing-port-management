#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setStyleSheet(R"(

QWidget{
    font-family: Segoe UI;
    font-size: 13px;
    background-color: #e0f2fe;
    color: #0f172a;
}

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
}

QWidget#menu QPushButton{
    background:transparent;
    color:white;
    padding:12px;
    text-align:left;
    border-radius:8px;
}

QWidget#menu QPushButton:hover{
    background:#2563eb;
}

QLineEdit,QComboBox{
    background:white;
    padding:8px;
    border-radius:8px;
    border:1px solid #94a3b8;
}

QTableWidget{
    background:white;
    border-radius:10px;
}

QHeaderView::section{
    background:#1e3a8a;
    color:white;
    padding:6px;
}

QPushButton{
    background:#1e40af;
    color:white;
    padding:10px;
    border-radius:8px;
}

QPushButton:hover{
    background:#2563eb;
}

)");

    MainWindow w;
    w.show();
    return a.exec();
}
