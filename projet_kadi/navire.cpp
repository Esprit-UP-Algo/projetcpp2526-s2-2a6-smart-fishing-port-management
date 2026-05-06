#include "navire.h"
#include "ui_navire.h"

#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>
#include <QRegularExpressionValidator>
#include <QRegularExpression>
#include <QPdfWriter>
#include <QPainter>
#include <QFileDialog>
#include <QHeaderView>
#include <QDialog>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QVBoxLayout>
#include "stats_dialog.h"
#include "history_dialog.h"
#include "interdit_dialog.h"
#include "chatbot_dialog.h"
#include <QStyle>

NAVIRE::NAVIRE(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::NAVIRE)
    , m_model(nullptr)
    , m_currentSort("id_navire ASC")
    , m_trayIcon(new QSystemTrayIcon(this))
{
    ui->setupUi(this);

    // Connexion à la base
    if (!m_connection.createconnect()) {
        QMessageBox::critical(this, tr("Erreur"), tr("Impossible de se connecter à la base de données."));
        // On ne va pas plus loin si la base est indisponible
        return;
    }

    initialiserTable();

    // Configuration du Tray Icon
    m_trayIcon->setIcon(style()->standardIcon(QStyle::SP_MessageBoxWarning));
    m_trayIcon->show();

    // Vérification des navires interdits au démarrage
    QSqlQuery qInt(m_connection.getDb());
    qInt.exec("SELECT COUNT(*) FROM navire WHERE statut = 'interdit'");
    int count = 0;
    if (qInt.next()) {
        count = qInt.value(0).toInt();
    }

    // Notification Système (Affichée dans tous les cas)
    m_trayIcon->showMessage(tr("Flotte de Navires"), 
                            tr("Il y a %1 navire(s) interdit(s) dans la flotte.").arg(count),
                            QSystemTrayIcon::Information, 3000);

    if (count > 0) {
        // Lancement du dialogue dédié uniquement si count > 0
        InterditDialog diag(m_connection.getDb(), this);
        diag.exec();
        rafraichirTable();
    }

    // Création de la table archive si elle n'existe pas (Oracle)
    QSqlQuery qArc(m_connection.getDb());
    qArc.exec("CREATE TABLE navire_archive ("
              "id_navire NUMBER PRIMARY KEY, "
              "nom VARCHAR2(100), "
              "immatriculation VARCHAR2(100), "
              "type VARCHAR2(100), "
              "capacite NUMBER, "
              "tiran_deau NUMBER(5,2), "
              "statut VARCHAR2(100))");
    // On ignore l'erreur si elle existe déjà (ORA-00955)

    // --- Contrôle de Saisie Dynamique (Visual only) ---
    // Connexions pour maj dynamiques
    connect(ui->lineEdit_nom, &QLineEdit::textChanged, this, &NAVIRE::controlerSaisie);
    connect(ui->lineEdit_immatriculation, &QLineEdit::textChanged, this, &NAVIRE::controlerSaisie);
    connect(ui->lineEdit_type, &QLineEdit::textChanged, this, &NAVIRE::controlerSaisie);
    connect(ui->spinBox_id_navire, QOverload<int>::of(&QSpinBox::valueChanged), this, &NAVIRE::controlerSaisie);
    connect(ui->spinBox_capacite, QOverload<int>::of(&QSpinBox::valueChanged), this, &NAVIRE::controlerSaisie);

    controlerSaisie();
    // ------------------------------------

    // Les slots on_pushButton_* seront automatiquement connectés par Qt (auto-connect)
}

NAVIRE::~NAVIRE()
{
    delete m_model;
    m_connection.closeconnect();
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

void NAVIRE::rafraichirTable(const QString &filtreNom, const QString &filtreStatut, const QString &triOrder)
{
    if (!m_model || !m_connection.getDb().isOpen())
        return;

    m_currentSort = triOrder;

    QString sql = "SELECT id_navire, nom, immatriculation, type, capacite, tiran_deau, statut FROM navire";
    QStringList filters;

    if (!filtreNom.isEmpty()) {
        filters << "LOWER(nom) LIKE LOWER('%" + filtreNom + "%')";
    }
    if (filtreStatut != "Tous les statuts") {
        filters << "statut = '" + filtreStatut + "'";
    }

    if (!filters.isEmpty()) {
        sql += " WHERE " + filters.join(" AND ");
    }

    sql += " ORDER BY " + m_currentSort;

    m_model->setQuery(sql, m_connection.getDb());
    
    if (m_model->lastError().isValid()) {
        qDebug() << "SQL Error:" << m_model->lastError().text();
    }

    m_model->setHeaderData(0, Qt::Horizontal, tr("ID"));
    m_model->setHeaderData(1, Qt::Horizontal, tr("Nom"));
    m_model->setHeaderData(2, Qt::Horizontal, tr("Immatriculation"));
    m_model->setHeaderData(3, Qt::Horizontal, tr("Type"));
    m_model->setHeaderData(4, Qt::Horizontal, tr("Capacité"));
    m_model->setHeaderData(5, Qt::Horizontal, tr("Tirant d'eau (m)"));
    m_model->setHeaderData(6, Qt::Horizontal, tr("Statut"));
}

void NAVIRE::viderFormulaire()
{
    ui->spinBox_id_navire->setValue(1);
    ui->lineEdit_nom->clear();
    ui->lineEdit_immatriculation->clear();
    ui->lineEdit_type->clear();
    ui->spinBox_capacite->setValue(1);
    ui->doubleSpinBox_tiran_deau->setValue(0.0);
    ui->comboBox_statut->setCurrentIndex(0);

    // Revalider après vidage
    controlerSaisie();
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
    if (!m_connection.getDb().isOpen()) {
        QMessageBox::warning(this, tr("Base de données"), tr("La connexion à la base n'est pas ouverte."));
        return;
    }

    int id = ui->spinBox_id_navire->value();
    QString nom = ui->lineEdit_nom->text();
    QString imm = ui->lineEdit_immatriculation->text();
    QString type = ui->lineEdit_type->text();
    int capacite = ui->spinBox_capacite->value();
    double tiranDeau = ui->doubleSpinBox_tiran_deau->value();
    QString statut = ui->comboBox_statut->currentText();

    if (id <= 0 || nom.isEmpty() || imm.isEmpty() || type.isEmpty()) {
        QMessageBox::warning(this, tr("Champs manquants"), tr("Veuillez remplir tous les champs obligatoires."));
        return;
    }

    QSqlQuery query(m_connection.getDb());
    query.prepare("INSERT INTO navire (id_navire, nom, immatriculation, type, capacite, tiran_deau, statut) "
                  "VALUES (:id, :nom, :immatriculation, :type, :capacite, :tiran_deau, :statut)");
    query.bindValue(":id", id);
    query.bindValue(":nom", nom);
    query.bindValue(":immatriculation", imm);
    query.bindValue(":type", type);
    query.bindValue(":capacite", capacite);
    query.bindValue(":tiran_deau", tiranDeau);
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
    if (!m_connection.getDb().isOpen()) {
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
    double tiranDeau = ui->doubleSpinBox_tiran_deau->value();
    QString statut = ui->comboBox_statut->currentText();

    QSqlQuery query(m_connection.getDb());
    query.prepare("UPDATE navire SET nom = :nom, immatriculation = :immatriculation, "
                  "type = :type, capacite = :capacite, tiran_deau = :tiran_deau, statut = :statut "
                  "WHERE id_navire = :id");
    query.bindValue(":nom", nom);
    query.bindValue(":immatriculation", imm);
    query.bindValue(":type", type);
    query.bindValue(":capacite", capacite);
    query.bindValue(":tiran_deau", tiranDeau);
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
    if (!m_connection.getDb().isOpen()) {
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

    QSqlQuery qArc(m_connection.getDb());
    // Archiver avant de supprimer
    qArc.prepare("INSERT INTO navire_archive (id_navire, nom, immatriculation, type, capacite, tiran_deau, statut) "
                 "SELECT id_navire, nom, immatriculation, type, capacite, tiran_deau, statut FROM navire WHERE id_navire = :id");
    qArc.bindValue(":id", id);
    qArc.exec();

    QSqlQuery query(m_connection.getDb());
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
    ui->doubleSpinBox_tiran_deau->setValue(m_model->data(m_model->index(row, 5)).toDouble());

    QString statut = m_model->data(m_model->index(row, 6)).toString();
    int idx = ui->comboBox_statut->findText(statut);
    if (idx >= 0)
        ui->comboBox_statut->setCurrentIndex(idx);

    // Revalider après sélection (les textChanged vont aussi trigger, mais par sécurité)
    controlerSaisie();
}

void NAVIRE::controlerSaisie()
{
    QString nom = ui->lineEdit_nom->text().trimmed();
    QString imm = ui->lineEdit_immatriculation->text().trimmed();
    QString type = ui->lineEdit_type->text().trimmed();

    // Vérification manuelle par Regex pour le feedback visuel
    bool nomValide = QRegularExpression("^[A-Za-z ]+$").match(nom).hasMatch() && !nom.isEmpty();
    bool immValide = QRegularExpression("^[0-9]+$").match(imm).hasMatch() && !imm.isEmpty();
    bool typeValide = QRegularExpression("^[A-Za-z ]+$").match(type).hasMatch() && !type.isEmpty();

    auto updateStyle = [](QLineEdit* le, bool valid, bool empty) {
        if (empty) {
            le->setStyleSheet(""); // par défaut
        } else if (valid) {
            le->setStyleSheet("border: 2px solid #22c55e; background-color: #f0fdf4; border-radius: 6px;"); // vert
        } else {
            le->setStyleSheet("border: 2px solid #ef4444; background-color: #fef2f2; border-radius: 6px;"); // rouge
        }
    };

    updateStyle(ui->lineEdit_nom, nomValide, nom.isEmpty());
    updateStyle(ui->lineEdit_immatriculation, immValide, imm.isEmpty());
    updateStyle(ui->lineEdit_type, typeValide, type.isEmpty());
}

void NAVIRE::on_lineEdit_recherche_textChanged(const QString &arg1)
{
    rafraichirTable(arg1, ui->comboBox_filtre_statut->currentText(), m_currentSort);
}

void NAVIRE::on_comboBox_filtre_statut_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    rafraichirTable(ui->lineEdit_recherche->text(), ui->comboBox_filtre_statut->currentText(), m_currentSort);
}

void NAVIRE::on_pushButton_tri_asc_clicked()
{
    rafraichirTable(ui->lineEdit_recherche->text(), ui->comboBox_filtre_statut->currentText(), "immatriculation ASC");
}

void NAVIRE::on_pushButton_tri_desc_clicked()
{
    rafraichirTable(ui->lineEdit_recherche->text(), ui->comboBox_filtre_statut->currentText(), "immatriculation DESC");
}

void NAVIRE::on_pushButton_export_pdf_clicked()
{
    // --- Sélection des colonnes ---
    int totalCols = m_model->columnCount();
    QDialog colDialog(this);
    colDialog.setWindowTitle(tr("Colonnes à exporter"));
    QVBoxLayout *colLayout = new QVBoxLayout(&colDialog);
    colLayout->addWidget(new QLabel(tr("Sélectionnez les colonnes à inclure dans le PDF :"), &colDialog));

    QList<QCheckBox*> checkboxes;
    for (int j = 0; j < totalCols; j++) {
        QString header = m_model->headerData(j, Qt::Horizontal).toString();
        QCheckBox *cb = new QCheckBox(header, &colDialog);
        cb->setChecked(true);
        checkboxes.append(cb);
        colLayout->addWidget(cb);
    }

    QDialogButtonBox *btnBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &colDialog);
    connect(btnBox, &QDialogButtonBox::accepted, &colDialog, &QDialog::accept);
    connect(btnBox, &QDialogButtonBox::rejected, &colDialog, &QDialog::reject);
    colLayout->addWidget(btnBox);

    if (colDialog.exec() != QDialog::Accepted) return;

    QList<int> selectedCols;
    for (int j = 0; j < checkboxes.size(); j++) {
        if (checkboxes[j]->isChecked())
            selectedCols.append(j);
    }
    if (selectedCols.isEmpty()) {
        QMessageBox::warning(this, tr("Export PDF"), tr("Veuillez sélectionner au moins une colonne."));
        return;
    }

    // --- Choix du fichier ---
    QString fileName = QFileDialog::getSaveFileName(this, tr("Exporter en PDF"), "", tr("Fichiers PDF (*.pdf)"));
    if (fileName.isEmpty()) return;
    if (!fileName.endsWith(".pdf", Qt::CaseInsensitive)) fileName += ".pdf";

    // --- Génération PDF ---
    QPdfWriter pdfWriter(fileName);
    pdfWriter.setPageSize(QPageSize(QPageSize::A4));
    pdfWriter.setPageOrientation(QPageLayout::Landscape);
    pdfWriter.setPageMargins(QMarginsF(30, 30, 30, 30));

    QPainter painter(&pdfWriter);
    if (!painter.isActive()) return;

    painter.setFont(QFont("Arial", 10));
    painter.drawText(4000, 500, tr("LISTE DES NAVIRES"));

    int rowCount = m_model->rowCount();
    int x = 500;
    int y = 1500;
    int xStep = 1500;
    int yStep = 500;

    for (int j = 0; j < selectedCols.size(); j++)
        painter.drawText(x + (j * xStep), y, m_model->headerData(selectedCols[j], Qt::Horizontal).toString());

    y += yStep;
    painter.drawLine(x, y - 100, x + (selectedCols.size() * xStep), y - 100);

    for (int i = 0; i < rowCount; i++) {
        for (int j = 0; j < selectedCols.size(); j++)
            painter.drawText(x + (j * xStep), y, m_model->data(m_model->index(i, selectedCols[j])).toString());
        y += yStep;
        if (y > 9000) {
            pdfWriter.newPage();
            y = 1000;
        }
    }

    painter.end();
    QMessageBox::information(this, tr("Export PDF"), tr("La liste des navires a été exportée avec succès."));
}

void NAVIRE::on_pushButton_stats_clicked()
{
    StatsDialog dialog(m_connection.getDb(), this);
    dialog.exec();
    rafraichirTable(); // Refresh in case stats or indirect changes happen
}

void NAVIRE::on_pushButton_historique_clicked()
{
    HistoryDialog dialog(m_connection.getDb(), this);
    dialog.exec();
    rafraichirTable(); // Refresh table to show restored navires
}
void NAVIRE::on_pushButton_chatbot_clicked()
{
    ChatbotDialog dialog(m_connection.getDb(), this);
    dialog.exec();
}

