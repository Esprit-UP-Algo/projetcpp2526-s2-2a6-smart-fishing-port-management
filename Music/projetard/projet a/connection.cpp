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
    db.setDatabaseName(
        "Driver={Oracle in OraDB23Home1};"
        "DBQ=localhost:1521/freepdb1;"
        "UID=daniel;"
        "PWD=30100;"
        "AutoCommit=1;"
        );

    if (db.open()) {
        qDebug() << "DB ouverte avec succès";
        return true;
    } else {
        qDebug() << "Erreur DB : " << db.lastError().text();
        return false;
    }
}