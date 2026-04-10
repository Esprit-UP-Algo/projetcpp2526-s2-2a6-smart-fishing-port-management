#include "connection.h"
#include <QCoreApplication>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QMutex>
#include <QMutexLocker>

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
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    if (p_instance == nullptr) {
        p_instance = new Connection();
    }
    return p_instance;
}

// Méthode pour établir la connexion
bool Connection::createConnect()
{
    bool test = false;

    db.setDatabaseName("FishTech"); // DSN name
    db.setUserName("Daniel");
    db.setPassword("Daniel123");

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
        // ODBC connection failed - this is expected if no ODBC driver is installed
        // Fallback silently to SQLite
        
        // Close and clean up the failed ODBC connection
        QString oldName = db.connectionName();
        if (db.isOpen()) db.close();
        // Clear handle before removing
        db = QSqlDatabase();
        if (!oldName.isEmpty()) QSqlDatabase::removeDatabase(oldName);

        // Try SQLite as fallback
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
        } else {
            // If both ODBC and SQLite fail, show error
            qWarning() << "Échec de connexion à la base de données:" << db.lastError().text();
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
    QString name = db.connectionName();
    // Clear local handle before removing connection
    db = QSqlDatabase();
    if (!name.isEmpty()) {
        QSqlDatabase::removeDatabase(name);
    }
}
