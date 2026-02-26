#include "gestionnavires.h"
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>
#include <QThread>
#include "connection.h"

// ============ Navire ============
Navire::Navire() : nom(""), immatriculation(""), capacite(0), statut("À quai") {}
Navire::Navire(QString n, QString i, int c, QString s) : nom(n), immatriculation(i), capacite(c), statut(s) {}
QString Navire::getNom() const { return nom; }
QString Navire::getImmatriculation() const { return immatriculation; }
int Navire::getCapacite() const { return capacite; }
QString Navire::getStatut() const { return statut; }
void Navire::setNom(QString n) { nom = n; }
void Navire::setImmatriculation(QString i) { immatriculation = i; }
void Navire::setCapacite(int c) { capacite = c; }
void Navire::setStatut(QString s) { statut = s; }

// ============ DialogNavire ============
DialogNavire::DialogNavire(QWidget *parent, Navire *navire, bool adminMode)
    : QDialog(parent), modeModification(navire != nullptr), modeAdmin(adminMode)
{
    configurerInterface();
    appliquerStyles();

    if (modeModification && navire) {
        lineNom->setText(navire->getNom());
        lineImmatriculation->setText(navire->getImmatriculation());
        lineImmatriculation->setReadOnly(true);
        spinCapacite->setValue(navire->getCapacite());
        comboStatut->setCurrentText(navire->getStatut());
    }
}

void DialogNavire::configurerInterface()
{
    setWindowTitle(modeModification ? "Modifier le navire" : "Ajouter un navire");
    resize(450, 350); // réduit

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15); // réduit
    mainLayout->setContentsMargins(20, 20, 20, 20); // réduit

    QLabel *titre = new QLabel(modeModification ? "✏️ Modifier le navire" : "➕ Ajouter un navire");
    titre->setAlignment(Qt::AlignCenter);
    titre->setStyleSheet("font-size: 18px; font-weight: bold; color: #1e3a8a;"); // réduit
    mainLayout->addWidget(titre);

    QFormLayout *formLayout = new QFormLayout;
    formLayout->setSpacing(10); // réduit
    formLayout->setLabelAlignment(Qt::AlignRight);

    lineNom = new QLineEdit;
    lineNom->setPlaceholderText("Nom du navire");
    lineNom->setMinimumHeight(35); // réduit

    lineImmatriculation = new QLineEdit;
    lineImmatriculation->setPlaceholderText("Immatriculation");
    lineImmatriculation->setMinimumHeight(35);

    spinCapacite = new QSpinBox;
    spinCapacite->setRange(1, 9999);
    spinCapacite->setValue(100);
    spinCapacite->setSuffix(" tonnes");
    spinCapacite->setMinimumHeight(35);

    comboStatut = new QComboBox;
    comboStatut->addItems({"À quai", "En mer", "Interdit"});
    comboStatut->setMinimumHeight(35);

    formLayout->addRow("Nom:", lineNom);
    formLayout->addRow("Immatriculation:", lineImmatriculation);
    formLayout->addRow("Capacité:", spinCapacite);
    formLayout->addRow("Statut:", comboStatut);

    mainLayout->addLayout(formLayout);
    mainLayout->addStretch();

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->setSpacing(10); // réduit

    btnAnnuler = new QPushButton("Annuler");
    btnAnnuler->setMinimumHeight(35);
    btnAnnuler->setCursor(Qt::PointingHandCursor);

    btnEnregistrer = new QPushButton("Enregistrer");
    btnEnregistrer->setMinimumHeight(35);
    btnEnregistrer->setCursor(Qt::PointingHandCursor);
    btnEnregistrer->setDefault(true);

    if (modeAdmin && modeModification) {
        btnSupprimer = new QPushButton("Supprimer");
        btnSupprimer->setMinimumHeight(35);
        btnSupprimer->setCursor(Qt::PointingHandCursor);
        btnLayout->addWidget(btnSupprimer);
        connect(btnSupprimer, &QPushButton::clicked, this, &DialogNavire::supprimer);
    }

    btnLayout->addStretch();
    btnLayout->addWidget(btnAnnuler);
    btnLayout->addWidget(btnEnregistrer);
    mainLayout->addLayout(btnLayout);

    connect(btnEnregistrer, &QPushButton::clicked, this, &DialogNavire::valider);
    connect(btnAnnuler, &QPushButton::clicked, this, &QDialog::reject);
}

void DialogNavire::valider()
{
    if (lineNom->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Le nom est obligatoire.");
        return;
    }
    if (lineImmatriculation->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Erreur", "L'immatriculation est obligatoire.");
        return;
    }
    if (spinCapacite->value() <= 0) {
        QMessageBox::warning(this, "Erreur", "La capacité doit être supérieure à 0.");
        return;
    }
    accept();
}

void DialogNavire::supprimer()
{
    if (QMessageBox::question(this, "Confirmation", "Voulez-vous vraiment supprimer ce navire ?",
                              QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        done(2);
    }
}

Navire DialogNavire::getNavire() const
{
    Navire n;
    n.setNom(lineNom->text().trimmed());
    n.setImmatriculation(lineImmatriculation->text().trimmed());
    n.setCapacite(spinCapacite->value());
    n.setStatut(comboStatut->currentText());
    return n;
}

void DialogNavire::appliquerStyles()
{
    setStyleSheet(R"(
        QDialog { background-color: #f8fafc; }
        QLineEdit, QSpinBox, QComboBox {
            padding: 8px; /* réduit */
            border: 1px solid #cbd5e1;
            font-weight: 600;
            background-color: white;
            color: #475569;
        }
    )");
}

GestionNavires::GestionNavires(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Barre d'outils
    QHBoxLayout *toolbar = new QHBoxLayout;
    toolbar->setContentsMargins(8, 8, 8, 8); // réduit
    toolbar->setSpacing(8); // réduit
    toolbar->addStretch();

    btnAjouter = new QPushButton("➕ Ajouter");
    btnModifier = new QPushButton("✏️ Modifier");
    btnSupprimer = new QPushButton("🗑️ Supprimer");
    btnConsulter = new QPushButton("📋 Consulter");

    QList<QPushButton*> toolButtons = {btnAjouter, btnModifier, btnSupprimer, btnConsulter};
    foreach(QPushButton *btn, toolButtons) {
        btn->setMinimumHeight(35); // réduit
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet("background-color: #3b82f6; color: white; border-radius: 5px; padding: 6px 12px; font-weight: bold;"); // réduit
        toolbar->addWidget(btn);
    }
    toolbar->addStretch();

    mainLayout->addLayout(toolbar);

    // Cadre principal
    QWidget *mainFrame = new QWidget;
    mainFrame->setObjectName("pageFrame");
    mainFrame->setStyleSheet("background: white; border-radius: 8px; margin: 8px;"); // reduced background to match mainwindow

    pages = new QStackedWidget(mainFrame);
    pages->setObjectName("pagesNavires");

    configurerTableauBord();
    configurerGestion();

    pages->addWidget(pageTableauBord);
    pages->addWidget(pageGestion);

    QVBoxLayout *frameLayout = new QVBoxLayout(mainFrame);
    frameLayout->addWidget(pages);
    frameLayout->setContentsMargins(15, 15, 15, 15); // réduit

    mainLayout->addWidget(mainFrame);

    connect(btnAjouter, &QPushButton::clicked, this, &GestionNavires::afficherGestion);
    connect(btnModifier, &QPushButton::clicked, this, &GestionNavires::modifierNavire);
    connect(btnSupprimer, &QPushButton::clicked, this, &GestionNavires::supprimerNavire);
    connect(btnConsulter, &QPushButton::clicked, this, &GestionNavires::afficherTableauBord);

    appliquerStyles();
    afficherTableauBord();
}

void GestionNavires::configurerTableauBord()
{
    pageTableauBord = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(pageTableauBord);
    layout->setContentsMargins(8, 0, 8, 8);
    layout->setSpacing(0);

    QLabel *titre = new QLabel("Tableau de bord des navires");
    titre->setStyleSheet("font-size: 22px; font-weight: bold; color: #1e3a8a;");

    QFrame *frameStats = new QFrame;
    frameStats->setStyleSheet("background-color: white; border-radius: 8px; border: 1px solid #e2e8f0; padding: 8px;");

    QVBoxLayout *statsInnerLayout = new QVBoxLayout(frameStats);
    statsInnerLayout->setContentsMargins(0,0,0,0);
    statsInnerLayout->setSpacing(4);

    QLabel *titreStats = new QLabel("📊 Statistiques des navires");
    titreStats->setStyleSheet("font-size: 16px; font-weight: bold; color: #1e293b;");

    QHBoxLayout *statsLayout = new QHBoxLayout;
    statsLayout->setSpacing(4);

    // compact stat widgets: numeric label (member) + description label
    auto makeStatWidget = [&](QLabel *&valueLabel, const QString &desc, const QString &color) -> QWidget* {
        QWidget *w = new QWidget;
        QVBoxLayout *l = new QVBoxLayout(w);
        l->setContentsMargins(4,4,4,4);
        l->setSpacing(2);

        // gradient background using the provided color
        QColor base(color);
        QColor start = base.lighter(180);
        QString bg = QString("qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 %1, stop:1 %2);")
                     .arg(start.name(), base.name());

        valueLabel = new QLabel("0");
        valueLabel->setAlignment(Qt::AlignCenter);
        valueLabel->setStyleSheet(QString("font-size: 18px; font-weight: bold; color: %1;").arg(color));

        QLabel *descLabel = new QLabel(desc);
        descLabel->setAlignment(Qt::AlignCenter);
        descLabel->setStyleSheet("font-size: 13px; color: #64748b;");

        l->addWidget(valueLabel);
        l->addWidget(descLabel);

        w->setStyleSheet(QString("background: %1 border-radius: 6px; padding: 4px;").arg(bg));
        return w;
    };

    statsLayout->addWidget(makeStatWidget(labelNbTotal, "🚢 Total", "#1e3a8a"));
    statsLayout->addWidget(makeStatWidget(labelNbQuai, "⚓ À quai", "#f59e0b"));
    statsLayout->addWidget(makeStatWidget(labelNbMer, "🌊 En mer", "#10b981"));
    statsLayout->addWidget(makeStatWidget(labelNbInterdit, "⛔ Interdits", "#ef4444"));
    statsLayout->addStretch();

    statsInnerLayout->addWidget(titreStats);
    statsInnerLayout->addLayout(statsLayout);

    QHBoxLayout *actionsLayout = new QHBoxLayout;
    lineRecherche = new QLineEdit;
    lineRecherche->setPlaceholderText("🔍 Rechercher par nom ou immatriculation...");
    lineRecherche->setFixedWidth(240);
    lineRecherche->setStyleSheet("padding: 6px; border: 1px solid #cbd5e1; border-radius: 6px; background: white;");

    actionsLayout->addWidget(lineRecherche);
    actionsLayout->addStretch();

    tableNavires = new QTableWidget;
    tableNavires->setColumnCount(5);
    tableNavires->setHorizontalHeaderLabels({"Nom", "Immatriculation", "Capacité", "Statut", "Actions"});
    tableNavires->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableNavires->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableNavires->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableNavires->verticalHeader()->setVisible(false);
    tableNavires->setMinimumHeight(250); // réduit
    tableNavires->setStyleSheet("background: white; border: 1px solid #e2e8f0; border-radius: 6px;"); // réduit

    layout->addWidget(titre);
    layout->addWidget(frameStats);
    layout->addLayout(actionsLayout);
    layout->addWidget(tableNavires);
    layout->addStretch();

    connect(lineRecherche, &QLineEdit::textChanged, this, &GestionNavires::rechercherNavire);
}

void GestionNavires::configurerGestion()
{
    pageGestion = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(pageGestion);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(15); // réduit

    QLabel *titre = new QLabel("Gestion des navires");
    titre->setStyleSheet("font-size: 20px; font-weight: bold; color: #1e3a8a;"); // réduit

    QHBoxLayout *contentLayout = new QHBoxLayout;
    contentLayout->setSpacing(20); // réduit

    QFrame *formFrame = new QFrame;
    formFrame->setObjectName("formFrame");
    formFrame->setStyleSheet("background-color: white; border-radius: 8px; border: 1px solid #e2e8f0; padding: 15px;"); // réduit
    formFrame->setFixedWidth(350); // réduit

    QVBoxLayout *formLayout = new QVBoxLayout(formFrame);
    formLayout->setSpacing(8); // réduit

    QLabel *titreForm = new QLabel("Ajouter un navire");
    titreForm->setStyleSheet("font-size: 16px; font-weight: bold; color: #1e3a8a;"); // réduit
    titreForm->setAlignment(Qt::AlignCenter);

    champNom = new QLineEdit;
    champNom->setPlaceholderText("Ex: Neptune");
    champNom->setStyleSheet("padding: 10px; border: 1px solid #cbd5e1; border-radius: 6px; background: white;"); // réduit

    champImmat = new QLineEdit;
    champImmat->setPlaceholderText("Ex: 123456789");
    champImmat->setStyleSheet("padding: 10px; border: 1px solid #cbd5e1; border-radius: 6px; background: white;");

    comboCapacite = new QComboBox;
    comboCapacite->addItems({"50","100","150","200","250","291","300","350","400"});
    comboCapacite->setStyleSheet("padding: 10px; border: 1px solid #cbd5e1; border-radius: 6px; background: white;");

    comboStatut = new QComboBox;
    comboStatut->addItems({"À quai","En mer","Interdit"});
    comboStatut->setStyleSheet("padding: 10px; border: 1px solid #cbd5e1; border-radius: 6px; background: white;");

    QPushButton *btnAjouterNavire = new QPushButton("➕ Ajouter le navire");
    btnAjouterNavire->setStyleSheet("background-color: #3b82f6; color: white; padding: 12px; border-radius: 6px; font-weight: bold; font-size: 13px;"); // réduit
    btnAjouterNavire->setCursor(Qt::PointingHandCursor);

    formLayout->addWidget(titreForm);
    formLayout->addSpacing(5);
    formLayout->addWidget(new QLabel("Nom du navire"));
    formLayout->addWidget(champNom);
    formLayout->addSpacing(3);
    formLayout->addWidget(new QLabel("Immatriculation"));
    formLayout->addWidget(champImmat);
    formLayout->addSpacing(3);
    formLayout->addWidget(new QLabel("Capacité (tonnes)"));
    formLayout->addWidget(comboCapacite);
    formLayout->addSpacing(3);
    formLayout->addWidget(new QLabel("Statut"));
    formLayout->addWidget(comboStatut);
    formLayout->addSpacing(6);
    formLayout->addWidget(btnAjouterNavire);
    formLayout->addStretch();

    QVBoxLayout *tableLayout = new QVBoxLayout;
    QLabel *titreTable = new QLabel("Liste des navires");
    titreTable->setStyleSheet("font-size: 15px; font-weight: bold; color: #1e293b;"); // réduit

    tableGestion = new QTableWidget;
    tableGestion->setColumnCount(4);
    tableGestion->setHorizontalHeaderLabels({"Nom", "Immatriculation", "Capacité", "Statut"});
    tableGestion->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableGestion->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableGestion->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableGestion->verticalHeader()->setVisible(false);
    tableGestion->setMinimumHeight(250); // réduit
    tableGestion->setStyleSheet("background: white; border: 1px solid #e2e8f0; border-radius: 6px;"); // réduit

    tableLayout->addWidget(titreTable);
    tableLayout->addWidget(tableGestion);

    contentLayout->addWidget(formFrame);
    contentLayout->addLayout(tableLayout);

    layout->addWidget(titre);
    layout->addLayout(contentLayout);

    connect(btnAjouterNavire, &QPushButton::clicked, this, &GestionNavires::ajouterNavire);
}



void GestionNavires::chargerNavires()
{
    tableNavires->setRowCount(0);
    for (int idx = 0; idx < listeNavires.size(); idx++) {
        const Navire &n = listeNavires[idx];
        int row = tableNavires->rowCount();
        tableNavires->insertRow(row);
        tableNavires->setItem(row, 0, new QTableWidgetItem(n.getNom()));
        tableNavires->setItem(row, 1, new QTableWidgetItem(n.getImmatriculation()));
        tableNavires->setItem(row, 2, new QTableWidgetItem(QString::number(n.getCapacite()) + " t"));

        QTableWidgetItem *statutItem = new QTableWidgetItem(n.getStatut());
        statutItem->setTextAlignment(Qt::AlignCenter);
        if (n.getStatut() == "À quai") {
            statutItem->setBackground(QColor("#fef3c7"));
            statutItem->setForeground(QColor("#92400e"));
        } else if (n.getStatut() == "En mer") {
            statutItem->setBackground(QColor("#d1fae5"));
            statutItem->setForeground(QColor("#065f46"));
        } else if (n.getStatut() == "Interdit") {
            statutItem->setBackground(QColor("#fee2e2"));
            statutItem->setForeground(QColor("#991b1b"));
        }
        tableNavires->setItem(row, 3, statutItem);

        QWidget *actionWidget = new QWidget;
        QHBoxLayout *actionLayout = new QHBoxLayout(actionWidget);
        actionLayout->setContentsMargins(3, 3, 3, 3);
        actionLayout->setSpacing(5);

        QPushButton *btnEdit = new QPushButton("✏️");
        btnEdit->setToolTip("Modifier");
        btnEdit->setMaximumWidth(35);
        btnEdit->setMinimumHeight(30);
        btnEdit->setStyleSheet("background-color: #3b82f6; color: white; border-radius: 5px; font-size: 14px; padding: 3px;");
        btnEdit->setCursor(Qt::PointingHandCursor);

        QPushButton *btnDelete = new QPushButton("🗑️");
        btnDelete->setToolTip("Supprimer");
        btnDelete->setMaximumWidth(35);
        btnDelete->setMinimumHeight(30);
        btnDelete->setStyleSheet("background-color: #ef4444; color: white; border-radius: 5px; font-size: 14px; padding: 3px;");
        btnDelete->setCursor(Qt::PointingHandCursor);

        actionLayout->addWidget(btnEdit);
        actionLayout->addWidget(btnDelete);
        actionLayout->addStretch();

        tableNavires->setCellWidget(row, 4, actionWidget);
        tableNavires->setRowHeight(row, 40);

        connect(btnEdit, &QPushButton::clicked, this, [this, idx]() {
            if (idx >= 0 && idx < listeNavires.size()) {
                DialogNavire dialog(this, &listeNavires[idx], true);
                int result = dialog.exec();
                if (result == QDialog::Accepted) {
                    listeNavires[idx] = dialog.getNavire();
                    chargerNavires();
                    mettreAJourStatistiques();
                    QMessageBox::information(this, "Succès", "Navire modifié avec succès");
                } else if (result == 2) {
                    listeNavires.removeAt(idx);
                    chargerNavires();
                    mettreAJourStatistiques();
                    QMessageBox::information(this, "Succès", "Navire supprimé avec succès");
                }
            }
        });

        connect(btnDelete, &QPushButton::clicked, this, [this, idx]() {
            if (idx >= 0 && idx < listeNavires.size() &&
                QMessageBox::question(this, "Confirmation", "Voulez-vous vraiment supprimer ce navire ?",
                                    QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
                listeNavires.removeAt(idx);
                chargerNavires();
                mettreAJourStatistiques();
                QMessageBox::information(this, "Succès", "Navire supprimé avec succès");
            }
        });
    }

    if (tableGestion) {
        tableGestion->setRowCount(0);
        for (const Navire &n : listeNavires) {
            int row = tableGestion->rowCount();
            tableGestion->insertRow(row);
            tableGestion->setItem(row, 0, new QTableWidgetItem(n.getNom()));
            tableGestion->setItem(row, 1, new QTableWidgetItem(n.getImmatriculation()));
            tableGestion->setItem(row, 2, new QTableWidgetItem(QString::number(n.getCapacite()) + " t"));
            tableGestion->setItem(row, 3, new QTableWidgetItem(n.getStatut()));
            tableGestion->setRowHeight(row, 35); // réduit
        }
    }
}

void GestionNavires::mettreAJourStatistiques()
{
    int total = listeNavires.size();
    int quai = 0, mer = 0, interdit = 0;
    for (const Navire &n : listeNavires) {
        if (n.getStatut() == "À quai") quai++;
        else if (n.getStatut() == "En mer") mer++;
        else if (n.getStatut() == "Interdit") interdit++;
    }
    if (labelNbTotal) labelNbTotal->setText(QString::number(total));
    if (labelNbQuai) labelNbQuai->setText(QString::number(quai));
    if (labelNbMer) labelNbMer->setText(QString::number(mer));
    if (labelNbInterdit) labelNbInterdit->setText(QString::number(interdit));
}

GestionNavires::NaviresStats GestionNavires::getStats() const {
    GestionNavires::NaviresStats s{0,0,0,0};
    s.total = listeNavires.size();
    for (const Navire &n : listeNavires) {
        if (n.getStatut() == "À quai") s.quai++;
        else if (n.getStatut() == "En mer") s.mer++;
        else if (n.getStatut() == "Interdit") s.interdit++;
    }
    return s;
}

void GestionNavires::ajouterNavire()
{
    QString nom = champNom->text().trimmed();
    QString immat = champImmat->text().trimmed();
    if (nom.isEmpty() || immat.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez remplir tous les champs");
        return;
    }
    for (const Navire &n : listeNavires) {
        if (n.getImmatriculation() == immat) {
            QMessageBox::warning(this, "Erreur", "Cette immatriculation existe déjà");
            return;
        }
    }

    // Try to save to DB within a transaction
    bool savedToDb = false;
    Connection conn;
    if (conn.createconnect()) {
        QSqlDatabase db = QSqlDatabase::database();
        if (db.isOpen()) {
            if (db.transaction()) {
                QSqlQuery q(db);
                q.prepare("INSERT INTO navires (nom, immatriculation, capacite, statut) VALUES (:nom, :immat, :cap, :stat)");
                q.bindValue(":nom", nom);
                q.bindValue(":immat", immat);
                q.bindValue(":cap", comboCapacite->currentText().toInt());
                q.bindValue(":stat", comboStatut->currentText());
                if (q.exec()) {
                    if (!db.commit()) {
                        qDebug() << "DB commit failed:" << db.lastError().text();
                        db.rollback();
                    } else {
                        savedToDb = true;
                    }
                } else {
                    qDebug() << "Failed to INSERT navire:" << q.lastError().text();
                    db.rollback();
                }
            } else {
                qDebug() << "Failed to start DB transaction:" << db.lastError().text();
            }
        }
    }

    // Update local list/UI regardless, but inform if DB wasn't updated
    listeNavires.append(Navire(nom, immat, comboCapacite->currentText().toInt(), comboStatut->currentText()));
    champNom->clear();
    champImmat->clear();
    comboCapacite->setCurrentIndex(0);
    comboStatut->setCurrentIndex(0);
    chargerNavires();
    mettreAJourStatistiques();
    if (savedToDb) QMessageBox::information(this, "Succès", "Navire ajouté avec succès (saved to DB)");
    else QMessageBox::information(this, "Succès", "Navire ajouté locally (DB not available)");
}

void GestionNavires::loadFromDb()
{
    listeNavires.clear();

    // Ensure DB connection is open (with retries)
    Connection conn;
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isValid() || !db.isOpen()) {
        const int maxAttempts = 3;
        int attempts = 0;
        bool connected = false;
        while (attempts < maxAttempts) {
            if (conn.createconnect()) { connected = true; break; }
            attempts++;
            qDebug() << "[GestionNavires] DB connect attempt" << attempts << "failed, retrying...";
            QThread::msleep(200);
            db = QSqlDatabase::database();
        }
        if (!connected) {
            qDebug() << "[GestionNavires] DB connect failed after retries; aborting loadFromDb.";
            return;
        }
    }

    db = QSqlDatabase::database();
    qDebug() << "[GestionNavires] Using DB connection:" << db.connectionName() << "driver:" << db.driverName();

    QSqlQuery q(db);
    if (!q.exec("SELECT nom, immatriculation, capacite, statut FROM navires")) {
        qDebug() << "Navires load error:" << q.lastError().text();
        return;
    }
    while (q.next()) {
        QString nom = q.value(0).toString();
        QString immat = q.value(1).toString();
        int cap = q.value(2).toInt();
        QString stat = q.value(3).toString();
        listeNavires.append(Navire(nom, immat, cap, stat));
    }
    chargerNavires();
    mettreAJourStatistiques();
}

void GestionNavires::modifierNavire()
{
    int row = tableNavires->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Attention", "Veuillez sélectionner un navire à modifier.");
        return;
    }

    QString immat = tableNavires->item(row, 1)->text();
    int index = -1;
    for (int i = 0; i < listeNavires.size(); i++) {
        if (listeNavires[i].getImmatriculation() == immat) {
            index = i;
            break;
        }
    }

    if (index >= 0) {
        DialogNavire dialog(this, &listeNavires[index], true);
        int result = dialog.exec();
                if (result == QDialog::Accepted) {
                    Navire updated = dialog.getNavire();
                    bool dbOk = false;
                    Connection conn;
                    if (conn.createconnect()) {
                        QSqlDatabase db = QSqlDatabase::database();
                        if (db.isOpen() && db.transaction()) {
                            QSqlQuery q(db);
                            q.prepare("UPDATE navires SET nom = :nom, capacite = :cap, statut = :stat WHERE immatriculation = :immat");
                            q.bindValue(":nom", updated.getNom());
                            q.bindValue(":cap", updated.getCapacite());
                            q.bindValue(":stat", updated.getStatut());
                            q.bindValue(":immat", updated.getImmatriculation());
                            if (q.exec()) {
                                if (!db.commit()) {
                                    qDebug() << "DB commit failed on update:" << db.lastError().text();
                                    db.rollback();
                                } else dbOk = true;
                            } else {
                                qDebug() << "Failed to UPDATE navire:" << q.lastError().text();
                                db.rollback();
                            }
                        }
                    }
                    listeNavires[index] = updated;
                    chargerNavires();
                    mettreAJourStatistiques();
                    if (dbOk) QMessageBox::information(this, "Succès", "Navire modifié avec succès (DB)");
                    else QMessageBox::information(this, "Succès", "Navire modifié locally (DB not available)");
                } else if (result == 2) {
                    // deletion via dialog
                    QString immatToDel = listeNavires[index].getImmatriculation();
                    bool dbDeleted = false;
                    Connection conn;
                    if (conn.createconnect()) {
                        QSqlDatabase db = QSqlDatabase::database();
                        if (db.isOpen() && db.transaction()) {
                            QSqlQuery q(db);
                            q.prepare("DELETE FROM navires WHERE immatriculation = :immat");
                            q.bindValue(":immat", immatToDel);
                            if (q.exec()) {
                                if (!db.commit()) {
                                    qDebug() << "DB commit failed on delete:" << db.lastError().text();
                                    db.rollback();
                                } else dbDeleted = true;
                            } else {
                                qDebug() << "Failed to DELETE navire:" << q.lastError().text();
                                db.rollback();
                            }
                        }
                    }
                    listeNavires.removeAt(index);
                    chargerNavires();
                    mettreAJourStatistiques();
                    if (dbDeleted) QMessageBox::information(this, "Succès", "Navire supprimé avec succès (DB)");
                    else QMessageBox::information(this, "Succès", "Navire supprimé locally (DB not available)");
                }
    }
}

void GestionNavires::supprimerNavire()
{
    int row = tableNavires->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Attention", "Veuillez sélectionner un navire à supprimer.");
        return;
    }

    if (QMessageBox::question(this, "Confirmation", "Voulez-vous vraiment supprimer ce navire ?",
                              QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        QString immat = tableNavires->item(row, 1)->text();
        bool dbDeleted = false;
        Connection conn;
        if (conn.createconnect()) {
            QSqlDatabase db = QSqlDatabase::database();
            if (db.isOpen() && db.transaction()) {
                QSqlQuery q(db);
                q.prepare("DELETE FROM navires WHERE immatriculation = :immat");
                q.bindValue(":immat", immat);
                if (q.exec()) {
                    if (!db.commit()) {
                        qDebug() << "DB commit failed on delete:" << db.lastError().text();
                        db.rollback();
                    } else dbDeleted = true;
                } else {
                    qDebug() << "Failed to DELETE navire:" << q.lastError().text();
                    db.rollback();
                }
            }
        }

        for (int i = 0; i < listeNavires.size(); i++) {
            if (listeNavires[i].getImmatriculation() == immat) {
                listeNavires.removeAt(i);
                break;
            }
        }
        chargerNavires();
        mettreAJourStatistiques();
        if (dbDeleted) QMessageBox::information(this, "Succès", "Navire supprimé avec succès (DB)");
        else QMessageBox::information(this, "Succès", "Navire supprimé locally (DB not available)");
    }
}

void GestionNavires::rechercherNavire(const QString &texte)
{
    for (int i = 0; i < tableNavires->rowCount(); i++) {
        bool match = false;
        if (tableNavires->item(i, 0) && tableNavires->item(i, 0)->text().contains(texte, Qt::CaseInsensitive))
            match = true;
        if (tableNavires->item(i, 1) && tableNavires->item(i, 1)->text().contains(texte, Qt::CaseInsensitive))
            match = true;
        tableNavires->setRowHidden(i, !match);
    }
}

void GestionNavires::afficherTableauBord() { pages->setCurrentWidget(pageTableauBord); }
void GestionNavires::afficherGestion() { pages->setCurrentWidget(pageGestion); }

void GestionNavires::appliquerStyles()
{
    setStyleSheet(R"(
        QWidget#pageFrame, QFrame#pageFrame { background-color: white; border-radius: 8px; border: 1px solid #e2e8f0; }
        QPushButton { background-color: #3b82f6; color: white; border-radius: 6px; padding: 8px; font-weight: 600; }
        QPushButton:hover { background-color: #2563eb; }
        QLineEdit, QSpinBox, QDoubleSpinBox, QComboBox, QTextEdit {
            padding: 8px;
            border: 1px solid #cbd5e1;
            border-radius: 6px;
            background-color: white;
            color: #0f172a; /* ensure text is dark (black-ish) */
        }
        QTableWidget {
            border: 1px solid #e2e8f0;
            border-radius: 6px;
            background-color: white;
            color: #0f172a; /* table item text color */
        }
        QHeaderView::section {
            background-color: #1e293b;
            color: white;
            padding: 8px;
            border: none;
            font-weight: 600;
        }
        QTableWidget::item:selected {
            background-color: #bfdbfe;
            color: #1e3a8a;
        }
        QTableWidget::item:hover {
            background-color: #f1f5f9;
        }
    )");
}
