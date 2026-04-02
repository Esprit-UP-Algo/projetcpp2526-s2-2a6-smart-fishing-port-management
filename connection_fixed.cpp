#include "connection.h"

Connection::Connection()
{

}

bool Connection::createconnect()
{
    bool test = false;
    
    // Check if connection already exists
    if (QSqlDatabase::contains("qt_sql_default_connection")) {
        QSqlDatabase db = QSqlDatabase::database("qt_sql_default_connection");
        if (db.isOpen()) {
            return true;  // Connection already open
        }
    }
    
    // Create new connection only if it doesn't exist
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName("FishTech");
    db.setUserName("DemniYoussef");
    db.setPassword("Demni123");

    if (db.open())
        test = true;

    return test;
}
