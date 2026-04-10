#ifndef CONNECTION_H
#define CONNECTION_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

class Connection
{
public:
    static Connection* instance(); // Accès à l'instance unique
    bool createConnect(); // Méthode pour créer la connexion
    void closeConnection(); // Fermer la connexion

    // Constructeur et destructeur publics pour compatibilité
    Connection();
    ~Connection();

private:
    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;

    static Connection* p_instance;
    QSqlDatabase db;
};

#endif // CONNECTION_H
