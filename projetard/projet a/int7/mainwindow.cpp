#include "mainwindow.h"
#include "gestionquai.h"
#include "gestionnavires.h"
#include "gestioncaptures.h"
#include "gestionutilisateurs.h"
#include "dashboard.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QStackedWidget>

MainWindow::MainWindow(QWidget *parent) : QWidget(parent)
{
    setWindowTitle("FishTech - Système de Gestion Portuaire");
    // Pas de resize fixe

    stackedLayout = new QStackedLayout(this);

    setupLoginPage();
    setupMainInterface();

    stackedLayout->addWidget(loginPage);   // index 0
    stackedLayout->addWidget(mainPage);    // index 1

    setLayout(stackedLayout);
    stackedLayout->setCurrentIndex(0);
}

MainWindow::~MainWindow() {}

void MainWindow::setupLoginPage()
{
    loginPage = new QWidget;

    QLabel *title = new QLabel("🐟 FishTech - Connexion");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 22px; font-weight: bold; color: #1e3a8a;");

    userEdit = new QLineEdit;
    userEdit->setPlaceholderText("Nom d'utilisateur");

    passEdit = new QLineEdit;
    passEdit->setPlaceholderText("Mot de passe");
    passEdit->setEchoMode(QLineEdit::Password);

    QFormLayout *form = new QFormLayout;
    form->addRow("Nom :", userEdit);
    form->addRow("Mot de passe :", passEdit);

    QPushButton *loginBtn = new QPushButton("Se connecter");
    loginBtn->setMinimumHeight(35);

    QLabel *infoLabel = new QLabel("Utilisez n'importe quel nom/mot de passe pour tester");
    infoLabel->setAlignment(Qt::AlignCenter);
    infoLabel->setStyleSheet("font-size: 11px; color: #666; margin-top: 8px;");

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addStretch();
    layout->addWidget(title);
    layout->addSpacing(20);
    layout->addLayout(form);
    layout->addSpacing(15);
    layout->addWidget(loginBtn);
    layout->addWidget(infoLabel);
    layout->addStretch();

    QWidget *formContainer = new QWidget;
    formContainer->setLayout(layout);
    formContainer->setFixedWidth(300);

    QHBoxLayout *mainLayout = new QHBoxLayout(loginPage);
    mainLayout->addStretch();
    mainLayout->addWidget(formContainer);
    mainLayout->addStretch();

    connect(loginBtn, &QPushButton::clicked, this, &MainWindow::onLoginClicked);
}

void MainWindow::setupMainInterface()
{
    mainPage = new QWidget;

    QHBoxLayout *rootLayout = new QHBoxLayout(mainPage);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    // ===== MENU LATÉRAL =====
    menuWidget = new QWidget;
    menuWidget->setObjectName("menuPrincipal");
    menuWidget->setFixedWidth(200);
    menuWidget->setStyleSheet("background-color: #0f172a;");

    QLabel *title = new QLabel("FISHTECH");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("color: white; font-size: 20px; font-weight: bold; padding: 15px 5px; background-color: #1e3a8a; border-radius: 8px; margin: 8px;");

    btnQuais = new QPushButton("🏗️ Quais");
    btnDashboard = new QPushButton("📊 Tableau de bord");
    btnNavires = new QPushButton("🚢 Navires");
    btnCaptures = new QPushButton("🎣 Captures");
    btnUtilisateurs = new QPushButton("👥 Utilisateurs");
    btnLogout = new QPushButton("🚪 Déconnexion");

    QList<QPushButton*> menuButtons = {btnQuais, btnNavires, btnCaptures, btnUtilisateurs, btnLogout};
    // insert dashboard as second item visually
    menuButtons.insert(1, btnDashboard);
    foreach(QPushButton *btn, menuButtons) {
        btn->setObjectName("btnMenuPrincipal");
        btn->setMinimumHeight(38);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet("background-color: white; color: #0f172a; text-align: left; padding: 8px 16px; border: none; border-radius: 6px; margin: 2px 8px; font-size: 13px; font-weight: bold;");
    }
    btnLogout->setStyleSheet("background-color: #ef4444; color: white; text-align: left; padding: 8px 16px; border: none; border-radius: 6px; margin: 2px 8px; font-size: 13px; font-weight: bold;");

    QVBoxLayout *menuLayout = new QVBoxLayout(menuWidget);
    menuLayout->addWidget(title);
    menuLayout->addSpacing(10);
    menuLayout->addWidget(btnQuais);
    menuLayout->addWidget(btnDashboard);
    menuLayout->addWidget(btnNavires);
    menuLayout->addWidget(btnCaptures);
    menuLayout->addWidget(btnUtilisateurs);
    menuLayout->addStretch();
    menuLayout->addWidget(btnLogout);
    menuLayout->setContentsMargins(0, 0, 0, 10);

    // ===== ZONE DE CONTENU =====
    QWidget *contentArea = new QWidget;
    contentArea->setObjectName("contentArea");
    contentArea->setStyleSheet("background: #1e3a8a; border-radius: 8px; margin: 5px;");

    contentStack = new QStackedWidget(contentArea);

    gestionQuaiWidget = new GestionQuai;
    gestionNaviresWidget = new GestionNavires;
    gestionCapturesWidget = new GestionCaptures;
    gestionUtilisateursWidget = new GestionUtilisateurs;
    dashboardWidget = new Dashboard(gestionQuaiWidget, gestionNaviresWidget, gestionCapturesWidget, gestionUtilisateursWidget);

    contentStack->addWidget(gestionQuaiWidget);        // index 0
    contentStack->addWidget(dashboardWidget);          // index 1
    contentStack->addWidget(gestionNaviresWidget);     // index 1
    contentStack->addWidget(gestionCapturesWidget);    // index 2
    contentStack->addWidget(gestionUtilisateursWidget);// index 3

    QVBoxLayout *contentLayout = new QVBoxLayout(contentArea);
    contentLayout->addWidget(contentStack);
    contentLayout->setContentsMargins(10, 10, 10, 10);

    rootLayout->addWidget(menuWidget);
    rootLayout->addWidget(contentArea);

    connect(btnQuais, &QPushButton::clicked, this, &MainWindow::showGestionQuais);
    connect(btnDashboard, &QPushButton::clicked, this, &MainWindow::showDashboard);
    connect(btnNavires, &QPushButton::clicked, this, &MainWindow::showGestionNavires);
    connect(btnCaptures, &QPushButton::clicked, this, &MainWindow::showGestionCaptures);
    connect(btnUtilisateurs, &QPushButton::clicked, this, &MainWindow::showGestionUtilisateurs);
    connect(btnLogout, &QPushButton::clicked, this, &MainWindow::onLogoutClicked);

    contentStack->setCurrentIndex(0);
}

void MainWindow::onLoginClicked()
{
    QString username = userEdit->text().trimmed();
    QString password = passEdit->text().trimmed();
    
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer un nom d'utilisateur et un mot de passe");
        return;
    }
    
    if (username == "0" && password == "0") {
        stackedLayout->setCurrentIndex(1);
    } else {
        QMessageBox::warning(this, "Erreur", "Nom d'utilisateur ou mot de passe incorrect");
    }
}

void MainWindow::onLogoutClicked()
{
    userEdit->clear();
    passEdit->clear();
    stackedLayout->setCurrentIndex(0);
}

void MainWindow::showGestionQuais()
{
    contentStack->setCurrentIndex(0);
}

void MainWindow::showDashboard()
{
    // refresh dashboard then show
    if (dashboardWidget) dashboardWidget->refresh();
    // dashboard is at index 1
    contentStack->setCurrentWidget(dashboardWidget);
}

void MainWindow::showGestionNavires()
{
    contentStack->setCurrentIndex(1);
}

void MainWindow::showGestionCaptures()
{
    contentStack->setCurrentIndex(2);
}

void MainWindow::showGestionUtilisateurs()
{
    contentStack->setCurrentIndex(3);
}
