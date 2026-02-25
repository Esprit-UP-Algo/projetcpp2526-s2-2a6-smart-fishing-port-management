#include "gestionnavires.h"

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
            border-radius: 5px; /* réduit */
            background-color: white;
        }
        QLineEdit:focus, QSpinBox:focus, QComboBox:focus {
            border: 2px solid #3b82f6;
        }
        QPushButton {
            padding: 8px 20px; /* réduit */
            border-radius: 5px; /* réduit */
            font-weight: 600;
        }
        QPushButton#btnEnregistrer {
            background-color: #3b82f6;
            color: white;
            border: none;
        }
        QPushButton#btnEnregistrer:hover { background-color: #2563eb; }
        QPushButton#btnSupprimer {
            background-color: #ef4444;
            color: white;
            border: none;
        }
        QPushButton#btnSupprimer:hover { background-color: #dc2626; }
        QPushButton#btnAnnuler {
            background-color: white;
            color: #475569;
            border: 1px solid #cbd5e1;
        }
        QPushButton#btnAnnuler:hover { background-color: #f1f5f9; }
    )");
}

// ============ GestionNavires ============
GestionNavires::GestionNavires(QWidget *parent) : QWidget(parent)
{
    listeNavires.append(Navire("Transporteur", "2765987658", 291, "À quai"));
    listeNavires.append(Navire("Neptune", "123456789", 291, "En mer"));
    listeNavires.append(Navire("Atlantis", "589543217", 291, "En mer"));
    listeNavires.append(Navire("Falboot", "N-AUTEUR", 291, "Interdit"));

    configurerInterface();
    chargerNavires();
    mettreAJourStatistiques();
}

void GestionNavires::configurerInterface()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Barre d'outils
    QHBoxLayout *toolbar = new QHBoxLayout;
    toolbar->setContentsMargins(8, 8, 8, 8); // réduit
    toolbar->setSpacing(8); // réduit

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
    mainFrame->setStyleSheet("background: #1e3a8a; border-radius: 8px; margin: 8px;"); // réduit

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
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(15);

    QLabel *titre = new QLabel("Tableau de bord des navires");
    titre->setStyleSheet("font-size: 22px; font-weight: bold; color: #1e3a8a;");

    QFrame *frameStats = new QFrame;
    frameStats->setStyleSheet("background-color: white; border-radius: 8px; border: 1px solid #e2e8f0; padding: 15px;");

    QVBoxLayout *statsInnerLayout = new QVBoxLayout(frameStats);

    QLabel *titreStats = new QLabel("📊 Statistiques des navires");
    titreStats->setStyleSheet("font-size: 16px; font-weight: bold; color: #1e293b; margin-bottom: 10px;");

    QHBoxLayout *statsLayout = new QHBoxLayout;
    statsLayout->setSpacing(10);

    labelNbTotal = new QLabel("🚢 Total: 0");
    labelNbQuai = new QLabel("⚓ À quai: 0");
    labelNbMer = new QLabel("🌊 En mer: 0");
    labelNbInterdit = new QLabel("⛔ Interdits: 0");

    QString statStyle = "font-size: 13px; font-weight: bold; color: #1e3a8a; padding: 8px; background: #f1f5f9; border-radius: 6px; border-left: 4px solid #3b82f6;";
    labelNbTotal->setStyleSheet(statStyle);
    labelNbQuai->setStyleSheet(statStyle);
    labelNbMer->setStyleSheet(statStyle);
    labelNbInterdit->setStyleSheet(statStyle);

    statsLayout->addWidget(labelNbTotal);
    statsLayout->addWidget(labelNbQuai);
    statsLayout->addWidget(labelNbMer);
    statsLayout->addWidget(labelNbInterdit);
    statsLayout->addStretch();

    statsInnerLayout->addWidget(titreStats);
    statsInnerLayout->addLayout(statsLayout);

    QHBoxLayout *actionsLayout = new QHBoxLayout;
    lineRecherche = new QLineEdit;
    lineRecherche->setPlaceholderText("🔍 Rechercher par nom ou immatriculation...");
    lineRecherche->setFixedWidth(250);
    lineRecherche->setStyleSheet("padding: 10px; border: 1px solid #cbd5e1; border-radius: 6px; background: white;");

    actionsLayout->addWidget(lineRecherche);
    actionsLayout->addStretch();

    tableNavires = new QTableWidget;
    tableNavires->setColumnCount(4);
    tableNavires->setHorizontalHeaderLabels({"Nom", "Immatriculation", "Capacité", "Statut"});
    tableNavires->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableNavires->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableNavires->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableNavires->verticalHeader()->setVisible(false);
    tableNavires->setMinimumHeight(250); // réduit
    tableNavires->setStyleSheet("background: white; border: 1px solid #e2e8f0; border-radius: 6px;"); // réduit

    layout->addWidget(titre);
    layout->addLayout(cartesLayout);
    layout->addLayout(actionsLayout);
    layout->addWidget(tableNavires);
    layout->addStretch();

    connect(lineRecherche, &QLineEdit::textChanged, this, &GestionNavires::rechercherNavire);
}

void GestionNavires::configurerGestion()
{
    pageGestion = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(pageGestion);
    layout->setContentsMargins(20, 20, 20, 20); // réduit
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
    formLayout->addSpacing(10);
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
    for (const Navire &n : listeNavires) {
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
        tableNavires->setRowHeight(row, 40); // réduit
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

    listeNavires.append(Navire(nom, immat, comboCapacite->currentText().toInt(), comboStatut->currentText()));
    champNom->clear();
    champImmat->clear();
    comboCapacite->setCurrentIndex(0);
    comboStatut->setCurrentIndex(0);
    chargerNavires();
    mettreAJourStatistiques();
    QMessageBox::information(this, "Succès", "Navire ajouté avec succès");
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
            listeNavires[index] = dialog.getNavire();
            chargerNavires();
            mettreAJourStatistiques();
            QMessageBox::information(this, "Succès", "Navire modifié avec succès");
        } else if (result == 2) {
            listeNavires.removeAt(index);
            chargerNavires();
            mettreAJourStatistiques();
            QMessageBox::information(this, "Succès", "Navire supprimé avec succès");
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
        for (int i = 0; i < listeNavires.size(); i++) {
            if (listeNavires[i].getImmatriculation() == immat) {
                listeNavires.removeAt(i);
                break;
            }
        }
        chargerNavires();
        mettreAJourStatistiques();
        QMessageBox::information(this, "Succès", "Navire supprimé avec succès");
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
        }
        QTableWidget {
            border: 1px solid #e2e8f0;
            border-radius: 6px;
            background-color: white;
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
