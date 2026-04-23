#ifndef CONNECTION_H
#define CONNECTION_H

#include <QSqlDatabase>

class Connection
{
private:
    QSqlDatabase db;

public:
    Connection();
    bool createconnect();
    void closeconnect();

    // Getter pour la base de données
    QSqlDatabase& getDb() { return db; }
};

#endif // CONNECTION_H
