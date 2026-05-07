#include "connection.h"
#include <QDebug>

Connection::Connection() {}

bool Connection::createconnect()
{
    if (QSqlDatabase::contains("qt_sql_default_connection")) {
        QSqlDatabase db = QSqlDatabase::database();
        if (db.isOpen()) return true;
    }

    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    
    // Use DSN that works in ODBC Administrator
    db.setDatabaseName("freepdb1");
    db.setUserName("daniel");
    db.setPassword("30100");

    if (db.open()) {
        qDebug() << "DB ouverte avec succès";
        return true;
    } else {
        qDebug() << "Erreur DB : " << db.lastError().text();
        qDebug() << "Details: " << db.lastError().driverText();
        return false;
    }
}