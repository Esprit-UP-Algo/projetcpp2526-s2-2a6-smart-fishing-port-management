#include "gestionquai.h"
#include "connection.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QFormLayout>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDate>
#include <QGroupBox>
#include <QGridLayout>
#include <algorithm>
#include <cmath>

// ============ MonCamembert ============
MonCamembert::MonCamembert(QWidget *parent) : QWidget(parent) {
    valeurOccupe = 0;
    valeurDispo = 0;
    setMinimumSize(200, 200);
}

void MonCamembert::setValeurs(int occupe, int dispo) {
    valeurOccupe = occupe;
    valeurDispo = dispo;
    update();
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
    setMinimumHeight(200);
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
    int barWidth = (width() - 2 * margin) / qMax(1, donnees.size());
    int maxH = height() - 2 * margin;

    if (showGrid) {
        p.setPen(QPen(QColor("#e2e8f0"), 1));
        for (int i = 0; i <= 10; ++i) {
            int y = height() - margin - (i * maxH / 10);
            p.drawLine(margin, y, width() - margin, y);
        }
    }

    p.setPen(QPen(Qt::black, 2));
    p.drawLine(margin, margin, margin, height() - margin);
    p.drawLine(margin, height() - margin, width() - margin, height() - margin);

    int maxValue = 0;
    for (const auto &data : donnees) if (data.second > maxValue) maxValue = data.second;

    for (int i = 0; i < donnees.size(); ++i) {
        int pourcent = donnees[i].second;
        int h = maxValue > 0 ? (pourcent * maxH) / maxValue : 0;
        int x = margin + i * barWidth + 5;
        int y = height() - margin - h;

        QColor barColor = pourcent > 90 ? QColor("#ef4444") : (pourcent > 60 ? QColor("#f59e0b") : QColor("#3b82f6"));
        p.setBrush(barColor);
        p.setPen(QPen(QColor("#1e293b"), 1));
        p.drawRect(x, y, barWidth - 10, h);

        if (showValues) {
            p.setPen(Qt::black);
            p.setFont(QFont("Segoe UI", 8));
            p.drawText(QRect(x, y - 15, barWidth - 10, 12), Qt::AlignCenter, QString::number(pourcent));
        }

        p.setPen(Qt::darkGray);
        p.setFont(QFont("Segoe UI", 7));
        p.drawText(QRect(x - 5, height() - margin + 3, barWidth + 10, 16), Qt::AlignCenter, donnees[i].first);
    }
}

// ============ GestionQuai ============
GestionQuai::GestionQuai(QWidget *parent)
    : QWidget(parent),
      table(nullptr),
      lineSearch(nullptr),
      statTotal(nullptr),
      statTotalPlaces(nullptr),
      statTotalNavires(nullptr),
      statMoyenneOccupation(nullptr),
      statDispos(nullptr),
      pages(nullptr),
      longueurBateauEdit(nullptr),
      resultatLabel(nullptr),
      spinJours(nullptr),
      spinMoyenneArrivees(nullptr),
      spinEcartType(nullptr),
      checkVariationsJournalieres(nullptr),
      checkWeekend(nullptr),
      checkEffetSaison(nullptr),
      progressSimulation(nullptr),
      histoWidget(nullptr),
      lblResultatSimul(nullptr),
      lblStatsDetaillees(nullptr),
      pageConsulter(nullptr),
      pageProposition(nullptr),
      pageSimulation(nullptr)
{
    setupUI();
    Connection::instance()->createconnect();
    loadData();
}

GestionQuai::~GestionQuai()
{
}

void GestionQuai::setupUI()
{
    // This widget is designed to be integrated into the MainWindow's stackedWidget
    // We need to create the content without a sidebar - the MainWindow already has one
    // So we create a simple layout with just the content
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Header - Titre
    QHBoxLayout *headerLayout = new QHBoxLayout();
    QLabel *title = new QLabel("Gestion des Quais", this);
    title->setStyleSheet("font-size: 28px; font-weight: bold; color: #03224c;");
    headerLayout->addWidget(title);
    headerLayout->addStretch();
    mainLayout->addLayout(headerLayout);

    // Search bar
    QHBoxLayout *searchLayout = new QHBoxLayout();
    QLabel *searchLabel = new QLabel("Rechercher:", this);
    searchLabel->setStyleSheet("font-weight: bold; color: #03224c;");
    lineSearch = new QLineEdit(this);
    lineSearch->setPlaceholderText("Rechercher par nom, adresse...");
    lineSearch->setMinimumWidth(250);
    searchLayout->addWidget(searchLabel);
    searchLayout->addWidget(lineSearch);
    searchLayout->addStretch();
    mainLayout->addLayout(searchLayout);

    // Stats section - Like employee/captures management
    QGroupBox *statsBox = new QGroupBox("📊 Statistiques des Quais", this);
    QHBoxLayout *statsLayout = new QHBoxLayout(statsBox);
    
    statTotal = new QLabel("Total: 0", statsBox);
    statTotalPlaces = new QLabel("Places Totales: 0", statsBox);
    statTotalNavires = new QLabel("Navires: 0", statsBox);
    statMoyenneOccupation = new QLabel("Moyenne: 0%", statsBox);
    statDispos = new QLabel("Disponibles: 0", statsBox);
    
    statTotal->setStyleSheet("background: #e3f2fd; color: #03224c; font-weight: bold; border-radius: 8px; padding: 10px;");
    statTotalPlaces->setStyleSheet("background: #e8f5e9; color: #065f46; font-weight: bold; border-radius: 8px; padding: 10px;");
    statTotalNavires->setStyleSheet("background: #fff3e0; color: #e65100; font-weight: bold; border-radius: 8px; padding: 10px;");
    statMoyenneOccupation->setStyleSheet("background: #fce4ec; color: #880e4f; font-weight: bold; border-radius: 8px; padding: 10px;");
    statDispos->setStyleSheet("background: #f3e5f5; color: #4a148c; font-weight: bold; border-radius: 8px; padding: 10px;");
    
    statsLayout->addWidget(statTotal);
    statsLayout->addWidget(statTotalPlaces);
    statsLayout->addWidget(statTotalNavires);
    statsLayout->addWidget(statMoyenneOccupation);
    statsLayout->addWidget(statDispos);
    mainLayout->addWidget(statsBox);

    // Page principale: Tableau des quais
    pageConsulter = new QWidget();
    QVBoxLayout *consulterLayout = new QVBoxLayout(pageConsulter);
    
    table = new QTableWidget(pageConsulter);
    table->setColumnCount(7);
    table->setHorizontalHeaderLabels({"ID", "Nom", "Places", "Navires", "Adresse", "Travaux", "État"});
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->verticalHeader()->setVisible(false);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    consulterLayout->addWidget(table, 1);

    // Buttons for consulter page - Like employee management
    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    QPushButton *btnAdd = new QPushButton("+ Ajouter", this);
    QPushButton *btnEdit = new QPushButton("✏️ Modifier", this);
    QPushButton *btnDelete = new QPushButton("🗑️ Supprimer", this);
    QPushButton *btnSortNom = new QPushButton("Trier par Nom", this);
    QPushButton *btnSortPlaces = new QPushButton("Trier par Places", this);
    QPushButton *btnSortNavires = new QPushButton("Trier par Navires", this);
    QPushButton *btnProposition = new QPushButton("💡 Proposition", this);
    QPushButton *btnSimulation = new QPushButton("🎮 Simulation", this);
    
    btnAdd->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; padding: 8px 16px; border-radius: 5px; font-weight: bold; }");
    btnEdit->setStyleSheet("QPushButton { background-color: #2196F3; color: white; padding: 8px 16px; border-radius: 5px; font-weight: bold; }");
    btnDelete->setStyleSheet("QPushButton { background-color: #F44336; color: white; padding: 8px 16px; border-radius: 5px; font-weight: bold; }");
    btnSortNom->setStyleSheet("QPushButton { background-color: #9C27B0; color: white; padding: 8px 12px; border-radius: 5px; }");
    btnSortPlaces->setStyleSheet("QPushButton { background-color: #FF9800; color: white; padding: 8px 12px; border-radius: 5px; }");
    btnSortNavires->setStyleSheet("QPushButton { background-color: #00BCD4; color: white; padding: 8px 12px; border-radius: 5px; }");
    btnProposition->setStyleSheet("QPushButton { background-color: #795548; color: white; padding: 8px 12px; border-radius: 5px; }");
    btnSimulation->setStyleSheet("QPushButton { background-color: #607D8B; color: white; padding: 8px 12px; border-radius: 5px; }");
    
    buttonsLayout->addWidget(btnAdd);
    buttonsLayout->addWidget(btnEdit);
    buttonsLayout->addWidget(btnDelete);
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(btnSortNom);
    buttonsLayout->addWidget(btnSortPlaces);
    buttonsLayout->addWidget(btnSortNavires);
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(btnProposition);
    buttonsLayout->addWidget(btnSimulation);
    consulterLayout->addLayout(buttonsLayout);
    
    mainLayout->addWidget(pageConsulter);

    // Page Proposition
    pageProposition = new QWidget();
    QVBoxLayout *propLayout = new QVBoxLayout(pageProposition);
    QLabel *propTitle = new QLabel("💡 Trouver le meilleur quai", pageProposition);
    propTitle->setStyleSheet("font-size: 20px; font-weight: bold; padding: 10px; color: #03224c;");
    propLayout->addWidget(propTitle);
    
    QLabel *propDesc = new QLabel("Entrez le nombre de places requises pour trouver le meilleur quai disponible:", pageProposition);
    propDesc->setStyleSheet("padding: 5px; color: #555;");
    propLayout->addWidget(propDesc);
    
    QFormLayout *propForm = new QFormLayout();
    longueurBateauEdit = new QLineEdit(pageProposition);
    longueurBateauEdit->setPlaceholderText("Nombre de places requises");
    propForm->addRow("Places requises:", longueurBateauEdit);
    propLayout->addLayout(propForm);
    
    QPushButton *btnCalculer = new QPushButton("🔍 Rechercher", pageProposition);
    btnCalculer->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; padding: 10px 20px; border-radius: 5px; font-weight: bold; }");
    QPushButton *btnFermerProp = new QPushButton("✖️ Fermer", pageProposition);
    btnFermerProp->setStyleSheet("QPushButton { background-color: #ef4444; color: white; padding: 10px 20px; border-radius: 5px; font-weight: bold; }");
    QHBoxLayout *propButtonsLayout = new QHBoxLayout();
    propButtonsLayout->addWidget(btnCalculer);
    propButtonsLayout->addWidget(btnFermerProp);
    propButtonsLayout->addStretch();
    propLayout->addLayout(propButtonsLayout);
    
    resultatLabel = new QLabel("Les résultats s'afficheront ici...", pageProposition);
    resultatLabel->setStyleSheet("background: white; border: 2px dashed #94a3b8; border-radius: 8px; padding: 20px; font-size: 14px; min-height: 100px;");
    resultatLabel->setWordWrap(true);
    propLayout->addWidget(resultatLabel);
    propLayout->addStretch();
    
    mainLayout->addWidget(pageProposition);

    // Page Simulation
    pageSimulation = new QWidget();
    QVBoxLayout *simulLayout = new QVBoxLayout(pageSimulation);
    QLabel *simulTitle = new QLabel("🎮 Simulation d'occupation", pageSimulation);
    simulTitle->setStyleSheet("font-size: 20px; font-weight: bold; padding: 10px; color: #03224c;");
    simulLayout->addWidget(simulTitle);
    
    // Simulation controls
    QGroupBox *simulBox = new QGroupBox("Paramètres de Simulation", pageSimulation);
    QGridLayout *simulGrid = new QGridLayout(simulBox);
    
    spinJours = new QSpinBox(simulBox);
    spinJours->setRange(7, 90);
    spinJours->setValue(30);
    spinJours->setSuffix(" jours");
    
    spinMoyenneArrivees = new QDoubleSpinBox(simulBox);
    spinMoyenneArrivees->setRange(0.1, 20.0);
    spinMoyenneArrivees->setValue(2.5);
    spinMoyenneArrivees->setSuffix(" bateaux/jour");
    
    spinEcartType = new QDoubleSpinBox(simulBox);
    spinEcartType->setRange(0.1, 10.0);
    spinEcartType->setValue(1.5);
    
    checkVariationsJournalieres = new QCheckBox("Variations journalières", simulBox);
    checkVariationsJournalieres->setChecked(true);
    checkWeekend = new QCheckBox("Effet week-end", simulBox);
    checkEffetSaison = new QCheckBox("Effet saisonnier", simulBox);
    
    simulGrid->addWidget(new QLabel("Durée:"), 0, 0);
    simulGrid->addWidget(spinJours, 0, 1);
    simulGrid->addWidget(new QLabel("Moyenne:"), 0, 2);
    simulGrid->addWidget(spinMoyenneArrivees, 0, 3);
    simulGrid->addWidget(new QLabel("Écart-type:"), 1, 0);
    simulGrid->addWidget(spinEcartType, 1, 1);
    simulGrid->addWidget(checkVariationsJournalieres, 2, 0);
    simulGrid->addWidget(checkWeekend, 2, 1);
    simulGrid->addWidget(checkEffetSaison, 2, 2);
    
    simulLayout->addWidget(simulBox);
    
    // Simulation buttons
    QHBoxLayout *simulButtons = new QHBoxLayout();
    QPushButton *btnLancerSimul = new QPushButton("▶️ Lancer la simulation", pageSimulation);
    btnLancerSimul->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; padding: 10px 20px; border-radius: 5px; font-weight: bold; }");
    QPushButton *btnAleatoire = new QPushButton("🎲 Scénario aléatoire", pageSimulation);
    btnAleatoire->setStyleSheet("QPushButton { background-color: #9C27B0; color: white; padding: 10px 20px; border-radius: 5px; }");
    simulButtons->addWidget(btnLancerSimul);
    simulButtons->addWidget(btnAleatoire);
    QPushButton *btnFermerSimul = new QPushButton("✖️ Fermer", pageSimulation);
    btnFermerSimul->setStyleSheet("QPushButton { background-color: #ef4444; color: white; padding: 10px 20px; border-radius: 5px; font-weight: bold; }");
    simulButtons->addWidget(btnFermerSimul);
    simulButtons->addStretch();
    simulLayout->addLayout(simulButtons);
    
    progressSimulation = new QProgressBar(pageSimulation);
    progressSimulation->setVisible(false);
    simulLayout->addWidget(progressSimulation);
    
    // Histogram
    histoWidget = new MonHistogramme(pageSimulation);
    histoWidget->setMinimumHeight(200);
    simulLayout->addWidget(histoWidget);
    
    // Results
    lblResultatSimul = new QLabel("", pageSimulation);
    lblResultatSimul->setStyleSheet("background: #e3f2fd; padding: 15px; border-radius: 8px;");
    lblResultatSimul->setWordWrap(true);
    simulLayout->addWidget(lblResultatSimul);
    
    lblStatsDetaillees = new QLabel("", pageSimulation);
    lblStatsDetaillees->setStyleSheet("background: #fff3e0; padding: 15px; border-radius: 8px;");
    lblStatsDetaillees->setWordWrap(true);
    simulLayout->addWidget(lblStatsDetaillees);
    
    mainLayout->addWidget(pageSimulation);

    // Initially show the main table view
    pageConsulter->show();
    pageProposition->hide();
    pageSimulation->hide();

    // Connect signals
    connect(lineSearch, &QLineEdit::textChanged, this, &GestionQuai::onSearch);
    connect(btnAdd, &QPushButton::clicked, this, &GestionQuai::onAdd);
    connect(btnEdit, &QPushButton::clicked, this, &GestionQuai::onEdit);
    connect(btnDelete, &QPushButton::clicked, this, &GestionQuai::onDelete);
    connect(btnSortNom, &QPushButton::clicked, this, &GestionQuai::onSortByNom);
    connect(btnSortPlaces, &QPushButton::clicked, this, &GestionQuai::onSortByPlaces);
    connect(btnSortNavires, &QPushButton::clicked, this, &GestionQuai::onSortByNavires);
    connect(btnProposition, &QPushButton::clicked, this, &GestionQuai::showPropositionPage);
    connect(btnSimulation, &QPushButton::clicked, this, &GestionQuai::showSimulationPage);
    connect(btnCalculer, &QPushButton::clicked, this, &GestionQuai::onTrouverMeilleur);
    connect(btnLancerSimul, &QPushButton::clicked, this, &GestionQuai::onLancerSimulation);
    connect(btnAleatoire, &QPushButton::clicked, this, &GestionQuai::onScenarioAleatoire);
    connect(btnFermerProp, &QPushButton::clicked, this, &GestionQuai::showMainPage);
    connect(btnFermerSimul, &QPushButton::clicked, this, &GestionQuai::showMainPage);
}

void GestionQuai::showPropositionPage()
{
    pageConsulter->hide();
    pageProposition->show();
    pageSimulation->hide();
}

void GestionQuai::showSimulationPage()
{
    pageConsulter->hide();
    pageProposition->hide();
    pageSimulation->show();
}

void GestionQuai::showMainPage()
{
    pageConsulter->show();
    pageProposition->hide();
    pageSimulation->hide();
}

void GestionQuai::loadData()
{
    table->setRowCount(0);
    
    // Données de démonstration
    QStringList ids = {"Q001", "Q002", "Q003", "Q004", "Q005"};
    QStringList noms = {"Quai Central", "Quai Nord", "Quai Sud", "Quai Est", "Quai Ouest"};
    QStringList places = {"50", "40", "35", "45", "30"};
    QStringList navires = {"35", "38", "20", "42", "28"};
    QStringList adresses = {"Port Central", "Boulevard Nord", "Avenue Sud", "Rue Est", "Place Ouest"};
    QStringList travaux = {"Non", "Non", "Oui", "Non", "Non"};

    for (int i = 0; i < ids.size(); ++i) {
        int row = table->rowCount();
        table->insertRow(row);
        table->setItem(row, 0, new QTableWidgetItem(ids[i]));
        table->setItem(row, 1, new QTableWidgetItem(noms[i]));
        table->setItem(row, 2, new QTableWidgetItem(places[i]));
        table->setItem(row, 3, new QTableWidgetItem(navires[i]));
        table->setItem(row, 4, new QTableWidgetItem(adresses[i]));
        table->setItem(row, 5, new QTableWidgetItem(travaux[i]));
        
        QString etat = calculerEtat(places[i].toInt(), navires[i].toInt(), travaux[i] == "Oui");
        table->setItem(row, 6, new QTableWidgetItem(etat));
    }
    
    updateStats();
    emit dataChanged();
}

void GestionQuai::updateStats()
{
    int total = table->rowCount();
    int totalPlaces = 0;
    int totalNavires = 0;
    int dispos = 0;
    
    for (int i = 0; i < total; ++i) {
        int places = table->item(i, 2) ? table->item(i, 2)->text().toInt() : 0;
        int navires = table->item(i, 3) ? table->item(i, 3)->text().toInt() : 0;
        bool travaux = table->item(i, 5) ? (table->item(i, 5)->text() == "Oui") : false;
        
        totalPlaces += places;
        totalNavires += navires;
        
        if (!travaux && navires < places) {
            dispos++;
        }
    }
    
    int moyenne = totalPlaces > 0 ? (totalNavires * 100 / totalPlaces) : 0;
    
    statTotal->setText(QString("Total: %1").arg(total));
    statTotalPlaces->setText(QString("Places: %1").arg(totalPlaces));
    statTotalNavires->setText(QString("Navires: %1").arg(totalNavires));
    statMoyenneOccupation->setText(QString("Moyenne: %1%").arg(moyenne));
    statDispos->setText(QString("Disponibles: %1").arg(dispos));
}

GestionQuai::Summary GestionQuai::getSummary() const
{
    Summary s;
    if (!table) return s;

    s.total = table->rowCount();
    for (int i = 0; i < table->rowCount(); ++i) {
        int places = table->item(i, 2) ? table->item(i, 2)->text().toInt() : 0;
        int navires = table->item(i, 3) ? table->item(i, 3)->text().toInt() : 0;
        bool travaux = table->item(i, 5) ? (table->item(i, 5)->text() == "Oui") : false;
        s.totalPlaces += places;
        s.totalNavires += navires;
        if (!travaux && navires < places) s.dispos++;
        // Count states from column 6 if present
        QString etat = table->item(i, 6) ? table->item(i, 6)->text() : QString();
        if (etat == "Disponible") s.countDisponible++;
        else if (etat == "Critique") s.countCritique++;
        else if (etat == "Plein" || etat == "Complet") s.countPlein++;
        else if (etat == "En travaux") s.countTravaux++;
    }
    s.moyenne = s.totalPlaces > 0 ? (s.totalNavires * 100 / s.totalPlaces) : 0;
    return s;
}

QString GestionQuai::calculerEtat(int places, int navires, bool travaux)
{
    if (travaux) return "En travaux";
    if (navires >= places) return "Plein";
    double occupation = (double)navires / places * 100;
    if (occupation >= 80) return "Critique";
    if (occupation >= 50) return "Modéré";
    return "Disponible";
}

void GestionQuai::onSearch(const QString &text)
{
    for (int i = 0; i < table->rowCount(); ++i) {
        bool match = false;
        for (int j = 0; j < table->columnCount(); ++j) {
            QTableWidgetItem *item = table->item(i, j);
            if (item && item->text().contains(text, Qt::CaseInsensitive)) {
                match = true;
                break;
            }
        }
        table->setRowHidden(i, !match);
    }
}

void GestionQuai::onAdd()
{
    QMessageBox::information(this, "Ajouter", "Fonctionnalité d'ajout de quai à implémenter avec base de données.");
}

void GestionQuai::onEdit()
{
    int row = table->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Sélection requise", "Veuillez sélectionner un quai à modifier.");
        return;
    }
    
    QString id = table->item(row, 0)->text();
    QString nom = table->item(row, 1)->text();
    
    QMessageBox::information(this, "Modifier", "Modification du quai: " + nom + " (ID: " + id + ")");
}

void GestionQuai::onDelete()
{
    int row = table->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Sélection requise", "Veuillez sélectionner un quai à supprimer.");
        return;
    }
    
    QString nom = table->item(row, 1)->text();
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Confirmer", "Supprimer le quai \"" + nom + "\"?",
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        table->removeRow(row);
        updateStats();
        QMessageBox::information(this, "Succès", "Quai supprimé!");
        emit dataChanged();
    }
}

void GestionQuai::onSortByNom()
{
    table->sortItems(1, Qt::AscendingOrder);
    QMessageBox::information(this, "Tri", "Quais triés par Nom");
}

void GestionQuai::onSortByPlaces()
{
    table->sortItems(2, Qt::DescendingOrder);
    QMessageBox::information(this, "Tri", "Quais triés par Places");
}

void GestionQuai::onSortByNavires()
{
    table->sortItems(3, Qt::DescendingOrder);
    QMessageBox::information(this, "Tri", "Quais triés par Navires");
}

void GestionQuai::onTrouverMeilleur()
{
    if (table->rowCount() == 0) {
        resultatLabel->setText("<span style='color: red;'>Aucun quai disponible!</span>");
        return;
    }
    
    int placesRequises = longueurBateauEdit->text().toInt();
    if (placesRequises == 0) placesRequises = 1;
    
    int meilleurIndice = -1;
    int maxPlacesLibres = -1;
    
    for (int i = 0; i < table->rowCount(); ++i) {
        bool travaux = table->item(i, 5)->text() == "Oui";
        if (travaux) continue;
        
        int places = table->item(i, 2)->text().toInt();
        int navires = table->item(i, 3)->text().toInt();
        int placesLibres = places - navires;
        
        if (placesLibres >= placesRequises && placesLibres > maxPlacesLibres) {
            maxPlacesLibres = placesLibres;
            meilleurIndice = i;
        }
    }
    
    if (meilleurIndice < 0) {
        resultatLabel->setText(QString("<span style='color: red;'>Aucun quai disponible pour %1 places!</span>").arg(placesRequises));
        return;
    }
    
    QString nom = table->item(meilleurIndice, 1)->text();
    QString adresse = table->item(meilleurIndice, 4)->text();
    int places = table->item(meilleurIndice, 2)->text().toInt();
    int navires = table->item(meilleurIndice, 3)->text().toInt();
    
    QString resultat = QString(
        "<b style='color: green;'>Quai recommandé!</b><br><br>"
        "<b>Nom:</b> %1<br>"
        "<b>Adresse:</b> %2<br>"
        "<b>Places totales:</b> %3<br>"
        "<b>Navires actuels:</b> %4<br>"
        "<b>Places libres:</b> <b style='color: blue;'>%5</b>"
    ).arg(nom).arg(adresse).arg(places).arg(navires).arg(places - navires);
    
    resultatLabel->setText(resultat);
}

void GestionQuai::onLancerSimulation()
{
    int jours = spinJours->value();
    double moyenne = spinMoyenneArrivees->value();
    double ecartType = spinEcartType->value();
    bool variations = checkVariationsJournalieres->isChecked();
    bool weekend = checkWeekend->isChecked();
    bool saison = checkEffetSaison->isChecked();
    
    progressSimulation->setVisible(true);
    progressSimulation->setRange(0, jours);
    progressSimulation->setValue(0);
    
    QCoreApplication::processEvents();
    QVector<int> arrivees = simulerArriveesBateaux(jours, moyenne, ecartType, variations);
    
    if (weekend) {
        for (int i = 0; i < arrivees.size(); ++i) {
            QDate date = QDate::currentDate().addDays(i);
            if (date.dayOfWeek() == 6 || date.dayOfWeek() == 7) {
                arrivees[i] = qMax(0, arrivees[i] / 2);
            }
        }
    }
    
    if (saison) {
        for (int i = 0; i < arrivees.size(); ++i) {
            double s = sin(2 * 3.14159 * i / 90) * 0.3 + 1.0;
            arrivees[i] = qMax(0, (int)(arrivees[i] * s));
        }
    }
    
    QVector<QPair<QString, int>> donnees;
    for (int i = 0; i < qMin(jours, 30); ++i) {
        QString label = QString("J%1").arg(i + 1);
        donnees.append(qMakePair(label, arrivees[i]));
    }
    
    histoWidget->setDonnees(donnees);
    
    StatsSimulation stats = analyserResultats(arrivees);
    
    QString resultat = QString(
        "<b>Résultats de la Simulation</b><br><br>"
        "<b>Jours:</b> %1<br>"
        "<b>Jour occupation max:</b> Jour %2 (%3 navires)<br>"
        "<b>Moyenne:</b> %4%<br>"
        "<b>Jours critiques:</b> %5"
    ).arg(jours).arg(stats.jourMaxOccupation).arg(stats.maxOccupation)
     .arg(QString::number(stats.moyenneOccupation, 'f', 1)).arg(stats.joursCritiques);
    
    lblResultatSimul->setText(resultat);
    lblStatsDetaillees->setText(genererRecommandations(stats));
    
    progressSimulation->setValue(jours);
    QTimer::singleShot(500, [this]() { progressSimulation->setVisible(false); });
}

void GestionQuai::onScenarioAleatoire()
{
    QRandomGenerator *gen = QRandomGenerator::global();
    spinJours->setValue(gen->bounded(7, 91));
    spinMoyenneArrivees->setValue(gen->bounded(1, 10) + gen->bounded(10) / 10.0);
    spinEcartType->setValue(gen->bounded(1, 5) + gen->bounded(10) / 10.0);
    checkVariationsJournalieres->setChecked(gen->bounded(2) == 1);
    checkWeekend->setChecked(gen->bounded(2) == 1);
    checkEffetSaison->setChecked(gen->bounded(2) == 1);
    
    QMessageBox::information(this, "Aléatoire", "Paramètres de simulation générés!");
}

StatsSimulation GestionQuai::analyserResultats(const QVector<int> &arrivees)
{
    StatsSimulation stats;
    stats.joursSaturation = 0;
    stats.jourMaxOccupation = 0;
    stats.maxOccupation = 0;
    stats.moyenneOccupation = 0;
    stats.joursCritiques = 0;
    
    if (arrivees.isEmpty()) return stats;
    
    int total = 0;
    for (int i = 0; i < arrivees.size(); ++i) {
        total += arrivees[i];
        if (arrivees[i] > stats.maxOccupation) {
            stats.maxOccupation = arrivees[i];
            stats.jourMaxOccupation = i + 1;
        }
        if (arrivees[i] >= 40) stats.joursCritiques++;
    }
    
    stats.moyenneOccupation = (double)total / arrivees.size();
    return stats;
}

QVector<int> GestionQuai::simulerArriveesBateaux(int jours, double moyenne, double ecartType, bool variationsJournalieres)
{
    QVector<int> arrivees;
    QRandomGenerator *gen = QRandomGenerator::global();
    
    for (int i = 0; i < jours; ++i) {
        double valeur = moyenne;
        if (variationsJournalieres) {
            double aleatoire = (gen->bounded(10000) / 10000.0) * 2.0 - 1.0;
            valeur += aleatoire * ecartType;
        }
        valeur = qMax(0.0, valeur);
        arrivees.append((int)valeur);
    }
    
    return arrivees;
}

QString GestionQuai::genererRecommandations(const StatsSimulation &stats)
{
    QString rec = "<b>Recommandations:</b><br><br>";
    
    if (stats.joursCritiques > (int)stats.tendance.size() / 3) {
        rec += "⚠️ <b>Attention:</b> Nombre de jours critiques élevé.<br>";
        rec += "   Envisagez d'ajouter des quais.<br><br>";
    }
    
    if (stats.moyenneOccupation > 70) {
        rec += "📈 <b>Occupation élevée:</b> Moyenne > 70%.<br><br>";
    }
    
    if (stats.joursCritiques <= (int)stats.tendance.size() / 5 && stats.moyenneOccupation < 50) {
        rec += "✅ <b>Situation optimale.</b><br><br>";
    }
    
    rec += "<i>Basé sur les données de simulation.</i>";
    return rec;
}
