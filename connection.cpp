#include "connection.h"

Connection::Connection()
{

}

bool Connection::createconnect()
{
    // Check if default connection exists and is open
    if (QSqlDatabase::contains()) {
        QSqlDatabase db = QSqlDatabase::database();
        if (db.isOpen()) {
            qDebug() << "Connection already open";
            return true;
        }
    }
    
    // Create new connection
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName("FishTech");
    db.setUserName("DemniYoussef");
    db.setPassword("Demni123");

    if (!db.open()) {
        qDebug() << "Failed to open database:" << db.lastError().text();
        return false;
    }
    
    qDebug() << "Database connection successful";
    return true;
}

