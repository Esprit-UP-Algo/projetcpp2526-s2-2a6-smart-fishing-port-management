#include "mainwindow.h"
#include "gestionquai.h"
#include "gestionnavires.h"
#include "gestioncaptures.h"
#include "gestionutilisateurs.h"
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
    resize(1250, 750);

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
    title->setStyleSheet("font-size: 26px; font-weight: bold; color: #1e3a8a;");

    userEdit = new QLineEdit;
    userEdit->setPlaceholderText("Nom d'utilisateur");

    passEdit = new QLineEdit;
    passEdit->setPlaceholderText("Mot de passe");
    passEdit->setEchoMode(QLineEdit::Password);

    QFormLayout *form = new QFormLayout;
    form->addRow("Nom :", userEdit);
    form->addRow("Mot de passe :", passEdit);

    QPushButton *loginBtn = new QPushButton("Se connecter");
    loginBtn->setMinimumHeight(40);

    QLabel *infoLabel = new QLabel("Utilisez n'importe quel nom/mot de passe pour tester");
    infoLabel->setAlignment(Qt::AlignCenter);
    infoLabel->setStyleSheet("font-size: 12px; color: #666; margin-top: 10px;");

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addStretch();
    layout->addWidget(title);
    layout->addSpacing(30);
    layout->addLayout(form);
    layout->addSpacing(20);
    layout->addWidget(loginBtn);
    layout->addWidget(infoLabel);
    layout->addStretch();

    QWidget *formContainer = new QWidget;
    formContainer->setLayout(layout);
    formContainer->setFixedWidth(350);

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
    menuWidget->setFixedWidth(240);
    menuWidget->setStyleSheet("background-color: #0f172a;");

    QLabel *title = new QLabel("FISHTECH");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("color: white; font-size: 24px; font-weight: bold; padding: 20px 10px; background-color: #1e3a8a; border-radius: 10px; margin: 10px;");

    btnQuais = new QPushButton("🏗️ Gestion des quais");
    btnNavires = new QPushButton("🚢 Gestion des navires");
    btnCaptures = new QPushButton("🎣 Gestion des captures");
    btnUtilisateurs = new QPushButton("👥 Gestion des utilisateurs");
    btnLogout = new QPushButton("🚪 Déconnexion");

    QList<QPushButton*> menuButtons = {btnQuais, btnNavires, btnCaptures, btnUtilisateurs, btnLogout};
    foreach(QPushButton *btn, menuButtons) {
        btn->setObjectName("btnMenuPrincipal");
        btn->setMinimumHeight(45);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet("background-color: white; color: #0f172a; text-align: left; padding: 12px 22px; border: none; border-radius: 8px; margin: 4px 10px; font-size: 14px; font-weight: bold;");
    }
    btnLogout->setStyleSheet("background-color: #ef4444; color: white; text-align: left; padding: 12px 22px; border: none; border-radius: 8px; margin: 4px 10px; font-size: 14px; font-weight: bold;");

    QVBoxLayout *menuLayout = new QVBoxLayout(menuWidget);
    menuLayout->addWidget(title);
    menuLayout->addSpacing(20);
    menuLayout->addWidget(btnQuais);
    menuLayout->addWidget(btnNavires);
    menuLayout->addWidget(btnCaptures);
    menuLayout->addWidget(btnUtilisateurs);
    menuLayout->addStretch();
    menuLayout->addWidget(btnLogout);
    menuLayout->setContentsMargins(0, 0, 0, 20);

    // ===== ZONE DE CONTENU =====
    QWidget *contentArea = new QWidget;
    contentArea->setObjectName("contentArea");
    contentArea->setStyleSheet("background: #1e3a8a; border-radius: 12px; margin: 10px;");

    contentStack = new QStackedWidget(contentArea);

    gestionQuaiWidget = new GestionQuai;
    gestionNaviresWidget = new GestionNavires;
    gestionCapturesWidget = new GestionCaptures;
    gestionUtilisateursWidget = new GestionUtilisateurs;

    contentStack->addWidget(gestionQuaiWidget);        // index 0
    contentStack->addWidget(gestionNaviresWidget);     // index 1
    contentStack->addWidget(gestionCapturesWidget);    // index 2
    contentStack->addWidget(gestionUtilisateursWidget);// index 3

    QVBoxLayout *contentLayout = new QVBoxLayout(contentArea);
    contentLayout->addWidget(contentStack);
    contentLayout->setContentsMargins(20, 20, 20, 20);

    rootLayout->addWidget(menuWidget);
    rootLayout->addWidget(contentArea);

    connect(btnQuais, &QPushButton::clicked, this, &MainWindow::showGestionQuais);
    connect(btnNavires, &QPushButton::clicked, this, &MainWindow::showGestionNavires);
    connect(btnCaptures, &QPushButton::clicked, this, &MainWindow::showGestionCaptures);
    connect(btnUtilisateurs, &QPushButton::clicked, this, &MainWindow::showGestionUtilisateurs);
    connect(btnLogout, &QPushButton::clicked, this, &MainWindow::onLogoutClicked);

    contentStack->setCurrentIndex(0);
}

void MainWindow::onLoginClicked()
{
    stackedLayout->setCurrentIndex(1);
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
