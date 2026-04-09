#include "quai.h"

QuaiDB::QuaiDB()
{
}

void QuaiDB::afficher(QSqlQueryModel *model)
{
    model->setQuery("SELECT id_quai, nom, places_total, navires_presents, adresse, etat FROM quai ORDER BY id_quai ASC");
}

bool QuaiDB::ajout()
{
    QSqlQuery query;
    query.prepare("INSERT INTO quai (id_quai, nom, places_total, navires_presents, adresse, etat) "
                  "VALUES (:id_quai, :nom, :places_total, :navires_presents, :adresse, :etat)");
    query.bindValue(":id_quai", id_quai);
    query.bindValue(":nom", nom);
    query.bindValue(":places_total", places_total);
    query.bindValue(":navires_presents", navires_presents);
    query.bindValue(":adresse", adresse);
    query.bindValue(":etat", etat);

    if (query.exec()) {
        return true;
    } else {
        return false;
    }
}

bool QuaiDB::modifier()
{
    QSqlQuery query;
    query.prepare("UPDATE quai SET nom = :nom, places_total = :places_total, navires_presents = :navires_presents, "
                  "adresse = :adresse, etat = :etat WHERE id_quai = :id_quai");
    query.bindValue(":id_quai", id_quai);
    query.bindValue(":nom", nom);
    query.bindValue(":places_total", places_total);
    query.bindValue(":navires_presents", navires_presents);
    query.bindValue(":adresse", adresse);
    query.bindValue(":etat", etat);

    if (query.exec()) {
        return true;
    } else {
        return false;
    }
}

bool QuaiDB::suprimer()
{
    QSqlQuery query;
    query.prepare("DELETE FROM quai WHERE id_quai = :id_quai");
    query.bindValue(":id_quai", id_quai);

    if (query.exec()) {
        return true;
    } else {
        return false;
    }
}

void QuaiDB::recherche(QSqlQueryModel *model, QString critere, QString ch)
{
    model->setQuery("SELECT * FROM quai WHERE " + critere + " LIKE '%" + ch + "%'");
}

void QuaiDB::trie(QSqlQueryModel *model, QString critere)
{
    model->setQuery("SELECT * FROM quai ORDER BY " + critere + " ASC");
}

int QuaiDB::countType(QString ch)
{
    QSqlQueryModel searchModel;
    searchModel.setQuery("SELECT * FROM quai WHERE etat LIKE '%" + ch + "%'");
    return searchModel.rowCount();
}

bool QuaiDB::existe()
{
    QSqlQuery query;
    query.prepare("SELECT * FROM quai WHERE id_quai = :id_quai");
    query.bindValue(":id_quai", id_quai);

    if (query.exec() && query.next()) {
        nom = query.value(1).toString();
        places_total = query.value(2).toInt();
        navires_presents = query.value(3).toInt();
        adresse = query.value(4).toString();
        etat = query.value(5).toString();
        return true;
    } else {
        return false;
    }
}
