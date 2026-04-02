#ifndef EMPLOYE_H
#define EMPLOYE_H

#include <QString>
#include <QJsonObject>
#include <QUuid>

/**
 * @class Employe
 * @brief Data structure representing an employee
 * 
 * Contains all employee information including:
 * - Basic info (id, name, position)
 * - Contact info (email, phone)
 * - Compensation (salary)
 * - Demographics (gender)
 */
struct Employe {
    QString id;           ///< Unique employee identifier
    QString nom;          ///< Full name
    QString poste;        ///< Job position (Pêcheur, Matelot, Docker, Chef de quai, Superviseur)
    QString email;        ///< Email address
    QString telephone;    ///< Phone number
    double salaire;       ///< Monthly salary in TND
    QString genre;        ///< Gender ("M" for Male, "F" for Female)

    /**
     * @brief Default constructor
     * Initializes all fields with default values
     */
    Employe() {
        id = QUuid::createUuid().toString().left(8);
        nom = "";
        poste = "Pêcheur";
        email = "";
        telephone = "";
        salaire = 0.0;
        genre = "M";
    }

    /**
     * @brief Constructor with parameters
     */
    Employe(QString i, QString n, QString p, QString e, QString t, double s, QString g = "M") {
        id = i;
        nom = n;
        poste = p;
        email = e;
        telephone = t;
        salaire = s;
        genre = g;
    }

    /**
     * @brief Serialize employee to JSON
     * @return QJsonObject containing all employee data
     */
    QJsonObject toJson() const {
        QJsonObject obj;
        obj["id"] = id;
        obj["nom"] = nom;
        obj["poste"] = poste;
        obj["email"] = email;
        obj["telephone"] = telephone;
        obj["salaire"] = salaire;
        obj["genre"] = genre;
        return obj;
    }

    /**
     * @brief Deserialize employee from JSON
     * @param obj QJsonObject to deserialize
     */
    void fromJson(const QJsonObject &obj) {
        id = obj["id"].toString();
        nom = obj["nom"].toString();
        poste = obj["poste"].toString();
        email = obj["email"].toString();
        telephone = obj["telephone"].toString();
        salaire = obj["salaire"].toDouble();
        genre = obj["genre"].toString("M");
    }
};

#endif // EMPLOYE_H
