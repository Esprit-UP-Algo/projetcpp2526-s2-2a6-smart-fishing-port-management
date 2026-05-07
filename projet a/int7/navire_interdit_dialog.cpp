#include "navire_interdit_dialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableView>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QHeaderView>

NavireInterditDialog::NavireInterditDialog(QSqlDatabase db, QWidget *parent)
    : QDialog(parent), m_db(db)
{
    setWindowTitle(tr("Gestion des Navires Interdits"));
    resize(600, 350);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(new QLabel(tr("Sélectionnez un navire pour modifier son statut :"), this));

    m_view = new QTableView(this);
    m_model = new QSqlQueryModel(this);
    m_view->setModel(m_model);
    m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_view->setSelectionMode(QAbstractItemView::SingleSelection);
    m_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    mainLayout->addWidget(m_view);

    QHBoxLayout *editLayout = new QHBoxLayout();
    editLayout->addWidget(new QLabel(tr("Nouveau Statut :"), this));
    m_comboStatut = new QComboBox(this);
    m_comboStatut->addItems({"à quai", "en mer"});
    QPushButton *btnUpdate = new QPushButton(tr("Mettre à jour"), this);
    editLayout->addWidget(m_comboStatut);
    editLayout->addWidget(btnUpdate);
    mainLayout->addLayout(editLayout);

    connect(btnUpdate, &QPushButton::clicked, this, &NavireInterditDialog::on_pushButton_mettre_a_jour_clicked);

    rafraichir();
}

void NavireInterditDialog::rafraichir()
{
    m_model->setQuery(
        "SELECT id_navire, nom, immatriculation, type, capacite FROM navire WHERE statut = 'interdit'",
        m_db);
}

int NavireInterditDialog::idSelectionne() const
{
    QModelIndex index = m_view->currentIndex();
    if (!index.isValid()) return -1;
    return m_model->data(m_model->index(index.row(), 0)).toInt();
}

void NavireInterditDialog::on_pushButton_mettre_a_jour_clicked()
{
    int id = idSelectionne();
    if (id < 0) { QMessageBox::warning(this, tr("Sélection"), tr("Veuillez sélectionner un navire.")); return; }

    QSqlQuery q(m_db);
    q.prepare("UPDATE navire SET statut = :statut WHERE id_navire = :id");
    q.bindValue(":statut", m_comboStatut->currentText());
    q.bindValue(":id", id);
    if (q.exec()) {
        QMessageBox::information(this, tr("Succès"), tr("Statut mis à jour."));
        rafraichir();
        if (m_model->rowCount() == 0) accept();
    } else {
        QMessageBox::critical(this, tr("Erreur"), q.lastError().text());
    }
}
