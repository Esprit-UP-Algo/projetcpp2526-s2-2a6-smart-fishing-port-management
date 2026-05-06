#ifndef QUAI_DB_H
#define QUAI_DB_H

#include <QString>
#include <QList>
#include <QDate>
#include <QSqlQueryModel>
#include <QSqlQuery>

class QuaiDB
{
public:
    QuaiDB();

    int id_quai;
    QString nom;
    int places_total;
    int navires_presents;
    QString adresse;
    QString etat;
    double tirant_eau_max;   // Nouveau
    QString equipements;     // Nouveau
    QDate date_debut;
    QDate date_fin;
    QString date_debut_txt;
    QString date_fin_txt;

    void afficher(QSqlQueryModel *model);
    bool ajout();
    bool modifier();
    bool suprimer();
    void recherche(QSqlQueryModel *model, QString critere, QString ch);
    void trie(QSqlQueryModel *model, QString critere);
    int countType(QString ch);
    bool existe();
    QList<QuaiDB> recupererQuaisEnTravaux();
    QList<QuaiDB> trouverQuaisCompatibles(const QString &equipement, double tirantMinimum);

    bool QuaiLibre(int id);

};

#endif // QUAI_DB_H
