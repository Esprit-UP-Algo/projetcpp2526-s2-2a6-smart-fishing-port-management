#include "gestionquai.h"

// ============ MonCamembert ============
MonCamembert::MonCamembert(QWidget *parent) : QWidget(parent) {
    valeurOccupe = 0; valeurDispo = 0;
    setMinimumSize(300, 300);
}

void MonCamembert::setValeurs(int occupe, int dispo) {
    valeurOccupe = occupe; valeurDispo = dispo; update();
}

void MonCamembert::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    int total = valeurOccupe + valeurDispo;
    if (total == 0) return;

    int angleOccupe = (valeurOccupe * 360 * 16) / total;
    int side = qMin(width(), height());
    QRectF rect((width() - side)/2.0 + 10, (height() - side)/2.0 + 10, side - 20, side - 20);

    painter.setBrush(QColor("#ef4444"));
    painter.setPen(Qt::NoPen);
    painter.drawPie(rect, 0, angleOccupe);
    painter.setBrush(QColor("#22c55e"));
    painter.drawPie(rect, angleOccupe, 360 * 16 - angleOccupe);

    painter.setPen(Qt::black);
    painter.setFont(QFont("Segoe UI", 10, QFont::Bold));
    painter.setBrush(QColor("#22c55e"));
    painter.drawRect(10, 10, 15, 15);
    painter.drawText(30, 23, "Disponible: " + QString::number(valeurDispo));
    painter.setBrush(QColor("#ef4444"));
    painter.drawRect(10, 35, 15, 15);
    painter.drawText(30, 48, "Occupé: " + QString::number(valeurOccupe));
}

// ============ MonHistogramme ============
MonHistogramme::MonHistogramme(QWidget *parent) : QWidget(parent) {
    setMinimumHeight(300);
    showGrid = true;
    showValues = true;
}

void MonHistogramme::setDonnees(const QVector<QPair<QString, int>> &data) {
    donnees = data;
    update();
}

void MonHistogramme::setOptions(bool grid, bool values) {
    showGrid = grid;
    showValues = values;
    update();
}

void MonHistogramme::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    if (donnees.isEmpty()) {
        p.drawText(rect(), Qt::AlignCenter, "Lancez la simulation...");
        return;
    }

    int margin = 50;
    int barWidth = (width() - 2 * margin) / donnees.size();
    int maxH = height() - 2 * margin;

    if (showGrid) {
        p.setPen(QPen(QColor("#e2e8f0"), 1));
        for (int i = 0; i <= 10; ++i) {
            int y = height() - margin - (i * maxH / 10);
            p.drawLine(margin, y, width() - margin, y);
            p.drawText(margin - 30, y + 5, QString::number(i * 10) + "%");
        }
    }

    p.setPen(QPen(Qt::black, 2));
    p.drawLine(margin, margin, margin, height() - margin);
    p.drawLine(margin, height() - margin, width() - margin, height() - margin);

    int maxValue = 0;
    for (const auto &data : donnees) if (data.second > maxValue) maxValue = data.second;

    for (int i = 0; i < donnees.size(); ++i) {
        int pourcent = donnees[i].second;
        int h = (pourcent * maxH) / 100;
        int x = margin + i * barWidth + 5;
        int y = height() - margin - h;

        QLinearGradient gradient(x, y, x, y + h);
        if (pourcent > 90) {
            gradient.setColorAt(0, QColor("#ef4444"));
            gradient.setColorAt(1, QColor("#b91c1c"));
        } else if (pourcent > 60) {
            gradient.setColorAt(0, QColor("#f59e0b"));
            gradient.setColorAt(1, QColor("#d97706"));
        } else {
            gradient.setColorAt(0, QColor("#3b82f6"));
            gradient.setColorAt(1, QColor("#1d4ed8"));
        }

        p.setBrush(gradient);
        p.setPen(QPen(QColor("#1e293b"), 1));
        p.drawRect(x, y, barWidth - 10, h);

        if (showValues) {
            p.setPen(Qt::black);
            p.setFont(QFont("Segoe UI", 9, QFont::Bold));
            p.drawText(QRect(x, y - 25, barWidth - 10, 20), Qt::AlignCenter, QString::number(pourcent) + "%");
        }

        p.setPen(Qt::darkGray);
        p.setFont(QFont("Segoe UI", 8));
        p.drawText(QRect(x - 5, height() - margin + 5, barWidth + 10, 20), Qt::AlignCenter, donnees[i].first);
    }

    p.save();
    p.translate(15, height() / 2);
    p.rotate(-90);
    p.setPen(Qt::black);
    p.setFont(QFont("Segoe UI", 10, QFont::Bold));
    p.drawText(0, 0, "Taux d'occupation (%)");
    p.restore();
}

// ============ GestionQuai ============
GestionQuai::GestionQuai(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Barre d'outils
    QHBoxLayout *toolbar = new QHBoxLayout;
    toolbar->setContentsMargins(10, 10, 10, 10);
    toolbar->setSpacing(10);

    btnAdd = new QPushButton("➕ Ajouter");
    btnEdit = new QPushButton("✏️ Modifier");
    btnDelete = new QPushButton("🗑️ Supprimer");
    btnView = new QPushButton("📋 Consulter");
    btnStats = new QPushButton("📊 Statistiques");
    btnProp = new QPushButton("💡 Proposition");
    btnSimul = new QPushButton("🔮 Simulation");
    btnCSV = new QPushButton("📥 Exporter CSV");

    QList<QPushButton*> toolButtons = {btnAdd, btnEdit, btnDelete, btnView, btnStats, btnProp, btnSimul, btnCSV};
    foreach(QPushButton *btn, toolButtons) {
        btn->setMinimumHeight(40);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet("background-color: #3b82f6; color: white; border-radius: 6px; padding: 8px 15px; font-weight: bold;");
        toolbar->addWidget(btn);
    }
    toolbar->addStretch();

    mainLayout->addLayout(toolbar);

    // Cadre principal
    QWidget *mainFrame = new QWidget;
    mainFrame->setObjectName("pageFrame");
    mainFrame->setStyleSheet("background: #1e3a8a; border-radius: 12px; margin: 10px;");

    pages = new QStackedWidget(mainFrame);
    pageAjouter = new QWidget;
    pageModifier = new QWidget;
    pageSupprimer = new QWidget;
    pageConsulter = new QWidget;
    pageStatistiques = new QWidget;
    pageProposition = new QWidget;
    pageSimulation = new QWidget;
    pageExportCSV = new QWidget;

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

    mainLayout->addWidget(mainFrame);

    // ============ TABLES ============
    tableConsulter = new QTableWidget(0,6);
    tableSupprimer = new QTableWidget(0,6);
    tableModifier = new QTableWidget(0,6);

    QStringList headers = {"ID","Nom","Longueur (m)","État","Occupé du","Occupé au"};
    tableConsulter->setHorizontalHeaderLabels(headers);
    tableSupprimer->setHorizontalHeaderLabels(headers);
    tableModifier->setHorizontalHeaderLabels(headers);

    tableConsulter->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableSupprimer->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableModifier->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // ============ PAGE AJOUTER ============
    QLabel *titreAjouter = new QLabel("➕ Ajouter un nouveau quai");
    titreAjouter->setObjectName("pageTitle");
    titreAjouter->setStyleSheet("color: white; font-size: 22px; font-weight: bold; padding: 15px 0;");

    QWidget *contentAjouter = new QWidget;
    contentAjouter->setObjectName("contentFrame");
    contentAjouter->setStyleSheet("background: white; border-radius: 8px; padding: 20px;");

    idEdit = new QLineEdit;
    idEdit->setPlaceholderText("Ex: Q001");
    nomEdit = new QLineEdit;
    nomEdit->setPlaceholderText("Ex: Quai central");
    longueurEdit = new QLineEdit;
    longueurEdit->setPlaceholderText("Ex: 150");
    dispoBox = new QComboBox;
    dispoBox->addItems({"Disponible","Occupé"});

    dateDebutEdit = new QDateEdit(QDate::currentDate());
    dateFinEdit = new QDateEdit(QDate::currentDate().addDays(7));
    dateDebutEdit->setCalendarPopup(true);
    dateFinEdit->setCalendarPopup(true);
    dateDebutEdit->hide();
    dateFinEdit->hide();

    QPushButton *btnSave = new QPushButton("💾 Enregistrer le quai");

    QFormLayout *formAdd = new QFormLayout;
    formAdd->addRow("ID :", idEdit);
    formAdd->addRow("Nom :", nomEdit);
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

    // ============ PAGE CONSULTER ============
    QLabel *titreConsulter = new QLabel("📋 Consultation des quais");
    titreConsulter->setObjectName("pageTitle");
    titreConsulter->setStyleSheet("color: white; font-size: 22px; font-weight: bold; padding: 15px 0;");

    QWidget *contentConsulter = new QWidget;
    contentConsulter->setObjectName("contentFrame");
    contentConsulter->setStyleSheet("background: white; border-radius: 8px; padding: 20px;");

    QVBoxLayout *viewLay = new QVBoxLayout(contentConsulter);

    QHBoxLayout *searchLayout = new QHBoxLayout();
    searchEdit = new QLineEdit();
    searchEdit->setPlaceholderText("🔍 Rechercher par nom ou ID...");
    sortBox = new QComboBox();
    sortBox->addItems({"Trier par...", "Nom (A-Z)", "Longueur (croissant)", "Longueur (décroissant)", "Disponibilité", "État"});

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

    // ============ PAGE STATISTIQUES ============
    QLabel *titreStats = new QLabel("📊 Statistiques des quais");
    titreStats->setObjectName("pageTitle");
    titreStats->setStyleSheet("color: white; font-size: 22px; font-weight: bold; padding: 15px 0;");

    QWidget *contentStats = new QWidget;
    contentStats->setObjectName("contentFrame");
    contentStats->setStyleSheet("background: white; border-radius: 8px; padding: 20px;");

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

    // ============ PAGE PROPOSITION ============
    QLabel *titreProp = new QLabel("💡 Trouver le quai idéal");
    titreProp->setObjectName("pageTitle");
    titreProp->setStyleSheet("color: white; font-size: 22px; font-weight: bold; padding: 15px 0;");

    QWidget *contentProp = new QWidget;
    contentProp->setObjectName("contentFrame");
    contentProp->setStyleSheet("background: white; border-radius: 8px; padding: 20px;");

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

    // ============ PAGE SIMULATION ============
    QLabel *titreSimul = new QLabel("🔮 Simulation avancée d'occupation");
    titreSimul->setObjectName("pageTitle");
    titreSimul->setStyleSheet("color: white; font-size: 22px; font-weight: bold; padding: 15px 0;");

    QWidget *contentSimul = new QWidget;
    contentSimul->setObjectName("contentFrame");
    contentSimul->setStyleSheet("background: white; border-radius: 8px; padding: 20px;");

    QVBoxLayout *simulLayout = new QVBoxLayout(contentSimul);

    spinJours = new QSpinBox;
    spinJours->setRange(7, 90);
    spinJours->setValue(30);
    spinJours->setSuffix(" jours");

    spinMoyenneArrivees = new QDoubleSpinBox;
    spinMoyenneArrivees->setRange(0.1, 20.0);
    spinMoyenneArrivees->setValue(2.5);
    spinMoyenneArrivees->setSuffix(" bateaux/jour");
    spinMoyenneArrivees->setDecimals(1);

    spinEcartType = new QDoubleSpinBox;
    spinEcartType->setRange(0.1, 10.0);
    spinEcartType->setValue(1.5);
    spinEcartType->setSuffix(" écart-type");
    spinEcartType->setDecimals(1);

    spinDureeSejour = new QSpinBox;
    spinDureeSejour->setRange(1, 30);
    spinDureeSejour->setValue(5);
    spinDureeSejour->setSuffix(" jours moyen");

    checkVariationsJournalieres = new QCheckBox("Variations journalières");
    checkVariationsJournalieres->setChecked(true);
    checkWeekend = new QCheckBox("Effet week-end");
    checkWeekend->setChecked(true);
    checkEffetSaison = new QCheckBox("Effet saisonnier");
    checkEffetSaison->setChecked(false);

    QHBoxLayout *controlsLayout1 = new QHBoxLayout;
    controlsLayout1->addWidget(new QLabel("Durée:"));
    controlsLayout1->addWidget(spinJours);
    controlsLayout1->addSpacing(15);
    controlsLayout1->addWidget(new QLabel("Arrivées moyennes:"));
    controlsLayout1->addWidget(spinMoyenneArrivees);
    controlsLayout1->addSpacing(15);
    controlsLayout1->addWidget(new QLabel("Variabilité:"));
    controlsLayout1->addWidget(spinEcartType);

    QHBoxLayout *controlsLayout2 = new QHBoxLayout;
    controlsLayout2->addWidget(new QLabel("Durée séjour:"));
    controlsLayout2->addWidget(spinDureeSejour);
    controlsLayout2->addSpacing(15);
    controlsLayout2->addWidget(checkVariationsJournalieres);
    controlsLayout2->addSpacing(15);
    controlsLayout2->addWidget(checkWeekend);
    controlsLayout2->addSpacing(15);
    controlsLayout2->addWidget(checkEffetSaison);

    QPushButton *btnLancerSimul = new QPushButton("🚀 Lancer la simulation");
    btnLancerSimul->setStyleSheet("background-color: #0891b2; color: white; font-size: 14px; padding: 12px;");

    btnScenarioAleatoire = new QPushButton("🎲 Générer scénario aléatoire");
    btnScenarioAleatoire->setStyleSheet("background-color: #8b5cf6; color: white; font-size: 14px; padding: 12px;");

    progressSimulation = new QProgressBar;
    progressSimulation->setVisible(false);
    progressSimulation->setRange(0, 100);

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addWidget(btnLancerSimul);
    buttonsLayout->addWidget(btnScenarioAleatoire);
    buttonsLayout->addWidget(progressSimulation);

    histoWidget = new MonHistogramme();
    histoWidget->setMinimumHeight(350);

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

    lblResultatSimul = new QLabel;
    lblResultatSimul->setObjectName("resultatSimulation");
    lblResultatSimul->setWordWrap(true);
    lblResultatSimul->setAlignment(Qt::AlignCenter);
    lblResultatSimul->setMinimumHeight(80);

    lblStatsDetaillees = new QLabel;
    lblStatsDetaillees->setWordWrap(true);
    lblStatsDetaillees->setAlignment(Qt::AlignLeft);
    lblStatsDetaillees->setStyleSheet("background: #f8fafc; border: 1px solid #cbd5e1; border-radius: 8px; padding: 15px;");

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

    // ============ PAGE EXPORT CSV ============
    QLabel *titreCSV = new QLabel("📊 Exporter le planning en CSV");
    titreCSV->setObjectName("pageTitle");
    titreCSV->setStyleSheet("color: white; font-size: 22px; font-weight: bold; padding: 15px 0;");

    QWidget *contentCSV = new QWidget;
    contentCSV->setObjectName("contentFrame");
    contentCSV->setStyleSheet("background: white; border-radius: 8px; padding: 20px;");

    QVBoxLayout *csvLayout = new QVBoxLayout(contentCSV);

    QFormLayout *formCSV = new QFormLayout;
    nomFichierEdit = new QLineEdit;
    nomFichierEdit->setText("planning_quais_" + QDate::currentDate().toString("yyyy-MM-dd"));
    nomFichierEdit->setPlaceholderText("Nom du fichier CSV");

    formatCSVBox = new QComboBox;
    formatCSVBox->addItems({"CSV Standard", "CSV Excel", "CSV avec séparateur point-virgule"});

    formCSV->addRow("Nom du fichier :", nomFichierEdit);
    formCSV->addRow("Format :", formatCSVBox);

    btnGenererCSV = new QPushButton("💾 Générer et exporter le CSV");
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

    // ============ PAGE SUPPRIMER ============
    QLabel *titreSupprimer = new QLabel("🗑️ Supprimer un quai");
    titreSupprimer->setObjectName("pageTitle");
    titreSupprimer->setStyleSheet("color: white; font-size: 22px; font-weight: bold; padding: 15px 0;");

    QWidget *contentSupprimer = new QWidget;
    contentSupprimer->setObjectName("contentFrame");
    contentSupprimer->setStyleSheet("background: white; border-radius: 8px; padding: 20px;");

    QVBoxLayout *supLay = new QVBoxLayout(contentSupprimer);
    QPushButton *btnDelRow = new QPushButton("🗑️ Supprimer la sélection");
    btnDelRow->setStyleSheet("background-color: #ef4444; color: white; font-size: 14px; padding: 12px;");
    supLay->addWidget(tableSupprimer);
    supLay->addWidget(btnDelRow, 0, Qt::AlignCenter);

    QVBoxLayout *supprimerMainLayout = new QVBoxLayout(pageSupprimer);
    supprimerMainLayout->addWidget(titreSupprimer);
    supprimerMainLayout->addWidget(contentSupprimer);
    supprimerMainLayout->setContentsMargins(0, 0, 0, 0);

    // ============ PAGE MODIFIER ============
    QLabel *titreModifier = new QLabel("✏️ Modifier un quai");
    titreModifier->setObjectName("pageTitle");
    titreModifier->setStyleSheet("color: white; font-size: 22px; font-weight: bold; padding: 15px 0;");

    QWidget *contentModifier = new QWidget;
    contentModifier->setObjectName("contentFrame");
    contentModifier->setStyleSheet("background: white; border-radius: 8px; padding: 20px;");

    QHBoxLayout *modLayout = new QHBoxLayout(contentModifier);
    modLayout->addWidget(tableModifier, 2);

    QWidget *formWidget = new QWidget;
    formWidget->setStyleSheet("background: white; border-radius: 8px; padding: 20px; border: 1px solid #cbd5e1;");

    idEditM = new QLineEdit;
    idEditM->setReadOnly(true);
    nomEditM = new QLineEdit;
    longueurEditM = new QLineEdit;
    dispoBoxM = new QComboBox;
    dispoBoxM->addItems({"Disponible","Occupé"});

    dateDebutEditM = new QDateEdit(QDate::currentDate());
    dateFinEditM = new QDateEdit(QDate::currentDate().addDays(7));
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

    // ============ CONNECTIONS ============
    connect(btnAdd, &QPushButton::clicked, this, &GestionQuai::showAjouter);
    connect(btnEdit, &QPushButton::clicked, this, &GestionQuai::showModifier);
    connect(btnDelete, &QPushButton::clicked, this, &GestionQuai::showSupprimer);
    connect(btnView, &QPushButton::clicked, this, &GestionQuai::showConsulter);
    connect(btnStats, &QPushButton::clicked, this, &GestionQuai::showStatistiques);
    connect(btnProp, &QPushButton::clicked, this, &GestionQuai::showProposition);
    connect(btnSimul, &QPushButton::clicked, this, &GestionQuai::showSimulation);
    connect(btnCSV, &QPushButton::clicked, this, &GestionQuai::showExportCSV);

    connect(btnSave, &QPushButton::clicked, this, &GestionQuai::ajouterQuai);
    connect(btnDelRow, &QPushButton::clicked, this, &GestionQuai::supprimerQuai);
    connect(tableModifier, &QTableWidget::cellClicked, this, &GestionQuai::remplirFormulaireDepuisTable);
    connect(btnUpdate, &QPushButton::clicked, this, &GestionQuai::modifierQuai);
    connect(btnCalculer, &QPushButton::clicked, this, &GestionQuai::trouverMeilleurQuai);
    connect(btnLancerSimul, &QPushButton::clicked, this, &GestionQuai::lancerSimulation);
    connect(btnGenererCSV, &QPushButton::clicked, this, &GestionQuai::exporterCSV);
    connect(btnScenarioAleatoire, &QPushButton::clicked, this, &GestionQuai::genererScenarioAleatoire);

    connect(dispoBox, &QComboBox::currentTextChanged, this, &GestionQuai::onEtatChanged);
    connect(dispoBoxM, &QComboBox::currentTextChanged, this, &GestionQuai::onEtatChangedM);
    connect(searchEdit, &QLineEdit::textChanged, this, &GestionQuai::rechercherQuai);
    connect(sortBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &GestionQuai::trierQuais);

    pages->setCurrentWidget(pageConsulter);
}

// ============ MÉTHODES DE NAVIGATION ============
void GestionQuai::showAjouter() { pages->setCurrentWidget(pageAjouter); }
void GestionQuai::showModifier() { syncTables(); pages->setCurrentWidget(pageModifier); }
void GestionQuai::showSupprimer() { syncTables(); pages->setCurrentWidget(pageSupprimer); }
void GestionQuai::showConsulter() { syncTables(); pages->setCurrentWidget(pageConsulter); }
void GestionQuai::showProposition() { pages->setCurrentWidget(pageProposition); }
void GestionQuai::showSimulation() { pages->setCurrentWidget(pageSimulation); }
void GestionQuai::showStatistiques() { updateStats(); pages->setCurrentWidget(pageStatistiques); }

void GestionQuai::showExportCSV() {
    int totalQuais = tableConsulter->rowCount();
    int dispo = 0, occupe = 0;
    for(int r=0; r<totalQuais; ++r) {
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
}

// ============ MÉTHODES DE GESTION ============
void GestionQuai::updateStats() {
    int dispo = 0, occupe = 0;
    for(int r=0; r<tableConsulter->rowCount(); ++r) {
        if(tableConsulter->item(r,3)->text() == "Occupé") occupe++;
        else dispo++;
    }
    monGraphique->setValeurs(occupe, dispo);
}

QString GestionQuai::genererContenuCSV() {
    QString csvContent;
    csvContent += "# Planning des quais - FISHTECH\n";
    csvContent += "# Système de gestion des quais\n";
    csvContent += "# Date de génération: " + QDate::currentDate().toString("dd/MM/yyyy") + "\n";

    int dispo = 0, occupe = 0, total = tableConsulter->rowCount();
    for(int r=0; r<total; ++r) {
        if(tableConsulter->item(r,3)->text() == "Occupé") occupe++;
        else dispo++;
    }

    csvContent += "# Statistiques générales\n";
    csvContent += "# Total quais: " + QString::number(total) + "\n";
    csvContent += "# Disponibles: " + QString::number(dispo) + "\n";
    csvContent += "# Occupés: " + QString::number(occupe) + "\n";
    csvContent += "# Taux d'occupation: " + QString::number(total > 0 ? (occupe * 100) / total : 0) + "%\n\n";

    QString separator = ",";
    if(formatCSVBox->currentText() == "CSV avec séparateur point-virgule") separator = ";";

    QStringList headers = {"ID","Nom","Longueur (m)","État","Occupé du","Occupé au"};
    csvContent += headers.join(separator) + "\n";

    for(int r=0; r<total; ++r) {
        QStringList rowData;
        for(int c=0; c<6; ++c) {
            QString cellValue = tableConsulter->item(r,c) ? tableConsulter->item(r,c)->text() : "";
            if(cellValue.contains(separator) || cellValue.contains("\""))
                cellValue = "\"" + cellValue.replace("\"", "\"\"") + "\"";
            rowData.append(cellValue);
        }
        csvContent += rowData.join(separator) + "\n";
    }
    return csvContent;
}

void GestionQuai::exporterCSV() {
    if(tableConsulter->rowCount() == 0) {
        QMessageBox::warning(this, "Erreur", "Aucun quai à exporter !");
        return;
    }

    QString defaultName = QDir::homePath() + "/" + nomFichierEdit->text() + ".csv";
    QString fileName = QFileDialog::getSaveFileName(this, "Enregistrer le CSV", defaultName, "Fichiers CSV (*.csv);;Tous les fichiers (*)");
    if(fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Erreur", "Impossible de créer le fichier !");
        return;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << genererContenuCSV();
    file.close();

    QMessageBox::information(this, "Succès", "CSV exporté avec succès !\n" + fileName);
    if(QMessageBox::question(this, "Ouvrir le CSV", "Voulez-vous ouvrir le fichier CSV généré ?", QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
}

void GestionQuai::onEtatChanged(const QString &etat) {
    bool occupe = (etat == "Occupé");
    dateDebutEdit->setVisible(occupe);
    dateFinEdit->setVisible(occupe);
}

void GestionQuai::onEtatChangedM(const QString &etat) {
    bool occupe = (etat == "Occupé");
    dateDebutEditM->setVisible(occupe);
    dateFinEditM->setVisible(occupe);
}

void GestionQuai::rechercherQuai(const QString &texte) {
    for (int r = 0; r < tableConsulter->rowCount(); ++r) {
        QString nom = tableConsulter->item(r, 1)->text().toLower();
        QString id = tableConsulter->item(r, 0)->text().toLower();
        QString searchText = texte.toLower();
        tableConsulter->setRowHidden(r, !(nom.contains(searchText) || id.contains(searchText)));
    }
}

void GestionQuai::trierQuais(int index) {
    if (index == 0) return;
    int col = 1;
    Qt::SortOrder order = Qt::AscendingOrder;
    if (index == 1) { col = 1; order = Qt::AscendingOrder; }
    else if (index == 2) { col = 2; order = Qt::AscendingOrder; }
    else if (index == 3) { col = 2; order = Qt::DescendingOrder; }
    else if (index == 4) { col = 3; order = Qt::AscendingOrder; }
    else if (index == 5) { col = 3; order = Qt::DescendingOrder; }

    tableConsulter->sortItems(col, order);
    syncTables();
}

void GestionQuai::trouverMeilleurQuai() {
    bool ok;
    int bateauLen = longueurBateauEdit->text().toInt(&ok);
    if(!ok || bateauLen <= 0) {
        resultatLabel->setText("❌ Veuillez entrer une longueur valide.");
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
        resultatLabel->setText("✅ <b>Quai trouvé !</b><br>"
                               "Nom : <b>" + nom + "</b><br>"
                                       "ID : <b>" + id + "</b><br>"
                                      "Longueur : <b>" + len + " m</b><br>"
                                       "Espace perdu : <b>" + QString::number(minPerte) + " m</b>");
    } else {
        resultatLabel->setText("❌ Aucun quai disponible pour cette taille de bateau (" + QString::number(bateauLen) + " m).");
    }
}

void GestionQuai::ajouterQuai() {
    if(idEdit->text().isEmpty() || nomEdit->text().isEmpty() || longueurEdit->text().isEmpty()) {
        QMessageBox::warning(this,"Erreur","Veuillez remplir tous les champs obligatoires.");
        return;
    }

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

    if(dispoBox->currentText()=="Occupé") {
        tableConsulter->setItem(r,4,new QTableWidgetItem(dateDebutEdit->date().toString("yyyy-MM-dd")));
        tableConsulter->setItem(r,5,new QTableWidgetItem(dateFinEdit->date().toString("yyyy-MM-dd")));
    } else {
        tableConsulter->setItem(r,4,new QTableWidgetItem("-"));
        tableConsulter->setItem(r,5,new QTableWidgetItem("-"));
    }
    idEdit->clear(); nomEdit->clear(); longueurEdit->clear();
    dispoBox->setCurrentIndex(0);
    syncTables();

    QMessageBox::information(this, "Succès", "Quai ajouté avec succès !");
}

void GestionQuai::supprimerQuai() {
    int r = tableSupprimer->currentRow();
    if(r >= 0) {
        QString nomQuai = tableSupprimer->item(r, 1)->text();
        QString idQuai = tableSupprimer->item(r, 0)->text();

        if(QMessageBox::question(this, "Confirmer la suppression",
                                  "Voulez-vous vraiment supprimer le quai :\n" + nomQuai + " (ID: " + idQuai + ")?",
                                  QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            tableConsulter->removeRow(r);
            syncTables();
            QMessageBox::information(this, "Succès", "Quai supprimé avec succès !");
        }
    } else {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner un quai à supprimer.");
    }
}

void GestionQuai::remplirFormulaireDepuisTable(int r, int) {
    if(r < 0 || r >= tableModifier->rowCount()) return;

    idEditM->setText(tableModifier->item(r,0)->text());
    nomEditM->setText(tableModifier->item(r,1)->text());
    longueurEditM->setText(tableModifier->item(r,2)->text());
    dispoBoxM->setCurrentText(tableModifier->item(r,3)->text());
    QString dateD = tableModifier->item(r,4)->text();
    if(dateD != "-") dateDebutEditM->setDate(QDate::fromString(dateD,"yyyy-MM-dd"));
    QString dateF = tableModifier->item(r,5)->text();
    if(dateF != "-") dateFinEditM->setDate(QDate::fromString(dateF,"yyyy-MM-dd"));
}

void GestionQuai::modifierQuai() {
    int r = tableModifier->currentRow();
    if(r < 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner un quai à modifier.");
        return;
    }

    if(nomEditM->text().isEmpty() || longueurEditM->text().isEmpty()) {
        QMessageBox::warning(this,"Erreur","Les champs Nom et Longueur sont obligatoires.");
        return;
    }

    QString ancienNom = tableConsulter->item(r,1)->text();
    QString nouveauNom = nomEditM->text();

    tableConsulter->item(r,1)->setText(nouveauNom);
    tableConsulter->item(r,2)->setData(Qt::DisplayRole, longueurEditM->text().toInt());
    tableConsulter->item(r,3)->setText(dispoBoxM->currentText());
    if(dispoBoxM->currentText()=="Occupé") {
        tableConsulter->setItem(r,4,new QTableWidgetItem(dateDebutEditM->date().toString("yyyy-MM-dd")));
        tableConsulter->setItem(r,5,new QTableWidgetItem(dateFinEditM->date().toString("yyyy-MM-dd")));
    } else {
        tableConsulter->setItem(r,4,new QTableWidgetItem("-"));
        tableConsulter->setItem(r,5,new QTableWidgetItem("-"));
    }
    syncTables();

    QMessageBox::information(this, "Succès", "Quai modifié avec succès !\n" + ancienNom + " → " + nouveauNom);
}

void GestionQuai::syncTables() {
    tableSupprimer->setRowCount(0);
    tableModifier->setRowCount(0);
    for(int r=0; r<tableConsulter->rowCount(); ++r) {
        tableSupprimer->insertRow(r);
        tableModifier->insertRow(r);
        for(int c=0; c<6; ++c) {
            QTableWidgetItem *item = tableConsulter->item(r,c);
            if(item) {
                tableSupprimer->setItem(r,c,new QTableWidgetItem(*item));
                tableModifier->setItem(r,c,new QTableWidgetItem(*item));
            }
        }
    }
}

// ============ MÉTHODES DE SIMULATION ============
QVector<int> GestionQuai::simulerArriveesBateaux(int jours, double moyenne, double ecartType, bool variationsJournalieres) {
    QVector<int> arrivees;
    arrivees.reserve(jours);
    QRandomGenerator *rg = QRandomGenerator::global();

    for (int jour = 0; jour < jours; ++jour) {
        double u1 = 0.0, u2 = 0.0;
        do {
            u1 = rg->generateDouble();
            u2 = rg->generateDouble();
        } while (u1 <= std::numeric_limits<double>::epsilon());

        double z = std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * M_PI * u2);
        int arriveesSimulees = std::max(0, static_cast<int>(moyenne + ecartType * z + 0.5));

        if (variationsJournalieres) {
            if (jour % 7 >= 4) arriveesSimulees = static_cast<int>(arriveesSimulees * 1.3);
            arriveesSimulees += rg->bounded(-2, 4);
        }
        arriveesSimulees = std::max(0, arriveesSimulees);
        arrivees.append(arriveesSimulees);
    }
    return arrivees;
}

StatsSimulation GestionQuai::analyserResultats(const QVector<QPair<QString, int>>& resultats) {
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

        if (occupation >= 100 && stats.joursSaturation == -1) stats.joursSaturation = i + 1;
        if (occupation > 80) stats.joursCritiques++;

        if (i >= 4) {
            int somme = 0;
            for (int j = i - 4; j <= i; ++j) somme += resultats[j].second;
            stats.tendance.append(somme / 5);
        }
    }

    if (!resultats.isEmpty()) stats.moyenneOccupation = static_cast<double>(total) / resultats.size();
    return stats;
}

QString GestionQuai::genererRecommandations(const StatsSimulation& stats) {
    QString recommandations = "<b>📋 Recommandations :</b><br>";

    if (stats.joursSaturation > 0) {
        recommandations += QString("• ⚠️ <b>Saturation prévue le jour %1</b><br>").arg(stats.joursSaturation);
        recommandations += "• Considérez une augmentation temporaire de capacité<br>";
    }
    if (stats.maxOccupation > 90) {
        recommandations += QString("• 📈 Pic d'occupation à %1%<br>").arg(stats.maxOccupation);
        recommandations += "• Planifiez les départs pour éviter les conflits<br>";
    }
    if (stats.joursCritiques > 0) {
        recommandations += QString("• 🔥 %1 jour(s) à plus de 80% d'occupation<br>").arg(stats.joursCritiques);
        recommandations += "• Activez le protocole haute fréquentation<br>";
    }
    if (stats.moyenneOccupation > 70) {
        recommandations += QString("• 📊 Occupation moyenne: %1%<br>").arg(qRound(stats.moyenneOccupation));
        recommandations += "• Performance optimale, surveillez la tendance<br>";
    } else if (stats.moyenneOccupation < 40) {
        recommandations += QString("• 📉 Occupation moyenne: %1%<br>").arg(qRound(stats.moyenneOccupation));
        recommandations += "• Capacité sous-utilisée, envisagez des promotions<br>";
    } else {
        recommandations += QString("• ✅ Occupation moyenne: %1%<br>").arg(qRound(stats.moyenneOccupation));
        recommandations += "• Niveau d'occupation équilibré<br>";
    }

    if (!stats.tendance.isEmpty()) {
        if (stats.tendance.last() > stats.tendance.first() + 10)
            recommandations += "• ↗️ <b>Tendance à la hausse</b> - Préparez-vous à l'affluence<br>";
        else if (stats.tendance.last() < stats.tendance.first() - 10)
            recommandations += "• ↘️ <b>Tendance à la baisse</b> - Évaluez les causes<br>";
        else
            recommandations += "• ➡️ <b>Tendance stable</b> - Situation contrôlée<br>";
    }
    return recommandations;
}

void GestionQuai::lancerSimulation() {
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

    progressSimulation->setVisible(true);
    progressSimulation->setValue(10);

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
            int joursRestants = QDate::currentDate().daysTo(dateFin);
            dureesSejour.append(std::max(0, joursRestants));
        }
    }

    progressSimulation->setValue(30);

    QVector<int> arriveesParJour = simulerArriveesBateaux(jours, moyenneArrivees, ecartType, checkVariationsJournalieres->isChecked());

    QVector<QPair<QString, int>> resultatsGraphique;
    QDate today = QDate::currentDate();
    progressSimulation->setValue(50);

    for(int i = 0; i < jours; ++i) {
        QDate dateSimulee = today.addDays(i + 1);

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

        int nouvellesArrivees = arriveesParJour[i];

        if (checkWeekend->isChecked()) {
            int jourSemaine = dateSimulee.dayOfWeek();
            if (jourSemaine == 6 || jourSemaine == 7)
                nouvellesArrivees = static_cast<int>(nouvellesArrivees * 1.4);
        }

        if (checkEffetSaison->isChecked()) {
            int mois = dateSimulee.month();
            if (mois >= 6 && mois <= 8)
                nouvellesArrivees = static_cast<int>(nouvellesArrivees * 1.3);
            else if (mois >= 11 || mois <= 2)
                nouvellesArrivees = static_cast<int>(nouvellesArrivees * 0.7);
        }

        occupationActuelle += nouvellesArrivees;

        QRandomGenerator *rg = QRandomGenerator::global();
        for (int k = 0; k < nouvellesArrivees; ++k) {
            int dureeSejour = dureeSejourMoyenne + rg->bounded(-2, 4);
            dureeSejour = std::max(1, dureeSejour);
            datesLiberation.append(dateSimulee.addDays(dureeSejour));
            dureesSejour.append(dureeSejour);
        }

        int pourcent = std::min(150, (occupationActuelle * 100) / capaciteTotale);
        resultatsGraphique.append(qMakePair("J" + QString::number(i + 1), pourcent));

        if (i % 5 == 0) progressSimulation->setValue(50 + (i * 40 / jours));
    }

    progressSimulation->setValue(90);

    StatsSimulation stats = analyserResultats(resultatsGraphique);
    histoWidget->setDonnees(resultatsGraphique);

    QString messagePrincipal;
    if (stats.joursSaturation > 0) {
        messagePrincipal = QString("⚠️ <b style='color:red; font-size:14px'>ALERTE SATURATION !</b><br>"
                                   "Le port sera complet dès le <b>jour %1</b>.<br>"
                                   "Occupation maximale: <b>%2%</b>").arg(stats.joursSaturation).arg(stats.maxOccupation);
    } else if (stats.maxOccupation > 85) {
        messagePrincipal = QString("🔶 <b style='color:orange; font-size:14px'>SITUATION TENDUE</b><br>"
                                   "Pic d'occupation à <b>%1%</b> le jour %2.<br>"
                                   "%3 jour(s) critiques (>80%)").arg(stats.maxOccupation).arg(stats.jourMaxOccupation).arg(stats.joursCritiques);
    } else {
        messagePrincipal = QString("✅ <b style='color:green; font-size:14px'>SITUATION STABLE</b><br>"
                                   "Occupation maximale: <b>%1%</b><br>"
                                   "Moyenne: <b>%2%</b> sur %3 jours").arg(stats.maxOccupation).arg(qRound(stats.moyenneOccupation)).arg(jours);
    }

    lblResultatSimul->setText(messagePrincipal);

    QString statsText = QString("<b>📊 Statistiques détaillées :</b><br>"
                                "• Arrivées moyennes simulées: <b>%1 bateaux/jour</b><br>"
                                "• Durée moyenne de séjour: <b>%2 jours</b><br>"
                                "• Capacité totale: <b>%3 quais</b><br>"
                                "• Jours à forte occupation (>80%%) : <b>%4</b><br>"
                                "• Occupation moyenne: <b>%5%</b><br><br>")
                            .arg(moyenneArrivees, 0, 'f', 1)
                            .arg(dureeSejourMoyenne)
                            .arg(capaciteTotale)
                            .arg(stats.joursCritiques)
                            .arg(qRound(stats.moyenneOccupation));

    statsText += genererRecommandations(stats);
    lblStatsDetaillees->setText(statsText);

    progressSimulation->setValue(100);
    QTimer::singleShot(1000, this, [this]() {
        progressSimulation->setVisible(false);
        progressSimulation->setValue(0);
    });
}

void GestionQuai::genererScenarioAleatoire() {
    QRandomGenerator *rg = QRandomGenerator::global();
    spinJours->setValue(rg->bounded(14, 61));
    spinMoyenneArrivees->setValue(rg->bounded(10, 50) / 10.0);
    spinEcartType->setValue(rg->bounded(5, 20) / 10.0);
    spinDureeSejour->setValue(rg->bounded(3, 8));
    checkVariationsJournalieres->setChecked(rg->bounded(0, 2) == 1);
    checkWeekend->setChecked(rg->bounded(0, 2) == 1);
    checkEffetSaison->setChecked(rg->bounded(0, 2) == 1);
    QMessageBox::information(this, "Scénario généré", "Un scénario de simulation aléatoire a été généré.");
}
