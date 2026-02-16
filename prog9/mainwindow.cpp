#include "mainwindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QHeaderView>
#include <QMessageBox>
#include <QStatusBar>
#include <QDate>
#include <QTextStream>
#include <QFile>
#include <QProgressBar>
#include <QGroupBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QRandomGenerator>
#include <QTime>
#include <cmath>
#include <QtMath>
#include <QDebug>
#include <QDir>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    resize(1250, 750);

    // ===== CONFIGURATION DE LA FENÊTRE PRINCIPALE =====
    setWindowTitle("FISHTECH - Gestion des Quais");

    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    QHBoxLayout *root = new QHBoxLayout(central);

    // ===== MENU GAUCHE =====
    QWidget *menu = new QWidget;
    menu->setObjectName("menu");
    menu->setFixedWidth(260);

    // Titre FISHTECH en blanc
    fishtechTitle = new QLabel("FISHTECH");
    fishtechTitle->setObjectName("fishtechTitle");

    // Boutons du menu
    btnAdd    = new QPushButton("➕ Ajouter un quai");
    btnEdit   = new QPushButton("✏️ Modifier un quai");
    btnDelete = new QPushButton("🗑️ Supprimer un quai");
    btnView   = new QPushButton("📋 Consulter les quais");
    btnStats  = new QPushButton("📊 Statistiques");
    btnProp   = new QPushButton("💡 Proposition Quai");
    btnSimul  = new QPushButton("🔮 Simulation Avancée");
    btnCSV    = new QPushButton("📊 Exporter CSV");
    btnSimul->setObjectName("btnSimul");
    btnCSV->setObjectName("btnPDF");

    // Ajouter un espacement en bas
    QWidget *spacer = new QWidget;
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QVBoxLayout *menuLayout = new QVBoxLayout(menu);
    menuLayout->addWidget(fishtechTitle);
    menuLayout->addSpacing(10);
    menuLayout->addWidget(btnAdd);
    menuLayout->addWidget(btnEdit);
    menuLayout->addWidget(btnDelete);
    menuLayout->addWidget(btnView);
    menuLayout->addWidget(btnStats);
    menuLayout->addWidget(btnProp);
    menuLayout->addWidget(btnSimul);
    menuLayout->addWidget(btnCSV);
    menuLayout->addWidget(spacer);
    menuLayout->setSpacing(5);
    menuLayout->setContentsMargins(0, 0, 0, 10);

    // ===== CADRE PRINCIPAL POUR LES PAGES =====
    QWidget *mainFrame = new QWidget;
    mainFrame->setObjectName("pageFrame");

    pages = new QStackedWidget(mainFrame);
    pageAjouter      = new QWidget;
    pageModifier     = new QWidget;
    pageSupprimer    = new QWidget;
    pageConsulter    = new QWidget;
    pageStatistiques = new QWidget;
    pageProposition  = new QWidget;
    pageSimulation   = new QWidget;
    pageExportCSV    = new QWidget;

    pages->addWidget(pageAjouter);
    pages->addWidget(pageModifier);
    pages->addWidget(pageSupprimer);
    pages->addWidget(pageConsulter);
    pages->addWidget(pageStatistiques);
    pages->addWidget(pageProposition);
    pages->addWidget(pageSimulation);
    pages->addWidget(pageExportCSV);

    QVBoxLayout *frameLayout = new QVBoxLayout(mainFrame);
    frameLayout->addWidget(pages);
    frameLayout->setContentsMargins(20, 20, 20, 20);

    root->addWidget(menu);
    root->addWidget(mainFrame);

    // ================= AJOUT =================
    QLabel *titreAjouter = new QLabel("➕ Ajouter un Nouveau Quai");
    titreAjouter->setObjectName("pageTitle");

    QWidget *contentAjouter = new QWidget;
    contentAjouter->setObjectName("contentFrame");

    idEdit = new QLineEdit;
    idEdit->setPlaceholderText("Ex: Q001");
    nomEdit = new QLineEdit;
    nomEdit->setPlaceholderText("Ex: Quai Principal");
    longueurEdit = new QLineEdit;
    longueurEdit->setPlaceholderText("Ex: 150 (en mètres)");
    dispoBox = new QComboBox;
    dispoBox->addItems({"Disponible","Occupé"});

    dateDebutEdit = new QDateEdit(QDate::currentDate());
    dateFinEdit   = new QDateEdit(QDate::currentDate().addDays(7));
    dateDebutEdit->setCalendarPopup(true);
    dateFinEdit->setCalendarPopup(true);
    dateDebutEdit->hide();
    dateFinEdit->hide();

    QPushButton *btnSave = new QPushButton("💾 Enregistrer le quai");

    QFormLayout *formAdd = new QFormLayout;
    formAdd->addRow("ID du quai :", idEdit);
    formAdd->addRow("Nom du quai :", nomEdit);
    formAdd->addRow("Longueur (m) :", longueurEdit);
    formAdd->addRow("État :", dispoBox);
    formAdd->addRow("Occupé du :", dateDebutEdit);
    formAdd->addRow("Occupé au :", dateFinEdit);
    formAdd->addRow(btnSave);

    QVBoxLayout *pageAjouterLayout = new QVBoxLayout(contentAjouter);
    pageAjouterLayout->addLayout(formAdd);
    pageAjouterLayout->addStretch();

    QVBoxLayout *ajouterMainLayout = new QVBoxLayout(pageAjouter);
    ajouterMainLayout->addWidget(titreAjouter);
    ajouterMainLayout->addWidget(contentAjouter);
    ajouterMainLayout->setContentsMargins(0, 0, 0, 0);

    // ================= TABLES =================
    tableConsulter = new QTableWidget(0,6);
    tableSupprimer = new QTableWidget(0,6);
    tableModifier  = new QTableWidget(0,6);

    QStringList headers = {"ID","Nom","Longueur (m)","État","Occupé du","Occupé au"};
    tableConsulter->setHorizontalHeaderLabels(headers);
    tableSupprimer->setHorizontalHeaderLabels(headers);
    tableModifier->setHorizontalHeaderLabels(headers);

    tableConsulter->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableSupprimer->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableModifier->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // ================= CONSULTER =================
    QLabel *titreConsulter = new QLabel("📋 Consultation des Quais");
    titreConsulter->setObjectName("pageTitle");

    QWidget *contentConsulter = new QWidget;
    contentConsulter->setObjectName("contentFrame");

    QVBoxLayout *viewLay = new QVBoxLayout(contentConsulter);

    QHBoxLayout *searchLayout = new QHBoxLayout();
    searchEdit = new QLineEdit();
    searchEdit->setPlaceholderText("🔍 Rechercher par nom ou ID...");
    sortBox = new QComboBox();
    sortBox->addItems({"Trier par...", "Nom (A-Z)", "Longueur (Croissant)", "Longueur (Décroissant)", "Disponibilité", "État"});

    searchLayout->addWidget(new QLabel("Recherche:"));
    searchLayout->addWidget(searchEdit);
    searchLayout->addSpacing(20);
    searchLayout->addWidget(new QLabel("Trier:"));
    searchLayout->addWidget(sortBox);

    viewLay->addLayout(searchLayout);
    viewLay->addWidget(tableConsulter);

    QVBoxLayout *consulterMainLayout = new QVBoxLayout(pageConsulter);
    consulterMainLayout->addWidget(titreConsulter);
    consulterMainLayout->addWidget(contentConsulter);
    consulterMainLayout->setContentsMargins(0, 0, 0, 0);

    // ================= STATISTIQUES =================
    QLabel *titreStats = new QLabel("📊 Statistiques des Quais");
    titreStats->setObjectName("pageTitle");

    QWidget *contentStats = new QWidget;
    contentStats->setObjectName("contentFrame");

    QVBoxLayout *statsLayout = new QVBoxLayout(contentStats);
    monGraphique = new MonCamembert();
    statsLayout->addWidget(monGraphique);

    QLabel *statsText = new QLabel("Statistiques en temps réel");
    statsText->setAlignment(Qt::AlignCenter);
    statsText->setStyleSheet("font-size: 14px; color: #475569; margin-top: 20px;");
    statsLayout->addWidget(statsText);
    statsLayout->addStretch();

    QVBoxLayout *statsMainLayout = new QVBoxLayout(pageStatistiques);
    statsMainLayout->addWidget(titreStats);
    statsMainLayout->addWidget(contentStats);
    statsMainLayout->setContentsMargins(0, 0, 0, 0);

    // ================= PROPOSITION =================
    QLabel *titreProp = new QLabel("💡 Trouver le Quai Idéal");
    titreProp->setObjectName("pageTitle");

    QWidget *contentProp = new QWidget;
    contentProp->setObjectName("contentFrame");

    QVBoxLayout *propLayout = new QVBoxLayout(contentProp);
    QLabel *lblInst = new QLabel("Entrez la longueur de votre bateau :");
    lblInst->setStyleSheet("font-size: 14px; color: #475569;");
    longueurBateauEdit = new QLineEdit;
    longueurBateauEdit->setPlaceholderText("Exemple: 120 (en mètres)");

    QPushButton *btnCalculer = new QPushButton("🔍 Rechercher le meilleur quai");
    resultatLabel = new QLabel("Résultats s'afficheront ici...");
    resultatLabel->setStyleSheet("background: white; border: 2px dashed #94a3b8; border-radius: 10px; padding: 20px; font-size: 14px;");
    resultatLabel->setWordWrap(true);
    resultatLabel->setAlignment(Qt::AlignCenter);

    propLayout->addWidget(lblInst);
    propLayout->addWidget(longueurBateauEdit);
    propLayout->addWidget(btnCalculer);
    propLayout->addSpacing(20);
    propLayout->addWidget(resultatLabel);
    propLayout->addStretch();

    QVBoxLayout *propMainLayout = new QVBoxLayout(pageProposition);
    propMainLayout->addWidget(titreProp);
    propMainLayout->addWidget(contentProp);
    propMainLayout->setContentsMargins(0, 0, 0, 0);

    // ================= SIMULATION AMÉLIORÉE =================
    QLabel *titreSimul = new QLabel("🔮 Simulation Avancée d'Occupation");
    titreSimul->setObjectName("pageTitle");

    QWidget *contentSimul = new QWidget;
    contentSimul->setObjectName("contentFrame");

    QVBoxLayout *simulLayout = new QVBoxLayout(contentSimul);

    // Contrôles principaux
    QHBoxLayout *controlsLayout1 = new QHBoxLayout;
    spinJours = new QSpinBox;
    spinJours->setRange(7, 90);
    spinJours->setValue(30);
    spinJours->setSuffix(" jours");
    spinJours->setToolTip("Durée de la simulation");

    spinMoyenneArrivees = new QDoubleSpinBox;
    spinMoyenneArrivees->setRange(0.1, 20.0);
    spinMoyenneArrivees->setValue(2.5);
    spinMoyenneArrivees->setSuffix(" bateaux/jour");
    spinMoyenneArrivees->setDecimals(1);
    spinMoyenneArrivees->setToolTip("Nombre moyen d'arrivées par jour");

    spinEcartType = new QDoubleSpinBox;
    spinEcartType->setRange(0.1, 10.0);
    spinEcartType->setValue(1.5);
    spinEcartType->setSuffix(" écart-type");
    spinEcartType->setDecimals(1);
    spinEcartType->setToolTip("Variation des arrivées (plus grand = plus variable)");

    controlsLayout1->addWidget(new QLabel("Durée:"));
    controlsLayout1->addWidget(spinJours);
    controlsLayout1->addSpacing(15);
    controlsLayout1->addWidget(new QLabel("Arrivées moyennes:"));
    controlsLayout1->addWidget(spinMoyenneArrivees);
    controlsLayout1->addSpacing(15);
    controlsLayout1->addWidget(new QLabel("Variabilité:"));
    controlsLayout1->addWidget(spinEcartType);

    // Contrôles avancés
    QHBoxLayout *controlsLayout2 = new QHBoxLayout;
    spinDureeSejour = new QSpinBox;
    spinDureeSejour->setRange(1, 30);
    spinDureeSejour->setValue(5);
    spinDureeSejour->setSuffix(" jours moyen");
    spinDureeSejour->setToolTip("Durée moyenne de séjour des bateaux");

    checkVariationsJournalieres = new QCheckBox("Variations journalières");
    checkVariationsJournalieres->setChecked(true);
    checkVariationsJournalieres->setToolTip("Simule des pics en fin de semaine");

    checkWeekend = new QCheckBox("Effet week-end");
    checkWeekend->setChecked(true);
    checkWeekend->setToolTip("Plus d'arrivées le week-end");

    checkEffetSaison = new QCheckBox("Effet saisonnier");
    checkEffetSaison->setChecked(false);
    checkEffetSaison->setToolTip("Variation selon la période de l'année");

    controlsLayout2->addWidget(new QLabel("Durée séjour:"));
    controlsLayout2->addWidget(spinDureeSejour);
    controlsLayout2->addSpacing(15);
    controlsLayout2->addWidget(checkVariationsJournalieres);
    controlsLayout2->addSpacing(15);
    controlsLayout2->addWidget(checkWeekend);
    controlsLayout2->addSpacing(15);
    controlsLayout2->addWidget(checkEffetSaison);

    // Boutons de contrôle
    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    QPushButton *btnLancerSimul = new QPushButton("🚀 Lancer la simulation");
    btnLancerSimul->setStyleSheet("background-color: #0891b2; color: white; font-size: 14px; padding: 12px;");

    btnScenarioAleatoire = new QPushButton("🎲 Générer scénario aléatoire");
    btnScenarioAleatoire->setStyleSheet("background-color: #8b5cf6; color: white; font-size: 14px; padding: 12px;");

    progressSimulation = new QProgressBar;
    progressSimulation->setVisible(false);
    progressSimulation->setRange(0, 100);
    progressSimulation->setTextVisible(true);

    buttonsLayout->addWidget(btnLancerSimul);
    buttonsLayout->addWidget(btnScenarioAleatoire);
    buttonsLayout->addWidget(progressSimulation);

    // Graphique amélioré
    histoWidget = new MonHistogramme();
    histoWidget->setMinimumHeight(350);

    // Options d'affichage du graphique
    QHBoxLayout *graphOptionsLayout = new QHBoxLayout;
    QCheckBox *checkShowGrid = new QCheckBox("Afficher la grille");
    checkShowGrid->setChecked(true);
    QCheckBox *checkShowValues = new QCheckBox("Afficher les valeurs");
    checkShowValues->setChecked(true);

    graphOptionsLayout->addWidget(checkShowGrid);
    graphOptionsLayout->addWidget(checkShowValues);
    graphOptionsLayout->addStretch();

    connect(checkShowGrid, &QCheckBox::toggled, [this, checkShowValues](bool checked) {
        histoWidget->setOptions(checked, checkShowValues->isChecked());
    });
    connect(checkShowValues, &QCheckBox::toggled, [this, checkShowGrid](bool checked) {
        histoWidget->setOptions(checkShowGrid->isChecked(), checked);
    });

    // Résultats et statistiques
    lblResultatSimul = new QLabel;
    lblResultatSimul->setObjectName("resultatSimulation");
    lblResultatSimul->setWordWrap(true);
    lblResultatSimul->setAlignment(Qt::AlignCenter);
    lblResultatSimul->setMinimumHeight(80);

    lblStatsDetaillees = new QLabel;
    lblStatsDetaillees->setWordWrap(true);
    lblStatsDetaillees->setAlignment(Qt::AlignLeft);
    lblStatsDetaillees->setStyleSheet("background: #f8fafc; border: 1px solid #cbd5e1; border-radius: 8px; padding: 15px; font-size: 12px;");

    // Assemblage du layout
    simulLayout->addLayout(controlsLayout1);
    simulLayout->addLayout(controlsLayout2);
    simulLayout->addSpacing(10);
    simulLayout->addLayout(buttonsLayout);
    simulLayout->addSpacing(20);
    simulLayout->addLayout(graphOptionsLayout);
    simulLayout->addWidget(histoWidget);
    simulLayout->addSpacing(10);
    simulLayout->addWidget(lblResultatSimul);
    simulLayout->addSpacing(10);
    simulLayout->addWidget(lblStatsDetaillees);
    simulLayout->addStretch();

    QVBoxLayout *simulMainLayout = new QVBoxLayout(pageSimulation);
    simulMainLayout->addWidget(titreSimul);
    simulMainLayout->addWidget(contentSimul);
    simulMainLayout->setContentsMargins(0, 0, 0, 0);

    // ================= EXPORT CSV =================
    QLabel *titreCSV = new QLabel("📊 Exporter le Planning en CSV");
    titreCSV->setObjectName("pageTitle");

    QWidget *contentCSV = new QWidget;
    contentCSV->setObjectName("contentFrame");

    QVBoxLayout *csvLayout = new QVBoxLayout(contentCSV);

    QFormLayout *formCSV = new QFormLayout;
    nomFichierEdit = new QLineEdit;
    nomFichierEdit->setText("planning_quais_" + QDate::currentDate().toString("yyyy-MM-dd"));
    nomFichierEdit->setPlaceholderText("Nom du fichier CSV");

    formatCSVBox = new QComboBox;
    formatCSVBox->addItems({"CSV Standard", "CSV Excel", "CSV avec Sépareur Point-Virgule"});

    formCSV->addRow("Nom du fichier :", nomFichierEdit);
    formCSV->addRow("Format :", formatCSVBox);

    btnGenererCSV = new QPushButton("💾 Générer et Exporter le CSV");
    btnGenererCSV->setStyleSheet("background-color: #059669; color: white; font-size: 14px; padding: 12px;");

    lblPreviewCSV = new QLabel;
    lblPreviewCSV->setWordWrap(true);
    lblPreviewCSV->setAlignment(Qt::AlignCenter);
    lblPreviewCSV->setStyleSheet("background: white; border: 2px solid #10b981; border-radius: 10px; padding: 15px; margin-top: 20px;");

    csvLayout->addLayout(formCSV);
    csvLayout->addWidget(btnGenererCSV, 0, Qt::AlignCenter);
    csvLayout->addSpacing(20);
    csvLayout->addWidget(lblPreviewCSV);
    csvLayout->addStretch();

    QVBoxLayout *csvMainLayout = new QVBoxLayout(pageExportCSV);
    csvMainLayout->addWidget(titreCSV);
    csvMainLayout->addWidget(contentCSV);
    csvMainLayout->setContentsMargins(0, 0, 0, 0);

    // ================= SUPPRIMER =================
    QLabel *titreSupprimer = new QLabel("🗑️ Supprimer un Quai");
    titreSupprimer->setObjectName("pageTitle");

    QWidget *contentSupprimer = new QWidget;
    contentSupprimer->setObjectName("contentFrame");

    QVBoxLayout *supLay = new QVBoxLayout(contentSupprimer);

    QPushButton *btnDelRow = new QPushButton("🗑️ Supprimer la sélection");
    btnDelRow->setStyleSheet("background-color: #ef4444; color: white; font-size: 14px; padding: 12px;");

    supLay->addWidget(tableSupprimer);
    supLay->addWidget(btnDelRow, 0, Qt::AlignCenter);

    QVBoxLayout *supprimerMainLayout = new QVBoxLayout(pageSupprimer);
    supprimerMainLayout->addWidget(titreSupprimer);
    supprimerMainLayout->addWidget(contentSupprimer);
    supprimerMainLayout->setContentsMargins(0, 0, 0, 0);

    // ================= MODIFIER =================
    QLabel *titreModifier = new QLabel("✏️ Modifier un Quai");
    titreModifier->setObjectName("pageTitle");

    QWidget *contentModifier = new QWidget;
    contentModifier->setObjectName("contentFrame");

    QHBoxLayout *modLayout = new QHBoxLayout(contentModifier);

    // Partie gauche : tableau
    modLayout->addWidget(tableModifier, 2);

    // Partie droite : formulaire
    QWidget *formWidget = new QWidget;
    formWidget->setStyleSheet("background: white; border-radius: 8px; padding: 20px; border: 1px solid #cbd5e1;");

    idEditM = new QLineEdit; idEditM->setReadOnly(true);
    nomEditM = new QLineEdit;
    longueurEditM = new QLineEdit;
    dispoBoxM = new QComboBox;
    dispoBoxM->addItems({"Disponible","Occupé"});

    dateDebutEditM = new QDateEdit(QDate::currentDate());
    dateFinEditM   = new QDateEdit(QDate::currentDate().addDays(7));
    dateDebutEditM->setCalendarPopup(true);
    dateFinEditM->setCalendarPopup(true);
    dateDebutEditM->hide();
    dateFinEditM->hide();

    QPushButton *btnUpdate = new QPushButton("✔️ Modifier le quai");
    btnUpdate->setStyleSheet("background-color: #059669; color: white; font-size: 14px; padding: 12px;");

    QFormLayout *formM = new QFormLayout(formWidget);
    formM->addRow("ID :", idEditM);
    formM->addRow("Nom :", nomEditM);
    formM->addRow("Longueur (m) :", longueurEditM);
    formM->addRow("État :", dispoBoxM);
    formM->addRow("Occupé du :", dateDebutEditM);
    formM->addRow("Occupé au :", dateFinEditM);
    formM->addRow(btnUpdate);

    modLayout->addWidget(formWidget, 1);

    QVBoxLayout *modMainLayout = new QVBoxLayout(pageModifier);
    modMainLayout->addWidget(titreModifier);
    modMainLayout->addWidget(contentModifier);
    modMainLayout->setContentsMargins(0, 0, 0, 0);

    // ================= CONNECTIONS =================
    connect(btnAdd,&QPushButton::clicked,this,&MainWindow::showAjouter);
    connect(btnEdit,&QPushButton::clicked,this,&MainWindow::showModifier);
    connect(btnDelete,&QPushButton::clicked,this,&MainWindow::showSupprimer);
    connect(btnView,&QPushButton::clicked,this,&MainWindow::showConsulter);
    connect(btnStats,&QPushButton::clicked,this,&MainWindow::showStatistiques);
    connect(btnProp,&QPushButton::clicked,this,&MainWindow::showProposition);
    connect(btnSimul,&QPushButton::clicked,this,&MainWindow::showSimulation);
    connect(btnCSV,&QPushButton::clicked,this,&MainWindow::showExportPDF);

    connect(btnSave,&QPushButton::clicked,this,&MainWindow::ajouterQuai);
    connect(btnDelRow,&QPushButton::clicked,this,&MainWindow::supprimerQuai);
    connect(tableModifier,&QTableWidget::cellClicked,this,&MainWindow::remplirFormulaireDepuisTable);
    connect(btnUpdate,&QPushButton::clicked,this,&MainWindow::modifierQuai);
    connect(btnCalculer, &QPushButton::clicked, this, &MainWindow::trouverMeilleurQuai);
    connect(btnLancerSimul, &QPushButton::clicked, this, &MainWindow::lancerSimulation);
    connect(btnGenererCSV, &QPushButton::clicked, this, &MainWindow::exporterCSV);
    connect(btnScenarioAleatoire, &QPushButton::clicked, this, &MainWindow::genererScenarioAleatoire);

    connect(dispoBox,&QComboBox::currentTextChanged,this,&MainWindow::onEtatChanged);
    connect(dispoBoxM,&QComboBox::currentTextChanged,this,&MainWindow::onEtatChangedM);
    connect(searchEdit, &QLineEdit::textChanged, this, &MainWindow::rechercherQuai);
    connect(sortBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::trierQuais);

    pages->setCurrentWidget(pageConsulter);
    statusBar()->showMessage("FISHTECH – Gestion des quais • Prêt");
}

// Les méthodes restantes restent identiques...
// (Les méthodes showAjouter, showModifier, etc. ne sont pas modifiées)
// ============= SIMULATION AMÉLIORÉE =============

QVector<int> MainWindow::simulerArriveesBateaux(int jours, double moyenne, double ecartType, bool variationsJournalieres) {
    QVector<int> arrivees;
    arrivees.reserve(jours);

    QRandomGenerator *rg = QRandomGenerator::global();

    for (int jour = 0; jour < jours; ++jour) {
        // Distribution normale approximative (Box-Muller)
        double u1 = 0.0, u2 = 0.0;
        do {
            u1 = rg->generateDouble();
            u2 = rg->generateDouble();
        } while (u1 <= std::numeric_limits<double>::epsilon());

        double z = std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * M_PI * u2);

        int arriveesSimulees = std::max(0, static_cast<int>(moyenne + ecartType * z + 0.5));

        // Effets spéciaux
        if (variationsJournalieres) {
            // Pics en fin de semaine
            if (jour % 7 >= 4) { // Vendredi, samedi, dimanche
                arriveesSimulees = static_cast<int>(arriveesSimulees * 1.3);
            }

            // Variation aléatoire supplémentaire
            arriveesSimulees += rg->bounded(-2, 4); // -2 à +3
        }

        arriveesSimulees = std::max(0, arriveesSimulees);
        arrivees.append(arriveesSimulees);
    }

    return arrivees;
}

StatsSimulation MainWindow::analyserResultats(const QVector<QPair<QString, int>>& resultats) {
    StatsSimulation stats;
    stats.joursSaturation = -1;
    stats.jourMaxOccupation = -1;
    stats.maxOccupation = 0;
    stats.moyenneOccupation = 0;
    stats.joursCritiques = 0;

    int total = 0;

    for (int i = 0; i < resultats.size(); ++i) {
        int occupation = resultats[i].second;
        total += occupation;

        if (occupation > stats.maxOccupation) {
            stats.maxOccupation = occupation;
            stats.jourMaxOccupation = i + 1;
        }

        if (occupation >= 100 && stats.joursSaturation == -1) {
            stats.joursSaturation = i + 1;
        }

        if (occupation > 80) {
            stats.joursCritiques++;
        }

        // Calcul de la tendance (moyenne mobile sur 5 jours)
        if (i >= 4) {
            int somme = 0;
            for (int j = i - 4; j <= i; ++j) {
                somme += resultats[j].second;
            }
            stats.tendance.append(somme / 5);
        }
    }

    if (!resultats.isEmpty()) {
        stats.moyenneOccupation = static_cast<double>(total) / resultats.size();
    }

    return stats;
}

QString MainWindow::genererRecommandations(const StatsSimulation& stats) {
    QString recommandations = "<b>📋 Recommandations :</b><br>";

    if (stats.joursSaturation > 0) {
        recommandations += QString("• ⚠️ <b>Saturation prévue le jour %1</b><br>")
                               .arg(stats.joursSaturation);
        recommandations += "• Considérez une augmentation temporaire de capacité<br>";
    }

    if (stats.maxOccupation > 90) {
        recommandations += QString("• 📈 Pic d'occupation à %1%%<br>").arg(stats.maxOccupation);
        recommandations += "• Planifiez les départs pour éviter les conflits<br>";
    }

    if (stats.joursCritiques > 0) {
        recommandations += QString("• 🔥 %1 jour(s) à plus de 80%% d'occupation<br>")
                               .arg(stats.joursCritiques);
        recommandations += "• Activez le protocole haute fréquentation<br>";
    }

    if (stats.moyenneOccupation > 70) {
        recommandations += QString("• 📊 Occupation moyenne: %1%%<br>")
                               .arg(qRound(stats.moyenneOccupation));
        recommandations += "• Performance optimale, surveillez la tendance<br>";
    } else if (stats.moyenneOccupation < 40) {
        recommandations += QString("• 📉 Occupation moyenne: %1%%<br>")
                               .arg(qRound(stats.moyenneOccupation));
        recommandations += "• Capacité sous-utilisée, envisagez des promotions<br>";
    } else {
        recommandations += QString("• ✅ Occupation moyenne: %1%%<br>")
                               .arg(qRound(stats.moyenneOccupation));
        recommandations += "• Niveau d'occupation équilibré<br>";
    }

    // Analyse de tendance
    if (!stats.tendance.isEmpty()) {
        if (stats.tendance.last() > stats.tendance.first() + 10) {
            recommandations += "• ↗️ <b>Tendance à la hausse</b> - Préparez-vous à l'affluence<br>";
        } else if (stats.tendance.last() < stats.tendance.first() - 10) {
            recommandations += "• ↘️ <b>Tendance à la baisse</b> - Évaluez les causes<br>";
        } else {
            recommandations += "• ➡️ <b>Tendance stable</b> - Situation contrôlée<br>";
        }
    }

    return recommandations;
}

void MainWindow::lancerSimulation() {
    int jours = spinJours->value();
    double moyenneArrivees = spinMoyenneArrivees->value();
    double ecartType = spinEcartType->value();
    int dureeSejourMoyenne = spinDureeSejour->value();

    int capaciteTotale = tableConsulter->rowCount();
    if(capaciteTotale == 0) {
        lblResultatSimul->setText("❌ Erreur: Aucun quai n'est configuré dans le système.");
        lblStatsDetaillees->clear();
        return;
    }

    // Afficher la progression
    progressSimulation->setVisible(true);
    progressSimulation->setValue(10);

    // 1. État initial
    int occupationActuelle = 0;
    QList<QDate> datesLiberation;
    QList<int> dureesSejour;

    for(int r = 0; r < capaciteTotale; ++r) {
        QString etat = tableConsulter->item(r,3)->text();
        if(etat == "Occupé") {
            occupationActuelle++;
            QString dateStr = tableConsulter->item(r,5)->text();
            QDate dateFin = QDate::fromString(dateStr, "yyyy-MM-dd");
            datesLiberation.append(dateFin);

            // Calculer la durée de séjour restante
            int joursRestants = QDate::currentDate().daysTo(dateFin);
            dureesSejour.append(std::max(0, joursRestants));
        }
    }

    progressSimulation->setValue(30);

    // 2. Simulation des arrivées
    QVector<int> arriveesParJour = simulerArriveesBateaux(
        jours, moyenneArrivees, ecartType,
        checkVariationsJournalieres->isChecked()
        );

    // 3. Boucle de simulation
    QVector<QPair<QString, int>> resultatsGraphique;
    QDate today = QDate::currentDate();

    QList<int> nouveauxDeparts;
    nouveauxDeparts.reserve(jours);

    progressSimulation->setValue(50);

    for(int i = 0; i < jours; ++i) {
        QDate dateSimulee = today.addDays(i + 1);

        // A. Libérer les quais dont la date de fin est atteinte
        int departs = 0;
        for (int j = 0; j < datesLiberation.size(); ++j) {
            if (datesLiberation[j] <= dateSimulee) {
                departs++;
                datesLiberation.removeAt(j);
                dureesSejour.removeAt(j);
                j--;
            }
        }
        occupationActuelle = std::max(0, occupationActuelle - departs);
        nouveauxDeparts.append(departs);

        // B. Ajouter les nouvelles arrivées
        int nouvellesArrivees = arriveesParJour[i];

        // Effet week-end
        if (checkWeekend->isChecked()) {
            int jourSemaine = dateSimulee.dayOfWeek();
            if (jourSemaine == 6 || jourSemaine == 7) { // Samedi ou dimanche
                nouvellesArrivees = static_cast<int>(nouvellesArrivees * 1.4);
            }
        }

        // Effet saisonnier
        if (checkEffetSaison->isChecked()) {
            int mois = dateSimulee.month();
            if (mois >= 6 && mois <= 8) { // Été
                nouvellesArrivees = static_cast<int>(nouvellesArrivees * 1.3);
            } else if (mois >= 11 || mois <= 2) { // Hiver
                nouvellesArrivees = static_cast<int>(nouvellesArrivees * 0.7);
            }
        }

        occupationActuelle += nouvellesArrivees;

        // C. Ajouter les dates de libération pour les nouveaux arrivants
        QRandomGenerator *rg = QRandomGenerator::global();
        for (int k = 0; k < nouvellesArrivees; ++k) {
            int dureeSejour = dureeSejourMoyenne + rg->bounded(-2, 4); // -2 à +3
            dureeSejour = std::max(1, dureeSejour);
            datesLiberation.append(dateSimulee.addDays(dureeSejour));
            dureesSejour.append(dureeSejour);
        }

        // D. Calculer le pourcentage d'occupation
        int pourcent = std::min(150, (occupationActuelle * 100) / capaciteTotale);

        resultatsGraphique.append(qMakePair("J" + QString::number(i + 1), pourcent));

        // Mettre à jour la progression
        if (i % 5 == 0) {
            progressSimulation->setValue(50 + (i * 40 / jours));
        }
    }

    progressSimulation->setValue(90);

    // 4. Analyse des résultats
    StatsSimulation stats = analyserResultats(resultatsGraphique);

    // 5. Mise à jour de l'interface
    histoWidget->setDonnees(resultatsGraphique);

    // Message principal
    QString messagePrincipal;
    if (stats.joursSaturation > 0) {
        messagePrincipal = QString("⚠️ <b style='color:red; font-size:14px'>ALERTE SATURATION !</b><br>"
                                   "Le port sera complet dès le <b>jour %1</b>.<br>"
                                   "Occupation maximale: <b>%2%%</b>")
                               .arg(stats.joursSaturation)
                               .arg(stats.maxOccupation);
    } else if (stats.maxOccupation > 85) {
        messagePrincipal = QString("🔶 <b style='color:orange; font-size:14px'>SITUATION TENDUE</b><br>"
                                   "Pic d'occupation à <b>%1%%</b> le jour %2.<br>"
                                   "%3 jour(s) critiques (>80%%)")
                               .arg(stats.maxOccupation)
                               .arg(stats.jourMaxOccupation)
                               .arg(stats.joursCritiques);
    } else {
        messagePrincipal = QString("✅ <b style='color:green; font-size:14px'>SITUATION STABLE</b><br>"
                                   "Occupation maximale: <b>%1%%</b><br>"
                                   "Moyenne: <b>%2%%</b> sur %3 jours")
                               .arg(stats.maxOccupation)
                               .arg(qRound(stats.moyenneOccupation))
                               .arg(jours);
    }

    lblResultatSimul->setText(messagePrincipal);

    // Statistiques détaillées
    QString statsText = QString("<b>📊 Statistiques détaillées :</b><br>"
                                "• Arrivées moyennes simulées: <b>%1 bateaux/jour</b><br>"
                                "• Durée moyenne de séjour: <b>%2 jours</b><br>"
                                "• Capacité totale: <b>%3 quais</b><br>"
                                "• Jours à forte occupation (>80%%): <b>%4</b><br>"
                                "• Occupation moyenne: <b>%5%%</b><br><br>")
                            .arg(moyenneArrivees, 0, 'f', 1)
                            .arg(dureeSejourMoyenne)
                            .arg(capaciteTotale)
                            .arg(stats.joursCritiques)
                            .arg(qRound(stats.moyenneOccupation));

    statsText += genererRecommandations(stats);

    lblStatsDetaillees->setText(statsText);

    progressSimulation->setValue(100);

    // Utiliser QTimer correctement
    QTimer::singleShot(1000, this, [this]() {
        progressSimulation->setVisible(false);
        progressSimulation->setValue(0);
    });
}

void MainWindow::genererScenarioAleatoire() {
    // Générer des paramètres aléatoires réalistes
    QRandomGenerator *rg = QRandomGenerator::global();

    spinJours->setValue(rg->bounded(14, 61)); // 2 à 8 semaines
    spinMoyenneArrivees->setValue(rg->bounded(10, 50) / 10.0); // 1.0 à 5.0
    spinEcartType->setValue(rg->bounded(5, 20) / 10.0); // 0.5 à 2.0
    spinDureeSejour->setValue(rg->bounded(3, 8)); // 3 à 7 jours

    checkVariationsJournalieres->setChecked(rg->bounded(0, 2) == 1);
    checkWeekend->setChecked(rg->bounded(0, 2) == 1);
    checkEffetSaison->setChecked(rg->bounded(0, 2) == 1);

    QMessageBox::information(this, "Scénario généré",
                             "Un scénario de simulation aléatoire a été généré.\n"
                             "Cliquez sur 'Lancer la simulation' pour voir les résultats.");
}

void MainWindow::showAjouter(){
    pages->setCurrentWidget(pageAjouter);
    statusBar()->showMessage("Mode Ajout - Saisie d'un nouveau quai");
}
void MainWindow::showModifier(){
    syncTables();
    pages->setCurrentWidget(pageModifier);
    statusBar()->showMessage("Mode Modification - Sélectionnez un quai à modifier");
}
void MainWindow::showSupprimer(){
    syncTables();
    pages->setCurrentWidget(pageSupprimer);
    statusBar()->showMessage("Mode Suppression - Sélectionnez un quai à supprimer");
}
void MainWindow::showConsulter(){
    syncTables();
    pages->setCurrentWidget(pageConsulter);
    statusBar()->showMessage("Consultation des quais - " + QString::number(tableConsulter->rowCount()) + " quais disponibles");
}
void MainWindow::showProposition(){
    pages->setCurrentWidget(pageProposition);
    statusBar()->showMessage("Recherche du meilleur quai pour votre bateau");
}
void MainWindow::showSimulation(){
    pages->setCurrentWidget(pageSimulation);
    statusBar()->showMessage("Simulation avancée - Configurez les paramètres");
}
void MainWindow::showExportPDF(){
    int totalQuais = tableConsulter->rowCount();
    int dispo = 0, occupe = 0;

    for(int r=0; r<totalQuais; ++r){
        if(tableConsulter->item(r,3)->text() == "Occupé") occupe++;
        else dispo++;
    }

    lblPreviewCSV->setText("Le fichier CSV contiendra :\n"
                           "• " + QString::number(totalQuais) + " quais au total\n"
                                                           "• " + QString::number(dispo) + " quais disponibles\n"
                                                      "• " + QString::number(occupe) + " quais occupés\n"
                                                       "• Rapport généré le " + QDate::currentDate().toString("dd/MM/yyyy") + "\n"
                                                                           "• Tableau complet du planning\n"
                                                                           "• Statistiques détaillées");
    pages->setCurrentWidget(pageExportCSV);
    statusBar()->showMessage("Export CSV - Prévisualisation du rapport");
}

void MainWindow::showStatistiques(){
    updateStats();
    pages->setCurrentWidget(pageStatistiques);

    int total = tableConsulter->rowCount();
    int dispo = 0, occupe = 0;

    for(int r=0; r<total; ++r){
        if(!tableConsulter->isRowHidden(r)){
            if(tableConsulter->item(r,3)->text() == "Occupé") occupe++;
            else dispo++;
        }
    }

    statusBar()->showMessage("Statistiques - " + QString::number(occupe) + " occupés / " + QString::number(dispo) + " disponibles");
}

void MainWindow::updateStats(){
    int dispo = 0; int occupe = 0;
    for(int r=0; r<tableConsulter->rowCount(); ++r){
        if(!tableConsulter->isRowHidden(r)){
            if(tableConsulter->item(r,3)->text() == "Occupé") occupe++;
            else dispo++;
        }
    }
    monGraphique->setValeurs(occupe, dispo);
}

QString MainWindow::genererContenuCSV() {
    QString csvContent;

    csvContent += "# Planning des Quais - FISHTECH\n";
    csvContent += "# Système de Gestion des Quais\n";
    csvContent += "# Date de génération: " + QDate::currentDate().toString("dd/MM/yyyy") + "\n";

    int dispo = 0, occupe = 0;
    int total = tableConsulter->rowCount();
    for(int r=0; r<total; ++r){
        if(tableConsulter->item(r,3)->text() == "Occupé") occupe++;
        else dispo++;
    }

    csvContent += "# Statistiques générales\n";
    csvContent += "# Total quais: " + QString::number(total) + "\n";
    csvContent += "# Disponibles: " + QString::number(dispo) + "\n";
    csvContent += "# Occupés: " + QString::number(occupe) + "\n";

    int tauxOccupation = total > 0 ? (occupe * 100) / total : 0;
    csvContent += "# Taux d'occupation: " + QString::number(tauxOccupation) + "%\n\n";

    QString separator = ",";
    if(formatCSVBox->currentText() == "CSV avec Sépareur Point-Virgule") {
        separator = ";";
    }

    QStringList headers = {"ID","Nom","Longueur (m)","État","Occupé du","Occupé au"};
    csvContent += headers.join(separator) + "\n";

    for(int r=0; r<tableConsulter->rowCount(); ++r){
        QStringList rowData;
        for(int c=0; c<6; ++c){
            QString cellValue = tableConsulter->item(r,c) ? tableConsulter->item(r,c)->text() : "";
            if(cellValue.contains(separator) || cellValue.contains("\"")) {
                cellValue = "\"" + cellValue.replace("\"", "\"\"") + "\"";
            }
            rowData.append(cellValue);
        }
        csvContent += rowData.join(separator) + "\n";
    }

    return csvContent;
}

void MainWindow::exporterCSV() {
    if(tableConsulter->rowCount() == 0){
        QMessageBox::warning(this, "Erreur", "Aucun quai à exporter !");
        return;
    }

    QString defaultName = QDir::homePath() + "/" + nomFichierEdit->text() + ".csv";
    QString fileName = QFileDialog::getSaveFileName(this, "Enregistrer le CSV",
                                                    defaultName,
                                                    "Fichiers CSV (*.csv);;Tous les fichiers (*)");

    if(fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Erreur", "Impossible de créer le fichier !");
        return;
    }

    QTextStream out(&file);
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    out.setEncoding(QStringConverter::Utf8);
#else
    out.setCodec("UTF-8");
#endif

    QString csvContent = genererContenuCSV();
    out << csvContent;

    file.close();

    QMessageBox::information(this, "Succès",
                             "CSV exporté avec succès !\n" + fileName);

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Ouvrir le CSV",
                                  "Voulez-vous ouvrir le fichier CSV généré ?",
                                  QMessageBox::Yes | QMessageBox::No);

    if(reply == QMessageBox::Yes){
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
    }
}

void MainWindow::onEtatChanged(const QString &etat){
    bool occupe = (etat == "Occupé");
    dateDebutEdit->setVisible(occupe);
    dateFinEdit->setVisible(occupe);
}
void MainWindow::onEtatChangedM(const QString &etat){
    bool occupe = (etat == "Occupé");
    dateDebutEditM->setVisible(occupe);
    dateFinEditM->setVisible(occupe);
}

void MainWindow::rechercherQuai(const QString &texte) {
    for (int r = 0; r < tableConsulter->rowCount(); ++r) {
        QString nom = tableConsulter->item(r, 1)->text().toLower();
        QString id = tableConsulter->item(r, 0)->text().toLower();
        QString searchText = texte.toLower();
        tableConsulter->setRowHidden(r, !(nom.contains(searchText) || id.contains(searchText)));
    }
    statusBar()->showMessage("Recherche: " + texte + " - " + QString::number(tableConsulter->rowCount()) + " quais trouvés");
}

void MainWindow::trierQuais(int index) {
    if (index == 0) return;
    int col = 1;
    Qt::SortOrder order = Qt::AscendingOrder;
    if (index == 1) { col = 1; order = Qt::AscendingOrder; } // Nom A-Z
    else if (index == 2) { col = 2; order = Qt::AscendingOrder; } // Longueur croissant
    else if (index == 3) { col = 2; order = Qt::DescendingOrder; } // Longueur décroissant
    else if (index == 4) { col = 3; order = Qt::AscendingOrder; } // Disponibilité
    else if (index == 5) { col = 3; order = Qt::DescendingOrder; } // État

    tableConsulter->sortItems(col, order);
    syncTables();
    statusBar()->showMessage("Table triée selon " + sortBox->currentText());
}

void MainWindow::trouverMeilleurQuai() {
    bool ok;
    int bateauLen = longueurBateauEdit->text().toInt(&ok);
    if(!ok || bateauLen <= 0) {
        resultatLabel->setText("❌ Veuillez entrer une longueur valide.");
        statusBar()->showMessage("Erreur: Longueur invalide");
        return;
    }
    int meilleurRow = -1;
    int minPerte = 999999;
    for(int r = 0; r < tableConsulter->rowCount(); ++r) {
        QString etat = tableConsulter->item(r, 3)->text();
        int quaiLen = tableConsulter->item(r, 2)->data(Qt::DisplayRole).toInt();
        if (etat == "Disponible" && quaiLen >= bateauLen) {
            int perte = quaiLen - bateauLen;
            if (perte < minPerte) {
                minPerte = perte;
                meilleurRow = r;
            }
        }
    }
    if (meilleurRow != -1) {
        QString nom = tableConsulter->item(meilleurRow, 1)->text();
        QString len = tableConsulter->item(meilleurRow, 2)->text();
        QString id = tableConsulter->item(meilleurRow, 0)->text();
        resultatLabel->setText("✅ <b>Quai Trouvé !</b><br>"
                               "Nom : <b>" + nom + "</b><br>"
                                       "ID : <b>" + id + "</b><br>"
                                      "Longueur : <b>" + len + "m</b><br>"
                                       "Espace perdu : <b>" + QString::number(minPerte) + "m</b>");
        statusBar()->showMessage("Quai idéal trouvé: " + nom + " (ID: " + id + ")");
    } else {
        resultatLabel->setText("❌ Aucun quai disponible pour cette taille de bateau (" + QString::number(bateauLen) + "m).");
        statusBar()->showMessage("Aucun quai disponible pour " + QString::number(bateauLen) + "m");
    }
}

void MainWindow::ajouterQuai(){
    if(idEdit->text().isEmpty() || nomEdit->text().isEmpty() || longueurEdit->text().isEmpty()){
        QMessageBox::warning(this,"Erreur","Veuillez remplir tous les champs obligatoires.");
        return;
    }

    // Vérifier si l'ID existe déjà
    for(int r = 0; r < tableConsulter->rowCount(); ++r) {
        if(tableConsulter->item(r, 0)->text() == idEdit->text()) {
            QMessageBox::warning(this,"Erreur","Cet ID existe déjà. Veuillez en choisir un autre.");
            return;
        }
    }

    int r = tableConsulter->rowCount();
    tableConsulter->insertRow(r);
    tableConsulter->setItem(r,0,new QTableWidgetItem(idEdit->text()));
    tableConsulter->setItem(r,1,new QTableWidgetItem(nomEdit->text()));
    QTableWidgetItem *longItem = new QTableWidgetItem();
    longItem->setData(Qt::DisplayRole, longueurEdit->text().toInt());
    tableConsulter->setItem(r,2,longItem);
    tableConsulter->setItem(r,3,new QTableWidgetItem(dispoBox->currentText()));

    if(dispoBox->currentText()=="Occupé"){
        tableConsulter->setItem(r,4,new QTableWidgetItem(dateDebutEdit->date().toString("yyyy-MM-dd")));
        tableConsulter->setItem(r,5,new QTableWidgetItem(dateFinEdit->date().toString("yyyy-MM-dd")));
    }else{
        tableConsulter->setItem(r,4,new QTableWidgetItem("-"));
        tableConsulter->setItem(r,5,new QTableWidgetItem("-"));
    }
    idEdit->clear(); nomEdit->clear(); longueurEdit->clear();
    dispoBox->setCurrentIndex(0);
    syncTables();

    QMessageBox::information(this, "Succès", "Quai ajouté avec succès !");
    statusBar()->showMessage("Quai ajouté - " + QString::number(tableConsulter->rowCount()) + " quais au total");
}

void MainWindow::supprimerQuai(){
    int r = tableSupprimer->currentRow();
    if(r >= 0){
        QString nomQuai = tableSupprimer->item(r, 1)->text();
        QString idQuai = tableSupprimer->item(r, 0)->text();

        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Confirmer la suppression",
                                      "Voulez-vous vraiment supprimer le quai :\n" +
                                          nomQuai + " (ID: " + idQuai + ")?",
                                      QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            tableConsulter->removeRow(r);
            syncTables();
            statusBar()->showMessage("Quai supprimé: " + nomQuai);
            QMessageBox::information(this, "Succès", "Quai supprimé avec succès !");
        }
    } else {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner un quai à supprimer.");
    }
}

void MainWindow::remplirFormulaireDepuisTable(int r,int){
    if(r < 0 || r >= tableModifier->rowCount()) return;

    idEditM->setText(tableModifier->item(r,0)->text());
    nomEditM->setText(tableModifier->item(r,1)->text());
    longueurEditM->setText(tableModifier->item(r,2)->text());
    dispoBoxM->setCurrentText(tableModifier->item(r,3)->text());
    QString dateD = tableModifier->item(r,4)->text();
    if(dateD != "-") dateDebutEditM->setDate(QDate::fromString(dateD,"yyyy-MM-dd"));
    QString dateF = tableModifier->item(r,5)->text();
    if(dateF != "-") dateFinEditM->setDate(QDate::fromString(dateF,"yyyy-MM-dd"));

    statusBar()->showMessage("Modification du quai: " + tableModifier->item(r,1)->text());
}

void MainWindow::modifierQuai(){
    int r = tableModifier->currentRow();
    if(r < 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner un quai à modifier.");
        return;
    }

    if(nomEditM->text().isEmpty() || longueurEditM->text().isEmpty()){
        QMessageBox::warning(this,"Erreur","Les champs Nom et Longueur sont obligatoires.");
        return;
    }

    QString ancienNom = tableConsulter->item(r,1)->text();
    QString nouveauNom = nomEditM->text();

    tableConsulter->item(r,1)->setText(nouveauNom);
    tableConsulter->item(r,2)->setData(Qt::DisplayRole, longueurEditM->text().toInt());
    tableConsulter->item(r,3)->setText(dispoBoxM->currentText());
    if(dispoBoxM->currentText()=="Occupé"){
        tableConsulter->setItem(r,4,new QTableWidgetItem(dateDebutEditM->date().toString("yyyy-MM-dd")));
        tableConsulter->setItem(r,5,new QTableWidgetItem(dateFinEditM->date().toString("yyyy-MM-dd")));
    }else{
        tableConsulter->setItem(r,4,new QTableWidgetItem("-"));
        tableConsulter->setItem(r,5,new QTableWidgetItem("-"));
    }
    syncTables();

    QMessageBox::information(this, "Succès", "Quai modifié avec succès !\n" + ancienNom + " → " + nouveauNom);
    statusBar()->showMessage("Quai modifié: " + nouveauNom);
}

void MainWindow::syncTables(){
    tableSupprimer->setRowCount(0);
    tableModifier->setRowCount(0);
    for(int r=0; r<tableConsulter->rowCount(); ++r){
        tableSupprimer->insertRow(r);
        tableModifier->insertRow(r);
        for(int c=0; c<6; ++c){
            QTableWidgetItem *item = tableConsulter->item(r,c);
            if(item){
                tableSupprimer->setItem(r,c,new QTableWidgetItem(*item));
                tableModifier->setItem(r,c,new QTableWidgetItem(*item));
            }
        }
    }
}
