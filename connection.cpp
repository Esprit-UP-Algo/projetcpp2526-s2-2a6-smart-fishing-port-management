#include "connection.h"
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>
#include <QSqlQuery>

Connection::Connection() {}

bool Connection::createconnect()
{
    db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName("system_conn");
    db.setUserName("Kadi1");
    db.setPassword("Kadi08");

    if (db.open()) {
        QSqlQuery q(db);
        if (!q.exec("ALTER SESSION SET NLS_DATE_FORMAT = 'DD/MM/YYYY'")) {
            qDebug() << "NLS_DATE_FORMAT ERROR:" << q.lastError().text();
            return false;
        }
        qDebug() << "Connected to Oracle!";
        return true;
    } else {
        QMessageBox::critical(nullptr, "Error", "Connection failed: " + db.lastError().text());
        return false;
    }
}

void Connection::closeconnect()
{
    db.close();
}
