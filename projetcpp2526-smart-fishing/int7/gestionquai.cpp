#include "gestionquai.h"
#include "../quai.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QDate>
#include <QTextStream>
#include <QFile>
#include <QGroupBox>
#include <QFileDialog>
#include <QDir>
#include <QDesktopServices>
#include <QUrl>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QApplication>
#include <cmath>

// =====================================================================
// MonCamembert
// =====================================================================
MonCamembert::MonCamembert(QWidget *parent)
    : QWidget(parent), valeurOccupe(0), valeurDispo(1)
{
    setMinimumSize(220, 220);
}

void MonCamembert::setValeurs(int occupe, int dispo)
{
    valeurOccupe = occupe;
    valeurDispo  = dispo;
    update();
}

void MonCamembert::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    int total = valeurOccupe + valeurDispo;
    if (total == 0) return;

    QRect pieRect(30, 30, width() - 60, height() - 80);
    int angleOccupe = qRound(360.0 * valeurOccupe / total * 16);
    int angleDispo  = 360 * 16 - angleOccupe;

    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#ef4444"));
    p.drawPie(pieRect, 90 * 16, -angleOccupe);
    p.setBrush(QColor("#22c55e"));
    p.drawPie(pieRect, 90 * 16 - angleOccupe, -angleDispo);

    // Légende
    int ly = height() - 35;
    p.setBrush(QColor("#ef4444"));
    p.drawRect(10, ly, 12, 12);
    p.setPen(Qt::black);
    p.drawText(26, ly + 11, QString("Occupés: %1").arg(valeurOccupe));
    p.setBrush(QColor("#22c55e"));
    p.drawRect(140, ly, 12, 12);
    p.setPen(Qt::black);
    p.drawText(156, ly + 11, QString("Libres: %1").arg(valeurDispo));
}

// =====================================================================
// MonHistogramme
// =====================================================================
MonHistogramme::MonHistogramme(QWidget *parent)
    : QWidget(parent), showGrid(true), showValues(true)
{
    setMinimumSize(300, 200);
}

void MonHistogramme::setDonnees(const QVector<QPair<QString, int>> &data)
{
    donnees = data;
    update();
}

void MonHistogramme::setOptions(bool grid, bool values)
{
    showGrid   = grid;
    showValues = values;
    update();
}

void MonHistogramme::paintEvent(QPaintEvent *)
{
    if (donnees.isEmpty()) return;
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const int mL = 45, mB = 35, mT = 15, mR = 10;
    int W = width()  - mL - mR;
    int H = height() - mB - mT;

    p.fillRect(rect(), Qt::white);

    int maxVal = 0;
    for (const auto &d : donnees) maxVal = qMax(maxVal, d.second);
    if (maxVal == 0) maxVal = 1;

    // Grille et axes Y
    if (showGrid) {
        for (int i = 1; i <= 5; ++i) {
            int y = mT + H - H * i / 5;
            p.setPen(QPen(QColor("#e2e8f0"), 1, Qt::DashLine));
            p.drawLine(mL, y, mL + W, y);
            p.setPen(QColor("#94a3b8"));
            QFont f = p.font(); f.setPointSize(7); p.setFont(f);
            p.drawText(2, y + 4, QString::number(maxVal * i / 5) + "%");
        }
    }

    // Barres
    int n = donnees.size();
    if (n == 0) return;
    double barW = (double)W / n;
    for (int i = 0; i < n; ++i) {
        int val = donnees[i].second;
        int bH  = (int)((double)val / maxVal * H);
        int x   = mL + (int)(i * barW);
        int y   = mT + H - bH;

        QColor col = (val >= 90) ? QColor("#ef4444") :
                         (val >= 70) ? QColor("#f97316") :
                         QColor("#3b82f6");
        p.fillRect(x + 2, y, (int)barW - 4, bH, col);

        if (showValues && bH > 14) {
            p.setPen(Qt::white);
            QFont f = p.font(); f.setPointSize(7); p.setFont(f);
            p.drawText(x + 2, y + bH - 3, (int)barW - 4, 14,
                       Qt::AlignCenter, QString::number(val) + "%");
        }

        if (n <= 20 || i % qMax(1, n / 10) == 0) {
            p.setPen(QColor("#64748b"));
            QFont f = p.font(); f.setPointSize(6); p.setFont(f);
            p.drawText(x, mT + H + 3, (int)barW, 20,
                       Qt::AlignCenter, donnees[i].first);
        }
    }

    // Axes
    p.setPen(QPen(Qt::black, 1));
    p.drawLine(mL, mT, mL, mT + H);
    p.drawLine(mL, mT + H, mL + W, mT + H);
}

// =====================================================================
// PageQuai – Constructeur
// =====================================================================
PageQuai::PageQuai(QWidget *parent)
    : QWidget(parent)
{
    // -------- stacked widget --------
    pages = new QStackedWidget(this);

    pageAjouter           = new QWidget;
    pageModifier          = new QWidget;
    pageSupprimer         = new QWidget;
    pageConsulter         = new QWidget;
    pageStatistiques      = new QWidget;
    pageMaintenance       = new QWidget;
    pageRapportOccupation = new QWidget;
    pageExportCSV         = new QWidget;

    pages->addWidget(pageAjouter);
    pages->addWidget(pageModifier);
    pages->addWidget(pageSupprimer);
    pages->addWidget(pageConsulter);
    pages->addWidget(pageStatistiques);
    pages->addWidget(pageMaintenance);
    pages->addWidget(pageRapportOccupation);
    pages->addWidget(pageExportCSV);

    // -------- menu latéral --------
    QWidget *menuQuai = new QWidget;
    menuQuai->setObjectName("menuQuai");
    menuQuai->setFixedWidth(200);

    QPushButton *btnAdd     = new QPushButton("➕ Ajouter");
    QPushButton *btnEdit    = new QPushButton("✏️ Modifier");
    QPushButton *btnDelete  = new QPushButton("🗑️ Supprimer");
    QPushButton *btnView    = new QPushButton("📋 Consulter");
    QPushButton *btnStats   = new QPushButton("📊 Stats");
    QPushButton *btnMaint   = new QPushButton("🔧 Maintenance");
    QPushButton *btnRapport = new QPushButton("📈 Rapport");
    QPushButton *btnCSV     = new QPushButton("📥 Export CSV");

    btnMaint->setObjectName("btnMaint");
    btnRapport->setObjectName("btnRapport");
    btnCSV->setObjectName("btnPDF");

    QVBoxLayout *menuLayout = new QVBoxLayout(menuQuai);
    menuLayout->addWidget(btnAdd);
    menuLayout->addWidget(btnEdit);
    menuLayout->addWidget(btnDelete);
    menuLayout->addWidget(btnView);
    menuLayout->addWidget(btnStats);
    menuLayout->addWidget(btnMaint);
    menuLayout->addWidget(btnRapport);
    menuLayout->addWidget(btnCSV);
    menuLayout->addStretch();

    QWidget *mainFrame = new QWidget;
    mainFrame->setObjectName("pageFrame");
    QVBoxLayout *frameLayout = new QVBoxLayout(mainFrame);
    frameLayout->addWidget(pages);
    frameLayout->setContentsMargins(20, 20, 20, 20);

    QHBoxLayout *rootLayout = new QHBoxLayout(this);
    rootLayout->addWidget(menuQuai);
    rootLayout->addWidget(mainFrame);

    // ============================================================
    // PAGE AJOUTER
    // ============================================================
    {
        QLabel *titre = new QLabel("➕ Ajouter un Nouveau Quai");
        titre->setObjectName("pageTitle");
        QWidget *content = new QWidget; content->setObjectName("contentFrame");

        idEdit      = new QLineEdit; idEdit->setPlaceholderText("Ex: Q001");
        nomEdit     = new QLineEdit; nomEdit->setPlaceholderText("Ex: Quai Est");
        placesEdit  = new QLineEdit; placesEdit->setPlaceholderText("Nombre total de places");
        naviresEdit = new QLineEdit; naviresEdit->setPlaceholderText("Navires présents");
        adresseEdit = new QLineEdit; adresseEdit->setPlaceholderText("Adresse");
        travauxCheck    = new QCheckBox("En travaux");
        QPushButton *btnSave = new QPushButton("💾 Enregistrer");
        btnAnnulerAjout = new QPushButton("❌ Annuler");

        QHBoxLayout *btnRow = new QHBoxLayout;
        btnRow->addWidget(btnSave); btnRow->addWidget(btnAnnulerAjout); btnRow->addStretch();

        QFormLayout *form = new QFormLayout;
        form->addRow("ID :",             idEdit);
        form->addRow("Nom :",            nomEdit);
        form->addRow("Places totales :", placesEdit);
        form->addRow("Navires :",        naviresEdit);
        form->addRow("Adresse :",        adresseEdit);
        form->addRow("",                 travauxCheck);
        form->addRow(btnRow);

        QVBoxLayout *cl = new QVBoxLayout(content);
        cl->addLayout(form); cl->addStretch();

        QVBoxLayout *pl = new QVBoxLayout(pageAjouter);
        pl->addWidget(titre); pl->addWidget(content); pl->setContentsMargins(0,0,0,0);

        connect(btnSave,         &QPushButton::clicked, this, &PageQuai::ajouterQuai);
        connect(btnAnnulerAjout, &QPushButton::clicked, this, &PageQuai::annulerAjout);
    }

    // ============================================================
    // TABLES partagées
    // ============================================================
    QStringList hdrs = {"ID","Nom","Places","Navires","Adresse","Travaux","État"};
    tableConsulter = new QTableWidget(0, 7);
    tableSupprimer = new QTableWidget(0, 7);
    tableModifier  = new QTableWidget(0, 7);
    for (QTableWidget *t : {tableConsulter, tableSupprimer, tableModifier}) {
        t->setHorizontalHeaderLabels(hdrs);
        t->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        t->setAlternatingRowColors(true);
        t->setSelectionBehavior(QAbstractItemView::SelectRows);
        t->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }

    // ============================================================
    // PAGE CONSULTER
    // ============================================================
    {
        QLabel *titre = new QLabel("📋 Consultation des Quais");
        titre->setObjectName("pageTitle");
        QWidget *content = new QWidget; content->setObjectName("contentFrame");

        searchEdit = new QLineEdit; searchEdit->setPlaceholderText("🔍 Rechercher...");
        sortBox    = new QComboBox;
        sortBox->addItems({"Trier...", "Nom", "Places (croissant)", "Places (décroissant)",
                           "Navires (croissant)", "Navires (décroissant)", "État"});

        QHBoxLayout *searchRow = new QHBoxLayout;
        searchRow->addWidget(new QLabel("Recherche:")); searchRow->addWidget(searchEdit);
        searchRow->addSpacing(20); searchRow->addWidget(new QLabel("Trier:")); searchRow->addWidget(sortBox);

        QVBoxLayout *cl = new QVBoxLayout(content);
        cl->addLayout(searchRow); cl->addWidget(tableConsulter);

        QVBoxLayout *pl = new QVBoxLayout(pageConsulter);
        pl->addWidget(titre); pl->addWidget(content); pl->setContentsMargins(0,0,0,0);

        connect(searchEdit, &QLineEdit::textChanged, this, &PageQuai::rechercherQuai);
        connect(sortBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &PageQuai::trierQuais);
    }

    // ============================================================
    // PAGE SUPPRIMER
    // ============================================================
    {
        QLabel *titre = new QLabel("🗑️ Supprimer un Quai");
        titre->setObjectName("pageTitle");
        QWidget *content = new QWidget; content->setObjectName("contentFrame");

        QPushButton *btnDel = new QPushButton("🗑️ Supprimer la sélection");
        btnDel->setStyleSheet("background-color:#ef4444; color:white;");

        QVBoxLayout *cl = new QVBoxLayout(content);
        cl->addWidget(tableSupprimer); cl->addWidget(btnDel, 0, Qt::AlignCenter);

        QVBoxLayout *pl = new QVBoxLayout(pageSupprimer);
        pl->addWidget(titre); pl->addWidget(content); pl->setContentsMargins(0,0,0,0);

        connect(btnDel, &QPushButton::clicked, this, &PageQuai::supprimerQuai);
    }

    // ============================================================
    // PAGE MODIFIER
    // ============================================================
    {
        QLabel *titre = new QLabel("✏️ Modifier un Quai");
        titre->setObjectName("pageTitle");
        QWidget *content = new QWidget; content->setObjectName("contentFrame");
        QHBoxLayout *hl = new QHBoxLayout(content);

        hl->addWidget(tableModifier, 2);

        QWidget *formW = new QWidget;
        formW->setStyleSheet("background:white; border-radius:8px; padding:20px;");
        idEditM      = new QLineEdit; idEditM->setReadOnly(true);
        nomEditM     = new QLineEdit;
        placesEditM  = new QLineEdit;
        naviresEditM = new QLineEdit;
        adresseEditM = new QLineEdit;
        travauxCheckM   = new QCheckBox("En travaux");
        QPushButton *btnUpdate = new QPushButton("✔️ Modifier");
        btnAnnulerModif        = new QPushButton("❌ Annuler");

        QHBoxLayout *btnRow = new QHBoxLayout;
        btnUpdate->setMinimumHeight(38);
        btnAnnulerModif->setMinimumHeight(38);
        btnUpdate->setCursor(Qt::PointingHandCursor);
        btnAnnulerModif->setCursor(Qt::PointingHandCursor);
        btnUpdate->setStyleSheet("background-color:#22c55e; color:white; font-weight:bold; border:none; border-radius:6px; padding:8px 18px;");
        btnAnnulerModif->setStyleSheet("background-color:#ef4444; color:white; font-weight:bold; border:none; border-radius:6px; padding:8px 18px;");
        btnRow->addWidget(btnUpdate); btnRow->addWidget(btnAnnulerModif); btnRow->addStretch();

        QVBoxLayout *formContainer = new QVBoxLayout(formW);
        formContainer->setContentsMargins(0, 0, 0, 0);
        formContainer->setSpacing(16);

        QFormLayout *form = new QFormLayout;
        form->addRow("ID :",      idEditM);
        form->addRow("Nom :",     nomEditM);
        form->addRow("Places :",  placesEditM);
        form->addRow("Navires :", naviresEditM);
        form->addRow("Adresse :", adresseEditM);
        form->addRow("",          travauxCheckM);
        formContainer->addLayout(form);
        formContainer->addLayout(btnRow);
        formContainer->addStretch();

        hl->addWidget(formW, 1);

        QVBoxLayout *pl = new QVBoxLayout(pageModifier);
        pl->addWidget(titre); pl->addWidget(content); pl->setContentsMargins(0,0,0,0);

        connect(tableModifier,  &QTableWidget::cellClicked, this, &PageQuai::remplirFormulaireDepuisTable);
        connect(btnUpdate,      &QPushButton::clicked,      this, &PageQuai::modifierQuai);
        connect(btnAnnulerModif,&QPushButton::clicked,      this, &PageQuai::annulerModif);
        connect(placesEditM,    &QLineEdit::textChanged,    this, &PageQuai::onPlacesChanged);
        connect(naviresEditM,   &QLineEdit::textChanged,    this, &PageQuai::onNaviresChanged);
        connect(travauxCheckM,  &QCheckBox::stateChanged,   this, &PageQuai::onTravauxChanged);
    }

    // ============================================================
    // PAGE STATISTIQUES
    // ============================================================
    {
        QLabel *titre = new QLabel("📊 Statistiques");
        titre->setObjectName("pageTitle");
        QWidget *content = new QWidget; content->setObjectName("contentFrame");

        monGraphique = new MonCamembert;
        QLabel *txt  = new QLabel("Statistiques en temps réel");
        txt->setAlignment(Qt::AlignCenter);

        QVBoxLayout *cl = new QVBoxLayout(content);
        cl->addWidget(monGraphique); cl->addWidget(txt); cl->addStretch();

        QVBoxLayout *pl = new QVBoxLayout(pageStatistiques);
        pl->addWidget(titre); pl->addWidget(content); pl->setContentsMargins(0,0,0,0);
    }

    // ============================================================
    // PAGE MAINTENANCE
    // ============================================================
    {
        QLabel *titre = new QLabel("🔧 Planification des Maintenances");
        titre->setObjectName("pageTitle");
        QWidget *content = new QWidget; content->setObjectName("contentFrame");
        QVBoxLayout *cl = new QVBoxLayout(content);

        // Bandeau résumé
        lblResumeMaint = new QLabel("Aucune maintenance planifiée.");
        lblResumeMaint->setStyleSheet(
            "background:#fef3c7; border:1px solid #f59e0b; border-radius:6px; padding:8px; font-weight:bold;");
        lblResumeMaint->setWordWrap(true);
        cl->addWidget(lblResumeMaint);

        // Formulaire ajout
        QGroupBox *grpForm = new QGroupBox("➕ Planifier une maintenance");
        grpForm->setStyleSheet(
            "QGroupBox{font-weight:bold; border:1px solid #cbd5e1; border-radius:6px;"
            " margin-top:8px; padding-top:8px;}");
        QFormLayout *formMaint = new QFormLayout(grpForm);

        comboQuaiMaint      = new QComboBox;
        dateMaintenanceEdit = new QDateEdit(QDate::currentDate().addDays(7));
        dateMaintenanceEdit->setCalendarPopup(true);
        dateMaintenanceEdit->setMinimumDate(QDate::currentDate());

        comboTypeMaint = new QComboBox;
        comboTypeMaint->addItems({"Préventive","Corrective","Inspection générale","Nettoyage","Réparation structurelle"});

        comboPriorite = new QComboBox;
        comboPriorite->addItems({"Haute","Moyenne","Basse"});

        lineResponsable = new QLineEdit; lineResponsable->setPlaceholderText("Nom du responsable");
        lineNotesMaint  = new QLineEdit; lineNotesMaint->setPlaceholderText("Notes / description");

        formMaint->addRow("Quai :",      comboQuaiMaint);
        formMaint->addRow("Date :",      dateMaintenanceEdit);
        formMaint->addRow("Type :",      comboTypeMaint);
        formMaint->addRow("Priorité :",  comboPriorite);
        formMaint->addRow("Responsable :", lineResponsable);
        formMaint->addRow("Notes :",     lineNotesMaint);

        cl->addWidget(grpForm);

        // Boutons
        QPushButton *btnAjMaint  = new QPushButton("💾 Planifier");
        btnAjMaint->setStyleSheet("background:#0891b2; color:white; font-weight:bold; padding:6px 16px;");
        QPushButton *btnSupMaint = new QPushButton("🗑️ Supprimer sélection");
        btnSupMaint->setStyleSheet("background:#ef4444; color:white; padding:6px 16px;");
        QPushButton *btnExpMaint = new QPushButton("📥 Exporter liste");
        btnExpMaint->setStyleSheet("background:#059669; color:white; padding:6px 16px;");

        QHBoxLayout *btnsMaint = new QHBoxLayout;
        btnsMaint->addWidget(btnAjMaint); btnsMaint->addWidget(btnSupMaint);
        btnsMaint->addWidget(btnExpMaint); btnsMaint->addStretch();
        cl->addLayout(btnsMaint);

        // Filtre
        comboFiltreStatut = new QComboBox;
        comboFiltreStatut->addItems({"Toutes","Haute priorité","Moyenne priorité","Basse priorité",
                                     "Préventive","Corrective","À venir (7j)","À venir (30j)"});
        QHBoxLayout *filtreRow = new QHBoxLayout;
        filtreRow->addWidget(new QLabel("Filtrer :")); filtreRow->addWidget(comboFiltreStatut); filtreRow->addStretch();
        cl->addLayout(filtreRow);

        // Table
        tableMaintenance = new QTableWidget(0, 7);
        tableMaintenance->setHorizontalHeaderLabels(
            {"Quai","Date prévue","Type","Priorité","Responsable","Notes","Jours restants"});
        tableMaintenance->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        tableMaintenance->setAlternatingRowColors(true);
        tableMaintenance->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableMaintenance->setEditTriggers(QAbstractItemView::NoEditTriggers);
        cl->addWidget(tableMaintenance);

        QVBoxLayout *pl = new QVBoxLayout(pageMaintenance);
        pl->addWidget(titre); pl->addWidget(content); pl->setContentsMargins(0,0,0,0);

        connect(btnAjMaint,  &QPushButton::clicked, this, &PageQuai::ajouterMaintenance);
        connect(btnSupMaint, &QPushButton::clicked, this, &PageQuai::supprimerMaintenance);
        connect(btnExpMaint, &QPushButton::clicked, this, &PageQuai::exporterMaintenances);
        connect(comboFiltreStatut, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &PageQuai::filtrerMaintenances);
    }

    // ============================================================
    // PAGE RAPPORT D'OCCUPATION
    // ============================================================
    {
        QLabel *titre = new QLabel("📈 Rapport d'Occupation des Quais");
        titre->setObjectName("pageTitle");
        QWidget *content = new QWidget; content->setObjectName("contentFrame");
        QVBoxLayout *cl = new QVBoxLayout(content);

        lblAlertes = new QLabel;
        lblAlertes->setWordWrap(true);
        lblAlertes->setStyleSheet(
            "background:#fee2e2; border:1px solid #ef4444; border-radius:6px; padding:8px; font-weight:bold;");
        lblAlertes->setVisible(false);
        cl->addWidget(lblAlertes);

        lblSummaryRapport = new QLabel("Cliquez sur 'Générer le rapport' pour analyser l'occupation.");
        lblSummaryRapport->setWordWrap(true);
        lblSummaryRapport->setStyleSheet(
            "background:#f0f9ff; border:1px solid #0891b2; border-radius:6px; padding:8px;");
        cl->addWidget(lblSummaryRapport);

        QPushButton *btnGen   = new QPushButton("🔄 Générer le rapport");
        btnGen->setStyleSheet("background:#0891b2; color:white; font-weight:bold; padding:6px 16px;");
        comboFiltreRapport = new QComboBox;
        comboFiltreRapport->addItems({"Tous les quais","Disponibles uniquement","Complets / Saturés",
                                      "En travaux","Taux > 80%","Taux < 30%"});
        QPushButton *btnExpRap = new QPushButton("📥 Exporter CSV");
        btnExpRap->setStyleSheet("background:#059669; color:white; padding:6px 16px;");

        QHBoxLayout *btnsRow = new QHBoxLayout;
        btnsRow->addWidget(btnGen);
        btnsRow->addSpacing(10);
        btnsRow->addWidget(new QLabel("Filtrer :"));
        btnsRow->addWidget(comboFiltreRapport);
        btnsRow->addSpacing(10);
        btnsRow->addWidget(btnExpRap);
        btnsRow->addStretch();
        cl->addLayout(btnsRow);

        tableRapport = new QTableWidget(0, 7);
        tableRapport->setHorizontalHeaderLabels(
            {"ID","Nom","Places totales","Navires","Places libres","Taux (%)","Statut"});
        tableRapport->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        tableRapport->setAlternatingRowColors(true);
        tableRapport->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableRapport->setEditTriggers(QAbstractItemView::NoEditTriggers);
        cl->addWidget(tableRapport);

        cl->addWidget(new QLabel("<b>Histogramme des taux d'occupation :</b>"));
        histoRapport = new MonHistogramme;
        histoRapport->setMinimumHeight(200);
        histoRapport->setOptions(true, true);
        cl->addWidget(histoRapport);

        QVBoxLayout *pl = new QVBoxLayout(pageRapportOccupation);
        pl->addWidget(titre); pl->addWidget(content); pl->setContentsMargins(0,0,0,0);

        connect(btnGen,    &QPushButton::clicked, this, &PageQuai::genererRapport);
        connect(btnExpRap, &QPushButton::clicked, this, &PageQuai::exporterRapport);
        connect(comboFiltreRapport, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &PageQuai::filtrerRapportParEtat);
    }

    // ============================================================
    // PAGE EXPORT CSV
    // ============================================================
    {
        QLabel *titre = new QLabel("📊 Export CSV");
        titre->setObjectName("pageTitle");
        QWidget *content = new QWidget; content->setObjectName("contentFrame");
        QVBoxLayout *cl = new QVBoxLayout(content);

        nomFichierEdit = new QLineEdit;
        nomFichierEdit->setText("planning_quais_" + QDate::currentDate().toString("yyyy-MM-dd"));
        formatCSVBox = new QComboBox;
        formatCSVBox->addItems({"CSV Standard","CSV Excel","CSV avec ;"});

        QFormLayout *formCSV = new QFormLayout;
        formCSV->addRow("Nom fichier :", nomFichierEdit);
        formCSV->addRow("Format :",      formatCSVBox);
        cl->addLayout(formCSV);

        btnGenererCSV = new QPushButton("💾 Générer CSV");
        btnGenererCSV->setStyleSheet("background-color:#059669; color:white;");
        cl->addWidget(btnGenererCSV, 0, Qt::AlignCenter);

        lblPreviewCSV = new QLabel;
        lblPreviewCSV->setWordWrap(true);
        lblPreviewCSV->setAlignment(Qt::AlignCenter);
        lblPreviewCSV->setStyleSheet("background:white; border:2px solid #10b981; border-radius:10px; padding:15px;");
        cl->addSpacing(20);
        cl->addWidget(lblPreviewCSV);
        cl->addStretch();

        QVBoxLayout *pl = new QVBoxLayout(pageExportCSV);
        pl->addWidget(titre); pl->addWidget(content); pl->setContentsMargins(0,0,0,0);

        connect(btnGenererCSV, &QPushButton::clicked, this, &PageQuai::exporterCSV);
    }

    // ============================================================
    // CONNEXIONS DU MENU
    // ============================================================
    connect(btnAdd,     &QPushButton::clicked, this, &PageQuai::showAjouter);
    connect(btnEdit,    &QPushButton::clicked, this, &PageQuai::showModifier);
    connect(btnDelete,  &QPushButton::clicked, this, &PageQuai::showSupprimer);
    connect(btnView,    &QPushButton::clicked, this, &PageQuai::showConsulter);
    connect(btnStats,   &QPushButton::clicked, this, &PageQuai::showStatistiques);
    connect(btnMaint,   &QPushButton::clicked, this, &PageQuai::showMaintenance);
    connect(btnRapport, &QPushButton::clicked, this, &PageQuai::showRapportOccupation);
    connect(btnCSV,     &QPushButton::clicked, this, &PageQuai::showExportCSV);

    syncTables();
    pages->setCurrentWidget(pageConsulter);
}

// =====================================================================
// UTILITAIRES
// =====================================================================
QString PageQuai::calculerEtat(int places, int navires, bool travaux)
{
    if (travaux)             return "En travaux";
    if (navires >= places)   return "Complet";
    return "Disponible";
}

void PageQuai::syncTables()
{
    tableConsulter->setRowCount(0);
    tableSupprimer->setRowCount(0);
    tableModifier->setRowCount(0);
    comboQuaiMaint->clear();

    for (const Quai &q : listeQuais) {
        int r = tableConsulter->rowCount();
        tableConsulter->insertRow(r);
        tableConsulter->setItem(r, 0, new QTableWidgetItem(q.getId()));
        tableConsulter->setItem(r, 1, new QTableWidgetItem(q.getNom()));
        tableConsulter->setItem(r, 2, new QTableWidgetItem(QString::number(q.getPlacesTotales())));
        tableConsulter->setItem(r, 3, new QTableWidgetItem(QString::number(q.getNaviresPresents())));
        tableConsulter->setItem(r, 4, new QTableWidgetItem(q.getAdresse()));
        tableConsulter->setItem(r, 5, new QTableWidgetItem(q.getTravaux() ? "Oui" : "Non"));
        tableConsulter->setItem(r, 6, new QTableWidgetItem(q.getEtat()));

        tableSupprimer->insertRow(r);
        tableModifier->insertRow(r);
        for (int c = 0; c < 7; ++c) {
            tableSupprimer->setItem(r, c, new QTableWidgetItem(*tableConsulter->item(r, c)));
            tableModifier->setItem(r, c,  new QTableWidgetItem(*tableConsulter->item(r, c)));
        }
        comboQuaiMaint->addItem(q.getNom() + " (" + q.getId() + ")");
    }
}

void PageQuai::updateStats()
{
    int dispo = 0, occupe = 0;
    for (const Quai &q : listeQuais) {
        if (q.getEtat() == "Disponible") dispo++;
        else occupe++;
    }
    monGraphique->setValeurs(occupe, dispo);
}

// =====================================================================
// NAVIGATION
// =====================================================================
void PageQuai::showAjouter()   { pages->setCurrentWidget(pageAjouter); }
void PageQuai::showModifier()  { syncTables(); pages->setCurrentWidget(pageModifier); }
void PageQuai::showSupprimer() { syncTables(); pages->setCurrentWidget(pageSupprimer); }
void PageQuai::showConsulter() { syncTables(); pages->setCurrentWidget(pageConsulter); }
void PageQuai::showStatistiques(){ updateStats(); pages->setCurrentWidget(pageStatistiques); }
void PageQuai::showExportCSV() { pages->setCurrentWidget(pageExportCSV); }

void PageQuai::showMaintenance()
{
    syncTables();
    refreshTableMaintenance();
    pages->setCurrentWidget(pageMaintenance);
}

void PageQuai::showRapportOccupation()
{
    genererRapport();
    pages->setCurrentWidget(pageRapportOccupation);
}

// =====================================================================
// AJOUTER QUAI
// =====================================================================
void PageQuai::ajouterQuai()
{
    const QString id = idEdit->text().trimmed();
    const QString nom = nomEdit->text().trimmed();
    const QString adresse = adresseEdit->text().trimmed();

    if (id.isEmpty() || nom.isEmpty() ||
        placesEdit->text().trimmed().isEmpty() || naviresEdit->text().trimmed().isEmpty() ||
        adresse.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Tous les champs sont obligatoires.");
        return;
    }

    for (const Quai &q : listeQuais) {
        if (q.getId() == id) {
            QMessageBox::warning(this, "Erreur", "Cet ID existe déjà.");
            return;
        }
    }

    bool ok;
    int places = placesEdit->text().toInt(&ok);
    if (!ok || places <= 0) { QMessageBox::warning(this, "Erreur", "Nombre de places invalide."); return; }
    int navires = naviresEdit->text().toInt(&ok);
    if (!ok || navires < 0 || navires > places) { QMessageBox::warning(this, "Erreur", "Nombre de navires invalide."); return; }

    int idQuai = id.toInt(&ok);
    if (!ok) {
        QMessageBox::warning(this, "Erreur", "L'identifiant du quai doit être numérique pour l'enregistrement en base.");
        return;
    }

    QuaiDB quaiDb;
    quaiDb.id_quai = idQuai;
    quaiDb.nom = nom;
    quaiDb.places_total = places;
    quaiDb.navires_presents = navires;
    quaiDb.adresse = adresse;
    quaiDb.etat = travauxCheck->isChecked() ? "En travaux" :
                  (navires >= places ? "Complet" : "Disponible");

    if (quaiDb.existe()) {
        QMessageBox::warning(this, "Erreur", "Cet ID existe déjà dans la base de données.");
        return;
    }

    if (!quaiDb.ajout()) {
        QMessageBox::warning(this, "Erreur", "Impossible d'ajouter le quai dans la base de données.");
        return;
    }

    listeQuais.append(Quai(id, nom, places, navires, adresse, travauxCheck->isChecked()));
    syncTables();
    idEdit->clear(); nomEdit->clear(); placesEdit->clear(); naviresEdit->clear(); adresseEdit->clear();
    travauxCheck->setChecked(false);
    QMessageBox::information(this, "Succès", "Quai ajouté avec succès.");
}

// =====================================================================
// SUPPRIMER QUAI
// =====================================================================
void PageQuai::supprimerQuai()
{
    int r = tableSupprimer->currentRow();
    if (r < 0) { QMessageBox::warning(this, "Erreur", "Sélectionnez un quai."); return; }
    QString id = tableSupprimer->item(r, 0)->text();
    if (QMessageBox::question(this, "Confirmation", "Supprimer le quai " + id + " ?") == QMessageBox::Yes) {
        for (int i = 0; i < listeQuais.size(); ++i) {
            if (listeQuais[i].getId() == id) { listeQuais.removeAt(i); break; }
        }
        syncTables();
    }
    QuaiDB quai;
    quai.id_quai = id.toInt() ;
    quai.suprimer();
}

// =====================================================================
// MODIFIER QUAI
// =====================================================================
void PageQuai::remplirFormulaireDepuisTable(int r, int)
{
    idEditM->setText(tableModifier->item(r, 0)->text());
    nomEditM->setText(tableModifier->item(r, 1)->text());
    placesEditM->setText(tableModifier->item(r, 2)->text());
    naviresEditM->setText(tableModifier->item(r, 3)->text());
    adresseEditM->setText(tableModifier->item(r, 4)->text());
    travauxCheckM->setChecked(tableModifier->item(r, 5)->text() == "Oui");
}

void PageQuai::modifierQuai()
{
    int r = tableModifier->currentRow();
    if (r < 0) { QMessageBox::warning(this, "Erreur", "Sélectionnez un quai dans le tableau."); return; }
    if (nomEditM->text().trimmed().isEmpty() || placesEditM->text().trimmed().isEmpty() ||
        naviresEditM->text().trimmed().isEmpty() || adresseEditM->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Tous les champs sont obligatoires."); return;
    }
    bool ok;
    int places = placesEditM->text().toInt(&ok);
    if (!ok || places <= 0) { QMessageBox::warning(this, "Erreur", "Nombre de places invalide."); return; }
    int navires = naviresEditM->text().toInt(&ok);
    if (!ok || navires < 0 || navires > places) { QMessageBox::warning(this, "Erreur", "Nombre de navires invalide."); return; }

    QString id = idEditM->text().trimmed();
    int idQuai = id.toInt(&ok);
    if (!ok) {
        QMessageBox::warning(this, "Erreur", "L'identifiant du quai doit être numérique.");
        return;
    }

    if (QMessageBox::question(this, "Confirmation", "Confirmer la modification du quai " + id + " ?") != QMessageBox::Yes) {
        return;
    }

    QuaiDB quaiDb;
    quaiDb.id_quai = idQuai;
    quaiDb.nom = nomEditM->text().trimmed();
    quaiDb.places_total = places;
    quaiDb.navires_presents = navires;
    quaiDb.adresse = adresseEditM->text().trimmed();
    quaiDb.etat = travauxCheckM->isChecked() ? "En travaux" :
                  (navires >= places ? "Complet" : "Disponible");

    if (!quaiDb.modifier()) {
        QMessageBox::warning(this, "Erreur", "Impossible de modifier le quai dans la base de données.");
        return;
    }

    for (int i = 0; i < listeQuais.size(); ++i) {
        if (listeQuais[i].getId() == id) {
            listeQuais[i].setNom(nomEditM->text().trimmed());
            listeQuais[i].setPlacesTotales(places);
            listeQuais[i].setNaviresPresents(navires);
            listeQuais[i].setAdresse(adresseEditM->text().trimmed());
            listeQuais[i].setTravaux(travauxCheckM->isChecked());
            break;
        }
    }
    syncTables();
    QMessageBox::information(this, "Succès", "Quai modifié avec succès.");
}

void PageQuai::annulerAjout()
{
    idEdit->clear(); nomEdit->clear(); placesEdit->clear(); naviresEdit->clear(); adresseEdit->clear();
    travauxCheck->setChecked(false);
    pages->setCurrentWidget(pageConsulter);
}

void PageQuai::annulerModif()
{
    idEditM->clear(); nomEditM->clear(); placesEditM->clear(); naviresEditM->clear(); adresseEditM->clear();
    travauxCheckM->setChecked(false);
    pages->setCurrentWidget(pageConsulter);
}

void PageQuai::onPlacesChanged(const QString &)  {}
void PageQuai::onNaviresChanged(const QString &) {}
void PageQuai::onTravauxChanged(int)             {}

// =====================================================================
// RECHERCHE / TRI
// =====================================================================
void PageQuai::rechercherQuai(const QString &texte)
{
    QString t = texte.toLower();
    for (int r = 0; r < tableConsulter->rowCount(); ++r) {
        bool match = tableConsulter->item(r, 0)->text().toLower().contains(t) ||
                     tableConsulter->item(r, 1)->text().toLower().contains(t) ||
                     tableConsulter->item(r, 4)->text().toLower().contains(t);
        tableConsulter->setRowHidden(r, !match);
    }
}

void PageQuai::trierQuais(int index)
{
    if (index == 0) return;
    int col = 1; Qt::SortOrder order = Qt::AscendingOrder;
    switch (index) {
    case 1: col = 1; order = Qt::AscendingOrder;  break;
    case 2: col = 2; order = Qt::AscendingOrder;  break;
    case 3: col = 2; order = Qt::DescendingOrder; break;
    case 4: col = 3; order = Qt::AscendingOrder;  break;
    case 5: col = 3; order = Qt::DescendingOrder; break;
    case 6: col = 6; order = Qt::AscendingOrder;  break;
    default: return;
    }
    tableConsulter->sortItems(col, order);
}

// =====================================================================
// MAINTENANCE
// =====================================================================
void PageQuai::refreshTableMaintenance()
{
    tableMaintenance->setRowCount(0);
    int filtreIdx = comboFiltreStatut->currentIndex();
    QDate today   = QDate::currentDate();
    int haute = 0, proche7j = 0;

    for (const MaintenanceEntry &m : maintenances) {
        int joursR = today.daysTo(m.datePrevue);
        // Filtres
        if (filtreIdx == 1 && m.priorite != "Haute")             continue;
        if (filtreIdx == 2 && m.priorite != "Moyenne")           continue;
        if (filtreIdx == 3 && m.priorite != "Basse")             continue;
        if (filtreIdx == 4 && m.typeIntervention != "Préventive") continue;
        if (filtreIdx == 5 && m.typeIntervention != "Corrective") continue;
        if (filtreIdx == 6 && (joursR < 0 || joursR > 7))        continue;
        if (filtreIdx == 7 && (joursR < 0 || joursR > 30))       continue;

        int r = tableMaintenance->rowCount();
        tableMaintenance->insertRow(r);
        tableMaintenance->setItem(r, 0, new QTableWidgetItem(m.nomQuai));
        tableMaintenance->setItem(r, 1, new QTableWidgetItem(m.datePrevue.toString("dd/MM/yyyy")));
        tableMaintenance->setItem(r, 2, new QTableWidgetItem(m.typeIntervention));
        tableMaintenance->setItem(r, 3, new QTableWidgetItem(m.priorite));
        tableMaintenance->setItem(r, 4, new QTableWidgetItem(m.responsable));
        tableMaintenance->setItem(r, 5, new QTableWidgetItem(m.notes));

        QString joursStr;
        if      (joursR < 0)  joursStr = "⛔ Dépassé";
        else if (joursR == 0) joursStr = "🔴 Aujourd'hui";
        else if (joursR <= 7) joursStr = "🟠 " + QString::number(joursR) + " j";
        else                  joursStr = "🟢 " + QString::number(joursR) + " j";
        tableMaintenance->setItem(r, 6, new QTableWidgetItem(joursStr));

        // Couleur de ligne
        QColor bg = (m.priorite == "Haute")   ? QColor("#fee2e2") :
                        (m.priorite == "Moyenne")  ? QColor("#fef3c7") :
                        Qt::white;
        for (int c = 0; c < 7; ++c) tableMaintenance->item(r, c)->setBackground(bg);

        if (m.priorite == "Haute") haute++;
        if (joursR >= 0 && joursR <= 7) proche7j++;
    }

    // Résumé
    if (maintenances.isEmpty()) {
        lblResumeMaint->setText("✅ Aucune maintenance planifiée.");
    } else {
        QString resume = QString("📋 Total : %1 maintenance(s)").arg(maintenances.size());
        if (haute > 0)    resume += QString("  |  🔴 Haute priorité : %1").arg(haute);
        if (proche7j > 0) resume += QString("  |  ⚠️ Dans les 7 prochains jours : %1").arg(proche7j);
        lblResumeMaint->setText(resume);
    }
}

void PageQuai::ajouterMaintenance()
{
    if (listeQuais.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Aucun quai disponible. Ajoutez d'abord un quai."); return;
    }
    if (lineResponsable->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez saisir le responsable."); return;
    }
    int idx = comboQuaiMaint->currentIndex();
    if (idx < 0 || idx >= listeQuais.size()) return;

    MaintenanceEntry entry;
    entry.idQuai           = listeQuais[idx].getId();
    entry.nomQuai          = listeQuais[idx].getNom();
    entry.datePrevue       = dateMaintenanceEdit->date();
    entry.typeIntervention = comboTypeMaint->currentText();
    entry.priorite         = comboPriorite->currentText();
    entry.responsable      = lineResponsable->text().trimmed();
    entry.notes            = lineNotesMaint->text().trimmed();

    maintenances.append(entry);
    lineResponsable->clear();
    lineNotesMaint->clear();
    refreshTableMaintenance();
    QMessageBox::information(this, "Succès", "Maintenance planifiée avec succès !");
}

void PageQuai::supprimerMaintenance()
{
    int r = tableMaintenance->currentRow();
    if (r < 0) { QMessageBox::warning(this, "Erreur", "Sélectionnez une ligne."); return; }
    QString nomQ = tableMaintenance->item(r, 0)->text();
    QString dateS= tableMaintenance->item(r, 1)->text();
    for (int i = 0; i < maintenances.size(); ++i) {
        if (maintenances[i].nomQuai == nomQ &&
            maintenances[i].datePrevue.toString("dd/MM/yyyy") == dateS) {
            maintenances.removeAt(i); break;
        }
    }
    refreshTableMaintenance();
    QMessageBox::information(this, "Succès", "Maintenance supprimée.");
}

void PageQuai::filtrerMaintenances(int) { refreshTableMaintenance(); }

void PageQuai::exporterMaintenances()
{
    if (maintenances.isEmpty()) { QMessageBox::warning(this, "Erreur", "Aucune maintenance à exporter."); return; }
    QString fileName = QFileDialog::getSaveFileName(this, "Exporter maintenances",
                                                    QDir::homePath() + "/maintenances_quais.csv", "CSV (*.csv)");
    if (fileName.isEmpty()) return;
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Erreur", "Impossible de créer le fichier."); return;
    }
    QTextStream out(&file);
    out << "Quai,Date prevue,Type,Priorite,Responsable,Notes\n";
    for (const MaintenanceEntry &m : maintenances)
        out << m.nomQuai << "," << m.datePrevue.toString("dd/MM/yyyy") << ","
            << m.typeIntervention << "," << m.priorite << ","
            << m.responsable << "," << m.notes << "\n";
    file.close();
    QMessageBox::information(this, "Succès", "Liste exportée !");
    if (QMessageBox::question(this, "Ouvrir", "Ouvrir le fichier ?") == QMessageBox::Yes)
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
}

// =====================================================================
// RAPPORT D'OCCUPATION
// =====================================================================
void PageQuai::genererRapport()
{
    rapportData.clear();
    tableRapport->setRowCount(0);

    int totalPlaces = 0, totalNavires = 0;
    int nbDispo = 0, nbSatures = 0, nbCritiques = 0, nbTravaux = 0;
    QStringList alertes;

    for (const Quai &q : listeQuais) {
        RapportOccupation ro;
        ro.idQuai          = q.getId();
        ro.nomQuai         = q.getNom();
        ro.placesTotales   = q.getPlacesTotales();
        ro.naviresPresents = q.getNaviresPresents();
        ro.placesLibres    = q.getPlacesTotales() - q.getNaviresPresents();
        ro.etat            = q.getEtat();
        ro.tauxOccupation  = (ro.placesTotales > 0)
                                ? (double)ro.naviresPresents / ro.placesTotales * 100.0
                                : 0.0;
        ro.alerte = "";

        if (q.getTravaux()) {
            ro.alerte = "En travaux"; nbTravaux++;
        } else if (ro.tauxOccupation >= 100.0) {
            ro.alerte = "Saturé"; nbSatures++;
            alertes << QString("🔴 %1 : SATURÉ (%2/%3 places)")
                           .arg(q.getNom()).arg(ro.naviresPresents).arg(ro.placesTotales);
        } else if (ro.tauxOccupation >= 80.0) {
            ro.alerte = "Critique"; nbCritiques++;
            alertes << QString("🟠 %1 : taux critique (%2%)")
                           .arg(q.getNom()).arg(QString::number(ro.tauxOccupation, 'f', 1));
        } else {
            nbDispo++;
        }
        totalPlaces  += ro.placesTotales;
        totalNavires += ro.naviresPresents;
        rapportData.append(ro);
    }

    double tauxGlobal = (totalPlaces > 0) ? (double)totalNavires / totalPlaces * 100.0 : 0.0;
    lblSummaryRapport->setText(
        QString("<b>📊 Résumé :</b>  Quais : %1  |  Places : %2  |  Navires : %3  |  "
                "Taux global : %4%  |  🟢 %5 dispo  |  🔴 %6 saturés  |  🟠 %7 critiques  |  🔧 %8 travaux")
            .arg(listeQuais.size()).arg(totalPlaces).arg(totalNavires)
            .arg(QString::number(tauxGlobal,'f',1))
            .arg(nbDispo).arg(nbSatures).arg(nbCritiques).arg(nbTravaux));

    if (!alertes.isEmpty()) {
        lblAlertes->setText("⚠️ ALERTES :\n" + alertes.join("\n"));
        lblAlertes->setVisible(true);
    } else {
        lblAlertes->setVisible(false);
    }

    refreshTableRapport();
}

void PageQuai::refreshTableRapport()
{
    tableRapport->setRowCount(0);
    int filtreIdx = comboFiltreRapport->currentIndex();
    QVector<QPair<QString, int>> histoData;

    for (const RapportOccupation &ro : rapportData) {
        if (filtreIdx == 1 && ro.etat  != "Disponible")       continue;
        if (filtreIdx == 2 && ro.alerte != "Saturé")          continue;
        if (filtreIdx == 3 && ro.etat  != "En travaux")       continue;
        if (filtreIdx == 4 && ro.tauxOccupation <= 80.0)      continue;
        if (filtreIdx == 5 && ro.tauxOccupation >= 30.0)      continue;

        int r = tableRapport->rowCount();
        tableRapport->insertRow(r);
        tableRapport->setItem(r, 0, new QTableWidgetItem(ro.idQuai));
        tableRapport->setItem(r, 1, new QTableWidgetItem(ro.nomQuai));
        tableRapport->setItem(r, 2, new QTableWidgetItem(QString::number(ro.placesTotales)));
        tableRapport->setItem(r, 3, new QTableWidgetItem(QString::number(ro.naviresPresents)));
        tableRapport->setItem(r, 4, new QTableWidgetItem(QString::number(ro.placesLibres)));
        tableRapport->setItem(r, 5, new QTableWidgetItem(QString::number(ro.tauxOccupation,'f',1) + " %"));

        QString statut;
        QColor  bg;
        if      (ro.alerte == "Saturé")   { statut = "🔴 Saturé";      bg = QColor("#fee2e2"); }
        else if (ro.alerte == "Critique") { statut = "🟠 Critique";     bg = QColor("#fef3c7"); }
        else if (ro.etat == "En travaux") { statut = "🔧 En travaux";   bg = QColor("#f3f4f6"); }
        else                              { statut = "🟢 Disponible";   bg = QColor("#f0fdf4"); }
        tableRapport->setItem(r, 6, new QTableWidgetItem(statut));
        for (int c = 0; c < 7; ++c) tableRapport->item(r, c)->setBackground(bg);

        histoData.append(qMakePair(ro.nomQuai, (int)ro.tauxOccupation));
    }
    histoRapport->setDonnees(histoData);
}

void PageQuai::filtrerRapportParEtat(int) { refreshTableRapport(); }

void PageQuai::exporterRapport()
{
    if (rapportData.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Générez d'abord le rapport."); return;
    }
    QString fileName = QFileDialog::getSaveFileName(this, "Exporter rapport",
                                                    QDir::homePath() + "/rapport_occupation_" + QDate::currentDate().toString("yyyy-MM-dd") + ".csv",
                                                    "CSV (*.csv)");
    if (fileName.isEmpty()) return;
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Erreur", "Impossible de créer le fichier."); return;
    }
    QTextStream out(&file);
    out << "# Rapport d'occupation - " << QDate::currentDate().toString("dd/MM/yyyy") << "\n";
    out << "ID,Nom,Places totales,Navires,Places libres,Taux (%),Etat\n";
    for (const RapportOccupation &ro : rapportData)
        out << ro.idQuai << "," << ro.nomQuai << "," << ro.placesTotales << ","
            << ro.naviresPresents << "," << ro.placesLibres << ","
            << QString::number(ro.tauxOccupation,'f',1) << "," << ro.etat << "\n";
    file.close();
    QMessageBox::information(this, "Succès", "Rapport exporté !");
    if (QMessageBox::question(this, "Ouvrir", "Ouvrir le fichier ?") == QMessageBox::Yes)
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
}

// =====================================================================
// EXPORT CSV GÉNÉRAL
// =====================================================================
QString PageQuai::genererContenuCSV()
{
    int dispo = 0, comp = 0, trav = 0;
    for (const Quai &q : listeQuais) {
        if      (q.getEtat() == "Disponible") dispo++;
        else if (q.getEtat() == "Complet")    comp++;
        else                                  trav++;
    }
    QString sep = (formatCSVBox->currentText() == "CSV avec ;") ? ";" : ",";
    QString csv;
    csv += "# Planning des Quais\n";
    csv += "# Date: " + QDate::currentDate().toString("dd/MM/yyyy") + "\n";
    csv += QString("# Total: %1 | Disponibles: %2 | Complets: %3 | Travaux: %4\n\n")
               .arg(listeQuais.size()).arg(dispo).arg(comp).arg(trav);
    csv += QStringList{"ID","Nom","Places","Navires","Adresse","Travaux","Etat"}.join(sep) + "\n";
    for (const Quai &q : listeQuais) {
        QStringList row;
        row << q.getId() << q.getNom()
            << QString::number(q.getPlacesTotales()) << QString::number(q.getNaviresPresents())
            << q.getAdresse() << (q.getTravaux() ? "Oui" : "Non") << q.getEtat();
        for (QString &s : row)
            if (s.contains(sep) || s.contains("\""))
                s = "\"" + s.replace("\"","\"\"") + "\"";
        csv += row.join(sep) + "\n";
    }
    return csv;
}

void PageQuai::exporterCSV()
{
    if (listeQuais.isEmpty()) { QMessageBox::warning(this,"Erreur","Aucun quai à exporter."); return; }
    QString defName = QDir::homePath() + "/" + nomFichierEdit->text() + ".csv";
    QString fileName = QFileDialog::getSaveFileName(this,"Enregistrer CSV", defName,"CSV (*.csv)");
    if (fileName.isEmpty()) return;
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this,"Erreur","Impossible de créer le fichier."); return;
    }
    QTextStream out(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
    out.setEncoding(QStringConverter::Utf8);
#else
    out.setCodec("UTF-8");
#endif
    out << genererContenuCSV();
    file.close();
    lblPreviewCSV->setText("✅ Fichier exporté : " + fileName);
    QMessageBox::information(this,"Succès","CSV exporté avec succès.");
    if (QMessageBox::question(this,"Ouvrir","Ouvrir le fichier ?") == QMessageBox::Yes)
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
}

void PageQuai::loadFromDb()
{
    listeQuais.clear();

    QSqlQuery query;
    if (!query.exec("SELECT ID_QUAI, NOM, PLACES_TOTAL, NAVIRES_PRESENTS, ADRESSE, ETAT FROM QUAI ORDER BY ID_QUAI")) {
        qDebug() << "Quai load error:" << query.lastError().text();
        syncTables();
        updateStats();
        return;
    }

    while (query.next()) {
        const QString id = query.value(0).toString();
        const QString nom = query.value(1).toString();
        const int places = query.value(2).toInt();
        const int navires = query.value(3).toInt();
        const QString adresse = query.value(4).toString();
        const QString etat = query.value(5).toString().trimmed();
        const bool travaux = etat.compare("En travaux", Qt::CaseInsensitive) == 0;

        listeQuais.append(Quai(id, nom, places, navires, adresse, travaux));
    }

    syncTables();
    updateStats();
}

PageQuai::QuaisStats PageQuai::getStats() const
{
    QuaisStats stats{listeQuais.size(), 0};
    for (const Quai &quai : listeQuais) {
        stats.totalNaviresPresent += quai.getNaviresPresents();
    }
    return stats;
}
