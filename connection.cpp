#include "connection.h"
#include <QCoreApplication>
#include <QDebug>

// Initialisation du pointeur d'instance
Connection* Connection::p_instance = nullptr;

// Constructeur
Connection::Connection()
{
    db = QSqlDatabase::addDatabase("QODBC");
}

// Destructeur
Connection::~Connection()
{
    closeConnection();
}

// Méthode statique pour obtenir l'instance unique
Connection* Connection::instance()
{
    if (p_instance == nullptr) {
        p_instance = new Connection();
    }
    return p_instance;
}

// Méthode pour établir la connexion
bool Connection::createconnect()
{
    bool test = false;

    db.setDatabaseName("FishTech"); // DSN name
    db.setUserName("DemniYoussef");
    db.setPassword("Demni123");

    if (db.open()) {
        // Ensure required tables exist
        QSqlQuery q(db);
        q.exec("CREATE TABLE IF NOT EXISTS captures (ID_Capture INTEGER PRIMARY KEY AUTOINCREMENT, Navire TEXT, Date_Capture TEXT, Type_Poisson TEXT, Quantite INTEGER)");
        q.exec("CREATE TABLE IF NOT EXISTS navires (ID_Navire INTEGER PRIMARY KEY AUTOINCREMENT, Nom TEXT, Immatriculation TEXT, Type TEXT, Capacite INTEGER, Statut TEXT)");
        q.exec("CREATE TABLE IF NOT EXISTS navire_history (ID_History INTEGER PRIMARY KEY AUTOINCREMENT, ID_Navire INTEGER, Old_Statut TEXT, New_Statut TEXT, Date_Change TEXT)");
        q.exec("CREATE TABLE IF NOT EXISTS quais (ID_Quai INTEGER PRIMARY KEY AUTOINCREMENT, Nom TEXT, Places INTEGER, Navires INTEGER, Adresse TEXT, Travaux INTEGER DEFAULT 0)");
        test = true;
        qDebug() << "Connexion à la base de données réussie";
    } else {
        qDebug() << "Erreur de connexion:" << db.lastError().text();
        
        // Fallback: use a local SQLite file so app works without ODBC DSN
        if (db.isOpen()) db.close();
        QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);

        db = QSqlDatabase::addDatabase("QSQLITE");
        QString dbFile = QCoreApplication::applicationDirPath() + "/fishtech.sqlite";
        db.setDatabaseName(dbFile);
        
        if (db.open()) {
            test = true;
            QSqlQuery q(db);
            q.exec("CREATE TABLE IF NOT EXISTS captures (ID_Capture INTEGER PRIMARY KEY AUTOINCREMENT, Navire TEXT, Date_Capture TEXT, Type_Poisson TEXT, Quantite INTEGER)");
            q.exec("CREATE TABLE IF NOT EXISTS navires (ID_Navire INTEGER PRIMARY KEY AUTOINCREMENT, Nom TEXT, Immatriculation TEXT, Type TEXT, Capacite INTEGER, Statut TEXT)");
            q.exec("CREATE TABLE IF NOT EXISTS navire_history (ID_History INTEGER PRIMARY KEY AUTOINCREMENT, ID_Navire INTEGER, Old_Statut TEXT, New_Statut TEXT, Date_Change TEXT)");
            q.exec("CREATE TABLE IF NOT EXISTS quais (ID_Quai INTEGER PRIMARY KEY AUTOINCREMENT, Nom TEXT, Places INTEGER, Navires INTEGER, Adresse TEXT, Travaux INTEGER DEFAULT 0)");
            qDebug() << "Connexion SQLite réussie";
        }
    }

    return test;
}

// Fermer la connexion
void Connection::closeConnection()
{
    if (db.isOpen()) {
        db.close();
    }
}
