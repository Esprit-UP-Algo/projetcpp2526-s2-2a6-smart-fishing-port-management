#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "addedit_employeedialog.h"
#include "forgotpassworddialog.h"
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QMessageBox>
#include <QPushButton>
#include <QLineEdit>
#include <QDebug>
#include <algorithm>
#include <limits>
#include <QStackedWidget>
#include <QChart>
#include <QChartView>
#include <QPieSeries>
#include <QPieSlice>
#include <QBarSeries>
#include <QBarSet>
#include <QBarCategoryAxis>
#include <QValueAxis>
#include <QDialog>
#include <QVBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QShowEvent>
#include <QResizeEvent>
#include <QHBoxLayout>
#include <QPainter>
#include <QScrollArea>
#include <QPdfWriter>
#include <QFileDialog>
#include <QDateTime>
#include <QPageSize>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>
#include "int7/gestionquai.h"
#include "int7/gestionnavires.h"
#include "int7/gestioncaptures.h"
#include "connection.h"
#include "planning.h"
#include "affectation.h"
#include "settingsdialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setupVisuals();
    setupTableEmployees();
    setupConnections();
    showLoginPage();
    updateEmployeeStats();
    updateSalaryStats();

    // Initialiser les widgets (ils seront créés à la première demande)
    quaiWidget = nullptr;
    navireWidget = nullptr;
    captureWidget = nullptr;
    utilWidget = nullptr;
    dashboardWidget = nullptr;
    planningWidget = nullptr;
    affectationWidget = nullptr;
    currentModuleWidget = nullptr;




}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupVisuals()
{
    // ── Window ──────────────────────────────────────────────
    setMinimumSize(1100, 650);
    setWindowTitle("⚓ FishTech — Gestion Portuaire");

    // ── LOGIN PAGE — features injected dynamically ───────────
    {
        // ── Image de fond (sera dimensionnée dans showEvent) ─────────────────
        m_loginBg = new QLabel(ui->loginLeftPanel);
        m_loginBg->setScaledContents(true);
        m_loginBg->setPixmap(QPixmap(":/images/port_bg.jpg"));
        m_loginBg->lower();
        m_loginOverlay = new QLabel(ui->loginLeftPanel);
        m_loginOverlay->setStyleSheet("background:rgba(2,11,24,0.65);");
        m_loginOverlay->lower();
        // Build feature rows into featuresContainer
        struct Feat { QString icon; QString title; QString desc; QString rgb; };
        QList<Feat> feats = {
            {"🚢","Gestion des navires",  "Suivi en temps réel de la flotte",   "59,130,246"},
            {"🐟","capture maritimes",   "Traçabilité complète des capture",   "16,185,129"},
            {"⚓","Gestion des quais",    "Planification et affectation",        "245,158,11"},
            {"👥","Gestion du personnel", "Employés, rôles et affectation",     "139,92,246"},
        };
        auto *featLay = qobject_cast<QVBoxLayout*>(ui->featuresContainer->layout());
        for (const auto &f : feats) {
            auto *row = new QWidget(ui->featuresContainer);
            row->setStyleSheet("background:transparent;");
            auto *h = new QHBoxLayout(row);
            h->setContentsMargins(0,0,0,0); h->setSpacing(16);
            auto *ico = new QLabel(f.icon, row);
            ico->setFixedSize(44,44);
            ico->setAlignment(Qt::AlignCenter);
            ico->setStyleSheet(QString(
                "font-size:20px;background:rgba(%1,.15);"
                "border-radius:12px;border:1px solid rgba(%1,.30);").arg(f.rgb));
            auto *vl = new QVBoxLayout; vl->setContentsMargins(0,0,0,0); vl->setSpacing(2);
            auto *t1 = new QLabel(f.title, row);
            t1->setStyleSheet("font-size:13px;font-weight:700;color:white;background:transparent;");
            auto *t2 = new QLabel(f.desc, row);
            t2->setStyleSheet("font-size:11px;color:rgba(255,255,255,.42);background:transparent;");
            vl->addWidget(t1); vl->addWidget(t2);
            h->addWidget(ico); h->addLayout(vl,1);
            if (featLay) featLay->addWidget(row);
        }

        // ── Slogan au-dessus des features ───────────────────────────────────
        auto *leftLay = qobject_cast<QVBoxLayout*>(ui->loginLeftPanel->layout());
        if (leftLay) {
            // Insérer slogan avant featuresContainer (index 5 = après les spacers/dividers)
            auto *sloganLbl = new QLabel(
                "La solution intelligente\npour une gestion portuaire\nmoderne et efficace.");
            sloganLbl->setStyleSheet(
                "font-size:22px;font-weight:900;color:white;"
                "line-height:1.3;background:transparent;");
            auto *sloganLine = new QFrame;
            sloganLine->setFrameShape(QFrame::HLine);
            sloganLine->setFixedHeight(3);
            sloganLine->setStyleSheet(
                "background:qlineargradient(x1:0,y1:0,x2:1,y2:0,"
                "stop:0 #3b82f6,stop:0.5 #8b5cf6,stop:1 transparent);border:none;");
            // Trouver l'index de featuresContainer
            int fcIdx = leftLay->indexOf(ui->featuresContainer);
            if (fcIdx >= 0) {
                leftLay->insertWidget(fcIdx, sloganLine);
                leftLay->insertWidget(fcIdx, sloganLbl);
                leftLay->insertSpacing(fcIdx+2, 16);
            }
        }

        // ── Flèche sur chaque feature row ────────────────────────────────────
        if (featLay) {
            for (int i = 0; i < featLay->count(); ++i) {
                auto *item = featLay->itemAt(i);
                if (!item || !item->widget()) continue;
                auto *rowW = item->widget();
                auto *h = qobject_cast<QHBoxLayout*>(rowW->layout());
                if (!h) continue;
                auto *arrow = new QLabel("›", rowW);
                arrow->setStyleSheet(
                    "font-size:18px;color:rgba(255,255,255,60);background:transparent;");
                arrow->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
                h->addWidget(arrow);
            }
        }

        // ── Bouton "OU" + "Connexion sécurisée" + footer dans le panneau droit
        auto *rightLay = qobject_cast<QVBoxLayout*>(ui->loginRightPanel->layout());
        if (rightLay) {
            // Trouver l'index du loginCard
            int cardIdx = rightLay->indexOf(ui->loginCard);
            if (cardIdx >= 0) {
                // Ajouter dans le cardLayout : séparateur OU + bouton connexion sécurisée
                auto *cardLay = qobject_cast<QVBoxLayout*>(ui->loginCard->layout());
                if (cardLay) {
                    // Séparateur OU
                    auto *orRow = new QHBoxLayout;
                    auto *line1 = new QFrame; line1->setFrameShape(QFrame::HLine);
                    line1->setStyleSheet("background:rgba(255,255,255,25);border:none;max-height:1px;");
                    auto *orLbl = new QLabel("OU");
                    orLbl->setAlignment(Qt::AlignCenter);
                    orLbl->setStyleSheet(
                        "font-size:11px;color:rgba(255,255,255,60);"
                        "background:transparent;padding:0 10px;");
                    auto *line2 = new QFrame; line2->setFrameShape(QFrame::HLine);
                    line2->setStyleSheet("background:rgba(255,255,255,25);border:none;max-height:1px;");
                    orRow->addWidget(line1,1); orRow->addWidget(orLbl); orRow->addWidget(line2,1);

                    // Bouton connexion sécurisée
                    auto *btnSecure = new QPushButton("🛡  Connexion sécurisée");
                    btnSecure->setMinimumHeight(46);
                    btnSecure->setCursor(Qt::PointingHandCursor);
                    btnSecure->setStyleSheet(
                        "QPushButton{background:transparent;color:white;"
                        "border:1.5px solid rgba(255,255,255,38);border-radius:12px;"
                        "font-size:13px;font-weight:600;}"
                        "QPushButton:hover{background:rgba(255,255,255,10);"
                        "border-color:rgba(255,255,255,60);}");

                    cardLay->addLayout(orRow);
                    cardLay->addSpacing(8);
                    cardLay->addWidget(btnSecure);
                }
            }

            // Footer SSL en bas du panneau droit
            auto *footer = new QWidget(ui->loginRightPanel);
            footer->setStyleSheet("background:transparent;");
            auto *fh = new QHBoxLayout(footer);
            fh->setContentsMargins(0,8,0,8); fh->setSpacing(6);
            auto *sslIco = new QLabel("🔒");
            sslIco->setStyleSheet("font-size:12px;background:transparent;");
            auto *sslTxt = new QLabel(
                "Vos données sont protégées avec un chiffrement SSL 256 bits");
            sslTxt->setStyleSheet(
                "font-size:10px;color:rgba(255,255,255,50);background:transparent;");
            auto *copy = new QLabel("© 2026 FishTech. Tous droits réservés.");
            copy->setStyleSheet(
                "font-size:10px;color:rgba(255,255,255,50);background:transparent;");
            fh->addWidget(sslIco); fh->addWidget(sslTxt,1);
            fh->addWidget(copy);
            rightLay->addWidget(footer);
        }
        // Toggle password visibility
        connect(ui->btnTogglePassword, &QPushButton::toggled, this, [this](bool show){
            ui->linePassword->setEchoMode(show ? QLineEdit::Normal : QLineEdit::Password);
        });

        // Register button
        connect(ui->btnRegister, &QPushButton::clicked, this, [this](){
            QMessageBox::information(this,"Inscription","Contactez votre administrateur pour créer un compte.");
        });

        connect(ui->lineUsername, &QLineEdit::textChanged, this, [this](){ ui->loginErrMsg->setVisible(false); });
        connect(ui->linePassword, &QLineEdit::textChanged, this, [this](){ ui->loginErrMsg->setVisible(false); });
        connect(ui->linePassword, &QLineEdit::returnPressed, ui->btnLogin, &QPushButton::click);
        connect(ui->lineUsername, &QLineEdit::returnPressed, ui->btnLogin, &QPushButton::click);
    }



    // ── Sidebar ──────────────────────────────────────────────
    ui->sidebar->setStyleSheet(
        "QWidget#sidebar {"
        "  background: qlineargradient(x1:0,y1:0,x2:0,y2:1,"
        "    stop:0 #0a1628, stop:0.6 #0d2d5e, stop:1 #0a1628);"
        "  border-right: 1px solid #1e3a8a;"
        "}"
    );
    ui->sidebar->setFixedWidth(230);

    ui->labelLogo->setStyleSheet(
        "font-size: 20px; font-weight: 900; color: white;"
        "padding: 22px 16px; background: rgba(255,255,255,0.06);"
        "border-bottom: 2px solid #1e40af; letter-spacing: 2px;"
    );
    ui->labelLogo->setAlignment(Qt::AlignCenter);
    ui->labelLogo->setMinimumHeight(70);

    // Sidebar nav buttons style
    const QString sidebarBtnStyle =
        "QPushButton {"
        "  background: transparent; color: #94a3b8;"
        "  border: none; border-radius: 10px;"
        "  text-align: left; padding: 12px 16px;"
        "  font-size: 13px; font-weight: 600; margin: 2px 8px;"
        "}"
        "QPushButton:hover {"
        "  background: rgba(59,130,246,0.18);"
        "  color: #e2e8f0;"
        "  border-left: 3px solid #3b82f6;"
        "  padding-left: 13px;"
        "}"
        "QPushButton:pressed {"
        "  background: rgba(59,130,246,0.3);"
        "  color: white;"
        "}";

    const QList<QPushButton*> navBtns = {
        ui->btnDashboard, ui->btnQuais, ui->btnShips,
        ui->btnCaptures, ui->btnEmployees, ui->btnSettings
    };
    for (QPushButton *btn : navBtns) {
        btn->setStyleSheet(sidebarBtnStyle);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setMinimumHeight(44);
    }

    // Logout button
    ui->btnLogout->setStyleSheet(
        "QPushButton {"
        "  background: qlineargradient(x1:0,y1:0,x2:0,y2:1,"
        "    stop:0 #ef4444, stop:1 #dc2626);"
        "  color: white; border: none; border-radius: 10px;"
        "  padding: 12px 16px; font-size: 13px; font-weight: 700;"
        "  margin: 4px 8px; text-align: left;"
        "}"
        "QPushButton:hover {"
        "  background: qlineargradient(x1:0,y1:0,x2:0,y2:1,"
        "    stop:0 #f87171, stop:1 #ef4444);"
        "  border-bottom: 3px solid #b91c1c;"
        "}"
        "QPushButton:pressed { background: #b91c1c; }"
    );
    ui->btnLogout->setCursor(Qt::PointingHandCursor);
    ui->btnLogout->setMinimumHeight(44);

    // ── Header / Title ───────────────────────────────────────
    ui->labelTitle->setStyleSheet(
        "font-size: 26px; font-weight: 800; color: #0a1628;"
        "padding: 8px 4px; letter-spacing: 0.5px;"
    );

    // ── Toolbar buttons ──────────────────────────────────────
    const QString toolBtnStyle =
        "QPushButton {"
        "  background: qlineargradient(x1:0,y1:0,x2:0,y2:1,"
        "    stop:0 #2563eb, stop:1 #1d4ed8);"
        "  color: white; border: none; border-radius: 8px;"
        "  padding: 8px 14px; font-weight: 700; font-size: 12px;"
        "}"
        "QPushButton:hover {"
        "  background: qlineargradient(x1:0,y1:0,x2:0,y2:1,"
        "    stop:0 #3b82f6, stop:1 #2563eb);"
        "  border-bottom: 3px solid #1e40af;"
        "}"
        "QPushButton:pressed { padding-top: 10px; background: #1d4ed8; }";

    ui->btnSortByPosition->setStyleSheet(toolBtnStyle);
    ui->btnSortBySalary->setStyleSheet(toolBtnStyle);
    ui->btnAnalytics->setStyleSheet(toolBtnStyle);
    ui->btnSortByPosition->setCursor(Qt::PointingHandCursor);
    ui->btnSortBySalary->setCursor(Qt::PointingHandCursor);
    ui->btnAnalytics->setCursor(Qt::PointingHandCursor);

    ui->btnAdd->setStyleSheet(
        "QPushButton {"
        "  background: qlineargradient(x1:0,y1:0,x2:0,y2:1,"
        "    stop:0 #10b981, stop:1 #059669);"
        "  color: white; border: none; border-radius: 8px;"
        "  padding: 8px 16px; font-weight: 800; font-size: 13px;"
        "}"
        "QPushButton:hover {"
        "  background: qlineargradient(x1:0,y1:0,x2:0,y2:1,"
        "    stop:0 #34d399, stop:1 #10b981);"
        "  border-bottom: 3px solid #047857;"
        "}"
        "QPushButton:pressed { padding-top: 10px; background: #059669; }"
    );
    ui->btnAdd->setCursor(Qt::PointingHandCursor);

    // ── Search bar ───────────────────────────────────────────
    ui->lineSearch->setStyleSheet(
        "QLineEdit {"
        "  background: white; border: 2px solid #e2e8f0;"
        "  border-radius: 20px; padding: 8px 16px;"
        "  font-size: 13px; color: #1e293b;"
        "}"
        "QLineEdit:focus { border: 2px solid #3b82f6; }"
    );

    // ── Main content area background ─────────────────────────
    ui->employeesPage->setStyleSheet(
        "QWidget#employeesPage { background: #f0f4f8; }"
    );
}

void MainWindow::setupTableEmployees()
{
    QTableWidget* table = ui->tableEmployees;
    if (!table) return;

    table->verticalHeader()->setVisible(false);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Fixed);
    table->horizontalHeader()->setSectionResizeMode(7, QHeaderView::Fixed);
    table->setColumnWidth(6, 130);
    table->setColumnWidth(7, 140);
    table->verticalHeader()->setDefaultSectionSize(40);
    table->setRowCount(5);

    QStringList ids = {"1", "2", "3", "4", "5"};
    QStringList noms = {"Ali Ben", "Sara Khelifa", "Omar Said", "Lina Boudiaf", "Yassine Amrani"};
    QStringList postes = {"Pêcheur", "Matelot", "Docker", "Chef de quai", "Superviseur"};
    QStringList emails = {"ali@port.com", "sara@port.com", "omar@port.com", "lina@port.com", "yassine@port.com"};
    QStringList tels = {"0600000001", "0600000002", "0600000003", "0600000004", "0600000005"};
    QStringList salaires = {"35000", "32000", "38000", "45000", "50000"};

    for (int i = 0; i < 5; ++i) {
        table->setItem(i, 0, new QTableWidgetItem(ids[i]));
        table->setItem(i, 1, new QTableWidgetItem(noms[i]));
        table->setItem(i, 2, new QTableWidgetItem(postes[i]));
        table->setItem(i, 3, new QTableWidgetItem(emails[i]));
        table->setItem(i, 4, new QTableWidgetItem(tels[i]));
        table->setItem(i, 5, new QTableWidgetItem(salaires[i] + " TND"));

        QPushButton* btnEdit = new QPushButton("✏️ Modifier");
        btnEdit->setToolTip("Modifier cet employé");
        btnEdit->setStyleSheet(
            "QPushButton { background: qlineargradient(x1:0,y1:0,x2:0,y2:1,stop:0 #3b82f6,stop:1 #2563eb);"
            "  color:white; border:none; border-radius:6px; padding:4px 6px; font-weight:700; font-size:12px; }"
            "QPushButton:hover { background:#60a5fa; }"
            "QPushButton:pressed { background:#1d4ed8; }"
        );
        btnEdit->setCursor(Qt::PointingHandCursor);
        connect(btnEdit, &QPushButton::clicked, this, [this, i]() {
            onEditEmployeeClicked(i);
        });
        table->setCellWidget(i, 6, btnEdit);

        QPushButton* btnDelete = new QPushButton("🗑️ Supprimer");
        btnDelete->setToolTip("Supprimer cet employé");
        btnDelete->setStyleSheet(
            "QPushButton { background: qlineargradient(x1:0,y1:0,x2:0,y2:1,stop:0 #ef4444,stop:1 #dc2626);"
            "  color:white; border:none; border-radius:6px; padding:4px 6px; font-weight:700; font-size:12px; }"
            "QPushButton:hover { background:#f87171; }"
            "QPushButton:pressed { background:#b91c1c; }"
        );
        btnDelete->setCursor(Qt::PointingHandCursor);
        connect(btnDelete, &QPushButton::clicked, this, [this, i]() {
            onDeleteEmployeeClicked(i);
        });
        table->setCellWidget(i, 7, btnDelete);
    }

    table->setColumnHidden(0, true);
}

void MainWindow::setupConnections()
{
    connect(ui->btnLogin, &QPushButton::clicked, this, &MainWindow::onLoginClicked);
    connect(ui->btnForgotPassword, &QPushButton::clicked, this, &MainWindow::onForgotPasswordClicked);

    connect(ui->btnLogout, &QPushButton::clicked, this, &MainWindow::onLogoutClicked);
    connect(ui->btnAdd, &QPushButton::clicked, this, &MainWindow::onAddEmployeeClicked);
    connect(ui->lineSearch, &QLineEdit::textChanged, this, &MainWindow::onSearchEmployees);
    connect(ui->btnSortByPosition, &QPushButton::clicked, this, &MainWindow::onSortByPositionClicked);
    connect(ui->btnSortBySalary, &QPushButton::clicked, this, &MainWindow::onSortBySalaryClicked);
    connect(ui->btnAnalytics, &QPushButton::clicked, this, &MainWindow::onShowAnalytics);
    connect(ui->btnQuais, &QPushButton::clicked, this, &MainWindow::onShowQuais);
    connect(ui->btnShips, &QPushButton::clicked, this, &MainWindow::onShowShips);
    connect(ui->btnCaptures, &QPushButton::clicked, this, &MainWindow::onShowCaptures);
    connect(ui->btnDashboard, &QPushButton::clicked, this, &MainWindow::onShowDashboard);
    connect(ui->btnEmployees, &QPushButton::clicked, this, &MainWindow::onShowEmployeesContent);
    connect(ui->btnSettings,  &QPushButton::clicked, this, &MainWindow::onShowSettings);
}

// Helper to hide Employe-specific controls
static void hideEmployeeControls(Ui::MainWindow *ui)
{
    ui->statsBox->hide();
    ui->salaryStatsBox->hide();
    ui->tableEmployees->hide();
    ui->btnSortByPosition->hide();
    ui->btnSortBySalary->hide();
    ui->btnAnalytics->hide();
    ui->btnAdd->hide();
    ui->lineSearch->hide();
}

// Helper to show Employe-specific controls
static void showEmployeeControls(Ui::MainWindow *ui)
{
    ui->statsBox->show();
    ui->salaryStatsBox->show();
    ui->tableEmployees->show();
    ui->btnSortByPosition->show();
    ui->btnSortBySalary->show();
    ui->btnAnalytics->show();
    ui->btnAdd->show();
    ui->lineSearch->show();
}

void MainWindow::onShowQuais()
{
    if (!canAccessQuais()) {
        QMessageBox::warning(this, "Accès refusé", "Vous n'êtes pas autorisé à accéder à cette page.");
        return;
    }
    Connection conn;
    if (!conn.createconnect()) {
        QMessageBox::warning(this, "DB Connection", "Unable to connect to database (check DSN/credentials).");
        return;
    }

    if (!quaiWidget) {
        quaiWidget = new GestionQuai(this);
        quaiWidget->loadFromDb();
    }
    if (currentModuleWidget == quaiWidget) return;
    if (currentModuleWidget) {
        ui->contentLayout->removeWidget(currentModuleWidget);
        currentModuleWidget->hide();
    }
    hideEmployeeControls(ui);
    ui->contentLayout->addWidget(quaiWidget);
    quaiWidget->show();
    currentModuleWidget = quaiWidget;
    ui->labelTitle->show(); ui->labelTitle->setText("Gestion des Quais");
}

void MainWindow::onShowDashboard()
{
    if (!canAccessDashboard()) {
        QMessageBox::warning(this, "Accès refusé", "Vous n'êtes pas autorisé à accéder à cette page.");
        return;
    }
    Connection conn;
    if (!conn.createconnect()) {
        QMessageBox::warning(this, "DB Connection", "Unable to connect to database (check DSN/credentials).");
        return;
    }
    if (!quaiWidget) {
        quaiWidget = new GestionQuai(this);
        quaiWidget->loadFromDb();
    }
    if (!navireWidget) {
        navireWidget = new GestionNavires(this);
        navireWidget->loadFromDb();
    }
    if (!captureWidget) {
        captureWidget = new GestionCaptures(this);
        captureWidget->loadFromDb();
    }
    if (!utilWidget) {
        utilWidget = new GestionUtilisateurs(this);
        utilWidget->loadFromDb();
    }

    if (!dashboardWidget) {
        dashboardWidget = new Dashboard(quaiWidget, navireWidget, captureWidget, utilWidget);
    }
    if (currentModuleWidget == dashboardWidget) return;
    if (currentModuleWidget) {
        ui->contentLayout->removeWidget(currentModuleWidget);
        currentModuleWidget->hide();
    }
    hideEmployeeControls(ui);
    dashboardWidget->refresh();
    ui->contentLayout->addWidget(dashboardWidget);
    dashboardWidget->show();
    currentModuleWidget = dashboardWidget;
    ui->labelTitle->hide();
}

void MainWindow::onShowShips()
{
    if (!canAccessShips()) {
        QMessageBox::warning(this, "Accès refusé", "Vous n'êtes pas autorisé à accéder à cette page.");
        return;
    }
    if (!navireWidget) {
        navireWidget = new GestionNavires(this);
        navireWidget->loadFromDb();
    }
    if (currentModuleWidget == navireWidget) return;
    if (currentModuleWidget) {
        ui->contentLayout->removeWidget(currentModuleWidget);
        currentModuleWidget->hide();
    }
    hideEmployeeControls(ui);
    ui->contentLayout->addWidget(navireWidget);
    navireWidget->show();
    currentModuleWidget = navireWidget;
    ui->labelTitle->show(); ui->labelTitle->setText("Gestion des Navires");
}

void MainWindow::onShowCaptures()
{
    if (!canAccessCaptures()) {
        QMessageBox::warning(this, "Accès refusé", "Vous n'êtes pas autorisé à accéder à cette page.");
        return;
    }
    if (!captureWidget) {
        captureWidget = new GestionCaptures(this);
        captureWidget->loadFromDb();
    }
    if (currentModuleWidget == captureWidget) return;
    if (currentModuleWidget) {
        ui->contentLayout->removeWidget(currentModuleWidget);
        currentModuleWidget->hide();
    }
    hideEmployeeControls(ui);
    ui->contentLayout->addWidget(captureWidget);
    captureWidget->show();
    currentModuleWidget = captureWidget;
    ui->labelTitle->hide(); // banner interne du widget
}

void MainWindow::onShowEmployeesContent()
{
    if (!canAccessEmployees()) {
        QMessageBox::warning(this, "Accès refusé", "Vous n'êtes pas autorisé à accéder à cette page.");
        return;
    }
    if (currentModuleWidget) {
        ui->contentLayout->removeWidget(currentModuleWidget);
        currentModuleWidget->hide();
        currentModuleWidget = nullptr;
    }
    showEmployeeControls(ui);
    ui->labelTitle->show(); ui->labelTitle->setText("Employees");
    loadEmployeesFromDb();
    updateEmployeeStats();
    updateSalaryStats();
}

void MainWindow::setupRoleBasedUI()
{
    ui->btnQuais->show();
    ui->btnDashboard->show();
    ui->btnShips->show();
    ui->btnCaptures->show();
    ui->btnEmployees->show();
    ui->btnSettings->show();

    if (currentUserRole == "Pêcheur") {
        ui->btnQuais->hide();
        ui->btnDashboard->hide();
        ui->btnShips->hide();
        ui->btnEmployees->hide();
        ui->btnSettings->hide();
    } else if (currentUserRole == "Docker") {
        ui->btnDashboard->hide();
        ui->btnShips->hide();
        ui->btnCaptures->hide();
        ui->btnEmployees->hide();
        ui->btnSettings->hide();
    } else if (currentUserRole == "Matelot") {
        ui->btnQuais->hide();
        ui->btnDashboard->hide();
        ui->btnCaptures->hide();
        ui->btnEmployees->hide();
        ui->btnSettings->hide();
    } else if (currentUserRole == "Superviseur") {
        // show all buttons
    } else if (currentUserRole == "Chef de quai") {
        ui->btnDashboard->hide();
    } else {
        ui->btnDashboard->hide();
        ui->btnQuais->hide();
        ui->btnShips->hide();
        ui->btnCaptures->hide();
        ui->btnEmployees->hide();
        ui->btnSettings->hide();
    }
}

bool MainWindow::canAccessQuais() const
{
    return currentUserRole == "Docker"
        || currentUserRole == "Superviseur"
        || currentUserRole == "Chef de quai";
}

bool MainWindow::canAccessDashboard() const
{
    return currentUserRole == "Superviseur";
}

bool MainWindow::canAccessShips() const
{
    return currentUserRole == "Matelot"
        || currentUserRole == "Superviseur"
        || currentUserRole == "Chef de quai";
}

bool MainWindow::canAccessCaptures() const
{
    return currentUserRole == "Pêcheur"
        || currentUserRole == "Superviseur"
        || currentUserRole == "Chef de quai";
}

bool MainWindow::canAccessEmployees() const
{
    return currentUserRole == "Superviseur"
        || currentUserRole == "Chef de quai";
}

bool MainWindow::canAccessSettings() const
{
    return currentUserRole == "Superviseur"
        || currentUserRole == "Chef de quai";
}

void MainWindow::showLoginPage()
{
    ui->stackedWidget->setCurrentIndex(LoginPage);
    ui->lineUsername->clear();
    ui->linePassword->clear();
}

void MainWindow::showEmployeesPage()
{
    ui->stackedWidget->setCurrentIndex(EmployeesPage);
    updateEmployeeStats();
    updateSalaryStats();
}
void MainWindow::onLoginClicked()
{
    QString username = ui->lineUsername->text().trimmed();
    QString password = ui->linePassword->text();
    auto *errMsg = ui->loginPage->findChild<QLabel*>("loginErrMsg");
    if (username.isEmpty() || password.isEmpty()) {
        if (errMsg) { errMsg->setText("⚠  Veuillez remplir tous les champs."); errMsg->setVisible(true); }
        return;
    }

    Connection conn;
    if (!conn.createconnect()) {
        if (errMsg) { errMsg->setText("Erreur : Connexion base de donnees impossible."); errMsg->setVisible(true); }
        return;
    }

    QSqlQuery q;
    q.prepare("SELECT id, nom, poste FROM employee WHERE email = :email AND password = :password");
    q.bindValue(":email", username);
    q.bindValue(":password", password);

    if (!q.exec()) {
        if (errMsg) { errMsg->setText("Erreur base de donnees : " + q.lastError().text()); errMsg->setVisible(true); }
        return;
    }

    if (q.next()) {
        if (errMsg) errMsg->setVisible(false);
        currentUserRole = q.value(2).toString().trimmed();
        setupRoleBasedUI();
        showEmployeesPage();
        if (currentUserRole == "Pêcheur") {
            onShowCaptures();
        } else if (currentUserRole == "Docker") {
            onShowQuais();
        } else if (currentUserRole == "Matelot") {
            onShowShips();
        } else if (currentUserRole == "Superviseur") {
            onShowDashboard();
        } else if (currentUserRole == "Chef de quai") {
            onShowQuais();
        } else {
            onShowDashboard();
        }
    } else {
        if (errMsg) { errMsg->setText("Email ou mot de passe incorrect."); errMsg->setVisible(true); }
    }
}

void MainWindow::onForgotPasswordClicked()
{
    ForgotPasswordDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        QMessageBox::information(this, "Demande envoyée", "Votre demande de récupération de mot de passe a été envoyée.");
    }
}

void MainWindow::onLogoutClicked()
{
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Logout", "Are you sure you want to logout?");
    if (reply == QMessageBox::Yes) {
        currentUserRole.clear();
        setupRoleBasedUI();
        showLoginPage();
    }
}

void MainWindow::onAddEmployeeClicked()
{
    AddEditEmployeeDialog dialog(this, false);
    if (dialog.exec() != QDialog::Accepted) return;

    Connection conn;
    if (!conn.createconnect()) {
        QMessageBox::warning(this, "Erreur", "Connexion base de donnees impossible."); return;
    }

    QSqlQuery q;
    q.prepare("INSERT INTO employee (id, nom, poste, email, password, telephone, salaire) "
              "SELECT NVL(MAX(id),0)+1, :nom, :poste, :email, :password, :tel, :sal FROM employee");
    q.bindValue(":nom",      dialog.getNom());
    q.bindValue(":poste",    dialog.getPoste());
    q.bindValue(":email",    dialog.getEmail());
    q.bindValue(":password", dialog.getPassword());
    q.bindValue(":tel",      dialog.getTelephone());
    q.bindValue(":sal",      dialog.getSalaire());

    if (!q.exec()) {
        QMessageBox::critical(this, "Erreur", "Impossible d'ajouter : " + q.lastError().text()); return;
    }

    loadEmployeesFromDb();
    updateEmployeeStats();
    updateSalaryStats();
    QMessageBox::information(this, "Succes", "employee ajoute avec succes !");
}
void MainWindow::onEditEmployeeClicked(int row)
{
    QTableWidget* table = ui->tableEmployees;
    if (!table || row < 0 || row >= table->rowCount()) return;

    QString id        = table->item(row, 0)->text();
    QString nom       = table->item(row, 1)->text();
    QString poste     = table->item(row, 2)->text();
    QString email     = table->item(row, 3)->text();
    QString telephone = table->item(row, 4)->text();
    double  salary    = table->item(row, 5)->text().replace(" TND","").toDouble();

    // Fetch password from database
    Connection connFetch;
    if (!connFetch.createconnect()) {
        QMessageBox::warning(this, "Erreur", "Connexion base de donnees impossible."); return;
    }

    QSqlQuery qFetch;
    qFetch.prepare("SELECT password FROM employee WHERE id = :id");
    qFetch.bindValue(":id", id.toInt());
    QString password;
    if (qFetch.exec() && qFetch.next()) {
        password = qFetch.value(0).toString();
    }

    AddEditEmployeeDialog dialog(this, true);
    dialog.setEmployeeData(id, nom, poste, email, telephone, password, salary);
    if (dialog.exec() != QDialog::Accepted) return;

    Connection conn;
    if (!conn.createconnect()) {
        QMessageBox::warning(this, "Erreur", "Connexion base de donnees impossible."); return;
    }

    QSqlQuery q;
    q.prepare("UPDATE employee SET nom=:nom, poste=:poste, email=:email, password=:password, telephone=:tel, salaire=:sal WHERE id=:id");
    q.bindValue(":nom",      dialog.getNom());
    q.bindValue(":poste",    dialog.getPoste());
    q.bindValue(":email",    dialog.getEmail());
    q.bindValue(":password", dialog.getPassword());
    q.bindValue(":tel",      dialog.getTelephone());
    q.bindValue(":sal",      dialog.getSalaire());
    q.bindValue(":id",       id.toInt());

    if (!q.exec()) {
        QMessageBox::critical(this, "Erreur", "Impossible de modifier : " + q.lastError().text()); return;
    }

    loadEmployeesFromDb();
    updateEmployeeStats();
    updateSalaryStats();
    QMessageBox::information(this, "Succes", "employee modifie avec succes !");
}
void MainWindow::onDeleteEmployeeClicked(int row)
{
    QTableWidget* table = ui->tableEmployees;
    if (!table || row < 0 || row >= table->rowCount()) return;

    QString id  = table->item(row, 0)->text();
    QString nom = table->item(row, 1)->text();

    if (QMessageBox::question(this, "Confirmation",
        QString("Supprimer l'employee %1 ?").arg(nom),
        QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes) return;

    Connection conn;
    if (!conn.createconnect()) {
        QMessageBox::warning(this, "Erreur", "Connexion base de donnees impossible."); return;
    }

    QSqlQuery q;
    q.prepare("DELETE FROM employee WHERE id = :id");
    q.bindValue(":id", id.toInt());

    if (!q.exec()) {
        QMessageBox::critical(this, "Erreur", "Impossible de supprimer : " + q.lastError().text()); return;
    }

    loadEmployeesFromDb();
    updateEmployeeStats();
    updateSalaryStats();
    QMessageBox::information(this, "Succes", "employee supprime avec succes !");
}
void MainWindow::onSearchEmployees(const QString &text)
{
    QTableWidget* table = ui->tableEmployees;
    if (!table) return;

    for (int i = 0; i < table->rowCount(); ++i) {
        bool match = false;
        for (int j = 0; j < table->columnCount() - 2; ++j) {
            QTableWidgetItem* item = table->item(i, j);
            if (item && item->text().contains(text, Qt::CaseInsensitive)) {
                match = true;
                break;
            }
        }
        table->setRowHidden(i, !match);
    }
}

void MainWindow::loadEmployeesFromDb()
{
    QTableWidget* table = ui->tableEmployees;
    if (!table) return;

    Connection conn;
    if (!conn.createconnect()) {
        qDebug() << "Failed to connect to database for Employe loading";
        return;
    }

    table->setRowCount(0);

    QSqlQuery query;
    if (!query.exec("SELECT id, nom, poste, email, telephone, salaire FROM employee ORDER BY id")) {
        qDebug() << "Error loading employees:" << query.lastError().text();
        return;
    }

    int rowCount = 0;
    while (query.next()) {
        table->insertRow(rowCount);

        int id = query.value(0).toInt();
        QString nom = query.value(1).toString();
        QString poste = query.value(2).toString();
        QString email = query.value(3).toString();
        QString telephone = query.value(4).toString();
        double salaire = query.value(5).toDouble();

        table->setItem(rowCount, 0, new QTableWidgetItem(QString::number(id)));
        table->setItem(rowCount, 1, new QTableWidgetItem(nom));
        table->setItem(rowCount, 2, new QTableWidgetItem(poste));
        table->setItem(rowCount, 3, new QTableWidgetItem(email));
        table->setItem(rowCount, 4, new QTableWidgetItem(telephone));
        table->setItem(rowCount, 5, new QTableWidgetItem(QString::number(salaire, 'f', 2) + " TND"));

        QPushButton* btnEdit = new QPushButton("✏️ Modifier");
        btnEdit->setToolTip("Modifier cet employé");
        btnEdit->setStyleSheet(
            "QPushButton { background: qlineargradient(x1:0,y1:0,x2:0,y2:1,stop:0 #3b82f6,stop:1 #2563eb);"
            "  color:white; border:none; border-radius:6px; padding:4px 6px; font-weight:700; font-size:12px; }"
            "QPushButton:hover { background:#60a5fa; }"
            "QPushButton:pressed { background:#1d4ed8; }"
        );
        btnEdit->setCursor(Qt::PointingHandCursor);
        connect(btnEdit, &QPushButton::clicked, [this, rowCount]() { onEditEmployeeClicked(rowCount); });
        table->setCellWidget(rowCount, 6, btnEdit);

        QPushButton* btnDelete = new QPushButton("🗑️ Supprimer");
        btnDelete->setToolTip("Supprimer cet employé");
        btnDelete->setStyleSheet(
            "QPushButton { background: qlineargradient(x1:0,y1:0,x2:0,y2:1,stop:0 #ef4444,stop:1 #dc2626);"
            "  color:white; border:none; border-radius:6px; padding:4px 6px; font-weight:700; font-size:12px; }"
            "QPushButton:hover { background:#f87171; }"
            "QPushButton:pressed { background:#b91c1c; }"
        );
        btnDelete->setCursor(Qt::PointingHandCursor);
        connect(btnDelete, &QPushButton::clicked, [this, rowCount]() { onDeleteEmployeeClicked(rowCount); });
        table->setCellWidget(rowCount, 7, btnDelete);

        rowCount++;
    }

    qDebug() << "Loaded" << rowCount << "employees from database";
}

void MainWindow::updateEmployeeStats()
{
    QTableWidget* table = ui->tableEmployees;
    if (!table) return;

    int countPecheur = 0, countMatelot = 0, countDocker = 0, countChefQuai = 0, countSuperviseur = 0;

    for (int i = 0; i < table->rowCount(); ++i) {
        QString poste = table->item(i, 2) ? table->item(i, 2)->text() : "";
        if (poste == "Pêcheur") countPecheur++;
        else if (poste == "Matelot") countMatelot++;
        else if (poste == "Docker") countDocker++;
        else if (poste == "Chef de quai") countChefQuai++;
        else if (poste == "Superviseur") countSuperviseur++;
    }

    ui->statPecheur->setText(QString("🎣 Pêcheur: %1").arg(countPecheur));
    ui->statMatelot->setText(QString("⛵ Matelot: %1").arg(countMatelot));
    ui->statDocker->setText(QString("📦 Docker: %1").arg(countDocker));
    ui->statChefQuai->setText(QString("👔 Chef de quai: %1").arg(countChefQuai));
    ui->statSuperviseur->setText(QString("📋 Superviseur: %1").arg(countSuperviseur));
}

void MainWindow::updateSalaryStats()
{
    QTableWidget* table = ui->tableEmployees;
    if (!table || table->rowCount() == 0) {
        ui->salaryAverage->setText("📊 Salaire Moyen: 0 TND");
        ui->salaryHighest->setText("📈 Salaire Max: 0 TND");
        ui->salaryLowest->setText("📉 Salaire Min: 0 TND");
        ui->salaryTotal->setText("💵 Total Mensuel: 0 TND");
        return;
    }

    double total = 0;
    double highest = 0;
    double lowest = std::numeric_limits<double>::max();

    for (int i = 0; i < table->rowCount(); ++i) {
        QString salaryText = table->item(i, 5) ? table->item(i, 5)->text() : "0";
        salaryText.replace(" TND", "").replace(",", "");
        double salary = salaryText.toDouble();

        total += salary;
        if (salary > highest) highest = salary;
        if (salary < lowest) lowest = salary;
    }

    double average = total / table->rowCount();

    ui->salaryAverage->setText(QString("📊 Salaire Moyen: %1 TND").arg(static_cast<int>(average)));
    ui->salaryHighest->setText(QString("📈 Salaire Max: %1 TND").arg(static_cast<int>(highest)));
    ui->salaryLowest->setText(QString("📉 Salaire Min: %1 TND").arg(static_cast<int>(lowest)));
    ui->salaryTotal->setText(QString("💵 Total Mensuel: %1 TND").arg(static_cast<int>(total)));
}

void MainWindow::onSortByPositionClicked()
{
    QTableWidget* table = ui->tableEmployees;
    if (!table || table->rowCount() == 0)
        return;

    QList<QStringList> rows;
    for (int i = 0; i < table->rowCount(); ++i) {
        QStringList row;
        for (int j = 0; j < table->columnCount() - 2; ++j) {
            row << (table->item(i, j) ? table->item(i, j)->text() : "");
        }
        rows.append(row);
    }

    std::sort(rows.begin(), rows.end(), [](const QStringList& a, const QStringList& b) {
        return a[2] < b[2];
    });

    for (int i = 0; i < rows.size(); ++i) {
        for (int j = 0; j < rows[i].size(); ++j) {
            if (j < table->columnCount() - 2) {
                table->item(i, j)->setText(rows[i][j]);
            }
        }
    }

    QMessageBox::information(this, "Tri", "Employés triés par Poste ✓");
}

void MainWindow::onSortBySalaryClicked()
{
    QTableWidget* table = ui->tableEmployees;
    if (!table || table->rowCount() == 0)
        return;

    QList<QStringList> rows;
    for (int i = 0; i < table->rowCount(); ++i) {
        QStringList row;
        for (int j = 0; j < table->columnCount() - 2; ++j) {
            row << (table->item(i, j) ? table->item(i, j)->text() : "");
        }
        rows.append(row);
    }

    std::sort(rows.begin(), rows.end(), [](const QStringList& a, const QStringList& b) {
        QString salaryAStr = a[5];
        QString salaryBStr = b[5];
        double salaryA = salaryAStr.replace(" TND", "").toDouble();
        double salaryB = salaryBStr.replace(" TND", "").toDouble();
        return salaryA > salaryB;
    });

    for (int i = 0; i < rows.size(); ++i) {
        for (int j = 0; j < rows[i].size(); ++j) {
            if (j < table->columnCount() - 2) {
                table->item(i, j)->setText(rows[i][j]);
            }
        }
    }

    QMessageBox::information(this, "Tri", "Employés triés par Salaire (décroissant) ✓");
}

void MainWindow::onShowAnalytics()
{
    QTableWidget* table = ui->tableEmployees;
    if (!table || table->rowCount() == 0) {
        QMessageBox::warning(this, "Error", "No Employe data available");
        return;
    }

    QDialog* analyticsDialog = new QDialog(this);
    analyticsDialog->setWindowTitle("📊 Employe Analytics & Statistics");
    analyticsDialog->setGeometry(100, 100, 1000, 700);

    QVBoxLayout* layout = new QVBoxLayout(analyticsDialog);
    QHBoxLayout* chartsLayout = new QHBoxLayout();

    QMap<QString, double> salaryByPosition;
    for (int i = 0; i < table->rowCount(); ++i) {
        QString poste = table->item(i, 2)->text();
        QString salaryText = table->item(i, 5)->text().replace(" TND", "");
        double salary = salaryText.toDouble();
        salaryByPosition[poste] += salary;
    }

    // --- PIE CHART ---
    QPieSeries* pieSeries = new QPieSeries();
    QColor colors[] = {QColor(33, 150, 243), QColor(244, 67, 54), QColor(76, 175, 80),
                       QColor(255, 152, 0), QColor(156, 39, 176)};
    int colorIdx = 0;

    for (auto it = salaryByPosition.begin(); it != salaryByPosition.end(); ++it) {
        QPieSlice* slice = pieSeries->append(it.key() + ": " + QString::number(it.value(), 'f', 0) + " TND", it.value());
        slice->setColor(colors[colorIdx % 5]);
        colorIdx++;
    }

    QChart* pieChart = new QChart();
    pieChart->addSeries(pieSeries);
    pieChart->setTitle("Salary Distribution by Position");
    pieChart->setAnimationOptions(QChart::SeriesAnimations);

    analyticsPieChart = reinterpret_cast<void*>(pieChart);

    QChartView* pieChartView = new QChartView(pieChart);
    pieChartView->setRenderHint(QPainter::Antialiasing);
    pieChartView->setMinimumWidth(450);

    // --- BAR CHART ---
    QMap<QString, int> positionCount;
    for (int i = 0; i < table->rowCount(); ++i) {
        QString poste = table->item(i, 2)->text();
        positionCount[poste]++;
    }

    QBarSet* barSet = new QBarSet("Count");
    QStringList categories;

    for (auto it = positionCount.begin(); it != positionCount.end(); ++it) {
        categories << it.key();
        *barSet << it.value();
        barSet->setColor(QColor(33, 150, 243));
    }

    QBarSeries* barSeries = new QBarSeries();
    barSeries->append(barSet);

    QChart* barChart = new QChart();
    barChart->addSeries(barSeries);
    barChart->setTitle("Employee Count by Position");
    barChart->setAnimationOptions(QChart::SeriesAnimations);

    analyticsBarChart = reinterpret_cast<void*>(barChart);

    QBarCategoryAxis* axisX = new QBarCategoryAxis();
    axisX->append(categories);
    barChart->addAxis(axisX, Qt::AlignBottom);
    barSeries->attachAxis(axisX);

    QValueAxis* axisY = new QValueAxis();
    barChart->addAxis(axisY, Qt::AlignLeft);
    barSeries->attachAxis(axisY);

    QChartView* barChartView = new QChartView(barChart);
    barChartView->setRenderHint(QPainter::Antialiasing);
    barChartView->setMinimumWidth(450);

    chartsLayout->addWidget(pieChartView);
    chartsLayout->addWidget(barChartView);

    // --- STATS TEXT ---
    QString statsText = "<b>📈 SALARY STATISTICS</b><br><br>";

    double totalSalary = 0;
    double highestSalary = 0;
    double lowestSalary = std::numeric_limits<double>::max();
    int totalEmployees = table->rowCount();

    for (int i = 0; i < table->rowCount(); ++i) {
        QString salaryText = table->item(i, 5)->text().replace(" TND", "");
        double salary = salaryText.toDouble();
        totalSalary += salary;
        if (salary > highestSalary) highestSalary = salary;
        if (salary < lowestSalary) lowestSalary = salary;
    }

    double averageSalary = totalSalary / totalEmployees;

    statsText += QString("Total Employees: <b>%1</b><br>").arg(totalEmployees);
    statsText += QString("Average Salary: <b>%1 TND</b><br>").arg(static_cast<int>(averageSalary));
    statsText += QString("Highest Salary: <b>%1 TND</b><br>").arg(static_cast<int>(highestSalary));
    statsText += QString("Lowest Salary: <b>%1 TND</b><br>").arg(static_cast<int>(lowestSalary));
    statsText += QString("Total Monthly Payroll: <b>%1 TND</b><br><br>").arg(static_cast<int>(totalSalary));

    statsText += "<b>👤 POSITION BREAKDOWN</b><br>";
    for (auto it = positionCount.begin(); it != positionCount.end(); ++it) {
        statsText += QString("%1: <b>%2</b> employees<br>").arg(it.key()).arg(it.value());
    }

    analyticsStatsText = statsText;

    QLabel* statsLabel = new QLabel(statsText);
    statsLabel->setStyleSheet("background-color: #f0f4f8; padding: 15px; border-radius: 8px; color: #03224c; font-weight: 500;");
    statsLabel->setMinimumHeight(200);
    statsLabel->setWordWrap(true);

    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidget(statsLabel);
    scrollArea->setWidgetResizable(true);

    layout->addLayout(chartsLayout, 3);
    layout->addWidget(scrollArea, 1);

    QHBoxLayout* buttonLayout = new QHBoxLayout();

    QPushButton* pdfBtn = new QPushButton("📄 Generate PDF");
    pdfBtn->setStyleSheet("QPushButton { background-color: #1e40af; color: white; padding: 10px; border-radius: 5px; font-weight: bold; }");
    connect(pdfBtn, &QPushButton::clicked, this, &MainWindow::generateAnalyticsPDF);
    buttonLayout->addWidget(pdfBtn);

    QPushButton* closeBtn = new QPushButton("Close");
    closeBtn->setStyleSheet("QPushButton { background-color: #03224c; color: white; padding: 10px; border-radius: 5px; font-weight: bold; }");
    connect(closeBtn, &QPushButton::clicked, analyticsDialog, &QDialog::accept);
    buttonLayout->addWidget(closeBtn);

    layout->addLayout(buttonLayout);

    analyticsDialog->exec();
}

void MainWindow::generateAnalyticsPDF()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Enregistrer le PDF", "", "PDF Files (*.pdf)");
    if (fileName.isEmpty()) return;
    if (!fileName.endsWith(".pdf")) fileName += ".pdf";

    QPdfWriter pdfWriter(fileName);
    pdfWriter.setPageSize(QPageSize(QPageSize::A4));
    pdfWriter.setResolution(96);

    QPainter painter;
    if (!painter.begin(&pdfWriter)) {
        QMessageBox::critical(this, "Erreur", "Impossible de creer le fichier PDF");
        return;
    }

    QTableWidget* table = ui->tableEmployees;
    int W  = pdfWriter.width();
    int mg = 40;
    int y  = 40;

    double totalSalary = 0, highSalary = 0, lowSalary = std::numeric_limits<double>::max();
    QMap<QString,int>    posCount;
    QMap<QString,double> posSalary;
    for (int i = 0; i < table->rowCount(); ++i) {
        if (!table->item(i,2) || !table->item(i,5)) continue;
        QString poste = table->item(i,2)->text();
        double  sal   = table->item(i,5)->text().replace(" TND","").toDouble();
        totalSalary += sal;
        if (sal > highSalary) highSalary = sal;
        if (sal < lowSalary)  lowSalary  = sal;
        posCount[poste]++;
        posSalary[poste] += sal;
    }
    int    nbEmp  = table->rowCount();
    double avgSal = nbEmp > 0 ? totalSalary / nbEmp : 0;

    // HEADER
    painter.setBrush(QColor(30, 64, 175));
    painter.setPen(Qt::NoPen);
    painter.drawRect(0, 0, W, 90);
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 22, QFont::Bold));
    painter.drawText(mg, 0, W - 2*mg, 60, Qt::AlignVCenter | Qt::AlignLeft, "Rapport Statistiques employee");
    painter.setFont(QFont("Arial", 10));
    painter.drawText(mg, 55, W - 2*mg, 30, Qt::AlignVCenter | Qt::AlignLeft,
                     "Genere le : " + QDateTime::currentDateTime().toString("dd/MM/yyyy  hh:mm"));
    y = 110;

    // SECTION STATS
    painter.setPen(QColor(30, 64, 175));
    painter.setFont(QFont("Arial", 13, QFont::Bold));
    painter.drawText(mg, y, W - 2*mg, 28, Qt::AlignVCenter | Qt::AlignLeft, "Statistiques Generales");
    y += 30;
    painter.setPen(QColor(200, 210, 230));
    painter.drawLine(mg, y, W - mg, y);
    y += 12;

    QStringList sLabels = {"Nombre d employee", "Salaire moyen", "Salaire le plus haut", "Salaire le plus bas", "Masse salariale totale"};
    QStringList sValues = {
        QString::number(nbEmp),
        QString::number((int)avgSal)     + " TND",
        QString::number((int)highSalary) + " TND",
        QString::number((int)lowSalary)  + " TND",
        QString::number((int)totalSalary)+ " TND"
    };
    int colW = (W - 2*mg) / 2;
    for (int i = 0; i < sLabels.size(); ++i) {
        int col = i % 2, row = i / 2;
        int bx = mg + col * colW, by = y + row * 52;
        painter.setBrush(col == 0 ? QColor(240,245,255) : QColor(230,238,255));
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(bx+4, by, colW-8, 46, 6, 6);
        painter.setPen(QColor(80,100,140));
        painter.setFont(QFont("Arial", 9));
        painter.drawText(bx+14, by+4, colW-20, 20, Qt::AlignVCenter|Qt::AlignLeft, sLabels[i]);
        painter.setPen(QColor(20,50,120));
        painter.setFont(QFont("Arial", 13, QFont::Bold));
        painter.drawText(bx+14, by+22, colW-20, 22, Qt::AlignVCenter|Qt::AlignLeft, sValues[i]);
    }
    y += ((sLabels.size()+1)/2)*52 + 20;

    // SECTION REPARTITION PAR POSTE
    painter.setPen(QColor(30, 64, 175));
    painter.setFont(QFont("Arial", 13, QFont::Bold));
    painter.drawText(mg, y, W-2*mg, 28, Qt::AlignVCenter|Qt::AlignLeft, "Repartition par Poste");
    y += 30;
    painter.setPen(QColor(200,210,230));
    painter.drawLine(mg, y, W-mg, y);
    y += 12;

    int cw = (W-2*mg)/4;
    QStringList tHeaders = {"Poste","Nb employee","Salaire Moyen","Total Salaires"};
    painter.setBrush(QColor(30,64,175)); painter.setPen(Qt::NoPen);
    painter.drawRect(mg, y, W-2*mg, 30);
    painter.setPen(Qt::white); painter.setFont(QFont("Arial",10,QFont::Bold));
    for (int c=0;c<4;++c)
        painter.drawText(mg+c*cw+6, y, cw-6, 30, Qt::AlignVCenter|Qt::AlignLeft, tHeaders[c]);
    y += 30;
    int trow=0;
    for (auto it=posCount.begin(); it!=posCount.end(); ++it,++trow) {
        painter.setBrush(trow%2==0 ? QColor(248,250,255) : QColor(235,242,255));
        painter.setPen(Qt::NoPen);
        painter.drawRect(mg, y, W-2*mg, 28);
        painter.setPen(QColor(40,60,100)); painter.setFont(QFont("Arial",10));
        double avg = posSalary[it.key()]/it.value();
        QStringList rv = {it.key(), QString::number(it.value()),
                          QString::number((int)avg)+" TND",
                          QString::number((int)posSalary[it.key()])+" TND"};
        for (int c=0;c<4;++c)
            painter.drawText(mg+c*cw+6, y, cw-6, 28, Qt::AlignVCenter|Qt::AlignLeft, rv[c]);
        painter.setPen(QColor(210,220,240));
        painter.drawLine(mg, y+28, W-mg, y+28);
        y += 28;
    }
    y += 20;

    // SECTION LISTE Employe
    painter.setPen(QColor(30,64,175)); painter.setFont(QFont("Arial",13,QFont::Bold));
    painter.drawText(mg, y, W-2*mg, 28, Qt::AlignVCenter|Qt::AlignLeft, "Liste des employee");
    y += 30;
    painter.setPen(QColor(200,210,230));
    painter.drawLine(mg, y, W-mg, y);
    y += 12;

    QStringList eHeaders = {"Nom","Poste","Email","Telephone","Salaire"};
    int ecw[5] = {150,110,180,120,110};
    painter.setBrush(QColor(30,64,175)); painter.setPen(Qt::NoPen);
    painter.drawRect(mg, y, W-2*mg, 30);
    painter.setPen(Qt::white); painter.setFont(QFont("Arial",10,QFont::Bold));
    int ex=mg;
    for (int c=0;c<5;++c) {
        painter.drawText(ex+4, y, ecw[c]-4, 30, Qt::AlignVCenter|Qt::AlignLeft, eHeaders[c]);
        ex += ecw[c];
    }
    y += 30;
    for (int i=0; i<table->rowCount(); ++i) {
        if (y > pdfWriter.height()-60) { pdfWriter.newPage(); y=40; }
        painter.setBrush(i%2==0 ? QColor(248,250,255) : QColor(235,242,255));
        painter.setPen(Qt::NoPen);
        painter.drawRect(mg, y, W-2*mg, 26);
        painter.setPen(QColor(40,60,100)); painter.setFont(QFont("Arial",9));
        QStringList cv = {
            table->item(i,1)?table->item(i,1)->text():"",
            table->item(i,2)?table->item(i,2)->text():"",
            table->item(i,3)?table->item(i,3)->text():"",
            table->item(i,4)?table->item(i,4)->text():"",
            table->item(i,5)?table->item(i,5)->text():""
        };
        ex=mg;
        for (int c=0;c<5;++c) {
            painter.drawText(ex+4, y, ecw[c]-4, 26, Qt::AlignVCenter|Qt::AlignLeft, cv[c]);
            ex += ecw[c];
        }
        painter.setPen(QColor(210,220,240));
        painter.drawLine(mg, y+26, W-mg, y+26);
        y += 26;
    }

    // FOOTER
    int fh = pdfWriter.height();
    painter.setBrush(QColor(30,64,175)); painter.setPen(Qt::NoPen);
    painter.drawRect(0, fh-35, W, 35);
    painter.setPen(Qt::white); painter.setFont(QFont("Arial",8));
    painter.drawText(mg, fh-35, W-2*mg, 35, Qt::AlignVCenter|Qt::AlignCenter,
                     "Port Management System  |  Rapport confidentiel");

    painter.end();
    QMessageBox::information(this, "Succes", "PDF genere avec succes !\n" + fileName);
}

void MainWindow::onShowPlanning()
{
    Connection conn;
    if (!conn.createconnect()) {
        QMessageBox::warning(this, "DB Connection", "Unable to connect to database (check DSN/credentials).");
        return;
    }

    if (!planningWidget) {
        planningWidget = new PlanningWidget(this);
    }
    if (currentModuleWidget == planningWidget) return;
    if (currentModuleWidget) {
        ui->contentLayout->removeWidget(currentModuleWidget);
        currentModuleWidget->hide();
    }
    hideEmployeeControls(ui);
    ui->contentLayout->addWidget(planningWidget);
    planningWidget->show();
    currentModuleWidget = planningWidget;
    ui->labelTitle->setText("Planning global (Maintenances & Navires)");
    planningWidget->refresh();
}

void MainWindow::onShowAffectation()
{
    Connection conn;
    if (!conn.createconnect()) {
        QMessageBox::warning(this, "DB Connection", "Unable to connect to database (check DSN/credentials).");
        return;
    }

    if (!affectationWidget) {
        affectationWidget = new AffectationWidget(this);
    }
    if (currentModuleWidget == affectationWidget) return;
    if (currentModuleWidget) {
        ui->contentLayout->removeWidget(currentModuleWidget);
        currentModuleWidget->hide();
    }
    hideEmployeeControls(ui);
    ui->contentLayout->addWidget(affectationWidget);
    affectationWidget->show();
    currentModuleWidget = affectationWidget;
    ui->labelTitle->setText("Affectation intelligente des navires");
    affectationWidget->refreshQuais();
}



void MainWindow::onShowSettings()
{
    SettingsDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        qApp->setStyleSheet(dlg.buildStyleSheet());
        QFont f("Segoe UI", dlg.fontSize());
        f.setHintingPreference(QFont::PreferFullHinting);
        qApp->setFont(f);
    }
}
void MainWindow::showEvent(QShowEvent *e)
{
    QMainWindow::showEvent(e);
    updateLoginBackground();
}

void MainWindow::updateLoginBackground()
{
    if (!m_loginBg || !ui->loginLeftPanel) return;
    QRect r = ui->loginLeftPanel->rect();
    if (r.isEmpty()) return;
    m_loginBg->setGeometry(r);
    m_loginBg->lower();
    if (m_loginOverlay) {
        m_loginOverlay->setGeometry(r);
        m_loginOverlay->raise();
    }
    // Remonter tous les vrais widgets au-dessus
    for (QObject *child : ui->loginLeftPanel->children()) {
        QWidget *w = qobject_cast<QWidget*>(child);
        if (w && w != m_loginBg && w != m_loginOverlay) w->raise();
    }
}
void MainWindow::resizeEvent(QResizeEvent *e)
{
    QMainWindow::resizeEvent(e);
    updateLoginBackground();
}
