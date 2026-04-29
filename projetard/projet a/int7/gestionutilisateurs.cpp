#include "gestionutilisateurs.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

DialogEmploye::DialogEmploye(QWidget *parent, bool isEditMode)
    : QDialog(parent), isEditMode(isEditMode)
{
    setWindowTitle(isEditMode ? "Modifier l'employé" : "Ajouter un employé");
    setFixedSize(500, 450);
    setStyleSheet("background-color: white;");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    QLabel *titre = new QLabel(isEditMode ? "✏️ Modifier l'employé" : "➕ Ajouter un employé");
    titre->setAlignment(Qt::AlignCenter);
    titre->setStyleSheet("font-size: 18px; font-weight: bold; color: #1e3a8a; margin-bottom: 8px;");
    mainLayout->addWidget(titre);

    QFrame *formFrame = new QFrame;
    formFrame->setStyleSheet("background-color: #f8fafc; border-radius: 8px; padding: 15px; border: 1px solid #e2e8f0;");

    QFormLayout *formLayout = new QFormLayout(formFrame);
    formLayout->setSpacing(12);
    formLayout->setLabelAlignment(Qt::AlignRight);
    formLayout->setFormAlignment(Qt::AlignLeft);

    lineId = new QLineEdit;
    lineId->setPlaceholderText("ID automatique");
    lineId->setReadOnly(true);
    lineId->setMinimumHeight(35);
    lineId->setStyleSheet("background-color: #f1f5f9; border: 1px solid #cbd5e1; border-radius: 5px; padding: 6px;");

    lineNom = new QLineEdit;
    lineNom->setPlaceholderText("Nom complet");
    lineNom->setMinimumHeight(35);
    lineNom->setStyleSheet("border: 1px solid #cbd5e1; border-radius: 5px; padding: 6px;");

    comboPoste = new QComboBox;
    comboPoste->addItems({"Pêcheur", "Matelot", "Docker", "Chef de quai", "Superviseur"});
    comboPoste->setMinimumHeight(35);
    comboPoste->setStyleSheet("border: 1px solid #cbd5e1; border-radius: 5px; padding: 6px;");

    lineEmail = new QLineEdit;
    lineEmail->setPlaceholderText("email@example.com");
    lineEmail->setMinimumHeight(35);
    lineEmail->setStyleSheet("border: 1px solid #cbd5e1; border-radius: 5px; padding: 6px;");

    lineTelephone = new QLineEdit;
    lineTelephone->setPlaceholderText("0600000000");
    lineTelephone->setMinimumHeight(35);
    lineTelephone->setStyleSheet("border: 1px solid #cbd5e1; border-radius: 5px; padding: 6px;");

    spinSalaire = new QDoubleSpinBox;
    spinSalaire->setRange(0, 1000000);
    spinSalaire->setValue(35000);
    spinSalaire->setSuffix(" TND");
    spinSalaire->setMinimumHeight(35);
    spinSalaire->setStyleSheet("border: 1px solid #cbd5e1; border-radius: 5px; padding: 6px;");

    formLayout->addRow("<b>ID:</b>", lineId);
    formLayout->addRow("<b>Nom:</b>", lineNom);
    formLayout->addRow("<b>Poste:</b>", comboPoste);
    formLayout->addRow("<b>Email:</b>", lineEmail);
    formLayout->addRow("<b>Téléphone:</b>", lineTelephone);
    formLayout->addRow("<b>Salaire:</b>", spinSalaire);

    mainLayout->addWidget(formFrame);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->setSpacing(15);

    btnSave = new QPushButton(isEditMode ? "✔️ Modifier" : "💾 Ajouter");
    btnCancel = new QPushButton("❌ Annuler");

    btnSave->setMinimumHeight(40);
    btnCancel->setMinimumHeight(40);
    btnSave->setMinimumWidth(120);
    btnCancel->setMinimumWidth(120);
    btnSave->setCursor(Qt::PointingHandCursor);
    btnCancel->setCursor(Qt::PointingHandCursor);

    btnSave->setStyleSheet("background-color: #3b82f6; color: white; font-weight: bold; font-size: 14px; border-radius: 6px; padding: 8px;");
    btnCancel->setStyleSheet("background-color: #ef4444; color: white; font-weight: bold; font-size: 14px; border-radius: 6px; padding: 8px;");

    btnLayout->addStretch();
    btnLayout->addWidget(btnSave);
    btnLayout->addWidget(btnCancel);
    btnLayout->addStretch();

    mainLayout->addLayout(btnLayout);

    connect(btnSave, &QPushButton::clicked, this, &DialogEmploye::onSaveClicked);
    connect(btnCancel, &QPushButton::clicked, this, &DialogEmploye::onCancelClicked);
}

DialogEmploye::~DialogEmploye() {}

QString DialogEmploye::getId() const { return lineId->text(); }
QString DialogEmploye::getNom() const { return lineNom->text().trimmed(); }
QString DialogEmploye::getPoste() const { return comboPoste->currentText(); }
QString DialogEmploye::getEmail() const { return lineEmail->text().trimmed(); }
QString DialogEmploye::getTelephone() const { return lineTelephone->text().trimmed(); }
double DialogEmploye::getSalaire() const { return spinSalaire->value(); }

void DialogEmploye::setEmployeData(const QString &id, const QString &nom, const QString &poste,
                                   const QString &email, const QString &telephone, double salaire)
{
    lineId->setText(id);
    lineNom->setText(nom);
    comboPoste->setCurrentText(poste);
    lineEmail->setText(email);
    lineTelephone->setText(telephone);
    spinSalaire->setValue(salaire);
}

void DialogEmploye::onSaveClicked()
{
    if (validateInputs()) {
        accept();
    }
}

void DialogEmploye::onCancelClicked()
{
    reject();
}

bool DialogEmploye::validateInputs()
{
    if (lineNom->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer un nom.");
        lineNom->setFocus();
        return false;
    }

    if (comboPoste->currentText().isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner un poste.");
        comboPoste->setFocus();
        return false;
    }

    QString email = lineEmail->text().trimmed();
    if (email.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer un email.");
        lineEmail->setFocus();
        return false;
    }
    QRegularExpression emailRegex(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
    if (!emailRegex.match(email).hasMatch()) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer un email valide.");
        lineEmail->setFocus();
        return false;
    }

    QString phone = lineTelephone->text().trimmed();
    if (phone.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer un numéro de téléphone.");
        lineTelephone->setFocus();
        return false;
    }
    QRegularExpression phoneRegex(R"(^[0-9+\-\s()]{10,}$)");
    if (!phoneRegex.match(phone).hasMatch()) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer un numéro valide (minimum 10 chiffres).");
        lineTelephone->setFocus();
        return false;
    }

    if (spinSalaire->value() <= 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer un salaire valide.");
        spinSalaire->setFocus();
        return false;
    }

    return true;
}

GestionUtilisateurs::GestionUtilisateurs(QWidget *parent) : QWidget(parent)
{
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataDir);
    m_jsonPath = QDir(dataDir).filePath("employes.json");

    configurerInterface();
    chargerEmployes();
    actualiserTable();
    mettreAJourStatistiques();
    mettreAJourStatistiquesSalaires();
}

GestionUtilisateurs::~GestionUtilisateurs()
{
    sauvegarderEmployes();
}

void GestionUtilisateurs::configurerInterface()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    QHBoxLayout *toolbar = new QHBoxLayout;
    toolbar->setContentsMargins(8, 8, 8, 8);
    toolbar->setSpacing(8);

    btnAjouter = new QPushButton("➕ Ajouter");
    btnModifier = new QPushButton("✏️ Modifier");
    btnSupprimer = new QPushButton("🗑️ Supprimer");
    btnConsulter = new QPushButton("📋 Consulter");
    btnExporter = new QPushButton("📥 Exporter CSV");

    QList<QPushButton*> toolButtons = {btnAjouter, btnModifier, btnSupprimer, btnConsulter, btnExporter};
    foreach(QPushButton *btn, toolButtons) {
        btn->setMinimumHeight(35);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet("background-color: #3b82f6; color: white; border-radius: 5px; padding: 6px 12px; font-weight: bold;");
        toolbar->addWidget(btn);
    }
    toolbar->addStretch();

    mainLayout->addLayout(toolbar);

    QWidget *mainFrame = new QWidget;
    mainFrame->setObjectName("pageFrame");
    mainFrame->setStyleSheet("background: #1e3a8a; border-radius: 8px; margin: 8px;");

    configurerTableauBord();

    QVBoxLayout *frameLayout = new QVBoxLayout(mainFrame);
    frameLayout->addWidget(pageTableauBord);
    frameLayout->setContentsMargins(15, 15, 15, 15);

    mainLayout->addWidget(mainFrame);

    connect(btnAjouter, &QPushButton::clicked, this, &GestionUtilisateurs::ajouterEmploye);
    connect(btnModifier, &QPushButton::clicked, this, &GestionUtilisateurs::modifierEmploye);
    connect(btnSupprimer, &QPushButton::clicked, this, &GestionUtilisateurs::supprimerEmploye);
    connect(btnConsulter, &QPushButton::clicked, this, &GestionUtilisateurs::actualiserTable);
    connect(btnExporter, &QPushButton::clicked, this, &GestionUtilisateurs::exporterCSV);

    appliquerStyles();
}

void GestionUtilisateurs::configurerTableauBord()
{
    pageTableauBord = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(pageTableauBord);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(15);

    QLabel *titre = new QLabel("👥 Gestion des utilisateurs");
    titre->setStyleSheet("font-size: 22px; font-weight: bold; color: #1e3a8a; margin-bottom: 8px;");

    QFrame *frameStats = new QFrame;
    frameStats->setStyleSheet("background-color: white; border-radius: 8px; border: 1px solid #e2e8f0; padding: 15px;");

    QVBoxLayout *statsInnerLayout = new QVBoxLayout(frameStats);

    QLabel *titreStats = new QLabel("📊 Répartition par poste");
    titreStats->setStyleSheet("font-size: 16px; font-weight: bold; color: #1e293b; margin-bottom: 10px;");

    QHBoxLayout *postesLayout = new QHBoxLayout;
    postesLayout->setSpacing(10);

    statPecheur = new QLabel("🎣 Pêcheur: 0");
    statMatelot = new QLabel("⛵ Matelot: 0");
    statDocker = new QLabel("📦 Docker: 0");
    statChefQuai = new QLabel("👔 Chef de quai: 0");
    statSuperviseur = new QLabel("📋 Superviseur: 0");

    QString statStyle = "font-size: 13px; font-weight: bold; color: #1e3a8a; padding: 8px; background: #f1f5f9; border-radius: 6px; border-left: 4px solid #3b82f6;";
    statPecheur->setStyleSheet(statStyle);
    statMatelot->setStyleSheet(statStyle);
    statDocker->setStyleSheet(statStyle);
    statChefQuai->setStyleSheet(statStyle);
    statSuperviseur->setStyleSheet(statStyle);

    postesLayout->addWidget(statPecheur);
    postesLayout->addWidget(statMatelot);
    postesLayout->addWidget(statDocker);
    postesLayout->addWidget(statChefQuai);
    postesLayout->addWidget(statSuperviseur);
    postesLayout->addStretch();

    statsInnerLayout->addWidget(titreStats);
    statsInnerLayout->addLayout(postesLayout);

    QFrame *frameSalaires = new QFrame;
    frameSalaires->setStyleSheet("background-color: white; border-radius: 8px; border: 1px solid #e2e8f0; padding: 15px; margin-top: 5px;");

    QVBoxLayout *salairesInnerLayout = new QVBoxLayout(frameSalaires);

    QLabel *titreSalaires = new QLabel("💰 Statistiques des salaires");
    titreSalaires->setStyleSheet("font-size: 16px; font-weight: bold; color: #1e293b; margin-bottom: 10px;");

    QHBoxLayout *salairesLayout = new QHBoxLayout;
    salairesLayout->setSpacing(10);

    salaryAverage = new QLabel("📊 Salaire moyen: 0 TND");
    salaryHighest = new QLabel("📈 Salaire max: 0 TND");
    salaryLowest = new QLabel("📉 Salaire min: 0 TND");
    salaryTotal = new QLabel("💵 Total mensuel: 0 TND");

    QString salaryStyle = "font-size: 13px; font-weight: bold; color: #047857; padding: 8px; background: #f1f5f9; border-radius: 6px; border-left: 4px solid #10b981;";
    salaryAverage->setStyleSheet(salaryStyle);
    salaryHighest->setStyleSheet(salaryStyle);
    salaryLowest->setStyleSheet(salaryStyle);
    salaryTotal->setStyleSheet(salaryStyle);

    salairesLayout->addWidget(salaryAverage);
    salairesLayout->addWidget(salaryHighest);
    salairesLayout->addWidget(salaryLowest);
    salairesLayout->addWidget(salaryTotal);
    salairesLayout->addStretch();

    salairesInnerLayout->addWidget(titreSalaires);
    salairesInnerLayout->addLayout(salairesLayout);

    QHBoxLayout *actionsLayout = new QHBoxLayout;
    actionsLayout->setSpacing(10);

    lineRecherche = new QLineEdit;
    lineRecherche->setPlaceholderText("🔍 Rechercher par nom, email ou poste...");
    lineRecherche->setFixedWidth(300);
    lineRecherche->setMinimumHeight(40);
    lineRecherche->setStyleSheet("padding: 10px; border: 1px solid #cbd5e1; border-radius: 6px; background: white; font-size: 13px;");

    actionsLayout->addWidget(lineRecherche);
    actionsLayout->addStretch();

    tableEmployes = new QTableWidget;
    tableEmployes->setColumnCount(7);
    QStringList headers = {"ID", "Nom", "Poste", "Email", "Téléphone", "Salaire", "Actions"};
    tableEmployes->setHorizontalHeaderLabels(headers);
    tableEmployes->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableEmployes->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableEmployes->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableEmployes->verticalHeader()->setVisible(false);
    tableEmployes->setMinimumHeight(250);
    tableEmployes->setAlternatingRowColors(true);
    tableEmployes->setStyleSheet("background: white; border: 1px solid #e2e8f0; border-radius: 6px;");

    layout->addWidget(titre);
    layout->addWidget(frameStats);
    layout->addWidget(frameSalaires);
    layout->addLayout(actionsLayout);
    layout->addWidget(tableEmployes);

    connect(lineRecherche, &QLineEdit::textChanged, this, &GestionUtilisateurs::rechercherEmploye);
}

void GestionUtilisateurs::chargerEmployes()
{
    listeEmployes.clear();

    QFile file(m_jsonPath);
    if (!file.exists()) {
        listeEmployes.append(Employe("1", "Ali Ben Salem", "Pêcheur", "ali.ben@port.com", "0600000001", 35000));
        listeEmployes.append(Employe("2", "Sara Khelifa", "Matelot", "sara.khelifa@port.com", "0600000002", 32000));
        listeEmployes.append(Employe("3", "Omar Said", "Docker", "omar.said@port.com", "0600000003", 38000));
        listeEmployes.append(Employe("4", "Lina Boudiaf", "Chef de quai", "lina.boudiaf@port.com", "0600000004", 45000));
        listeEmployes.append(Employe("5", "Yassine Amrani", "Superviseur", "yassine.amrani@port.com", "0600000005", 50000));
        sauvegarderEmployes();
        return;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "Erreur", "Impossible de lire le fichier de données.");
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isArray()) return;

    QJsonArray array = doc.array();
    for (const QJsonValue &val : array) {
        if (val.isObject()) {
            Employe e;
            e.fromJson(val.toObject());
            listeEmployes.append(e);
        }
    }
}

void GestionUtilisateurs::sauvegarderEmployes()
{
    QJsonArray array;
    for (const Employe &e : listeEmployes) {
        array.append(e.toJson());
    }

    QJsonDocument doc(array);
    QFile file(m_jsonPath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
    }
}

void GestionUtilisateurs::actualiserTable()
{
    if (!tableEmployes) return;

    tableEmployes->setRowCount(0);

    for (const Employe &e : listeEmployes) {
        int row = tableEmployes->rowCount();
        tableEmployes->insertRow(row);

        tableEmployes->setItem(row, 0, new QTableWidgetItem(e.id));
        tableEmployes->setItem(row, 1, new QTableWidgetItem(e.nom));
        tableEmployes->setItem(row, 2, new QTableWidgetItem(e.poste));
        tableEmployes->setItem(row, 3, new QTableWidgetItem(e.email));
        tableEmployes->setItem(row, 4, new QTableWidgetItem(e.telephone));

        QTableWidgetItem *salaireItem = new QTableWidgetItem(QString::number(e.salaire, 'f', 0) + " TND");
        salaireItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        tableEmployes->setItem(row, 5, salaireItem);

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

        tableEmployes->setCellWidget(row, 6, actionWidget);
        tableEmployes->setRowHeight(row, 40);

        connect(btnEdit, &QPushButton::clicked, this, [this, row]() {
            QString id = tableEmployes->item(row, 0)->text();
            for (int i = 0; i < listeEmployes.size(); i++) {
                if (listeEmployes[i].id == id) {
                    DialogEmploye dialog(this, true);
                    dialog.setEmployeData(listeEmployes[i].id, listeEmployes[i].nom,
                                          listeEmployes[i].poste, listeEmployes[i].email,
                                          listeEmployes[i].telephone, listeEmployes[i].salaire);
                    if (dialog.exec() == QDialog::Accepted) {
                        listeEmployes[i].nom = dialog.getNom();
                        listeEmployes[i].poste = dialog.getPoste();
                        listeEmployes[i].email = dialog.getEmail();
                        listeEmployes[i].telephone = dialog.getTelephone();
                        listeEmployes[i].salaire = dialog.getSalaire();
                        sauvegarderEmployes();
                        actualiserTable();
                        mettreAJourStatistiques();
                        mettreAJourStatistiquesSalaires();
                        QMessageBox::information(this, "Succès", "Employé modifié avec succès !");
                    }
                    break;
                }
            }
        });

        connect(btnDelete, &QPushButton::clicked, this, [this, row]() {
            QString nom = tableEmployes->item(row, 1)->text();
            if (QMessageBox::question(this, "Confirmation",
                                      QString("Voulez-vous vraiment supprimer l'employé '%1' ?").arg(nom),
                                      QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
                QString id = tableEmployes->item(row, 0)->text();
                for (int i = 0; i < listeEmployes.size(); i++) {
                    if (listeEmployes[i].id == id) {
                        listeEmployes.removeAt(i);
                        break;
                    }
                }
                sauvegarderEmployes();
                actualiserTable();
                mettreAJourStatistiques();
                mettreAJourStatistiquesSalaires();
                QMessageBox::information(this, "Succès", "Employé supprimé avec succès !");
            }
        });
    }

    mettreAJourStatistiques();
    mettreAJourStatistiquesSalaires();
}

void GestionUtilisateurs::mettreAJourStatistiques()
{
    int pecheur = 0, matelot = 0, docker = 0, chefQuai = 0, superviseur = 0;

    for (const Employe &e : listeEmployes) {
        if (e.poste == "Pêcheur") pecheur++;
        else if (e.poste == "Matelot") matelot++;
        else if (e.poste == "Docker") docker++;
        else if (e.poste == "Chef de quai") chefQuai++;
        else if (e.poste == "Superviseur") superviseur++;
    }

    statPecheur->setText(QString("🎣 Pêcheur: %1").arg(pecheur));
    statMatelot->setText(QString("⛵ Matelot: %1").arg(matelot));
    statDocker->setText(QString("📦 Docker: %1").arg(docker));
    statChefQuai->setText(QString("👔 Chef de quai: %1").arg(chefQuai));
    statSuperviseur->setText(QString("📋 Superviseur: %1").arg(superviseur));
}

GestionUtilisateurs::UsersStats GestionUtilisateurs::getStats() const {
    GestionUtilisateurs::UsersStats s{0,0,0,0,0,0,0.0};
    s.total = listeEmployes.size();
    double sum = 0.0;
    for (const Employe &e : listeEmployes) {
        QString p = e.poste;
        if (p == "Pêcheur") s.pecheur++;
        else if (p == "Matelot") s.matelot++;
        else if (p == "Docker") s.docker++;
        else if (p == "Chef de quai") s.chefQuai++;
        else if (p == "Superviseur") s.superviseur++;
        sum += e.salaire;
    }
    s.avgSalaire = s.total ? (sum / s.total) : 0.0;
    return s;
}

void GestionUtilisateurs::mettreAJourStatistiquesSalaires()
{
    if (listeEmployes.isEmpty()) {
        salaryAverage->setText("📊 Salaire moyen: 0 TND");
        salaryHighest->setText("📈 Salaire max: 0 TND");
        salaryLowest->setText("📉 Salaire min: 0 TND");
        salaryTotal->setText("💵 Total mensuel: 0 TND");
        return;
    }

    double total = 0;
    double highest = 0;
    double lowest = std::numeric_limits<double>::max();

    for (const Employe &e : listeEmployes) {
        total += e.salaire;
        if (e.salaire > highest) highest = e.salaire;
        if (e.salaire < lowest) lowest = e.salaire;
    }

    double average = total / listeEmployes.size();

    salaryAverage->setText(QString("📊 Salaire moyen: %1 TND").arg(qRound(average)));
    salaryHighest->setText(QString("📈 Salaire max: %1 TND").arg(qRound(highest)));
    salaryLowest->setText(QString("📉 Salaire min: %1 TND").arg(qRound(lowest)));
    salaryTotal->setText(QString("💵 Total mensuel: %1 TND").arg(qRound(total)));
}

void GestionUtilisateurs::ajouterEmploye()
{
    DialogEmploye dialog(this, false);
    dialog.setEmployeData(QUuid::createUuid().toString().left(8), "", "Pêcheur", "", "", 35000);

    if (dialog.exec() == QDialog::Accepted) {
        Employe e;
        e.id = dialog.getId().isEmpty() ? QUuid::createUuid().toString().left(8) : dialog.getId();
        e.nom = dialog.getNom();
        e.poste = dialog.getPoste();
        e.email = dialog.getEmail();
        e.telephone = dialog.getTelephone();
        e.salaire = dialog.getSalaire();

        listeEmployes.append(e);
        sauvegarderEmployes();
        actualiserTable();
        mettreAJourStatistiques();
        mettreAJourStatistiquesSalaires();
        QMessageBox::information(this, "Succès", "Employé ajouté avec succès !");
    }
}

void GestionUtilisateurs::loadFromDb()
{
    listeEmployes.clear();
    QSqlQuery q;
    if (!q.exec("SELECT id, nom, poste, email, telephone, salaire FROM employes")) {
        qDebug() << "Users load error:" << q.lastError().text();
        return;
    }
    while (q.next()) {
        Employe e;
        e.id = q.value(0).toString();
        e.nom = q.value(1).toString();
        e.poste = q.value(2).toString();
        e.email = q.value(3).toString();
        e.telephone = q.value(4).toString();
        e.salaire = q.value(5).toDouble();
        listeEmployes.append(e);
    }
    actualiserTable();
    mettreAJourStatistiques();
    mettreAJourStatistiquesSalaires();
}

void GestionUtilisateurs::modifierEmploye()
{
    int row = tableEmployes->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Attention", "Veuillez sélectionner un employé à modifier.");
        return;
    }

    QString id = tableEmployes->item(row, 0)->text();
    for (int i = 0; i < listeEmployes.size(); i++) {
        if (listeEmployes[i].id == id) {
            DialogEmploye dialog(this, true);
            dialog.setEmployeData(listeEmployes[i].id, listeEmployes[i].nom,
                                  listeEmployes[i].poste, listeEmployes[i].email,
                                  listeEmployes[i].telephone, listeEmployes[i].salaire);
            if (dialog.exec() == QDialog::Accepted) {
                listeEmployes[i].nom = dialog.getNom();
                listeEmployes[i].poste = dialog.getPoste();
                listeEmployes[i].email = dialog.getEmail();
                listeEmployes[i].telephone = dialog.getTelephone();
                listeEmployes[i].salaire = dialog.getSalaire();
                sauvegarderEmployes();
                actualiserTable();
                mettreAJourStatistiques();
                mettreAJourStatistiquesSalaires();
                QMessageBox::information(this, "Succès", "Employé modifié avec succès !");
            }
            break;
        }
    }
}

void GestionUtilisateurs::supprimerEmploye()
{
    int row = tableEmployes->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Attention", "Veuillez sélectionner un employé à supprimer.");
        return;
    }

    QString nom = tableEmployes->item(row, 1)->text();
    if (QMessageBox::question(this, "Confirmation",
                              QString("Voulez-vous vraiment supprimer l'employé '%1' ?").arg(nom),
                              QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        QString id = tableEmployes->item(row, 0)->text();
        for (int i = 0; i < listeEmployes.size(); i++) {
            if (listeEmployes[i].id == id) {
                listeEmployes.removeAt(i);
                break;
            }
        }
        sauvegarderEmployes();
        actualiserTable();
        mettreAJourStatistiques();
        mettreAJourStatistiquesSalaires();
        QMessageBox::information(this, "Succès", "Employé supprimé avec succès !");
    }
}

void GestionUtilisateurs::rechercherEmploye(const QString &texte)
{
    for (int i = 0; i < tableEmployes->rowCount(); i++) {
        bool match = false;
        if (tableEmployes->item(i, 1) && tableEmployes->item(i, 1)->text().contains(texte, Qt::CaseInsensitive))
            match = true;
        if (tableEmployes->item(i, 2) && tableEmployes->item(i, 2)->text().contains(texte, Qt::CaseInsensitive))
            match = true;
        if (tableEmployes->item(i, 3) && tableEmployes->item(i, 3)->text().contains(texte, Qt::CaseInsensitive))
            match = true;
        tableEmployes->setRowHidden(i, !match);
    }
}

void GestionUtilisateurs::exporterCSV()
{
    if (tableEmployes->rowCount() == 0) {
        QMessageBox::warning(this, "Erreur", "Aucune donnée à exporter.");
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, "Exporter les employés en CSV",
                                                    QDir::homePath() + "/employes_" + QDate::currentDate().toString("yyyyMMdd") + ".csv",
                                                    "Fichiers CSV (*.csv);;Tous les fichiers (*)");

    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Erreur", "Impossible de créer le fichier.");
        return;
    }

    QTextStream out(&file);
    // Suppression de out.setCodec("UTF-8"); (obsolète sous Qt6)

    for (int col = 0; col < tableEmployes->columnCount() - 1; col++) {
        out << tableEmployes->horizontalHeaderItem(col)->text();
        if (col < tableEmployes->columnCount() - 2) out << ",";
    }
    out << "\n";

    for (int row = 0; row < tableEmployes->rowCount(); row++) {
        if (tableEmployes->isRowHidden(row)) continue;
        for (int col = 0; col < tableEmployes->columnCount() - 1; col++) {
            QString cell = tableEmployes->item(row, col) ? tableEmployes->item(row, col)->text() : "";
            if (cell.contains(",")) cell = "\"" + cell + "\"";
            out << cell;
            if (col < tableEmployes->columnCount() - 2) out << ",";
        }
        out << "\n";
    }

    file.close();

    QMessageBox::information(this, "Succès",
                             QString("Export CSV réussi !\nFichier: %1").arg(fileName));

    if (QMessageBox::question(this, "Ouvrir", "Voulez-vous ouvrir le fichier ?") == QMessageBox::Yes) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
    }
}

void GestionUtilisateurs::appliquerStyles()
{
    setStyleSheet(R"(
        QTableWidget::item:selected {
            background-color: #bfdbfe;
            color: #1e3a8a;
        }
        QTableWidget::item:hover {
            background-color: #f1f5f9;
        }
        QHeaderView::section:horizontal {
            background-color: #1e293b;
            color: white;
            padding: 8px;
            border: none;
            font-weight: bold;
            font-size: 13px;
        }
    )");
}
