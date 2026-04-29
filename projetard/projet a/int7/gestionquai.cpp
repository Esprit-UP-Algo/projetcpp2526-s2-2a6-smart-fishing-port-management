#include "gestionquai.h"
#include "../quai.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QDate>
#include <QTextStream>
#include <QFile>
#include <QFileDialog>
#include <QDir>
#include <QDesktopServices>
#include <QUrl>
#include <QDialog>
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QApplication>
#include <QRegularExpression>
#include <cmath>
#include "../qrcode.hpp"

namespace {

bool quaiColumnExists(const QString &columnName)
{
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM USER_TAB_COLUMNS WHERE TABLE_NAME = 'QUAI' AND COLUMN_NAME = :column_name");
    query.bindValue(":column_name", columnName.toUpper());

    if (!query.exec() || !query.next()) {
        qDebug() << "Unable to inspect QUAI columns:" << query.lastError().text();
        return false;
    }

    return query.value(0).toInt() > 0;
}

void ensureQuaiDateColumns()
{
    const bool hasDateDebut = quaiColumnExists("DATE_DEBUT");
    const bool hasDateFin = quaiColumnExists("DATE_FIN");

    if (hasDateDebut && hasDateFin) {
        return;
    }

    QStringList missingColumns;
    if (!hasDateDebut) {
        missingColumns << "date_debut DATE";
    }
    if (!hasDateFin) {
        missingColumns << "date_fin DATE";
    }

    QSqlQuery alterQuery;
    const QString sql = QString("ALTER TABLE quai ADD (%1)").arg(missingColumns.join(", "));
    if (!alterQuery.exec(sql)) {
        qDebug() << "Unable to alter QUAI table:" << alterQuery.lastError().text();
    }
}

QDate resolvedDateOrToday(const QDate &value)
{
    return value.isValid() ? value : QDate::currentDate();
}

QDate parseOracleDateText(const QVariant &value)
{
    const QString brut = value.toString();
    QString texte;
    for (int i = 0; i < brut.size(); ++i) {
        const QChar c = brut.at(i);
        if (c.isDigit() || c == '-' || c == '/' || c == ' ' || c == ':' || c == 'T') {
            texte += c;
        }
    }
    texte = texte.trimmed();

    QRegularExpression isoRegex("(\\d{4})-(\\d{2})-(\\d{2})");
    QRegularExpressionMatch isoMatch = isoRegex.match(texte);
    if (isoMatch.hasMatch()) {
        return QDate(isoMatch.captured(1).toInt(),
                     isoMatch.captured(2).toInt(),
                     isoMatch.captured(3).toInt());
    }

    QRegularExpression frRegex("(\\d{2})[/-](\\d{2})[/-](\\d{2,4})");
    QRegularExpressionMatch frMatch = frRegex.match(texte);
    if (frMatch.hasMatch()) {
        int year = frMatch.captured(3).toInt();
        if (frMatch.captured(3).size() == 2) {
            year += 2000;
        }
        return QDate(year,
                     frMatch.captured(2).toInt(),
                     frMatch.captured(1).toInt());
    }

    return QDate();
}

}

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
// MonHistogramme (gardé pour les stats)
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
    pages = new QStackedWidget(this);

    pageAjouter      = new QWidget;
    pageModifier     = new QWidget;
    pageSupprimer    = new QWidget;
    pageConsulter    = new QWidget;
    pageStatistiques = new QWidget;
    pagePlanning     = new QWidget;
    pageAffectation  = new QWidget;
    pageExportCSV    = new QWidget;

    pages->addWidget(pageAjouter);
    pages->addWidget(pageModifier);
    pages->addWidget(pageSupprimer);
    pages->addWidget(pageConsulter);
    pages->addWidget(pageStatistiques);
    pages->addWidget(pagePlanning);
    pages->addWidget(pageAffectation);
    pages->addWidget(pageExportCSV);

    // Menu latéral
    QWidget *menuQuai = new QWidget;
    menuQuai->setObjectName("menuQuai");
    menuQuai->setFixedWidth(200);

    QPushButton *btnAdd        = new QPushButton("➕ Ajouter");
    QPushButton *btnEdit       = new QPushButton("✏️ Modifier");
    QPushButton *btnDelete     = new QPushButton("🗑️ Supprimer");
    QPushButton *btnView       = new QPushButton("📋 Consulter");
    QPushButton *btnStats      = new QPushButton("📊 Stats");
    QPushButton *btnPlanning   = new QPushButton("📅 Planning");
    QPushButton *btnAffectation = new QPushButton("⚡ Affectation");
    QPushButton *btnCSV        = new QPushButton("📥 Export CSV");

    btnPlanning->setObjectName("btnPlanning");
    btnAffectation->setObjectName("btnAffectation");
    btnCSV->setObjectName("btnPDF");

    QVBoxLayout *menuLayout = new QVBoxLayout(menuQuai);
    menuLayout->addWidget(btnAdd);
    menuLayout->addWidget(btnEdit);
    menuLayout->addWidget(btnDelete);
    menuLayout->addWidget(btnView);
    menuLayout->addWidget(btnStats);
    menuLayout->addWidget(btnPlanning);
    menuLayout->addWidget(btnAffectation);
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
        travauxCheck = new QCheckBox("En travaux");
        dateDebutLabel = new QLabel("Date debut :");
        dateDebutEdit = new QDateEdit(QDate::currentDate()); dateDebutEdit->setCalendarPopup(true); dateDebutEdit->setDisplayFormat("dd/MM/yyyy");
        dateFinLabel = new QLabel("Date fin :");
        dateFinEdit = new QDateEdit(QDate::currentDate()); dateFinEdit->setCalendarPopup(true); dateFinEdit->setDisplayFormat("dd/MM/yyyy");
        tirantEdit      = new QDoubleSpinBox; tirantEdit->setRange(0,30); tirantEdit->setSuffix(" m"); tirantEdit->setValue(0);
        equipementsEdit = new QLineEdit; equipementsEdit->setPlaceholderText("Ex: Grue,Carburant,Electricite");

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
        form->addRow(dateDebutLabel,     dateDebutEdit);
        form->addRow(dateFinLabel,       dateFinEdit);
        form->addRow("Tirant d'eau max (m) :", tirantEdit);
        form->addRow("Équipements :",    equipementsEdit);
        form->addRow("",                 travauxCheck);
        form->addRow(btnRow);

        QVBoxLayout *cl = new QVBoxLayout(content);
        cl->addLayout(form); cl->addStretch();

        QVBoxLayout *pl = new QVBoxLayout(pageAjouter);
        pl->addWidget(titre); pl->addWidget(content); pl->setContentsMargins(0,0,0,0);

        connect(btnSave,         &QPushButton::clicked, this, &PageQuai::ajouterQuai);
        connect(btnAnnulerAjout, &QPushButton::clicked, this, &PageQuai::annulerAjout);
        connect(travauxCheck,    &QCheckBox::stateChanged, this, &PageQuai::onTravauxChanged);
        updateTravauxDateVisibility(false);
    }

    // ============================================================
    // TABLES – 9 colonnes (ID,Nom,Places,Navires,Adresse,Travaux,État,Tirant,Équipements)
    // ============================================================
    QStringList hdrs = {"ID","Nom","Places","Navires","Adresse","Travaux","État","Tirant (m)","Équipements"};
    tableConsulter = new QTableWidget(0, 9);
    tableSupprimer = new QTableWidget(0, 9);
    tableModifier  = new QTableWidget(0, 9);
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
        connect(tableConsulter, &QTableWidget::cellDoubleClicked, this, &PageQuai::onTableConsulterDoubleClicked);
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
        travauxCheckM = new QCheckBox("En travaux");
        dateDebutLabelM = new QLabel("Date debut :");
        dateDebutEditM = new QDateEdit(QDate::currentDate()); dateDebutEditM->setCalendarPopup(true); dateDebutEditM->setDisplayFormat("dd/MM/yyyy");
        dateFinLabelM = new QLabel("Date fin :");
        dateFinEditM = new QDateEdit(QDate::currentDate()); dateFinEditM->setCalendarPopup(true); dateFinEditM->setDisplayFormat("dd/MM/yyyy");
        tirantEditM     = new QDoubleSpinBox; tirantEditM->setRange(0,30); tirantEditM->setSuffix(" m");
        equipementsEditM = new QLineEdit;
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
        form->addRow(dateDebutLabelM, dateDebutEditM);
        form->addRow(dateFinLabelM, dateFinEditM);
        form->addRow("Tirant d'eau max (m) :", tirantEditM);
        form->addRow("Équipements :", equipementsEditM);
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
        updateTravauxDateVisibility(true);
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
    // PAGE PLANNING (intégration du widget Planning)
    // ============================================================
    {
        planningWidget = new PlanningWidget;
        QVBoxLayout *pl = new QVBoxLayout(pagePlanning);
        pl->addWidget(planningWidget);
        pl->setContentsMargins(0,0,0,0);
    }

    // ============================================================
    // PAGE AFFECTATION (intégration du widget Affectation)
    // ============================================================
    {
        affectationWidget = new AffectationWidget;
        QVBoxLayout *pl = new QVBoxLayout(pageAffectation);
        pl->addWidget(affectationWidget);
        pl->setContentsMargins(0,0,0,0);
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

    // Connexions des boutons du menu
    connect(btnAdd,        &QPushButton::clicked, this, &PageQuai::showAjouter);
    connect(btnEdit,       &QPushButton::clicked, this, &PageQuai::showModifier);
    connect(btnDelete,     &QPushButton::clicked, this, &PageQuai::showSupprimer);
    connect(btnView,       &QPushButton::clicked, this, &PageQuai::showConsulter);
    connect(btnStats,      &QPushButton::clicked, this, &PageQuai::showStatistiques);
    connect(btnPlanning,   &QPushButton::clicked, this, &PageQuai::showPlanning);
    connect(btnAffectation,&QPushButton::clicked, this, &PageQuai::showAffectation);
    connect(btnCSV,        &QPushButton::clicked, this, &PageQuai::showExportCSV);

    syncTables();
    pages->setCurrentWidget(pageConsulter);




    int ret=a.connect_arduino(); // lancer la connexion à arduino
    switch(ret){
    case(0):qDebug()<< "arduino is available and connected to : "<< a.getarduino_port_name();
        break;
    case(1):qDebug() << "arduino is available but not connected to :" <<a.getarduino_port_name();
        break;
    case(-1):qDebug() << "arduino is not available";
    }
    QObject::connect(a.getserial(),SIGNAL(readyRead()),this,SLOT(update_label()));
}

void PageQuai::update_label()
{

    data=a.read_from_arduino();
    qDebug() << id ;
    QuaiDB q;
    if(q.QuaiLibre(id))
    {
        a.write_to_arduino("1");
        loadFromDb();
    }
    else
    {
        a.write_to_arduino("0");
    }

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

void PageQuai::updateTravauxDateVisibility(bool formulaireModification)
{
    QLabel *debutLabel = formulaireModification ? dateDebutLabelM : dateDebutLabel;
    QDateEdit *debutEdit = formulaireModification ? dateDebutEditM : dateDebutEdit;
    QLabel *finLabel = formulaireModification ? dateFinLabelM : dateFinLabel;
    QDateEdit *finEdit = formulaireModification ? dateFinEditM : dateFinEdit;
    QCheckBox *travaux = formulaireModification ? travauxCheckM : travauxCheck;

    const bool visible = travaux && travaux->isChecked();

    if (debutLabel) debutLabel->setVisible(visible);
    if (debutEdit) debutEdit->setVisible(visible);
    if (finLabel) finLabel->setVisible(visible);
    if (finEdit) finEdit->setVisible(visible);

    if (!visible) {
        const QDate today = QDate::currentDate();
        if (debutEdit) debutEdit->setDate(today);
        if (finEdit) finEdit->setDate(today);
    }
}

void PageQuai::syncTables()
{
    tableConsulter->setRowCount(0);
    tableSupprimer->setRowCount(0);
    tableModifier->setRowCount(0);

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
        tableConsulter->setItem(r, 7, new QTableWidgetItem(QString::number(q.getTirantEauMax(), 'f', 1)));
        tableConsulter->setItem(r, 8, new QTableWidgetItem(q.getEquipements()));

        tableSupprimer->insertRow(r);
        tableModifier->insertRow(r);
        for (int c = 0; c < 9; ++c) {
            tableSupprimer->setItem(r, c, new QTableWidgetItem(*tableConsulter->item(r, c)));
            tableModifier->setItem(r, c,  new QTableWidgetItem(*tableConsulter->item(r, c)));
        }
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
void PageQuai::showPlanning()  { planningWidget->refresh(); pages->setCurrentWidget(pagePlanning); }
void PageQuai::showAffectation(){ affectationWidget->refreshQuais(); pages->setCurrentWidget(pageAffectation); }
void PageQuai::showExportCSV() { pages->setCurrentWidget(pageExportCSV); }

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

    const bool enTravaux = travauxCheck->isChecked();
    const QDate dateDebutTravaux = enTravaux ? dateDebutEdit->date() : QDate();
    const QDate dateFinTravaux = enTravaux ? dateFinEdit->date() : QDate();
    if (enTravaux && dateFinTravaux < dateDebutTravaux) {
        QMessageBox::warning(this, "Erreur", "La date de fin doit etre superieure ou egale a la date de debut.");
        return;
    }

    double tirant = tirantEdit->value();
    QString equipements = equipementsEdit->text().trimmed();

    QuaiDB quaiDb;
    quaiDb.id_quai = idQuai;
    quaiDb.nom = nom;
    quaiDb.places_total = places;
    quaiDb.navires_presents = navires;
    quaiDb.adresse = adresse;
    quaiDb.etat = enTravaux ? "En travaux" :
                      (navires >= places ? "Complet" : "Disponible");
    quaiDb.tirant_eau_max = tirant;
    quaiDb.equipements = equipements;
    quaiDb.date_debut = dateDebutTravaux;
    quaiDb.date_fin = dateFinTravaux;

    if (quaiDb.existe()) {
        QMessageBox::warning(this, "Erreur", "Cet ID existe déjà dans la base de données.");
        return;
    }

    if (!quaiDb.ajout()) {
        QMessageBox::warning(this, "Erreur", "Impossible d'ajouter le quai dans la base de données.");
        return;
    }

    listeQuais.append(Quai(id, nom, places, navires, adresse, enTravaux, tirant, equipements, dateDebutTravaux, dateFinTravaux));
    syncTables();
    idEdit->clear(); nomEdit->clear(); placesEdit->clear(); naviresEdit->clear(); adresseEdit->clear();
    tirantEdit->setValue(0); equipementsEdit->clear();
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
    quai.id_quai = id.toInt();
    quai.suprimer();
}

// =====================================================================
// MODIFIER QUAI
// =====================================================================
void PageQuai::remplirFormulaireDepuisTable(int r, int)
{
    if (r < 0 || r >= listeQuais.size()) {
        return;
    }

    const Quai &quai = listeQuais.at(r);

    idEditM->setText(tableModifier->item(r, 0)->text());
    nomEditM->setText(tableModifier->item(r, 1)->text());
    placesEditM->setText(tableModifier->item(r, 2)->text());
    naviresEditM->setText(tableModifier->item(r, 3)->text());
    adresseEditM->setText(tableModifier->item(r, 4)->text());
    dateDebutEditM->setDate(resolvedDateOrToday(quai.getDateDebutTravaux()));
    dateFinEditM->setDate(resolvedDateOrToday(quai.getDateFinTravaux()));
    travauxCheckM->setChecked(quai.getTravaux());
    tirantEditM->setValue(tableModifier->item(r, 7)->text().toDouble());
    equipementsEditM->setText(tableModifier->item(r, 8)->text());
}

void PageQuai::onTableConsulterDoubleClicked(int row, int col)
{
    Q_UNUSED(col);
    if (row < 0 || row >= tableConsulter->rowCount()) return;

    QString text;
    for (int c = 0; c < tableConsulter->columnCount(); ++c) {
        QTableWidgetItem *headerItem = tableConsulter->horizontalHeaderItem(c);
        QTableWidgetItem *cellItem = tableConsulter->item(row, c);
        const QString header = headerItem ? headerItem->text() : QString("Champ %1").arg(c + 1);
        const QString value = cellItem ? cellItem->text() : "";
        text += header + ": " + value;
        if (c < tableConsulter->columnCount() - 1) text += "\n";
    }

    using namespace qrcodegen;
    const QrCode qr = QrCode::encodeText(text.toUtf8().constData(), QrCode::Ecc::MEDIUM);
    const int size = qr.getSize();
    QImage qrImage(size, size, QImage::Format_RGB32);
    const QRgb black = qRgb(0, 0, 0);
    const QRgb white = qRgb(255, 255, 255);
    for (int y = 0; y < size; ++y)
        for (int x = 0; x < size; ++x)
            qrImage.setPixel(x, y, qr.getModule(x, y) ? black : white);

    QDialog dialog(this);
    dialog.setWindowTitle("QR Code du quai");
    dialog.setFixedSize(280, 280);
    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    layout->setContentsMargins(20, 20, 20, 20);
    QLabel *qrLabel = new QLabel;
    qrLabel->setAlignment(Qt::AlignCenter);
    qrLabel->setPixmap(QPixmap::fromImage(qrImage.scaled(240, 240, Qt::KeepAspectRatio, Qt::FastTransformation)));
    layout->addWidget(qrLabel);
    dialog.exec();


    id = tableConsulter->item(row, 0)->text().toInt();
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

    const bool enTravaux = travauxCheckM->isChecked();
    const QDate dateDebutTravaux = enTravaux ? dateDebutEditM->date() : QDate();
    const QDate dateFinTravaux = enTravaux ? dateFinEditM->date() : QDate();
    if (enTravaux && dateFinTravaux < dateDebutTravaux) {
        QMessageBox::warning(this, "Erreur", "La date de fin doit etre superieure ou egale a la date de debut.");
        return;
    }

    double tirant = tirantEditM->value();
    QString equip = equipementsEditM->text().trimmed();

    if (QMessageBox::question(this, "Confirmation", "Confirmer la modification du quai " + id + " ?") != QMessageBox::Yes) {
        return;
    }

    QuaiDB quaiDb;
    quaiDb.id_quai = idQuai;
    quaiDb.nom = nomEditM->text().trimmed();
    quaiDb.places_total = places;
    quaiDb.navires_presents = navires;
    quaiDb.adresse = adresseEditM->text().trimmed();
    quaiDb.etat = enTravaux ? "En travaux" :
                      (navires >= places ? "Complet" : "Disponible");
    quaiDb.tirant_eau_max = tirant;
    quaiDb.equipements = equip;
    quaiDb.date_debut = dateDebutTravaux;
    quaiDb.date_fin = dateFinTravaux;

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
            listeQuais[i].setTravaux(enTravaux);
            listeQuais[i].setTirantEauMax(tirant);
            listeQuais[i].setEquipements(equip);
            listeQuais[i].setDateDebutTravaux(dateDebutTravaux);
            listeQuais[i].setDateFinTravaux(dateFinTravaux);
            break;
        }
    }
    syncTables();
    QMessageBox::information(this, "Succès", "Quai modifié avec succès.");
}

void PageQuai::annulerAjout()
{
    idEdit->clear(); nomEdit->clear(); placesEdit->clear(); naviresEdit->clear(); adresseEdit->clear();
    tirantEdit->setValue(0); equipementsEdit->clear();
    travauxCheck->setChecked(false);
    pages->setCurrentWidget(pageConsulter);
}

void PageQuai::annulerModif()
{
    idEditM->clear(); nomEditM->clear(); placesEditM->clear(); naviresEditM->clear(); adresseEditM->clear();
    tirantEditM->setValue(0); equipementsEditM->clear();
    travauxCheckM->setChecked(false);
    pages->setCurrentWidget(pageConsulter);
}

void PageQuai::onPlacesChanged(const QString &)  {}
void PageQuai::onNaviresChanged(const QString &) {}
void PageQuai::onTravauxChanged(int)
{
    if (sender() == travauxCheckM) {
        updateTravauxDateVisibility(true);
        return;
    }

    updateTravauxDateVisibility(false);
}

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
// EXPORT CSV
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
    csv += QStringList{"ID","Nom","Places","Navires","Adresse","Travaux","État","Tirant (m)","Équipements"}.join(sep) + "\n";
    for (const Quai &q : listeQuais) {
        QStringList row;
        row << q.getId() << q.getNom()
            << QString::number(q.getPlacesTotales()) << QString::number(q.getNaviresPresents())
            << q.getAdresse() << (q.getTravaux() ? "Oui" : "Non") << q.getEtat()
            << QString::number(q.getTirantEauMax(), 'f', 1) << q.getEquipements();
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
    ensureQuaiDateColumns();

    QSqlQuery query;
    if (!query.exec("SELECT ID_QUAI, NOM, PLACES_TOTAL, NAVIRES_PRESENTS, ADRESSE, ETAT, TIRANT_EAU_MAX, EQUIPEMENTS, "
                    "TO_CHAR(DATE_DEBUT, 'YYYY-MM-DD') AS DATE_DEBUT_TXT, "
                    "TO_CHAR(DATE_FIN, 'YYYY-MM-DD') AS DATE_FIN_TXT "
                    "FROM QUAI ORDER BY ID_QUAI")) {
        qDebug() << "Quai load error:" << query.lastError().text();
        syncTables();
        updateStats();
        return;
    }

    const QSqlRecord record = query.record();
    const int idxDateDebut = record.indexOf("DATE_DEBUT_TXT");
    const int idxDateFin = record.indexOf("DATE_FIN_TXT");

    while (query.next()) {
        const QString id = query.value(0).toString();
        const QString nom = query.value(1).toString();
        const int places = query.value(2).toInt();
        const int navires = query.value(3).toInt();
        const QString adresse = query.value(4).toString();
        const QString etat = query.value(5).toString().trimmed();
        const bool travaux = etat.compare("En travaux", Qt::CaseInsensitive) == 0;
        const double tirant = query.value(6).toDouble();
        const QString equip = query.value(7).toString();
        const QDate dateDebutTravaux = parseOracleDateText(idxDateDebut >= 0 ? query.value(idxDateDebut) : QVariant());
        const QDate dateFinTravaux = parseOracleDateText(idxDateFin >= 0 ? query.value(idxDateFin) : QVariant());

        listeQuais.append(Quai(id, nom, places, navires, adresse, travaux, tirant, equip, dateDebutTravaux, dateFinTravaux));
    }

    syncTables();
    updateStats();
}

PageQuai::QuaisStats PageQuai::getStats() const
{
    QuaisStats stats{static_cast<int>(listeQuais.size()), 0};
    for (const Quai &quai : listeQuais) {
        stats.totalNaviresPresent += quai.getNaviresPresents();
    }
    return stats;
}
