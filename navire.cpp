#include "navire.h"
#include "ui_navire.h"

#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>
#include <QAbstractItemView>
#include "connection.h"

NAVIRE::NAVIRE(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::NAVIRE)
    , m_model(nullptr)
{
    ui->setupUi(this);

    // Connexion à la base via singleton
    if (!Connection::instance()->createConnect()) {
        QMessageBox::critical(this, tr("Erreur"), tr("Impossible de se connecter à la base de données."));
        // On ne va pas plus loin si la base est indisponible
        return;
    }

    initialiserTable();

    // Les slots on_pushButton_* seront automatiquement connectés par Qt (auto-connect)
}

NAVIRE::~NAVIRE()
{
    delete m_model;
    Connection::instance()->closeConnection();
    delete ui;
}

void NAVIRE::initialiserTable()
{
    m_model = new QSqlQueryModel(this);
    ui->tableView_navires->setModel(m_model);
    ui->tableView_navires->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView_navires->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView_navires->setEditTriggers(QAbstractItemView::NoEditTriggers);

    rafraichirTable();
}

void NAVIRE::rafraichirTable()
{
    if (!m_model)
        return;

    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen())
        return;

    m_model->setQuery("SELECT id_navire, nom, immatriculation, type, capacite, statut FROM navire ORDER BY id_navire");
    m_model->setHeaderData(0, Qt::Horizontal, tr("ID"));
    m_model->setHeaderData(1, Qt::Horizontal, tr("Nom"));
    m_model->setHeaderData(2, Qt::Horizontal, tr("Immatriculation"));
    m_model->setHeaderData(3, Qt::Horizontal, tr("Type"));
    m_model->setHeaderData(4, Qt::Horizontal, tr("Capacité"));
    m_model->setHeaderData(5, Qt::Horizontal, tr("Statut"));
}

void NAVIRE::viderFormulaire()
{
    ui->spinBox_id_navire->setValue(1);
    ui->lineEdit_nom->clear();
    ui->lineEdit_immatriculation->clear();
    ui->lineEdit_type->clear();
    ui->spinBox_capacite->setValue(1);
    ui->comboBox_statut->setCurrentIndex(0);
}

int NAVIRE::idSelectionne() const
{
    QModelIndex index = ui->tableView_navires->currentIndex();
    if (!index.isValid())
        return -1;

    int row = index.row();
    QModelIndex idIndex = m_model->index(row, 0); // colonne 0 = id_navire
    return m_model->data(idIndex).toInt();
}

void NAVIRE::on_pushButton_ajouter_clicked()
{
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) {
        QMessageBox::warning(this, tr("Base de données"), tr("La connexion à la base n'est pas ouverte."));
        return;
    }

    int id = ui->spinBox_id_navire->value();
    QString nom = ui->lineEdit_nom->text();
    QString imm = ui->lineEdit_immatriculation->text();
    QString type = ui->lineEdit_type->text();
    int capacite = ui->spinBox_capacite->value();
    QString statut = ui->comboBox_statut->currentText();

    if (id <= 0 || nom.isEmpty() || imm.isEmpty() || type.isEmpty()) {
        QMessageBox::warning(this, tr("Champs manquants"), tr("Veuillez remplir tous les champs obligatoires."));
        return;
    }

    QSqlQuery query(db);
    // insertion explicite de id_navire selon le schéma fourni
    query.prepare("INSERT INTO navire (id_navire, nom, immatriculation, type, capacite, statut) "
                  "VALUES (:id, :nom, :immatriculation, :type, :capacite, :statut)");
    query.bindValue(":id", id);
    query.bindValue(":nom", nom);
    query.bindValue(":immatriculation", imm);
    query.bindValue(":type", type);
    query.bindValue(":capacite", capacite);
    query.bindValue(":statut", statut);

    if (!query.exec()) {
        QMessageBox::critical(this, tr("Erreur d'insertion"),
                              tr("Impossible d'ajouter le navire : %1").arg(query.lastError().text()));
        return;
    }

    QMessageBox::information(this, tr("Succès"),
                             tr("Navire ajouté avec succès."));

    rafraichirTable();
    viderFormulaire();
}

void NAVIRE::on_pushButton_modifier_clicked()
{
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) {
        QMessageBox::warning(this, tr("Base de données"), tr("La connexion à la base n'est pas ouverte."));
        return;
    }

    int id = idSelectionne();
    if (id < 0) {
        QMessageBox::warning(this, tr("Sélection"), tr("Veuillez sélectionner un navire à modifier."));
        return;
    }

    QString nom = ui->lineEdit_nom->text();
    QString imm = ui->lineEdit_immatriculation->text();
    QString type = ui->lineEdit_type->text();
    int capacite = ui->spinBox_capacite->value();
    QString statut = ui->comboBox_statut->currentText();

    QSqlQuery query(db);
    query.prepare("UPDATE navire SET nom = :nom, immatriculation = :immatriculation, "
                  "type = :type, capacite = :capacite, statut = :statut "
                  "WHERE id_navire = :id");
    query.bindValue(":nom", nom);
    query.bindValue(":immatriculation", imm);
    query.bindValue(":type", type);
    query.bindValue(":capacite", capacite);
    query.bindValue(":statut", statut);
    query.bindValue(":id", id);

    if (!query.exec()) {
        QMessageBox::critical(this, tr("Erreur de modification"),
                              tr("Impossible de modifier le navire : %1").arg(query.lastError().text()));
        return;
    }

    QMessageBox::information(this, tr("Succès"),
                             tr("Navire modifié avec succès."));

    rafraichirTable();
}

void NAVIRE::on_pushButton_supprimer_clicked()
{
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) {
        QMessageBox::warning(this, tr("Base de données"), tr("La connexion à la base n'est pas ouverte."));
        return;
    }

    int id = idSelectionne();
    if (id < 0) {
        QMessageBox::warning(this, tr("Sélection"), tr("Veuillez sélectionner un navire à supprimer."));
        return;
    }

    auto reply = QMessageBox::question(this, tr("Confirmation"),
                                       tr("Voulez-vous vraiment supprimer ce navire ?"));
    if (reply != QMessageBox::Yes)
        return;

    QSqlQuery query(db);
    query.prepare("DELETE FROM navire WHERE id_navire = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        QMessageBox::critical(this, tr("Erreur de suppression"),
                              tr("Impossible de supprimer le navire : %1").arg(query.lastError().text()));
        return;
    }

    rafraichirTable();
    viderFormulaire();
}

void NAVIRE::on_tableView_navires_clicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    int row = index.row();

    ui->spinBox_id_navire->setValue(m_model->data(m_model->index(row, 0)).toInt());
    ui->lineEdit_nom->setText(m_model->data(m_model->index(row, 1)).toString());
    ui->lineEdit_immatriculation->setText(m_model->data(m_model->index(row, 2)).toString());
    ui->lineEdit_type->setText(m_model->data(m_model->index(row, 3)).toString());
    ui->spinBox_capacite->setValue(m_model->data(m_model->index(row, 4)).toInt());

    QString statut = m_model->data(m_model->index(row, 5)).toString();
    int idx = ui->comboBox_statut->findText(statut);
    if (idx >= 0)
        ui->comboBox_statut->setCurrentIndex(idx);
}
