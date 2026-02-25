#include "gestionquai.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QHeaderView>
#include <QDateTime>
#include <QDebug>

// ============ MonCamembert ============
MonCamembert::MonCamembert(QWidget *parent) : QWidget(parent) {
    valeurOccupe = 0; valeurDispo = 0;
    setMinimumSize(180, 180);
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
    painter.setFont(QFont("Segoe UI", 9, QFont::Bold));
    painter.setBrush(QColor("#22c55e"));
    painter.drawRect(10, 10, 12, 12);
    painter.drawText(28, 21, "Disponible: " + QString::number(valeurDispo));
    painter.setBrush(QColor("#ef4444"));
    painter.drawRect(10, 30, 12, 12);
    painter.drawText(28, 41, "Occupé: " + QString::number(valeurOccupe));
}

// ============ MonHistogramme ============
MonHistogramme::MonHistogramme(QWidget *parent) : QWidget(parent) {
    setMinimumHeight(180);
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

    int margin = 40;
    int barWidth = (width() - 2 * margin) / donnees.size();
    int maxH = height() - 2 * margin;

    if (showGrid) {
        p.setPen(QPen(QColor("#e2e8f0"), 1));
        for (int i = 0; i <= 10; ++i) {
            int y = height() - margin - (i * maxH / 10);
            p.drawLine(margin, y, width() - margin, y);
            p.drawText(margin - 25, y + 4, QString::number(i * 10) + "%");
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
            p.setFont(QFont("Segoe UI", 8, QFont::Bold));
            p.drawText(QRect(x, y - 22, barWidth - 10, 18), Qt::AlignCenter, QString::number(pourcent) + "%");
        }

        p.setPen(Qt::darkGray);
        p.setFont(QFont("Segoe UI", 7));
        p.drawText(QRect(x - 5, height() - margin + 3, barWidth + 10, 16), Qt::AlignCenter, donnees[i].first);
    }

    p.save();
    p.translate(12, height() / 2);
    p.rotate(-90);
    p.setPen(Qt::black);
    p.setFont(QFont("Segoe UI", 9, QFont::Bold));
    p.drawText(0, 0, "Taux d'occupation (%)");
    p.restore();
}

// ============ GestionQuai ============
GestionQuai::GestionQuai(QWidget *parent) : QWidget(parent)
{
    configurerInterface();
    appliquerStyles();
    // Initialisation avec quelques quais par défaut (optionnel)
    // Exemple :
    // ajouterQuai(...) mais on peut laisser vide
}

void GestionQuai::configurerInterface()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Barre d'outils
    QHBoxLayout *toolbar = new QHBoxLayout;
    toolbar->setContentsMargins(8, 8, 8, 8);
    toolbar->setSpacing(8);

    btnAjouter = new QPushButton("➕ Ajouter");
    btnModifier = new QPushButton("✏️ Modifier");
    btnSupprimer = new QPushButton("🗑️ Supprimer");
    btnConsulter = new QPushButton("📋 Consulter");
    btnStatistiques = new QPushButton("📊 Statistiques");
    btnProposition = new QPushButton("💡 Proposition");
    btnSimulation = new QPushButton("🔮 Simulation");
    btnExportCSV = new QPushButton("📥 Exporter CSV");

    QList<QPushButton*> toolButtons = {btnAjouter, btnModifier, btnSupprimer, btnConsulter,
                                        btnStatistiques, btnProposition, btnSimulation, btnExportCSV};
    foreach(QPushButton *btn, toolButtons) {
        btn->setMinimumHeight(35);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet("background-color: #3b82f6; color: white; border-radius: 5px; padding: 6px 12px; font-weight: bold;");
        toolbar->addWidget(btn);
    }
    toolbar->addStretch();

    mainLayout->addLayout(toolbar);

    // Cadre principal
    QWidget *mainFrame = new QWidget;
    mainFrame->setObjectName("pageFrame");
    mainFrame->setStyleSheet("background: #1e3a8a; border-radius: 8px; margin: 8px;");

    pages = new QStackedWidget(mainFrame);
    configurerPages();

    QVBoxLayout *frameLayout = new QVBoxLayout(mainFrame);
    frameLayout->addWidget(pages);
    frameLayout->setContentsMargins(15, 15, 15, 15);

    mainLayout->addWidget(mainFrame);

    // Connexions
    connect(btnAjouter, &QPushButton::clicked, this, &GestionQuai::showAjouter);
    connect(btnModifier, &QPushButton::clicked, this, &GestionQuai::showModifier);
    connect(btnSupprimer, &QPushButton::clicked, this, &GestionQuai::showSupprimer);
    connect(btnConsulter, &QPushButton::clicked, this, &GestionQuai::showConsulter);
    connect(btnStatistiques, &QPushButton::clicked, this, &GestionQuai::showStatistiques);
    connect(btnProposition, &QPushButton::clicked, this, &GestionQuai::showProposition);
    connect(btnSimulation, &QPushButton::clicked, this, &GestionQuai::showSimulation);
    connect(btnExportCSV, &QPushButton::clicked, this, &GestionQuai::showExportCSV);

    pages->setCurrentWidget(pageConsulter);
}

void GestionQuai::configurerPages()
{
    pageAjouter = new QWidget;
    pageModifier = new QWidget;
    pageSupprimer = new QWidget;
    pageConsulter = new QWidget;
    pageStatistiques = new QWidget;
    pageProposition = new QWidget;
    pageSimulation = new QWidget;
    pageExportCSV = new QWidget;

    configurerAjouter();
    configurerConsulter();
    configurerStatistiques();
    configurerProposition();
    configurerSimulation();
    configurerExportCSV();
    configurerSupprimer();
    configurerModifier();

    pages->addWidget(pageAjouter);
    pages->addWidget(pageModifier);
    pages->addWidget(pageSupprimer);
    pages->addWidget(pageConsulter);
    pages->addWidget(pageStatistiques);
    pages->addWidget(pageProposition);
    pages->addWidget(pageSimulation);
    pages->addWidget(pageExportCSV);
}

void GestionQuai::appliquerStyles()
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
    )");
}

void GestionQuai::configurerAjouter()
{
    QLabel *titre = new QLabel("➕ Ajouter un nouveau quai");
    titre->setObjectName("pageTitle");

    QWidget *content = new QWidget;
    content->setObjectName("contentFrame");

    idEdit = new QLineEdit;
    idEdit->setPlaceholderText("Ex: Q001");
    nomEdit = new QLineEdit;
    nomEdit->setPlaceholderText("Ex: Quai central");
    placesEdit = new QLineEdit;
    placesEdit->setPlaceholderText("Nombre total de places");
    naviresEdit = new QLineEdit;
    naviresEdit->setPlaceholderText("Nombre de navires présents");
    adresseEdit = new QLineEdit;
    adresseEdit->setPlaceholderText("Adresse du quai");
    travauxCheck = new QCheckBox("En travaux");

    QPushButton *btnSave = new QPushButton("💾 Enregistrer le quai");
    btnAnnulerAjout = new QPushButton("❌ Annuler");
    btnAnnulerAjout->setStyleSheet("background-color: #ef4444; color: white; font-size: 13px; padding: 8px; border-radius: 6px;");

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addWidget(btnSave);
    btnLayout->addWidget(btnAnnulerAjout);
    btnLayout->addStretch();

    QFormLayout *form = new QFormLayout;
    form->setSpacing(10);
    form->addRow("ID :", idEdit);
    form->addRow("Nom :", nomEdit);
    form->addRow("Places totales :", placesEdit);
    form->addRow("Navires présents :", naviresEdit);
    form->addRow("Adresse :", adresseEdit);
    form->addRow("", travauxCheck);
    form->addRow(btnLayout);

    QVBoxLayout *contentLayout = new QVBoxLayout(content);
    contentLayout->addLayout(form);
    contentLayout->addStretch();

    QVBoxLayout *mainLayout = new QVBoxLayout(pageAjouter);
    mainLayout->addWidget(titre);
    mainLayout->addWidget(content);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    connect(btnSave, &QPushButton::clicked, this, &GestionQuai::ajouterQuai);
    connect(btnAnnulerAjout, &QPushButton::clicked, this, &GestionQuai::showConsulter);
}

void GestionQuai::configurerConsulter()
{
    QLabel *titre = new QLabel("📋 Consultation des quais");
    titre->setObjectName("pageTitle");

    QWidget *content = new QWidget;
    content->setObjectName("contentFrame");

    QVBoxLayout *viewLay = new QVBoxLayout(content);

    QHBoxLayout *searchLayout = new QHBoxLayout();
    searchEdit = new QLineEdit();
    searchEdit->setPlaceholderText("🔍 Rechercher par nom, ID ou adresse...");
    searchEdit->setFixedWidth(250);
    sortBox = new QComboBox();
    sortBox->addItems({"Trier par...", "Nom (A-Z)", "Places (Croissant)", "Places (Décroissant)",
                       "Navires (Croissant)", "Navires (Décroissant)", "État"});
    sortBox->setFixedWidth(180);

    searchLayout->addWidget(new QLabel("Recherche:"));
    searchLayout->addWidget(searchEdit);
    searchLayout->addSpacing(10);
    searchLayout->addWidget(new QLabel("Trier:"));
    searchLayout->addWidget(sortBox);
    searchLayout->addStretch();

    tableConsulter = new QTableWidget(0,7);
    QStringList headers = {"ID", "Nom", "Places totales", "Navires présents", "Adresse", "Travaux", "État"};
    tableConsulter->setHorizontalHeaderLabels(headers);
    tableConsulter->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableConsulter->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableConsulter->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableConsulter->verticalHeader()->setVisible(false);
    tableConsulter->setMinimumHeight(250);
    tableConsulter->setStyleSheet("background: white; border: 1px solid #e2e8f0; border-radius: 6px;");

    viewLay->addLayout(searchLayout);
    viewLay->addWidget(tableConsulter);

    QVBoxLayout *mainLayout = new QVBoxLayout(pageConsulter);
    mainLayout->addWidget(titre);
    mainLayout->addWidget(content);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    connect(searchEdit, &QLineEdit::textChanged, this, &GestionQuai::rechercherQuai);
    connect(sortBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &GestionQuai::trierQuais);
}

void GestionQuai::configurerStatistiques()
{
    QLabel *titre = new QLabel("📊 Statistiques des quais");
    titre->setObjectName("pageTitle");

    QWidget *content = new QWidget;
    content->setObjectName("contentFrame");

    QVBoxLayout *statsLayout = new QVBoxLayout(content);
    monGraphique = new MonCamembert();
    statsLayout->addWidget(monGraphique);

    QLabel *statsText = new QLabel("Statistiques en temps réel");
    statsText->setAlignment(Qt::AlignCenter);
    statsText->setStyleSheet("font-size: 13px; color: #475569; margin-top: 10px;");
    statsLayout->addWidget(statsText);
    statsLayout->addStretch();

    QVBoxLayout *mainLayout = new QVBoxLayout(pageStatistiques);
    mainLayout->addWidget(titre);
    mainLayout->addWidget(content);
    mainLayout->setContentsMargins(0, 0, 0, 0);
}

void GestionQuai::configurerProposition()
{
    QLabel *titre = new QLabel("💡 Trouver le quai idéal");
    titre->setObjectName("pageTitle");

    QWidget *content = new QWidget;
    content->setObjectName("contentFrame");

    QVBoxLayout *propLayout = new QVBoxLayout(content);
    QLabel *lblInst = new QLabel("Entrez le nombre de places nécessaires pour votre bateau :");
    lblInst->setStyleSheet("font-size: 13px; color: #475569;");
    longueurBateauEdit = new QLineEdit;
    longueurBateauEdit->setPlaceholderText("Exemple: 5 (places)");

    QPushButton *btnCalculer = new QPushButton("🔍 Rechercher le meilleur quai");
    resultatLabel = new QLabel("Résultats s'afficheront ici...");
    resultatLabel->setStyleSheet("background: white; border: 2px dashed #94a3b8; border-radius: 8px; padding: 15px; font-size: 13px;");
    resultatLabel->setWordWrap(true);
    resultatLabel->setAlignment(Qt::AlignCenter);
    resultatLabel->setMinimumHeight(60);

    propLayout->addWidget(lblInst);
    propLayout->addWidget(longueurBateauEdit);
    propLayout->addWidget(btnCalculer);
    propLayout->addSpacing(10);
    propLayout->addWidget(resultatLabel);
    propLayout->addStretch();

    QVBoxLayout *mainLayout = new QVBoxLayout(pageProposition);
    mainLayout->addWidget(titre);
    mainLayout->addWidget(content);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    connect(btnCalculer, &QPushButton::clicked, this, &GestionQuai::trouverMeilleurQuai);
}

void GestionQuai::configurerSimulation()
{
    QLabel *titre = new QLabel("🔮 Simulation avancée d'occupation");
    titre->setObjectName("pageTitle");

    QWidget *content = new QWidget;
    content->setObjectName("contentFrame");

    QVBoxLayout *simulLayout = new QVBoxLayout(content);

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
    controlsLayout1->addSpacing(10);
    controlsLayout1->addWidget(new QLabel("Arrivées moyennes:"));
    controlsLayout1->addWidget(spinMoyenneArrivees);
    controlsLayout1->addSpacing(10);
    controlsLayout1->addWidget(new QLabel("Variabilité:"));
    controlsLayout1->addWidget(spinEcartType);

    QHBoxLayout *controlsLayout2 = new QHBoxLayout;
    controlsLayout2->addWidget(new QLabel("Durée séjour:"));
    controlsLayout2->addWidget(spinDureeSejour);
    controlsLayout2->addSpacing(10);
    controlsLayout2->addWidget(checkVariationsJournalieres);
    controlsLayout2->addSpacing(10);
    controlsLayout2->addWidget(checkWeekend);
    controlsLayout2->addSpacing(10);
    controlsLayout2->addWidget(checkEffetSaison);

    QPushButton *btnLancerSimul = new QPushButton("🚀 Lancer la simulation");
    btnLancerSimul->setStyleSheet("background-color: #0891b2; color: white; font-size: 13px; padding: 8px;");

    btnScenarioAleatoire = new QPushButton("🎲 Générer scénario aléatoire");
    btnScenarioAleatoire->setStyleSheet("background-color: #8b5cf6; color: white; font-size: 13px; padding: 8px;");

    progressSimulation = new QProgressBar;
    progressSimulation->setVisible(false);
    progressSimulation->setRange(0, 100);

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addWidget(btnLancerSimul);
    buttonsLayout->addWidget(btnScenarioAleatoire);
    buttonsLayout->addWidget(progressSimulation);

    histoWidget = new MonHistogramme();
    histoWidget->setMinimumHeight(250);

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
    lblResultatSimul->setMinimumHeight(60);

    lblStatsDetaillees = new QLabel;
    lblStatsDetaillees->setWordWrap(true);
    lblStatsDetaillees->setAlignment(Qt::AlignLeft);
    lblStatsDetaillees->setStyleSheet("background: #f8fafc; border: 1px solid #cbd5e1; border-radius: 6px; padding: 10px;");
    lblStatsDetaillees->setMinimumHeight(80);

    simulLayout->addLayout(controlsLayout1);
    simulLayout->addLayout(controlsLayout2);
    simulLayout->addSpacing(8);
    simulLayout->addLayout(buttonsLayout);
    simulLayout->addSpacing(10);
    simulLayout->addLayout(graphOptionsLayout);
    simulLayout->addWidget(histoWidget);
    simulLayout->addSpacing(8);
    simulLayout->addWidget(lblResultatSimul);
    simulLayout->addSpacing(8);
    simulLayout->addWidget(lblStatsDetaillees);
    simulLayout->addStretch();

    QVBoxLayout *mainLayout = new QVBoxLayout(pageSimulation);
    mainLayout->addWidget(titre);
    mainLayout->addWidget(content);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    connect(btnLancerSimul, &QPushButton::clicked, this, &GestionQuai::lancerSimulation);
    connect(btnScenarioAleatoire, &QPushButton::clicked, this, &GestionQuai::genererScenarioAleatoire);
}

void GestionQuai::configurerExportCSV()
{
    QLabel *titre = new QLabel("📊 Exporter le planning en CSV");
    titre->setObjectName("pageTitle");

    QWidget *content = new QWidget;
    content->setObjectName("contentFrame");

    QVBoxLayout *csvLayout = new QVBoxLayout(content);

    QFormLayout *formCSV = new QFormLayout;
    formCSV->setSpacing(8);
    nomFichierEdit = new QLineEdit;
    nomFichierEdit->setText("planning_quais_" + QDate::currentDate().toString("yyyy-MM-dd"));
    nomFichierEdit->setPlaceholderText("Nom du fichier CSV");

    formatCSVBox = new QComboBox;
    formatCSVBox->addItems({"CSV Standard", "CSV Excel", "CSV avec séparateur point-virgule"});

    formCSV->addRow("Nom du fichier :", nomFichierEdit);
    formCSV->addRow("Format :", formatCSVBox);

    btnGenererCSV = new QPushButton("💾 Générer et exporter le CSV");
    btnGenererCSV->setStyleSheet("background-color: #059669; color: white; font-size: 13px; padding: 8px;");

    lblPreviewCSV = new QLabel;
    lblPreviewCSV->setWordWrap(true);
    lblPreviewCSV->setAlignment(Qt::AlignCenter);
    lblPreviewCSV->setStyleSheet("background: white; border: 2px solid #10b981; border-radius: 8px; padding: 10px; margin-top: 10px;");

    csvLayout->addLayout(formCSV);
    csvLayout->addWidget(btnGenererCSV, 0, Qt::AlignCenter);
    csvLayout->addSpacing(10);
    csvLayout->addWidget(lblPreviewCSV);
    csvLayout->addStretch();

    QVBoxLayout *mainLayout = new QVBoxLayout(pageExportCSV);
    mainLayout->addWidget(titre);
    mainLayout->addWidget(content);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    connect(btnGenererCSV, &QPushButton::clicked, this, &GestionQuai::exporterCSV);
}

void GestionQuai::configurerSupprimer()
{
    QLabel *titre = new QLabel("🗑️ Supprimer un quai");
    titre->setObjectName("pageTitle");

    QWidget *content = new QWidget;
    content->setObjectName("contentFrame");

    QVBoxLayout *supLay = new QVBoxLayout(content);

    tableSupprimer = new QTableWidget(0,7);
    QStringList headers = {"ID", "Nom", "Places totales", "Navires présents", "Adresse", "Travaux", "État"};
    tableSupprimer->setHorizontalHeaderLabels(headers);
    tableSupprimer->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableSupprimer->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableSupprimer->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableSupprimer->verticalHeader()->setVisible(false);
    tableSupprimer->setMinimumHeight(250);
    tableSupprimer->setStyleSheet("background: white; border: 1px solid #e2e8f0; border-radius: 6px;");

    QPushButton *btnDelRow = new QPushButton("🗑️ Supprimer la sélection");
    btnDelRow->setStyleSheet("background-color: #ef4444; color: white; font-size: 13px; padding: 8px;");

    supLay->addWidget(tableSupprimer);
    supLay->addWidget(btnDelRow, 0, Qt::AlignCenter);

    QVBoxLayout *mainLayout = new QVBoxLayout(pageSupprimer);
    mainLayout->addWidget(titre);
    mainLayout->addWidget(content);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    connect(btnDelRow, &QPushButton::clicked, this, &GestionQuai::supprimerQuai);
}

void GestionQuai::configurerModifier()
{
    QLabel *titre = new QLabel("✏️ Modifier un quai");
    titre->setObjectName("pageTitle");

    QWidget *content = new QWidget;
    content->setObjectName("contentFrame");

    QHBoxLayout *modLayout = new QHBoxLayout(content);

    tableModifier = new QTableWidget(0,7);
    QStringList headers = {"ID", "Nom", "Places totales", "Navires présents", "Adresse", "Travaux", "État"};
    tableModifier->setHorizontalHeaderLabels(headers);
    tableModifier->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableModifier->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableModifier->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableModifier->verticalHeader()->setVisible(false);
    tableModifier->setMinimumHeight(250);
    tableModifier->setStyleSheet("background: white; border: 1px solid #e2e8f0; border-radius: 6px;");

    modLayout->addWidget(tableModifier, 2);

    QWidget *formWidget = new QWidget;
    formWidget->setStyleSheet("background: white; border-radius: 6px; padding: 15px; border: 1px solid #cbd5e1;");

    idEditM = new QLineEdit; idEditM->setReadOnly(true);
    nomEditM = new QLineEdit;
    placesEditM = new QLineEdit;
    naviresEditM = new QLineEdit;
    adresseEditM = new QLineEdit;
    travauxCheckM = new QCheckBox("En travaux");

    QPushButton *btnUpdate = new QPushButton("✔️ Modifier le quai");
    btnAnnulerModif = new QPushButton("❌ Annuler");
    btnAnnulerModif->setStyleSheet("background-color: #ef4444; color: white; font-size: 13px; padding: 8px; border-radius: 6px;");

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addWidget(btnUpdate);
    btnLayout->addWidget(btnAnnulerModif);
    btnLayout->addStretch();

    QFormLayout *formM = new QFormLayout(formWidget);
    formM->setSpacing(8);
    formM->addRow("ID :", idEditM);
    formM->addRow("Nom :", nomEditM);
    formM->addRow("Places totales :", placesEditM);
    formM->addRow("Navires présents :", naviresEditM);
    formM->addRow("Adresse :", adresseEditM);
    formM->addRow("", travauxCheckM);
    formM->addRow(btnLayout);

    modLayout->addWidget(formWidget, 1);

    QVBoxLayout *mainLayout = new QVBoxLayout(pageModifier);
    mainLayout->addWidget(titre);
    mainLayout->addWidget(content);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    connect(tableModifier, &QTableWidget::cellClicked, this, &GestionQuai::remplirFormulaireDepuisTable);
    connect(btnUpdate, &QPushButton::clicked, this, &GestionQuai::modifierQuai);
    connect(btnAnnulerModif, &QPushButton::clicked, this, &GestionQuai::showConsulter);
    connect(placesEditM, &QLineEdit::textChanged, this, &GestionQuai::onPlacesChanged);
    connect(naviresEditM, &QLineEdit::textChanged, this, &GestionQuai::onNaviresChanged);
    connect(travauxCheckM, &QCheckBox::stateChanged, this, &GestionQuai::onTravauxChanged);
}

// ============ Méthodes de navigation ============
void GestionQuai::showAjouter() { pages->setCurrentWidget(pageAjouter); }
void GestionQuai::showModifier() { syncTables(); pages->setCurrentWidget(pageModifier); }
void GestionQuai::showSupprimer() { syncTables(); pages->setCurrentWidget(pageSupprimer); }
void GestionQuai::showConsulter() { syncTables(); pages->setCurrentWidget(pageConsulter); }
void GestionQuai::showStatistiques() { updateStats(); pages->setCurrentWidget(pageStatistiques); }
void GestionQuai::showProposition() { pages->setCurrentWidget(pageProposition); }
void GestionQuai::showSimulation() { pages->setCurrentWidget(pageSimulation); }
void GestionQuai::showExportCSV()
{
    int totalQuais = tableConsulter->rowCount();
    int dispo = 0, occupe = 0, travaux = 0;
    for(int r=0; r<totalQuais; ++r){
        QString etat = tableConsulter->item(r,6)->text();
        if(etat == "Disponible") dispo++;
        else if(etat == "Complet") occupe++;
        else if(etat == "En travaux") travaux++;
    }
    lblPreviewCSV->setText("Le fichier CSV contiendra :\n"
                           "• " + QString::number(totalQuais) + " quais au total\n"
                                                           "• " + QString::number(dispo) + " quais disponibles\n"
                                                      "• " + QString::number(occupe) + " quais complets\n"
                                                       "• " + QString::number(travaux) + " quais en travaux\n"
                                                        "• Rapport généré le " + QDate::currentDate().toString("dd/MM/yyyy") + "\n"
                                                                           "• Tableau complet du planning\n"
                                                                           "• Statistiques détaillées");
    pages->setCurrentWidget(pageExportCSV);
}

// ============ Méthodes utilitaires ============
QString GestionQuai::calculerEtat(int places, int navires, bool travaux) {
    if (travaux) return "En travaux";
    if (navires >= places) return "Complet";
    return "Disponible";
}

void GestionQuai::onPlacesChanged(const QString &) {
    int row = tableModifier->currentRow();
    if (row < 0) return;
    bool ok;
    int places = placesEditM->text().toInt(&ok);
    if (!ok) places = 0;
    int navires = naviresEditM->text().toInt(&ok);
    if (!ok) navires = 0;
    bool travaux = travauxCheckM->isChecked();
    QString etat = calculerEtat(places, navires, travaux);
    // Optionnel : afficher l'état dans un label
}

void GestionQuai::onNaviresChanged(const QString &) {
    onPlacesChanged("");
}

void GestionQuai::onTravauxChanged(int) {
    onPlacesChanged("");
}

void GestionQuai::syncTables() {
    tableSupprimer->setRowCount(0);
    tableModifier->setRowCount(0);
    for(int r=0; r<tableConsulter->rowCount(); ++r){
        tableSupprimer->insertRow(r);
        tableModifier->insertRow(r);
        for(int c=0; c<7; ++c){
            QTableWidgetItem *item = tableConsulter->item(r,c);
            if(item){
                tableSupprimer->setItem(r,c,new QTableWidgetItem(*item));
                tableModifier->setItem(r,c,new QTableWidgetItem(*item));
            }
        }
    }
}

void GestionQuai::updateStats() {
    int dispo = 0, occupe = 0;
    for(int r=0; r<tableConsulter->rowCount(); ++r){
        if(!tableConsulter->isRowHidden(r)){
            QString etat = tableConsulter->item(r,6)->text();
            if(etat == "Disponible") dispo++;
            else if(etat == "Complet") occupe++;
            else if(etat == "En travaux") occupe++; // compté comme occupé pour le camembert
        }
    }
    monGraphique->setValeurs(occupe, dispo);
}

// ============ CRUD ============
void GestionQuai::ajouterQuai() {
    if(idEdit->text().isEmpty() || nomEdit->text().isEmpty() || placesEdit->text().isEmpty() ||
        naviresEdit->text().isEmpty() || adresseEdit->text().isEmpty()){
        QMessageBox::warning(this,"Erreur","Veuillez remplir tous les champs.");
        return;
    }

    // Vérifier ID unique
    for(int r = 0; r < tableConsulter->rowCount(); ++r) {
        if(tableConsulter->item(r, 0)->text() == idEdit->text()) {
            QMessageBox::warning(this,"Erreur","Cet ID existe déjà. Veuillez en choisir un autre.");
            return;
        }
    }

    bool ok;
    int places = placesEdit->text().toInt(&ok);
    if (!ok || places <= 0) {
        QMessageBox::warning(this,"Erreur","Le nombre de places doit être un entier positif.");
        return;
    }
    int navires = naviresEdit->text().toInt(&ok);
    if (!ok || navires < 0 || navires > places) {
        QMessageBox::warning(this,"Erreur","Le nombre de navires doit être un entier entre 0 et le nombre de places.");
        return;
    }
    bool travaux = travauxCheck->isChecked();
    QString etat = calculerEtat(places, navires, travaux);

    int r = tableConsulter->rowCount();
    tableConsulter->insertRow(r);
    tableConsulter->setItem(r,0,new QTableWidgetItem(idEdit->text()));
    tableConsulter->setItem(r,1,new QTableWidgetItem(nomEdit->text()));
    tableConsulter->setItem(r,2,new QTableWidgetItem(QString::number(places)));
    tableConsulter->setItem(r,3,new QTableWidgetItem(QString::number(navires)));
    tableConsulter->setItem(r,4,new QTableWidgetItem(adresseEdit->text()));
    tableConsulter->setItem(r,5,new QTableWidgetItem(travaux ? "Oui" : "Non"));
    tableConsulter->setItem(r,6,new QTableWidgetItem(etat));

    idEdit->clear(); nomEdit->clear(); placesEdit->clear(); naviresEdit->clear(); adresseEdit->clear();
    travauxCheck->setChecked(false);
    syncTables();

    QMessageBox::information(this, "Succès", "Quai ajouté avec succès !");
}

void GestionQuai::supprimerQuai() {
    int r = tableSupprimer->currentRow();
    if(r >= 0){
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
    placesEditM->setText(tableModifier->item(r,2)->text());
    naviresEditM->setText(tableModifier->item(r,3)->text());
    adresseEditM->setText(tableModifier->item(r,4)->text());
    travauxCheckM->setChecked(tableModifier->item(r,5)->text() == "Oui");
}

void GestionQuai::modifierQuai() {
    int r = tableModifier->currentRow();
    if(r < 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner un quai à modifier.");
        return;
    }

    if(nomEditM->text().isEmpty() || placesEditM->text().isEmpty() ||
        naviresEditM->text().isEmpty() || adresseEditM->text().isEmpty()){
        QMessageBox::warning(this,"Erreur","Tous les champs doivent être remplis.");
        return;
    }

    bool ok;
    int places = placesEditM->text().toInt(&ok);
    if (!ok || places <= 0) {
        QMessageBox::warning(this,"Erreur","Le nombre de places doit être un entier positif.");
        return;
    }
    int navires = naviresEditM->text().toInt(&ok);
    if (!ok || navires < 0 || navires > places) {
        QMessageBox::warning(this,"Erreur","Le nombre de navires doit être un entier entre 0 et le nombre de places.");
        return;
    }
    bool travaux = travauxCheckM->isChecked();
    QString etat = calculerEtat(places, navires, travaux);

    tableConsulter->item(r,1)->setText(nomEditM->text());
    tableConsulter->item(r,2)->setText(placesEditM->text());
    tableConsulter->item(r,3)->setText(naviresEditM->text());
    tableConsulter->item(r,4)->setText(adresseEditM->text());
    tableConsulter->item(r,5)->setText(travaux ? "Oui" : "Non");
    tableConsulter->item(r,6)->setText(etat);

    syncTables();
    QMessageBox::information(this, "Succès", "Quai modifié avec succès !");
}

void GestionQuai::rechercherQuai(const QString &texte) {
    for (int r = 0; r < tableConsulter->rowCount(); ++r) {
        QString nom = tableConsulter->item(r, 1)->text().toLower();
        QString id = tableConsulter->item(r, 0)->text().toLower();
        QString adresse = tableConsulter->item(r, 4)->text().toLower();
        QString searchText = texte.toLower();
        tableConsulter->setRowHidden(r, !(nom.contains(searchText) || id.contains(searchText) || adresse.contains(searchText)));
    }
}

void GestionQuai::trierQuais(int index) {
    if (index == 0) return;
    int col = 1;
    Qt::SortOrder order = Qt::AscendingOrder;
    switch (index) {
    case 1: col = 1; order = Qt::AscendingOrder; break; // Nom A-Z
    case 2: col = 2; order = Qt::AscendingOrder; break; // Places croissant
    case 3: col = 2; order = Qt::DescendingOrder; break; // Places décroissant
    case 4: col = 3; order = Qt::AscendingOrder; break; // Navires croissant
    case 5: col = 3; order = Qt::DescendingOrder; break; // Navires décroissant
    case 6: col = 6; order = Qt::AscendingOrder; break; // État
    default: return;
    }
    tableConsulter->sortItems(col, order);
    syncTables();
}

void GestionQuai::trouverMeilleurQuai() {
    bool ok;
    int placesNecessaires = longueurBateauEdit->text().toInt(&ok);
    if(!ok || placesNecessaires <= 0) {
        resultatLabel->setText("❌ Veuillez entrer un nombre de places valide.");
        return;
    }
    int meilleurRow = -1;
    int minPerte = 999999;
    for(int r = 0; r < tableConsulter->rowCount(); ++r) {
        QString etat = tableConsulter->item(r, 6)->text();
        if (etat == "Disponible") {
            int places = tableConsulter->item(r, 2)->text().toInt();
            int navires = tableConsulter->item(r, 3)->text().toInt();
            int placesLibres = places - navires;
            if (placesLibres >= placesNecessaires) {
                int perte = placesLibres - placesNecessaires;
                if (perte < minPerte) {
                    minPerte = perte;
                    meilleurRow = r;
                }
            }
        }
    }
    if (meilleurRow != -1) {
        QString nom = tableConsulter->item(meilleurRow, 1)->text();
        QString places = tableConsulter->item(meilleurRow, 2)->text();
        QString id = tableConsulter->item(meilleurRow, 0)->text();
        int libres = tableConsulter->item(meilleurRow, 2)->text().toInt() - tableConsulter->item(meilleurRow, 3)->text().toInt();
        resultatLabel->setText("✅ <b>Quai trouvé !</b><br>"
                               "Nom : <b>" + nom + "</b><br>"
                                       "ID : <b>" + id + "</b><br>"
                                      "Places totales : <b>" + places + "</b><br>"
                                          "Places libres : <b>" + QString::number(libres) + "</b><br>"
                                                           "Espace perdu : <b>" + QString::number(minPerte) + " place(s)</b>");
    } else {
        resultatLabel->setText("❌ Aucun quai disponible avec " + QString::number(placesNecessaires) + " place(s) libre(s).");
    }
}

// ============ Simulation ============
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

    int capaciteTotale = 0;
    int occupationActuelle = 0;
    QList<int> placesParQuai;
    QList<int> naviresParQuai;
    QList<bool> travauxParQuai;

    for (int r = 0; r < tableConsulter->rowCount(); ++r) {
        int places = tableConsulter->item(r, 2)->text().toInt();
        int navires = tableConsulter->item(r, 3)->text().toInt();
        bool travaux = (tableConsulter->item(r, 5)->text() == "Oui");
        if (!travaux) {
            capaciteTotale += places;
            occupationActuelle += navires;
        }
        placesParQuai.append(places);
        naviresParQuai.append(navires);
        travauxParQuai.append(travaux);
    }

    if (capaciteTotale == 0) {
        lblResultatSimul->setText("❌ Erreur: Aucune place disponible (tous les quais sont en travaux ou inexistants).");
        lblStatsDetaillees->clear();
        return;
    }

    progressSimulation->setVisible(true);
    progressSimulation->setValue(10);

    QVector<int> arriveesParJour = simulerArriveesBateaux(jours, moyenneArrivees, ecartType, checkVariationsJournalieres->isChecked());

    progressSimulation->setValue(30);

    QList<QDate> datesLiberation;
    QDate today = QDate::currentDate();
    QRandomGenerator *rg = QRandomGenerator::global();
    for (int i = 0; i < occupationActuelle; ++i) {
        int dureeRestante = rg->bounded(1, dureeSejourMoyenne * 2);
        datesLiberation.append(today.addDays(dureeRestante));
    }

    progressSimulation->setValue(50);

    QVector<QPair<QString, int>> resultatsGraphique;
    int occupation = occupationActuelle;

    for (int i = 0; i < jours; ++i) {
        QDate dateSimulee = today.addDays(i + 1);

        int departs = 0;
        for (int j = 0; j < datesLiberation.size(); ++j) {
            if (datesLiberation[j] <= dateSimulee) {
                departs++;
                datesLiberation.removeAt(j);
                j--;
            }
        }
        occupation = std::max(0, occupation - departs);

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

        int placesLibres = capaciteTotale - occupation;
        nouvellesArrivees = std::min(nouvellesArrivees, placesLibres);
        occupation += nouvellesArrivees;

        for (int k = 0; k < nouvellesArrivees; ++k) {
            int dureeSejour = dureeSejourMoyenne + rg->bounded(-2, 4);
            dureeSejour = std::max(1, dureeSejour);
            datesLiberation.append(dateSimulee.addDays(dureeSejour));
        }

        int pourcent = (occupation * 100) / capaciteTotale;
        resultatsGraphique.append(qMakePair("J" + QString::number(i + 1), pourcent));

        if (i % 5 == 0) progressSimulation->setValue(50 + (i * 40 / jours));
    }

    progressSimulation->setValue(90);

    StatsSimulation stats = analyserResultats(resultatsGraphique);
    histoWidget->setDonnees(resultatsGraphique);

    QString messagePrincipal;
    if (stats.joursSaturation > 0) {
        messagePrincipal = QString("⚠️ <b style='color:red; font-size:13px'>ALERTE SATURATION !</b><br>"
                                   "Le port sera complet dès le <b>jour %1</b>.<br>"
                                   "Occupation maximale: <b>%2%</b>").arg(stats.joursSaturation).arg(stats.maxOccupation);
    } else if (stats.maxOccupation > 85) {
        messagePrincipal = QString("🔶 <b style='color:orange; font-size:13px'>SITUATION TENDUE</b><br>"
                                   "Pic d'occupation à <b>%1%</b> le jour %2.<br>"
                                   "%3 jour(s) critiques (>80%%)").arg(stats.maxOccupation).arg(stats.jourMaxOccupation).arg(stats.joursCritiques);
    } else {
        messagePrincipal = QString("✅ <b style='color:green; font-size:13px'>SITUATION STABLE</b><br>"
                                   "Occupation maximale: <b>%1%</b><br>"
                                   "Moyenne: <b>%2%</b> sur %3 jours").arg(stats.maxOccupation).arg(qRound(stats.moyenneOccupation)).arg(jours);
    }

    lblResultatSimul->setText(messagePrincipal);

    QString statsText = QString("<b>📊 Statistiques détaillées :</b><br>"
                                "• Arrivées moyennes simulées: <b>%1 bateaux/jour</b><br>"
                                "• Durée moyenne de séjour: <b>%2 jours</b><br>"
                                "• Capacité totale (places): <b>%3</b><br>"
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

// ============ Export CSV ============
QString GestionQuai::genererContenuCSV() {
    QString csvContent;
    csvContent += "# Planning des quais - FISHTECH\n";
    csvContent += "# Système de gestion des quais\n";
    csvContent += "# Date de génération: " + QDate::currentDate().toString("dd/MM/yyyy") + "\n";

    int dispo = 0, occupe = 0, travaux = 0;
    int total = tableConsulter->rowCount();
    for(int r=0; r<total; ++r){
        QString etat = tableConsulter->item(r,6)->text();
        if(etat == "Disponible") dispo++;
        else if(etat == "Complet") occupe++;
        else if(etat == "En travaux") travaux++;
    }

    csvContent += "# Statistiques générales\n";
    csvContent += "# Total quais: " + QString::number(total) + "\n";
    csvContent += "# Disponibles: " + QString::number(dispo) + "\n";
    csvContent += "# Complets: " + QString::number(occupe) + "\n";
    csvContent += "# En travaux: " + QString::number(travaux) + "\n";

    int placesTotales = 0, naviresTotaux = 0;
    for(int r=0; r<total; ++r){
        placesTotales += tableConsulter->item(r,2)->text().toInt();
        naviresTotaux += tableConsulter->item(r,3)->text().toInt();
    }
    csvContent += "# Places totales: " + QString::number(placesTotales) + "\n";
    csvContent += "# Navires présents: " + QString::number(naviresTotaux) + "\n\n";

    QString separator = ",";
    if(formatCSVBox->currentText() == "CSV avec séparateur point-virgule") separator = ";";

    QStringList headers = {"ID","Nom","Places totales","Navires présents","Adresse","Travaux","État"};
    csvContent += headers.join(separator) + "\n";

    for(int r=0; r<tableConsulter->rowCount(); ++r){
        QStringList rowData;
        for(int c=0; c<7; ++c){
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
