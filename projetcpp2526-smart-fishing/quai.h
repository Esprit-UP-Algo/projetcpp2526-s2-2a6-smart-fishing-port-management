#ifndef QUAI_DB_H
#define QUAI_DB_H

#include <QString>
#include <QSqlQueryModel>
#include <QSqlQuery>

class QuaiDB
{
public:
    QuaiDB(); // CONSTRUCTEUR

    int id_quai;
    QString nom;
    int places_total;
    int navires_presents;
    QString adresse;
    QString etat;

    void afficher(QSqlQueryModel *model);
    bool ajout();
    bool modifier();
    bool suprimer();
    void recherche(QSqlQueryModel *model, QString critere, QString ch);
    void trie(QSqlQueryModel *model, QString critere);
    int countType(QString ch);
    bool existe();
};

#endif // QUAI_DB_H
