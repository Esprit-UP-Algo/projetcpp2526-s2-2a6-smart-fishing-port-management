#include "quai.h"
#include <QSqlError>

namespace {

QVariant nullDateVariant()
{
    return QVariant(QMetaType(QMetaType::QDate));
}

QString sqlDateExpression(const QDate &date)
{
    if (!date.isValid()) {
        return "NULL";
    }

    return QString("TO_DATE('%1', 'YYYY-MM-DD')").arg(date.toString("yyyy-MM-dd"));
}

}

QuaiDB::QuaiDB()
{
}

void QuaiDB::afficher(QSqlQueryModel *model)
{
    model->setQuery("SELECT id_quai, nom, places_total, navires_presents, adresse, etat, tirant_eau_max, equipements FROM quai ORDER BY id_quai ASC");
}

bool QuaiDB::ajout()
{
    QSqlQuery query;
    const bool travauxActifs = etat.compare("En travaux", Qt::CaseInsensitive) == 0;
    const QString dateDebutSql = travauxActifs ? sqlDateExpression(date_debut) : "NULL";
    const QString dateFinSql = travauxActifs ? sqlDateExpression(date_fin) : "NULL";

    query.prepare(QString("INSERT INTO quai (id_quai, nom, places_total, navires_presents, adresse, etat, tirant_eau_max, equipements, date_debut, date_fin) "
                          "VALUES (:id_quai, :nom, :places_total, :navires_presents, :adresse, :etat, :tirant_eau_max, :equipements, %1, %2)")
                  .arg(dateDebutSql, dateFinSql));
    query.bindValue(":id_quai", id_quai);
    query.bindValue(":nom", nom);
    query.bindValue(":places_total", places_total);
    query.bindValue(":navires_presents", navires_presents);
    query.bindValue(":adresse", adresse);
    query.bindValue(":etat", etat);
    query.bindValue(":tirant_eau_max", tirant_eau_max);
    query.bindValue(":equipements", equipements);
    bool result = query.exec();
    if (!result)
        qDebug() << "QuaiDB::ajout error:" << query.lastError().text() << query.lastError().nativeErrorCode();
    return result;
}

bool QuaiDB::modifier()
{
    QSqlQuery query;
    const bool travauxActifs = etat.compare("En travaux", Qt::CaseInsensitive) == 0;
    const QString dateDebutSql = travauxActifs ? sqlDateExpression(date_debut) : "NULL";
    const QString dateFinSql = travauxActifs ? sqlDateExpression(date_fin) : "NULL";

    query.prepare(QString("UPDATE quai SET nom = :nom, places_total = :places_total, navires_presents = :navires_presents, "
                          "adresse = :adresse, etat = :etat, tirant_eau_max = :tirant_eau_max, equipements = :equipements, "
                          "date_debut = %1, date_fin = %2 "
                          "WHERE id_quai = :id_quai")
                  .arg(dateDebutSql, dateFinSql));
    query.bindValue(":id_quai", id_quai);
    query.bindValue(":nom", nom);
    query.bindValue(":places_total", places_total);
    query.bindValue(":navires_presents", navires_presents);
    query.bindValue(":adresse", adresse);
    query.bindValue(":etat", etat);
    query.bindValue(":tirant_eau_max", tirant_eau_max);
    query.bindValue(":equipements", equipements);
    return query.exec();
}

bool QuaiDB::suprimer()
{
    QSqlQuery query;
    query.prepare("DELETE FROM quai WHERE id_quai = :id_quai");
    query.bindValue(":id_quai", id_quai);
    return query.exec();
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
    query.prepare("SELECT nom, places_total, navires_presents, adresse, etat, tirant_eau_max, equipements, "
                  "TO_CHAR(date_debut, 'YYYY-MM-DD'), TO_CHAR(date_fin, 'YYYY-MM-DD') "
                  "FROM quai WHERE id_quai = :id_quai");
    query.bindValue(":id_quai", id_quai);
    if (query.exec() && query.next()) {
        nom = query.value(0).toString();
        places_total = query.value(1).toInt();
        navires_presents = query.value(2).toInt();
        adresse = query.value(3).toString();
        etat = query.value(4).toString();
        tirant_eau_max = query.value(5).toDouble();
        equipements = query.value(6).toString();
        date_debut = QDate::fromString(query.value(7).toString().trimmed(), "yyyy-MM-dd");
        date_fin = QDate::fromString(query.value(8).toString().trimmed(), "yyyy-MM-dd");
        return true;
    }
    return false;
}

QList<QuaiDB> QuaiDB::trouverQuaisCompatibles(const QString &equipement, double tirantMinimum)
{
    QList<QuaiDB> quaisCompatibles;

    QSqlQuery query;
    query.prepare(
        "SELECT id_quai, nom, places_total, navires_presents, adresse, etat, tirant_eau_max, equipements "
        "FROM quai "
        "WHERE (places_total - navires_presents) > 0 "
        "AND tirant_eau_max >= :tirant_minimum "
        "AND (:equipement IS NULL "
        "     OR INSTR(',' || UPPER(REPLACE(NVL(equipements, ''), ' ', '')) || ',', "
        "              ',' || UPPER(REPLACE(:equipement, ' ', '')) || ',') > 0) "
        "ORDER BY tirant_eau_max ASC, id_quai ASC");

    query.bindValue(":equipement", equipement);
    query.bindValue(":tirant_minimum", tirantMinimum);

    if (!query.exec()) {
        return quaisCompatibles;
    }

    while (query.next()) {
        QuaiDB quai;
        quai.id_quai = query.value(0).toInt();
        quai.nom = query.value(1).toString();
        quai.places_total = query.value(2).toInt();
        quai.navires_presents = query.value(3).toInt();
        quai.adresse = query.value(4).toString();
        quai.etat = query.value(5).toString();
        quai.tirant_eau_max = query.value(6).toDouble();
        quai.equipements = query.value(7).toString();
        quaisCompatibles.append(quai);
    }

    return quaisCompatibles;
}
bool QuaiDB::QuaiLibre(int id)
{
    QSqlQuery query;

    query.prepare("SELECT places_total, navires_presents, etat "
                  "FROM quai "
                  "WHERE id_quai = :id");

    query.bindValue(":id", id);

    if (!query.exec()) {
        return false;
    }

    if (query.next()) {
        int places = query.value(0).toInt();
        int navires = query.value(1).toInt();
        QString etat = query.value(2).toString();

        // Quai en travaux
        if (etat == "En travaux") {
            return false;
        }

        // Places disponibles
        if (places > navires) {

            QSqlQuery updateQuery;
            updateQuery.prepare(
                "UPDATE quai "
                "SET navires_presents = navires_presents + 1 "
                "WHERE id_quai = :id"
                );

            updateQuery.bindValue(":id", id);

            if (updateQuery.exec()) {
                return true;   // place réservée
            }
        }
    }

    return false;
}
