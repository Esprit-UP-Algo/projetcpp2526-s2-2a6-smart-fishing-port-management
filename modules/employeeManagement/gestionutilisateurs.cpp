#include "gestionutilisateurs.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include "connection.h"
#include <QThread>

DialogEmploye::DialogEmploye(QWidget *parent, bool isEditMode, const QList<Employe>* existingEmployes)
    : QDialog(parent), isEditMode(isEditMode), existingEmployes(existingEmployes)
{
    setWindowTitle(isEditMode ? "Modifier l'employé" : "Ajouter un employé");
    setFixedSize(500, 450); // réduit
    setStyleSheet("background-color: white;");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20); // réduit
    mainLayout->setSpacing(15); // réduit

    QLabel *titre = new QLabel(isEditMode ? "✏️ Modifier l'employé" : "➕ Ajouter un employé");
    titre->setAlignment(Qt::AlignCenter);
    titre->setStyleSheet("font-size: 18px; font-weight: bold; color: #1e3a8a; margin-bottom: 8px;"); // réduit
    mainLayout->addWidget(titre);

    QFrame *formFrame = new QFrame;
    formFrame->setStyleSheet("background-color: #f8fafc; border-radius: 8px; padding: 15px; border: 1px solid #e2e8f0;"); // réduit

    QFormLayout *formLayout = new QFormLayout(formFrame);
    formLayout->setSpacing(12); // réduit
    formLayout->setLabelAlignment(Qt::AlignRight);
    formLayout->setFormAlignment(Qt::AlignLeft);

    lineId = new QLineEdit;
    lineId->setPlaceholderText("ID automatique");
    lineId->setReadOnly(true);
    lineId->setMinimumHeight(35); // réduit
    lineId->setStyleSheet("background-color: #f1f5f9; border: 1px solid #cbd5e1; border-radius: 5px; padding: 6px;"); // réduit

    lineNom = new QLineEdit;
    lineNom->setPlaceholderText("Nom complet (2-100 caractères)");
    lineNom->setMaxLength(100);
    lineNom->setMinimumHeight(35);
    lineNom->setStyleSheet("border: 1px solid #cbd5e1; border-radius: 5px; padding: 6px;");

    comboPoste = new QComboBox;
    comboPoste->addItems({"Pêcheur", "Matelot", "Docker", "Chef de quai", "Superviseur"});
    comboPoste->setMinimumHeight(35);
    comboPoste->setStyleSheet("border: 1px solid #cbd5e1; border-radius: 5px; padding: 6px;");

    comboGenre = new QComboBox;
    comboGenre->addItems({"M", "F"});
    comboGenre->setMaxVisibleItems(2);
    comboGenre->setMinimumHeight(35);
    comboGenre->setStyleSheet("border: 1px solid #cbd5e1; border-radius: 5px; padding: 6px;");

    lineEmail = new QLineEdit;
    lineEmail->setPlaceholderText("email@example.com");
    lineEmail->setMaxLength(120);
    lineEmail->setMinimumHeight(35);
    lineEmail->setStyleSheet("border: 1px solid #cbd5e1; border-radius: 5px; padding: 6px;");

    lineTelephone = new QLineEdit;
    lineTelephone->setPlaceholderText("ex: 0600000000 ou +216600000000");
    lineTelephone->setMaxLength(20);
    lineTelephone->setMinimumHeight(35);
    lineTelephone->setStyleSheet("border: 1px solid #cbd5e1; border-radius: 5px; padding: 6px;");

    spinSalaire = new QDoubleSpinBox;
    spinSalaire->setRange(0, 1000000);
    spinSalaire->setValue(35000);
    spinSalaire->setSuffix(" TND");
    spinSalaire->setDecimals(0);
    spinSalaire->setMinimumHeight(35);
    spinSalaire->setStyleSheet("border: 1px solid #cbd5e1; border-radius: 5px; padding: 6px;");

    formLayout->addRow("<b>ID:</b>", lineId);
    formLayout->addRow("<b>Nom:</b>", lineNom);
    formLayout->addRow("<b>Poste:</b>", comboPoste);
    formLayout->addRow("<b>Genre:</b>", comboGenre);
    formLayout->addRow("<b>Email:</b>", lineEmail);
    formLayout->addRow("<b>Téléphone:</b>", lineTelephone);
    formLayout->addRow("<b>Salaire:</b>", spinSalaire);

    mainLayout->addWidget(formFrame);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->setSpacing(15); // réduit

    btnSave = new QPushButton(isEditMode ? "✔️ Modifier" : "💾 Ajouter");
    btnCancel = new QPushButton("❌ Annuler");

    btnSave->setMinimumHeight(40); // réduit
    btnCancel->setMinimumHeight(40);
    btnSave->setMinimumWidth(120); // réduit
    btnCancel->setMinimumWidth(120);
    btnSave->setCursor(Qt::PointingHandCursor);
    btnCancel->setCursor(Qt::PointingHandCursor);

    btnSave->setStyleSheet("background-color: #3b82f6; color: white; font-weight: bold; font-size: 14px; border-radius: 6px; padding: 8px;"); // réduit
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
QString DialogEmploye::getGenre() const { return comboGenre->currentText(); }

void DialogEmploye::setEmployeData(const QString &id, const QString &nom, const QString &poste,
                                   const QString &email, const QString &telephone, double salaire, const QString &genre)
{
    lineId->setText(id);
    lineNom->setText(nom);
    comboPoste->setCurrentText(poste);
    comboGenre->setCurrentText(genre.isEmpty() ? "M" : genre);
    lineEmail->setText(email);
    lineTelephone->setText(telephone);
    spinSalaire->setValue(salaire);
    
    // Save original values for uniqueness check during edit
    originalEmail = email;
    originalPhone = telephone;
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

bool DialogEmploye::isValidNom(const QString &nom) const
{
    QString n = nom.trimmed();
    if (n.length() < 2 || n.length() > 100) return false;
    // Vérifier que le nom ne contient que des lettres, espaces, tirets et apostrophes
    QRegularExpression re(R"(^[a-zA-ZÀ-ÿ\s\-']+$)");
    return re.match(n).hasMatch();
}

bool DialogEmploye::isValidEmail(const QString &email) const
{
    QString e = email.trimmed().toLower();
    // Regex RFC 5322 simplifiée mais robuste
    QRegularExpression re(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
    return re.match(e).hasMatch() && e.length() <= 120;
}

bool DialogEmploye::isValidPhone(const QString &phone) const
{
    QString p = phone.trimmed();
    // Enlever espaces, tirets, parenthèses et points
    p.remove(QRegularExpression(R"([\s\-\(\)\.])"));
    // Permettre: +1-15 chiffres (avec + optionsnel au début)
    QRegularExpression re(R"(^\+?[0-9]{6,15}$)");
    return re.match(p).hasMatch();
}

bool DialogEmploye::isValidSalaire(double salaire) const
{
    // Salaire valide: entre 0 et 1 000 000 TND
    return salaire >= 0 && salaire <= 1000000;
}

bool DialogEmploye::isEmailUnique(const QString &email) const
{
    if (!existingEmployes) return true;
    
    QString e = email.trimmed().toLower();
    for (const Employe &emp : *existingEmployes) {
        if (emp.email.trimmed().toLower() == e) {
            // Si c'est un mode édition, on permet l'email de l'employé en cours d'édition
            if (isEditMode && emp.email.trimmed().toLower() == originalEmail.trimmed().toLower())
                continue;
            return false;
        }
    }
    return true;
}

bool DialogEmploye::isPhoneUnique(const QString &phone) const
{
    if (!existingEmployes) return true;
    
    QString p = phone.trimmed();
    p.remove(QRegularExpression(R"([\s\-\(\)\.])"));
    
    for (const Employe &emp : *existingEmployes) {
        QString empPhone = emp.telephone.trimmed();
        empPhone.remove(QRegularExpression(R"([\s\-\(\)\.])"));
        
        if (empPhone == p) {
            // Si c'est un mode édition, on permet le téléphone de l'employé en cours d'édition
            if (isEditMode && empPhone == originalPhone.trimmed().remove(QRegularExpression(R"([\s\-\(\)\.]))")))
                continue;
            return false;
        }
    }
    return true;
}

bool DialogEmploye::validateInputs()
{
    // Validation du nom
    QString nom = lineNom->text();
    if (nom.trimmed().isEmpty()) {
        QMessageBox::warning(this, "Erreur de validation", 
            "❌ Le nom est obligatoire.");
        lineNom->setFocus();
        return false;
    }
    if (!isValidNom(nom)) {
        QMessageBox::warning(this, "Erreur de validation", 
            "❌ Le nom doit contenir entre 2 et 100 caractères et ne peut contenir que des lettres, espaces, tirets et apostrophes.");
        lineNom->setFocus();
        return false;
    }

    // Validation du poste (le combobox est toujours valide)

    // Validation de l'email
    QString email = lineEmail->text();
    if (email.trimmed().isEmpty()) {
        QMessageBox::warning(this, "Erreur de validation", 
            "❌ L'email est obligatoire.");
        lineEmail->setFocus();
        return false;
    }
    if (!isValidEmail(email)) {
        QMessageBox::warning(this, "Erreur de validation", 
            "❌ L'email est invalide. Format attendu: exemple@domaine.com");
        lineEmail->setFocus();
        return false;
    }
    if (!isEmailUnique(email)) {
        QMessageBox::warning(this, "Erreur de validation", 
            "❌ Cet email est déjà utilisé par un autre employé.");
        lineEmail->setFocus();
        return false;
    }

    // Validation du téléphone
    QString phone = lineTelephone->text();
    if (phone.trimmed().isEmpty()) {
        QMessageBox::warning(this, "Erreur de validation", 
            "❌ Le téléphone est obligatoire.");
        lineTelephone->setFocus();
        return false;
    }
    if (!isValidPhone(phone)) {
        QMessageBox::warning(this, "Erreur de validation", 
            "❌ Le téléphone est invalide. Entrez au moins 6 chiffres (ex: 0600000000 ou +216600000000).");
        lineTelephone->setFocus();
        return false;
    }
    if (!isPhoneUnique(phone)) {
        QMessageBox::warning(this, "Erreur de validation", 
            "❌ Ce numéro de téléphone est déjà utilisé par un autre employé.");
        lineTelephone->setFocus();
        return false;
    }

    // Validation du salaire
    double salaire = spinSalaire->value();
    if (!isValidSalaire(salaire)) {
        QMessageBox::warning(this, "Erreur de validation", 
            "❌ Le salaire doit être entre 0 et 1 000 000 TND.");
        spinSalaire->setFocus();
        return false;
    }

    return true;
}

GestionUtilisateurs::GestionUtilisateurs(QWidget *parent) : QWidget(parent)
{
    configurerInterface();
    loadFromDb();  // Load directly from database (single source of truth)
    actualiserTable();
    mettreAJourStatistiques();
    mettreAJourStatistiquesSalaires();
}

GestionUtilisateurs::~GestionUtilisateurs()
{
    // Database persists all changes automatically
    // No need to save to local JSON file
}

void GestionUtilisateurs::configurerInterface()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    QHBoxLayout *toolbar = new QHBoxLayout;
    toolbar->setContentsMargins(8, 8, 8, 8); // réduit
    toolbar->setSpacing(8); // réduit

    btnAjouter = new QPushButton("➕ Ajouter");
    btnModifier = new QPushButton("✏️ Modifier");
    btnSupprimer = new QPushButton("🗑️ Supprimer");
    btnConsulter = new QPushButton("📋 Consulter");
    btnExporter = new QPushButton("📥 Exporter CSV");

    QList<QPushButton*> toolButtons = {btnAjouter, btnModifier, btnSupprimer, btnConsulter, btnExporter};
    foreach(QPushButton *btn, toolButtons) {
        btn->setMinimumHeight(35); // réduit
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet("background-color: #3b82f6; color: white; border-radius: 5px; padding: 6px 12px; font-weight: bold;"); // réduit
        toolbar->addWidget(btn);
    }
    toolbar->addStretch();

    mainLayout->addLayout(toolbar);

    QWidget *mainFrame = new QWidget;
    mainFrame->setObjectName("pageFrame");
    mainFrame->setStyleSheet("background: #1e3a8a; border-radius: 8px; margin: 8px;"); // réduit

    configurerTableauBord();

    QVBoxLayout *frameLayout = new QVBoxLayout(mainFrame);
    frameLayout->addWidget(pageTableauBord);
    frameLayout->setContentsMargins(15, 15, 15, 15); // réduit

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
    layout->setContentsMargins(20, 20, 20, 20); // réduit
    layout->setSpacing(15); // réduit

    QLabel *titre = new QLabel("👥 Gestion des utilisateurs");
    titre->setStyleSheet("font-size: 22px; font-weight: bold; color: #1e3a8a; margin-bottom: 8px;"); // réduit

    QFrame *frameStats = new QFrame;
    frameStats->setStyleSheet("background-color: white; border-radius: 8px; border: 1px solid #e2e8f0; padding: 15px;"); // réduit

    QVBoxLayout *statsInnerLayout = new QVBoxLayout(frameStats);

    QLabel *titreStats = new QLabel("📊 Répartition par poste");
    titreStats->setStyleSheet("font-size: 16px; font-weight: bold; color: #1e293b; margin-bottom: 10px;"); // réduit

    QHBoxLayout *postesLayout = new QHBoxLayout;
    postesLayout->setSpacing(10); // réduit

    statPecheur = new QLabel("🎣 Pêcheur: 0");
    statMatelot = new QLabel("⛵ Matelot: 0");
    statDocker = new QLabel("📦 Docker: 0");
    statChefQuai = new QLabel("👔 Chef de quai: 0");
    statSuperviseur = new QLabel("📋 Superviseur: 0");

    QString statStyle = "font-size: 13px; font-weight: bold; color: #1e3a8a; padding: 8px; background: #f1f5f9; border-radius: 6px; border-left: 4px solid #3b82f6;"; // réduit
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
    frameSalaires->setStyleSheet("background-color: white; border-radius: 8px; border: 1px solid #e2e8f0; padding: 15px; margin-top: 5px;"); // réduit

    QVBoxLayout *salairesInnerLayout = new QVBoxLayout(frameSalaires);

    QLabel *titreSalaires = new QLabel("💰 Statistiques des salaires");
    titreSalaires->setStyleSheet("font-size: 16px; font-weight: bold; color: #1e293b; margin-bottom: 10px;"); // réduit

    QHBoxLayout *salairesLayout = new QHBoxLayout;
    salairesLayout->setSpacing(10); // réduit

    salaryAverage = new QLabel("📊 Salaire moyen: 0 TND");
    salaryHighest = new QLabel("📈 Salaire max: 0 TND");
    salaryLowest = new QLabel("📉 Salaire min: 0 TND");
    salaryTotal = new QLabel("💵 Total mensuel: 0 TND");

    QString salaryStyle = "font-size: 13px; font-weight: bold; color: #047857; padding: 8px; background: #f1f5f9; border-radius: 6px; border-left: 4px solid #10b981;"; // réduit
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
    actionsLayout->setSpacing(10); // réduit

    lineRecherche = new QLineEdit;
    lineRecherche->setPlaceholderText("🔍 Rechercher par nom, email ou poste...");
    lineRecherche->setFixedWidth(300); // réduit
    lineRecherche->setMinimumHeight(40); // réduit
    lineRecherche->setStyleSheet("padding: 10px; border: 1px solid #cbd5e1; border-radius: 6px; background: white; font-size: 13px;"); // réduit

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
    tableEmployes->setMinimumHeight(250); // réduit
    tableEmployes->setAlternatingRowColors(true);
    tableEmployes->setStyleSheet("background: white; border: 1px solid #e2e8f0; border-radius: 6px;"); // réduit

    layout->addWidget(titre);
    layout->addWidget(frameStats);
    layout->addWidget(frameSalaires);
    layout->addLayout(actionsLayout);
    layout->addWidget(tableEmployes);

    connect(lineRecherche, &QLineEdit::textChanged, this, &GestionUtilisateurs::rechercherEmploye);
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
        actionLayout->setContentsMargins(3, 3, 3, 3); // réduit
        actionLayout->setSpacing(5); // réduit

        QPushButton *btnEdit = new QPushButton("✏️");
        btnEdit->setToolTip("Modifier");
        btnEdit->setMaximumWidth(35); // réduit
        btnEdit->setMinimumHeight(30); // réduit
        btnEdit->setStyleSheet("background-color: #3b82f6; color: white; border-radius: 5px; font-size: 14px; padding: 3px;"); // réduit
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
        tableEmployes->setRowHeight(row, 40); // réduit

        connect(btnEdit, &QPushButton::clicked, this, [this, row]() {
            QString id = tableEmployes->item(row, 0)->text();
            for (int i = 0; i < listeEmployes.size(); i++) {
                if (listeEmployes[i].id == id) {
                    DialogEmploye dialog(this, true, &listeEmployes);
                    dialog.setEmployeData(listeEmployes[i].id, listeEmployes[i].nom,
                                          listeEmployes[i].poste, listeEmployes[i].email,
                                          listeEmployes[i].telephone, listeEmployes[i].salaire, listeEmployes[i].genre);
                    if (dialog.exec() == QDialog::Accepted) {
                        listeEmployes[i].nom = dialog.getNom().trimmed();
                        listeEmployes[i].poste = dialog.getPoste();
                        listeEmployes[i].genre = dialog.getGenre();
                        listeEmployes[i].email = dialog.getEmail().trimmed();
                        listeEmployes[i].telephone = dialog.getTelephone().trimmed();
                        listeEmployes[i].salaire = dialog.getSalaire();
                        actualiserTable();
                        mettreAJourStatistiques();
                        mettreAJourStatistiquesSalaires();
                        QMessageBox::information(this, "✅ Succès", "Employé modifié avec succès !");
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
    DialogEmploye dialog(this, false, &listeEmployes);
    dialog.setEmployeData(QUuid::createUuid().toString().left(8), "", "Pêcheur", "", "", 35000, "M");

    if (dialog.exec() == QDialog::Accepted) {
        Employe e;
        e.id = dialog.getId().isEmpty() ? QUuid::createUuid().toString().left(8) : dialog.getId();
        e.nom = dialog.getNom().trimmed();
        e.poste = dialog.getPoste();
        e.genre = dialog.getGenre();
        e.email = dialog.getEmail().trimmed();
        e.telephone = dialog.getTelephone().trimmed();
        e.salaire = dialog.getSalaire();

        listeEmployes.append(e);
        actualiserTable();
        mettreAJourStatistiques();
        mettreAJourStatistiquesSalaires();
        QMessageBox::information(this, "✅ Succès", "Employé ajouté avec succès !");
    }
}

void GestionUtilisateurs::loadFromDb()
{
    listeEmployes.clear();
    QSqlQuery q;
    if (!q.exec("SELECT id, nom, poste, email, telephone, salaire, genre FROM employes")) {
        qDebug() << "Users load error:" << q.lastError().text();
        return;  // No fallback - database is single source of truth
    }
    while (q.next()) {
        Employe e;
        e.id = q.value(0).toString();
        e.nom = q.value(1).toString();
        e.poste = q.value(2).toString();
        e.email = q.value(3).toString();
        e.telephone = q.value(4).toString();
        e.salaire = q.value(5).toDouble();
        e.genre = q.value(6).toString().isEmpty() ? "M" : q.value(6).toString();
        listeEmployes.append(e);
    }
    // Note: If database is empty, listeEmployes will be empty
    // No automatic fallback to JSON file
    actualiserTable();
    mettreAJourStatistiques();
    mettreAJourStatistiquesSalaires();
}

void GestionUtilisateurs::modifierEmploye()
{
    int row = tableEmployes->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "⚠️ Attention", "Veuillez sélectionner un employé à modifier.");
        return;
    }

    QString id = tableEmployes->item(row, 0)->text();
    for (int i = 0; i < listeEmployes.size(); i++) {
        if (listeEmployes[i].id == id) {
            DialogEmploye dialog(this, true, &listeEmployes);
            dialog.setEmployeData(listeEmployes[i].id, listeEmployes[i].nom,
                                  listeEmployes[i].poste, listeEmployes[i].email,
                                  listeEmployes[i].telephone, listeEmployes[i].salaire, listeEmployes[i].genre);
            if (dialog.exec() == QDialog::Accepted) {
                listeEmployes[i].nom = dialog.getNom().trimmed();
                listeEmployes[i].poste = dialog.getPoste();
                listeEmployes[i].genre = dialog.getGenre();
                listeEmployes[i].email = dialog.getEmail().trimmed();
                listeEmployes[i].telephone = dialog.getTelephone().trimmed();
                listeEmployes[i].salaire = dialog.getSalaire();
                actualiserTable();
                mettreAJourStatistiques();
                mettreAJourStatistiquesSalaires();
                QMessageBox::information(this, "✅ Succès", "Employé modifié avec succès !");
            }
            break;
        }
    }
}

void GestionUtilisateurs::supprimerEmploye()
{
    int row = tableEmployes->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "⚠️ Attention", "Veuillez sélectionner un employé à supprimer.");
        return;
    }

    QString nom = tableEmployes->item(row, 1)->text();
    QString id = tableEmployes->item(row, 0)->text();
    
    // Confirmation avec détails
    QMessageBox msgBox(QMessageBox::Warning, "⚠️ Confirmation de suppression",
                       QString("Êtes-vous sûr de vouloir supprimer l'employé ?\n\n"
                              "ID: %1\n"
                              "Nom: %2\n\n"
                              "Cette action est irréversible.").arg(id, nom),
                       QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    
    if (msgBox.exec() == QMessageBox::Yes) {
        for (int i = 0; i < listeEmployes.size(); i++) {
            if (listeEmployes[i].id == id) {
                listeEmployes.removeAt(i);
                break;
            }
        }
        mettreAJourStatistiquesSalaires();
        QMessageBox::information(this, "✅ Succès", QString("Employé '%1' supprimé avec succès !").arg(nom));
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
    out.setEncoding(QStringConverter::Utf8);

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
            padding: 8px; /* réduit */
            border: none;
            font-weight: bold;
            font-size: 13px; /* réduit */
        }
    )");
}
