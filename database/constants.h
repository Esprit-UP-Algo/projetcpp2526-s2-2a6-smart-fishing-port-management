#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>
#include <QStringList>

namespace Config {
    // Employee positions/roles
    const QStringList POSITIONS = {
        "Pêcheur",
        "Matelot",
        "Docker",
        "Chef de quai",
        "Superviseur",
        "Responsable RH",
        "Directeur Opérationnel"
    };

    // Role-to-Permission mappings
    const QStringList DIRECTOR_PAGES = {"Employees", "Captures", "Navires", "Quai", "Analytics", "Dashboard"};
    const QStringList HR_PAGES = {"Employees", "Analytics"};
    const QStringList PECHEUR_PAGES = {"Captures"};
    const QStringList MATELOT_PAGES = {"Navires"};
    const QStringList DOCKER_PAGES = {"Quai"};

    // Employee defaults
    const QString DEFAULT_POSITION = "Pêcheur";
    const QString DEFAULT_GENDER = "M";
    const double DEFAULT_SALARY = 35000.0;
    const QString DEFAULT_PASSWORD = "password"; // Should be hashed in DB

    // Salary constraints (TND - Tunisian Dinars)
    const double MIN_SALARY = 0.0;
    const double MAX_SALARY = 1000000.0;

    // Validation rules
    const int MIN_NAME_LENGTH = 2;
    const int MAX_NAME_LENGTH = 100;
    const int MIN_EMAIL_LENGTH = 5;
    const int MAX_EMAIL_LENGTH = 120;
    const int MIN_PHONE_LENGTH = 6;
    const int MAX_PHONE_LENGTH = 20;

    // Database
    const QString DB_TABLE_EMPLOYEES = "employes";
    const QString DB_CONNECTION_NAME = "FishTech";

    // File export
    const QString CSV_EXPORT_DELIMITER = ",";
    const QString CSV_EXPORT_LINE_ENDING = "\n";

    // UI Defaults
    const QString CURRENCY_SUFFIX = " TND";
    const int DEFAULT_SALARY_DECIMALS = 0;
} // namespace Config

#endif // CONSTANTS_H
