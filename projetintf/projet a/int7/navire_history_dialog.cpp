#include "navire_history_dialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableView>
#include <QPushButton>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QHeaderView>

NavireHistoryDialog::NavireHistoryDialog(QSqlDatabase db, QWidget *parent)
    : QDialog(parent), m_db(db)
{
    setWindowTitle(tr("Historique des Navires Supprimés"));
    resize(700, 400);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    m_view = new QTableView(this);
    m_model = new QSqlQueryModel(this);
    m_view->setModel(m_model);
    m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_view->setSelectionMode(QAbstractItemView::SingleSelection);
    m_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    mainLayout->addWidget(m_view);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *btnRestaurer = new QPushButton(tr("Restaurer Sélection"), this);
    QPushButton *btnSuppr     = new QPushButton(tr("Supprimer Définitivement"), this);
    btnLayout->addWidget(btnRestaurer);
    btnLayout->addWidget(btnSuppr);
    mainLayout->addLayout(btnLayout);

    connect(btnRestaurer, &QPushButton::clicked, this, &NavireHistoryDialog::on_pushButton_restaurer_clicked);
    connect(btnSuppr,     &QPushButton::clicked, this, &NavireHistoryDialog::on_pushButton_supprimer_def_clicked);

    rafraichir();
}

void NavireHistoryDialog::rafraichir()
{
    m_model->setQuery(
        "SELECT id_navire, nom, immatriculation, type, capacite, statut FROM navire ORDER BY id_navire",
        m_db);
    m_model->setHeaderData(1, Qt::Horizontal, tr("Nom"));
    m_model->setHeaderData(2, Qt::Horizontal, tr("Immatriculation"));
}

int NavireHistoryDialog::idSelectionne() const
{
    QModelIndex index = m_view->currentIndex();
    if (!index.isValid()) return -1;
    return m_model->data(m_model->index(index.row(), 0)).toInt();
}

void NavireHistoryDialog::on_pushButton_restaurer_clicked()
{
    int id = idSelectionne();
    if (id < 0) return;

    QSqlQuery q(m_db);
    q.prepare("INSERT INTO navire (nom, immatriculation, type, capacite, statut) "
              "SELECT nom, immatriculation, type, capacite, statut "
              "FROM navire WHERE id_navire = :id");
    q.bindValue(":id", id);
    if (q.exec()) {
        QSqlQuery qDel(m_db);
        qDel.prepare("DELETE FROM navire WHERE id_navire = :id");
        qDel.bindValue(":id", id);
        qDel.exec();
        QMessageBox::information(this, tr("Succès"), tr("Navire restauré avec succès."));
        rafraichir();
    } else {
        QMessageBox::critical(this, tr("Erreur"), tr("Impossible de restaurer : %1").arg(q.lastError().text()));
    }
}

void NavireHistoryDialog::on_pushButton_supprimer_def_clicked()
{
    int id = idSelectionne();
    if (id < 0) return;
    if (QMessageBox::question(this, tr("Confirmation"),
            tr("Supprimer définitivement ce navire ? Cette action est irréversible.")) == QMessageBox::Yes) {
        QSqlQuery q(m_db);
        q.prepare("DELETE FROM navire WHERE id_navire = :id");
        q.bindValue(":id", id);
        if (q.exec()) rafraichir();
    }
}
