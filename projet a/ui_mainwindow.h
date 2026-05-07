/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.11.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QStackedWidget *stackedWidget;
    QWidget *loginPage;
    QHBoxLayout *loginRootLayout;
    QWidget *loginLeftPanel;
    QVBoxLayout *leftPanelLayout;
    QLabel *loginAnchor;
    QLabel *loginBrand;
    QLabel *loginTagline;
    QSpacerItem *leftSpacer1;
    QFrame *leftDivider;
    QSpacerItem *leftSpacer2;
    QWidget *featuresContainer;
    QVBoxLayout *featuresLayout;
    QSpacerItem *leftSpacerBottom;
    QFrame *statsStrip;
    QHBoxLayout *statsLayout;
    QVBoxLayout *vboxLayout;
    QLabel *stat1Val;
    QLabel *stat1Lbl;
    QFrame *statDiv1;
    QVBoxLayout *vboxLayout1;
    QLabel *stat2Val;
    QLabel *stat2Lbl;
    QFrame *statDiv2;
    QVBoxLayout *vboxLayout2;
    QLabel *stat3Val;
    QLabel *stat3Lbl;
    QWidget *loginRightPanel;
    QVBoxLayout *rightPanelLayout;
    QSpacerItem *spacerItem;
    QFrame *loginCard;
    QVBoxLayout *cardLayout;
    QHBoxLayout *cardHeaderRow;
    QLabel *cardAnchor;
    QLabel *cardBrand;
    QSpacerItem *spacerItem1;
    QLabel *onlineBadge;
    QSpacerItem *spacerItem2;
    QLabel *cardSubtitle;
    QSpacerItem *spacerItem3;
    QFrame *accentLine;
    QSpacerItem *spacerItem4;
    QLabel *emailFieldLabel;
    QSpacerItem *spacerItem5;
    QHBoxLayout *emailRow;
    QLabel *emailIcon;
    QLineEdit *lineUsername;
    QSpacerItem *spacerItem6;
    QLabel *passFieldLabel;
    QSpacerItem *spacerItem7;
    QHBoxLayout *passwordRow;
    QLabel *passIcon;
    QLineEdit *linePassword;
    QPushButton *btnTogglePassword;
    QSpacerItem *spacerItem8;
    QHBoxLayout *rememberRow;
    QCheckBox *chkRemember;
    QSpacerItem *spacerItem9;
    QPushButton *btnForgotPassword;
    QSpacerItem *spacerItem10;
    QLabel *loginErrMsg;
    QPushButton *btnLogin;
    QSpacerItem *spacerItem11;
    QHBoxLayout *registerRow;
    QSpacerItem *spacerItem12;
    QLabel *registerLbl;
    QPushButton *btnRegister;
    QSpacerItem *spacerItem13;
    QSpacerItem *spacerItem14;
    QWidget *employeesPage;
    QHBoxLayout *employeesMainLayout;
    QWidget *sidebar;
    QVBoxLayout *sidebarLayout;
    QLabel *labelLogo;
    QPushButton *btnDashboard;
    QPushButton *btnQuais;
    QPushButton *btnShips;
    QPushButton *btnCaptures;
    QPushButton *btnEmployees;
    QPushButton *btnSettings;
    QWidget *sidebarSpacerWidget;
    QPushButton *btnLogout;
    QVBoxLayout *contentLayout;
    QHBoxLayout *headerLayout;
    QLabel *labelTitle;
    QSpacerItem *headerSpacer;
    QLineEdit *lineSearch;
    QPushButton *btnSortByPosition;
    QPushButton *btnSortBySalary;
    QPushButton *btnAnalytics;
    QPushButton *btnAdd;
    QGroupBox *statsBox;
    QHBoxLayout *statsLayout1;
    QLabel *statPecheur;
    QLabel *statMatelot;
    QLabel *statDocker;
    QLabel *statChefQuai;
    QLabel *statSuperviseur;
    QGroupBox *salaryStatsBox;
    QHBoxLayout *salaryStatsLayout;
    QLabel *salaryAverage;
    QLabel *salaryHighest;
    QLabel *salaryLowest;
    QLabel *salaryTotal;
    QTableWidget *tableEmployees;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1100, 650);
        stackedWidget = new QStackedWidget(MainWindow);
        stackedWidget->setObjectName("stackedWidget");
        loginPage = new QWidget();
        loginPage->setObjectName("loginPage");
        loginRootLayout = new QHBoxLayout(loginPage);
        loginRootLayout->setSpacing(0);
        loginRootLayout->setObjectName("loginRootLayout");
        loginRootLayout->setContentsMargins(0, 0, 0, 0);
        loginLeftPanel = new QWidget(loginPage);
        loginLeftPanel->setObjectName("loginLeftPanel");
        loginLeftPanel->setMinimumWidth(420);
        leftPanelLayout = new QVBoxLayout(loginLeftPanel);
        leftPanelLayout->setSpacing(0);
        leftPanelLayout->setObjectName("leftPanelLayout");
        leftPanelLayout->setContentsMargins(52, 52, 52, 40);
        loginAnchor = new QLabel(loginLeftPanel);
        loginAnchor->setObjectName("loginAnchor");

        leftPanelLayout->addWidget(loginAnchor);

        loginBrand = new QLabel(loginLeftPanel);
        loginBrand->setObjectName("loginBrand");

        leftPanelLayout->addWidget(loginBrand);

        loginTagline = new QLabel(loginLeftPanel);
        loginTagline->setObjectName("loginTagline");

        leftPanelLayout->addWidget(loginTagline);

        leftSpacer1 = new QSpacerItem(0, 30, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        leftPanelLayout->addItem(leftSpacer1);

        leftDivider = new QFrame(loginLeftPanel);
        leftDivider->setObjectName("leftDivider");
        leftDivider->setFrameShape(QFrame::HLine);

        leftPanelLayout->addWidget(leftDivider);

        leftSpacer2 = new QSpacerItem(0, 24, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        leftPanelLayout->addItem(leftSpacer2);

        featuresContainer = new QWidget(loginLeftPanel);
        featuresContainer->setObjectName("featuresContainer");
        featuresLayout = new QVBoxLayout(featuresContainer);
        featuresLayout->setSpacing(16);
        featuresLayout->setObjectName("featuresLayout");
        featuresLayout->setContentsMargins(0, 0, 0, 0);

        leftPanelLayout->addWidget(featuresContainer);

        leftSpacerBottom = new QSpacerItem(0, 20, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        leftPanelLayout->addItem(leftSpacerBottom);

        statsStrip = new QFrame(loginLeftPanel);
        statsStrip->setObjectName("statsStrip");
        statsLayout = new QHBoxLayout(statsStrip);
        statsLayout->setSpacing(0);
        statsLayout->setObjectName("statsLayout");
        statsLayout->setContentsMargins(16, 12, 16, 12);
        vboxLayout = new QVBoxLayout();
        vboxLayout->setSpacing(1);
        vboxLayout->setObjectName("vboxLayout");
        stat1Val = new QLabel(statsStrip);
        stat1Val->setObjectName("stat1Val");
        stat1Val->setAlignment(Qt::AlignCenter);

        vboxLayout->addWidget(stat1Val);

        stat1Lbl = new QLabel(statsStrip);
        stat1Lbl->setObjectName("stat1Lbl");
        stat1Lbl->setAlignment(Qt::AlignCenter);

        vboxLayout->addWidget(stat1Lbl);


        statsLayout->addLayout(vboxLayout);

        statDiv1 = new QFrame(statsStrip);
        statDiv1->setObjectName("statDiv1");
        statDiv1->setFrameShape(QFrame::VLine);

        statsLayout->addWidget(statDiv1);

        vboxLayout1 = new QVBoxLayout();
        vboxLayout1->setSpacing(1);
        vboxLayout1->setObjectName("vboxLayout1");
        stat2Val = new QLabel(statsStrip);
        stat2Val->setObjectName("stat2Val");
        stat2Val->setAlignment(Qt::AlignCenter);

        vboxLayout1->addWidget(stat2Val);

        stat2Lbl = new QLabel(statsStrip);
        stat2Lbl->setObjectName("stat2Lbl");
        stat2Lbl->setAlignment(Qt::AlignCenter);

        vboxLayout1->addWidget(stat2Lbl);


        statsLayout->addLayout(vboxLayout1);

        statDiv2 = new QFrame(statsStrip);
        statDiv2->setObjectName("statDiv2");
        statDiv2->setFrameShape(QFrame::VLine);

        statsLayout->addWidget(statDiv2);

        vboxLayout2 = new QVBoxLayout();
        vboxLayout2->setSpacing(1);
        vboxLayout2->setObjectName("vboxLayout2");
        stat3Val = new QLabel(statsStrip);
        stat3Val->setObjectName("stat3Val");
        stat3Val->setAlignment(Qt::AlignCenter);

        vboxLayout2->addWidget(stat3Val);

        stat3Lbl = new QLabel(statsStrip);
        stat3Lbl->setObjectName("stat3Lbl");
        stat3Lbl->setAlignment(Qt::AlignCenter);

        vboxLayout2->addWidget(stat3Lbl);


        statsLayout->addLayout(vboxLayout2);


        leftPanelLayout->addWidget(statsStrip);


        loginRootLayout->addWidget(loginLeftPanel);

        loginRightPanel = new QWidget(loginPage);
        loginRightPanel->setObjectName("loginRightPanel");
        rightPanelLayout = new QVBoxLayout(loginRightPanel);
        rightPanelLayout->setObjectName("rightPanelLayout");
        rightPanelLayout->setContentsMargins(56, 0, 56, 0);
        spacerItem = new QSpacerItem(0, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        rightPanelLayout->addItem(spacerItem);

        loginCard = new QFrame(loginRightPanel);
        loginCard->setObjectName("loginCard");
        cardLayout = new QVBoxLayout(loginCard);
        cardLayout->setSpacing(0);
        cardLayout->setObjectName("cardLayout");
        cardLayout->setContentsMargins(38, 34, 38, 34);
        cardHeaderRow = new QHBoxLayout();
        cardHeaderRow->setSpacing(10);
        cardHeaderRow->setObjectName("cardHeaderRow");
        cardAnchor = new QLabel(loginCard);
        cardAnchor->setObjectName("cardAnchor");

        cardHeaderRow->addWidget(cardAnchor);

        cardBrand = new QLabel(loginCard);
        cardBrand->setObjectName("cardBrand");

        cardHeaderRow->addWidget(cardBrand);

        spacerItem1 = new QSpacerItem(40, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        cardHeaderRow->addItem(spacerItem1);

        onlineBadge = new QLabel(loginCard);
        onlineBadge->setObjectName("onlineBadge");

        cardHeaderRow->addWidget(onlineBadge);


        cardLayout->addLayout(cardHeaderRow);

        spacerItem2 = new QSpacerItem(0, 5, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        cardLayout->addItem(spacerItem2);

        cardSubtitle = new QLabel(loginCard);
        cardSubtitle->setObjectName("cardSubtitle");

        cardLayout->addWidget(cardSubtitle);

        spacerItem3 = new QSpacerItem(0, 20, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        cardLayout->addItem(spacerItem3);

        accentLine = new QFrame(loginCard);
        accentLine->setObjectName("accentLine");
        accentLine->setFrameShape(QFrame::HLine);
        accentLine->setFixedHeight(2);

        cardLayout->addWidget(accentLine);

        spacerItem4 = new QSpacerItem(0, 20, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        cardLayout->addItem(spacerItem4);

        emailFieldLabel = new QLabel(loginCard);
        emailFieldLabel->setObjectName("emailFieldLabel");

        cardLayout->addWidget(emailFieldLabel);

        spacerItem5 = new QSpacerItem(0, 5, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        cardLayout->addItem(spacerItem5);

        emailRow = new QHBoxLayout();
        emailRow->setSpacing(0);
        emailRow->setObjectName("emailRow");
        emailIcon = new QLabel(loginCard);
        emailIcon->setObjectName("emailIcon");
        emailIcon->setAlignment(Qt::AlignCenter);
        emailIcon->setMinimumWidth(44);
        emailIcon->setMinimumHeight(46);

        emailRow->addWidget(emailIcon);

        lineUsername = new QLineEdit(loginCard);
        lineUsername->setObjectName("lineUsername");
        lineUsername->setMinimumHeight(46);

        emailRow->addWidget(lineUsername);


        cardLayout->addLayout(emailRow);

        spacerItem6 = new QSpacerItem(0, 14, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        cardLayout->addItem(spacerItem6);

        passFieldLabel = new QLabel(loginCard);
        passFieldLabel->setObjectName("passFieldLabel");

        cardLayout->addWidget(passFieldLabel);

        spacerItem7 = new QSpacerItem(0, 5, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        cardLayout->addItem(spacerItem7);

        passwordRow = new QHBoxLayout();
        passwordRow->setSpacing(0);
        passwordRow->setObjectName("passwordRow");
        passIcon = new QLabel(loginCard);
        passIcon->setObjectName("passIcon");
        passIcon->setAlignment(Qt::AlignCenter);
        passIcon->setMinimumWidth(44);
        passIcon->setMinimumHeight(46);

        passwordRow->addWidget(passIcon);

        linePassword = new QLineEdit(loginCard);
        linePassword->setObjectName("linePassword");
        linePassword->setMinimumHeight(46);
        linePassword->setEchoMode(QLineEdit::EchoMode::Password);

        passwordRow->addWidget(linePassword);

        btnTogglePassword = new QPushButton(loginCard);
        btnTogglePassword->setObjectName("btnTogglePassword");
        btnTogglePassword->setCheckable(true);
        btnTogglePassword->setMinimumWidth(44);
        btnTogglePassword->setMinimumHeight(46);
        btnTogglePassword->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        passwordRow->addWidget(btnTogglePassword);


        cardLayout->addLayout(passwordRow);

        spacerItem8 = new QSpacerItem(0, 12, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        cardLayout->addItem(spacerItem8);

        rememberRow = new QHBoxLayout();
        rememberRow->setObjectName("rememberRow");
        chkRemember = new QCheckBox(loginCard);
        chkRemember->setObjectName("chkRemember");

        rememberRow->addWidget(chkRemember);

        spacerItem9 = new QSpacerItem(40, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        rememberRow->addItem(spacerItem9);

        btnForgotPassword = new QPushButton(loginCard);
        btnForgotPassword->setObjectName("btnForgotPassword");
        btnForgotPassword->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        btnForgotPassword->setFlat(true);

        rememberRow->addWidget(btnForgotPassword);


        cardLayout->addLayout(rememberRow);

        spacerItem10 = new QSpacerItem(0, 18, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        cardLayout->addItem(spacerItem10);

        loginErrMsg = new QLabel(loginCard);
        loginErrMsg->setObjectName("loginErrMsg");
        loginErrMsg->setWordWrap(true);
        loginErrMsg->setVisible(false);

        cardLayout->addWidget(loginErrMsg);

        btnLogin = new QPushButton(loginCard);
        btnLogin->setObjectName("btnLogin");
        btnLogin->setMinimumHeight(50);
        btnLogin->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        cardLayout->addWidget(btnLogin);

        spacerItem11 = new QSpacerItem(0, 14, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        cardLayout->addItem(spacerItem11);

        registerRow = new QHBoxLayout();
        registerRow->setObjectName("registerRow");
        spacerItem12 = new QSpacerItem(40, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        registerRow->addItem(spacerItem12);

        registerLbl = new QLabel(loginCard);
        registerLbl->setObjectName("registerLbl");

        registerRow->addWidget(registerLbl);

        btnRegister = new QPushButton(loginCard);
        btnRegister->setObjectName("btnRegister");
        btnRegister->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        btnRegister->setFlat(true);

        registerRow->addWidget(btnRegister);

        spacerItem13 = new QSpacerItem(40, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        registerRow->addItem(spacerItem13);


        cardLayout->addLayout(registerRow);


        rightPanelLayout->addWidget(loginCard);

        spacerItem14 = new QSpacerItem(0, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        rightPanelLayout->addItem(spacerItem14);


        loginRootLayout->addWidget(loginRightPanel);

        stackedWidget->addWidget(loginPage);
        employeesPage = new QWidget();
        employeesPage->setObjectName("employeesPage");
        employeesMainLayout = new QHBoxLayout(employeesPage);
        employeesMainLayout->setSpacing(0);
        employeesMainLayout->setObjectName("employeesMainLayout");
        employeesMainLayout->setContentsMargins(0, 0, 0, 0);
        sidebar = new QWidget(employeesPage);
        sidebar->setObjectName("sidebar");
        sidebar->setMinimumWidth(220);
        sidebarLayout = new QVBoxLayout(sidebar);
        sidebarLayout->setSpacing(0);
        sidebarLayout->setObjectName("sidebarLayout");
        sidebarLayout->setContentsMargins(0, 0, 0, 0);
        labelLogo = new QLabel(sidebar);
        labelLogo->setObjectName("labelLogo");
        labelLogo->setAlignment(Qt::AlignmentFlag::AlignCenter);

        sidebarLayout->addWidget(labelLogo);

        btnDashboard = new QPushButton(sidebar);
        btnDashboard->setObjectName("btnDashboard");

        sidebarLayout->addWidget(btnDashboard);

        btnQuais = new QPushButton(sidebar);
        btnQuais->setObjectName("btnQuais");

        sidebarLayout->addWidget(btnQuais);

        btnShips = new QPushButton(sidebar);
        btnShips->setObjectName("btnShips");

        sidebarLayout->addWidget(btnShips);

        btnCaptures = new QPushButton(sidebar);
        btnCaptures->setObjectName("btnCaptures");

        sidebarLayout->addWidget(btnCaptures);

        btnEmployees = new QPushButton(sidebar);
        btnEmployees->setObjectName("btnEmployees");

        sidebarLayout->addWidget(btnEmployees);

        btnSettings = new QPushButton(sidebar);
        btnSettings->setObjectName("btnSettings");

        sidebarLayout->addWidget(btnSettings);

        sidebarSpacerWidget = new QWidget(sidebar);
        sidebarSpacerWidget->setObjectName("sidebarSpacerWidget");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(sidebarSpacerWidget->sizePolicy().hasHeightForWidth());
        sidebarSpacerWidget->setSizePolicy(sizePolicy);

        sidebarLayout->addWidget(sidebarSpacerWidget);

        btnLogout = new QPushButton(sidebar);
        btnLogout->setObjectName("btnLogout");

        sidebarLayout->addWidget(btnLogout);


        employeesMainLayout->addWidget(sidebar);

        contentLayout = new QVBoxLayout();
        contentLayout->setObjectName("contentLayout");
        headerLayout = new QHBoxLayout();
        headerLayout->setObjectName("headerLayout");
        labelTitle = new QLabel(employeesPage);
        labelTitle->setObjectName("labelTitle");

        headerLayout->addWidget(labelTitle);

        headerSpacer = new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        headerLayout->addItem(headerSpacer);

        lineSearch = new QLineEdit(employeesPage);
        lineSearch->setObjectName("lineSearch");

        headerLayout->addWidget(lineSearch);

        btnSortByPosition = new QPushButton(employeesPage);
        btnSortByPosition->setObjectName("btnSortByPosition");

        headerLayout->addWidget(btnSortByPosition);

        btnSortBySalary = new QPushButton(employeesPage);
        btnSortBySalary->setObjectName("btnSortBySalary");

        headerLayout->addWidget(btnSortBySalary);

        btnAnalytics = new QPushButton(employeesPage);
        btnAnalytics->setObjectName("btnAnalytics");

        headerLayout->addWidget(btnAnalytics);

        btnAdd = new QPushButton(employeesPage);
        btnAdd->setObjectName("btnAdd");

        headerLayout->addWidget(btnAdd);


        contentLayout->addLayout(headerLayout);

        statsBox = new QGroupBox(employeesPage);
        statsBox->setObjectName("statsBox");
        statsLayout1 = new QHBoxLayout(statsBox);
        statsLayout1->setSpacing(10);
        statsLayout1->setObjectName("statsLayout1");
        statPecheur = new QLabel(statsBox);
        statPecheur->setObjectName("statPecheur");
        statPecheur->setAlignment(Qt::AlignmentFlag::AlignCenter);

        statsLayout1->addWidget(statPecheur);

        statMatelot = new QLabel(statsBox);
        statMatelot->setObjectName("statMatelot");
        statMatelot->setAlignment(Qt::AlignmentFlag::AlignCenter);

        statsLayout1->addWidget(statMatelot);

        statDocker = new QLabel(statsBox);
        statDocker->setObjectName("statDocker");
        statDocker->setAlignment(Qt::AlignmentFlag::AlignCenter);

        statsLayout1->addWidget(statDocker);

        statChefQuai = new QLabel(statsBox);
        statChefQuai->setObjectName("statChefQuai");
        statChefQuai->setAlignment(Qt::AlignmentFlag::AlignCenter);

        statsLayout1->addWidget(statChefQuai);

        statSuperviseur = new QLabel(statsBox);
        statSuperviseur->setObjectName("statSuperviseur");
        statSuperviseur->setAlignment(Qt::AlignmentFlag::AlignCenter);

        statsLayout1->addWidget(statSuperviseur);


        contentLayout->addWidget(statsBox);

        salaryStatsBox = new QGroupBox(employeesPage);
        salaryStatsBox->setObjectName("salaryStatsBox");
        salaryStatsLayout = new QHBoxLayout(salaryStatsBox);
        salaryStatsLayout->setSpacing(10);
        salaryStatsLayout->setObjectName("salaryStatsLayout");
        salaryAverage = new QLabel(salaryStatsBox);
        salaryAverage->setObjectName("salaryAverage");
        salaryAverage->setAlignment(Qt::AlignmentFlag::AlignCenter);

        salaryStatsLayout->addWidget(salaryAverage);

        salaryHighest = new QLabel(salaryStatsBox);
        salaryHighest->setObjectName("salaryHighest");
        salaryHighest->setAlignment(Qt::AlignmentFlag::AlignCenter);

        salaryStatsLayout->addWidget(salaryHighest);

        salaryLowest = new QLabel(salaryStatsBox);
        salaryLowest->setObjectName("salaryLowest");
        salaryLowest->setAlignment(Qt::AlignmentFlag::AlignCenter);

        salaryStatsLayout->addWidget(salaryLowest);

        salaryTotal = new QLabel(salaryStatsBox);
        salaryTotal->setObjectName("salaryTotal");
        salaryTotal->setAlignment(Qt::AlignmentFlag::AlignCenter);

        salaryStatsLayout->addWidget(salaryTotal);


        contentLayout->addWidget(salaryStatsBox);

        tableEmployees = new QTableWidget(employeesPage);
        if (tableEmployees->columnCount() < 8)
            tableEmployees->setColumnCount(8);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableEmployees->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableEmployees->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tableEmployees->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        tableEmployees->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        tableEmployees->setHorizontalHeaderItem(4, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        tableEmployees->setHorizontalHeaderItem(5, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        tableEmployees->setHorizontalHeaderItem(6, __qtablewidgetitem6);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        tableEmployees->setHorizontalHeaderItem(7, __qtablewidgetitem7);
        tableEmployees->setObjectName("tableEmployees");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(tableEmployees->sizePolicy().hasHeightForWidth());
        tableEmployees->setSizePolicy(sizePolicy1);
        tableEmployees->setRowCount(0);
        tableEmployees->setColumnCount(8);

        contentLayout->addWidget(tableEmployees);


        employeesMainLayout->addLayout(contentLayout);

        stackedWidget->addWidget(employeesPage);
        MainWindow->setCentralWidget(stackedWidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "\342\232\223Fishtech", nullptr));
        MainWindow->setStyleSheet(QCoreApplication::translate("MainWindow", "\n"
"QMainWindow {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #f0f4f8, stop:1 #e0eaf5);\n"
"}\n"
"\n"
"/* Login Screen Styles */\n"
"QDialog {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #f0f4f8, stop:1 #e0eaf5);\n"
"}\n"
"\n"
"QWidget#headerContainer {\n"
"    background-color: #03224c;\n"
"    border-radius: 12px;\n"
"}\n"
"\n"
"QLabel#labelTitle {\n"
"    color: #03224c;\n"
"    font-size: 28px;\n"
"    font-weight: bold;\n"
"}\n"
"\n"
"QLabel#labelSubtitle {\n"
"    color: #e0f0ff;\n"
"    font-size: 14px;\n"
"}\n"
"\n"
"/* Sidebar */\n"
"#sidebar {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #03224c, stop:1 #0d47a1);\n"
"    color: white;\n"
"}\n"
"\n"
"#sidebar QLabel {\n"
"    color: white;\n"
"    font-size: 18px;\n"
"    font-weight: bold;\n"
"    padding: 20px 0px;\n"
"    border-bottom: 3px solid #2196F3;\n"
"}\n"
"\n"
"#sidebar QPushButton {\n"
"    color: white;\n"
"    background-color: #03224c;\n"
"    border: none;\n"
"    te"
                        "xt-align: left;\n"
"    padding: 14px 16px;\n"
"    font-size: 13px;\n"
"    font-weight: 500;\n"
"    border-radius: 6px;\n"
"    margin: 4px 8px;\n"
"}\n"
"\n"
"#sidebar QPushButton:hover {\n"
"    background-color: #1565c0;\n"
"    border-left: 4px solid #64b5f6;\n"
"    padding-left: 12px;\n"
"    color: white;\n"
"}\n"
"\n"
"#sidebar QPushButton:pressed {\n"
"    background-color: #0d47a1;\n"
"    color: white;\n"
"}\n"
"\n"
"#sidebar QPushButton#btnLogout {\n"
"    margin-top: 20px;\n"
"}\n"
"\n"
"#sidebar QPushButton#btnLogout:hover {\n"
"    background-color: #1565c0;\n"
"    color: white;\n"
"}\n"
"\n"
"/* Labels */\n"
"QLabel {\n"
"    color: #1e3a5f;\n"
"    font-size: 13px;\n"
"    font-weight: 500;\n"
"}\n"
"\n"
"/* Input Fields */\n"
"QLineEdit {\n"
"    background-color: white;\n"
"    border: 2px solid #cbd5e1;\n"
"    border-radius: 8px;\n"
"    padding: 10px;\n"
"    font-size: 13px;\n"
"    color: #1e3a5f;\n"
"}\n"
"\n"
"QLineEdit:focus {\n"
"    border: 2px solid #03224c;\n"
"    background"
                        "-color: #f0f9ff;\n"
"}\n"
"\n"
"QLineEdit::placeholder {\n"
"    color: #94a3b8;\n"
"}\n"
"\n"
"QComboBox {\n"
"    background-color: white;\n"
"    border: 2px solid #cbd5e1;\n"
"    border-radius: 8px;\n"
"    padding: 10px;\n"
"    font-size: 13px;\n"
"    color: #1e3a5f;\n"
"}\n"
"\n"
"QComboBox:focus {\n"
"    border: 2px solid #03224c;\n"
"    background-color: #f0f9ff;\n"
"}\n"
"\n"
"/* Buttons */\n"
"QPushButton {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #03224c, stop:1 #1e40af);\n"
"    color: white;\n"
"    border: none;\n"
"    border-radius: 8px;\n"
"    padding: 10px;\n"
"    font-weight: bold;\n"
"    font-size: 13px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #1e40af, stop:1 #1d4ed8);\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #1d4ed8, stop:1 #1e3a8a);\n"
"}\n"
"\n"
"QPushButton#btnCancel {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, st"
                        "op:0 #888888, stop:1 #666666);\n"
"}\n"
"\n"
"QPushButton#btnCancel:hover {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #666666, stop:1 #555555);\n"
"}\n"
"\n"
"QPushButton#btnForgotPassword {\n"
"    background: transparent;\n"
"    color: #1e3a5f;\n"
"    border: none;\n"
"    text-decoration: underline;\n"
"}\n"
"\n"
"QPushButton#btnForgotPassword:hover {\n"
"    color: #03224c;\n"
"}\n"
"\n"
"/* Table */\n"
"QTableWidget {\n"
"    background: white;\n"
"    border: 1px solid #e2e8f0;\n"
"    gridline-color: #e2e8f0;\n"
"}\n"
"\n"
"QHeaderView::section {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #03224c, stop:1 #1e40af);\n"
"    color: white;\n"
"    padding: 8px;\n"
"    border: none;\n"
"    font-weight: bold;\n"
"    height: 35px;\n"
"    width: 180px;\n"
"}\n"
"\n"
"QTableWidget::item {\n"
"    padding: 6px;\n"
"    height: 35px;\n"
"    width: 180px;\n"
"    color: #1e3a5f;\n"
"    background-color: white;\n"
"}\n"
"\n"
"QTableWidget::item:hover {\n"
"   "
                        " background-color: #f0f4f8;\n"
"}\n"
"\n"
"QTableWidget::item:selected {\n"
"    background-color: #dbeafe;\n"
"    color: #1e3a5f;\n"
"}\n"
"\n"
"/* GroupBox */\n"
"QGroupBox { \n"
"    border: 2px solid #03224c; \n"
"    border-radius: 12px; \n"
"    margin-top: 15px; \n"
"    padding-top: 15px; \n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #f8fbff, stop:1 #e8f1ff);\n"
"} \n"
"\n"
"QGroupBox::title { \n"
"    subcontrol-origin: margin; \n"
"    left: 10px; \n"
"    padding: 0 5px 0 5px; \n"
"    color: #03224c; \n"
"    font-weight: bold; \n"
"    font-size: 14px;\n"
"}\n"
"   ", nullptr));
        loginPage->setStyleSheet(QCoreApplication::translate("MainWindow", "QWidget#loginPage{background:qlineargradient(x1:0,y1:0,x2:1,y2:1,stop:0 #020b18,stop:0.4 #071a3e,stop:1 #0a2a6e);}", nullptr));
        loginLeftPanel->setStyleSheet(QCoreApplication::translate("MainWindow", "QWidget#loginLeftPanel{background:qlineargradient(x1:0,y1:1,x2:1,y2:0,stop:0 #020b18,stop:0.6 #071a3e,stop:1 #0d2d6b);border-right:1px solid rgba(255,255,255,20);}QLabel{background:transparent;color:white;}", nullptr));
        loginAnchor->setText(QCoreApplication::translate("MainWindow", "\342\232\223", nullptr));
        loginAnchor->setStyleSheet(QCoreApplication::translate("MainWindow", "font-size:48px;", nullptr));
        loginBrand->setText(QCoreApplication::translate("MainWindow", "FishTech", nullptr));
        loginBrand->setStyleSheet(QCoreApplication::translate("MainWindow", "font-size:40px;font-weight:900;letter-spacing:4px;margin-top:-6px;", nullptr));
        loginTagline->setText(QCoreApplication::translate("MainWindow", "Gestion Portuaire Intelligente", nullptr));
        loginTagline->setStyleSheet(QCoreApplication::translate("MainWindow", "font-size:13px;color:rgba(255,255,255,115);margin-top:2px;letter-spacing:1px;", nullptr));
        leftDivider->setStyleSheet(QCoreApplication::translate("MainWindow", "color:rgba(255,255,255,20);", nullptr));
        featuresContainer->setStyleSheet(QCoreApplication::translate("MainWindow", "background:transparent;", nullptr));
        statsStrip->setStyleSheet(QCoreApplication::translate("MainWindow", "QFrame#statsStrip{background:rgba(255,255,255,10);border-radius:12px;border:1px solid rgba(255,255,255,18);}QLabel{background:transparent;}", nullptr));
        stat1Val->setText(QCoreApplication::translate("MainWindow", "24/7", nullptr));
        stat1Val->setStyleSheet(QCoreApplication::translate("MainWindow", "font-size:16px;font-weight:900;color:white;", nullptr));
        stat1Lbl->setText(QCoreApplication::translate("MainWindow", "Disponible", nullptr));
        stat1Lbl->setStyleSheet(QCoreApplication::translate("MainWindow", "font-size:10px;color:rgba(255,255,255,100);", nullptr));
        statDiv1->setStyleSheet(QCoreApplication::translate("MainWindow", "color:rgba(255,255,255,25);", nullptr));
        stat2Val->setText(QCoreApplication::translate("MainWindow", "100%", nullptr));
        stat2Val->setStyleSheet(QCoreApplication::translate("MainWindow", "font-size:16px;font-weight:900;color:white;", nullptr));
        stat2Lbl->setText(QCoreApplication::translate("MainWindow", "S\303\251curis\303\251", nullptr));
        stat2Lbl->setStyleSheet(QCoreApplication::translate("MainWindow", "font-size:10px;color:rgba(255,255,255,100);", nullptr));
        statDiv2->setStyleSheet(QCoreApplication::translate("MainWindow", "color:rgba(255,255,255,25);", nullptr));
        stat3Val->setText(QCoreApplication::translate("MainWindow", "v2.0", nullptr));
        stat3Val->setStyleSheet(QCoreApplication::translate("MainWindow", "font-size:16px;font-weight:900;color:white;", nullptr));
        stat3Lbl->setText(QCoreApplication::translate("MainWindow", "Version", nullptr));
        stat3Lbl->setStyleSheet(QCoreApplication::translate("MainWindow", "font-size:10px;color:rgba(255,255,255,100);", nullptr));
        loginRightPanel->setStyleSheet(QCoreApplication::translate("MainWindow", "background:transparent;", nullptr));
        loginCard->setStyleSheet(QCoreApplication::translate("MainWindow", "QFrame#loginCard{background:rgba(255,255,255,18);border:1px solid rgba(255,255,255,36);border-radius:24px;}QFrame#loginCard QLabel{background:transparent;color:white;}QFrame#loginCard QCheckBox{background:transparent;color:rgba(255,255,255,140);font-size:12px;spacing:7px;}QFrame#loginCard QCheckBox::indicator{width:16px;height:16px;border-radius:4px;border:1.5px solid rgba(255,255,255,56);background:rgba(255,255,255,15);}QFrame#loginCard QCheckBox::indicator:checked{background:#3b82f6;border-color:#3b82f6;}", nullptr));
        cardAnchor->setText(QCoreApplication::translate("MainWindow", "\342\232\223", nullptr));
        cardAnchor->setStyleSheet(QCoreApplication::translate("MainWindow", "font-size:22px;", nullptr));
        cardBrand->setText(QCoreApplication::translate("MainWindow", "FishTech", nullptr));
        cardBrand->setStyleSheet(QCoreApplication::translate("MainWindow", "font-size:17px;font-weight:900;letter-spacing:2px;", nullptr));
        onlineBadge->setText(QCoreApplication::translate("MainWindow", "\342\227\217 En ligne", nullptr));
        onlineBadge->setStyleSheet(QCoreApplication::translate("MainWindow", "font-size:11px;color:#34d399;background:rgba(16,185,129,38);border-radius:10px;padding:3px 10px;border:1px solid rgba(16,185,129,76);", nullptr));
        cardSubtitle->setText(QCoreApplication::translate("MainWindow", "Connectez-vous \303\240 votre espace de travail", nullptr));
        cardSubtitle->setStyleSheet(QCoreApplication::translate("MainWindow", "font-size:12px;color:rgba(255,255,255,107);", nullptr));
        accentLine->setStyleSheet(QCoreApplication::translate("MainWindow", "background:qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 #3b82f6,stop:0.5 #8b5cf6,stop:1 transparent);border:none;", nullptr));
        emailFieldLabel->setText(QCoreApplication::translate("MainWindow", "IDENTIFIANT / EMAIL", nullptr));
        emailFieldLabel->setStyleSheet(QCoreApplication::translate("MainWindow", "font-size:10px;font-weight:700;color:rgba(255,255,255,140);letter-spacing:1px;", nullptr));
        emailIcon->setText(QCoreApplication::translate("MainWindow", "\342\234\211", nullptr));
        emailIcon->setStyleSheet(QCoreApplication::translate("MainWindow", "font-size:16px;color:rgba(255,255,255,115);background:rgba(255,255,255,15);border:1.5px solid rgba(255,255,255,38);border-right:none;border-radius:11px 0 0 11px;", nullptr));
        lineUsername->setPlaceholderText(QCoreApplication::translate("MainWindow", "votre@email.com", nullptr));
        lineUsername->setStyleSheet(QCoreApplication::translate("MainWindow", "QLineEdit{background:rgba(255,255,255,20);border:1.5px solid rgba(255,255,255,38);border-radius:0 11px 11px 0;padding:11px 14px;font-size:13px;color:white;}QLineEdit:focus{border:1.5px solid #3b82f6;background:rgba(59,130,246,25);}QLineEdit::placeholder{color:rgba(255,255,255,76);}", nullptr));
        passFieldLabel->setText(QCoreApplication::translate("MainWindow", "MOT DE PASSE", nullptr));
        passFieldLabel->setStyleSheet(QCoreApplication::translate("MainWindow", "font-size:10px;font-weight:700;color:rgba(255,255,255,140);letter-spacing:1px;", nullptr));
        passIcon->setText(QCoreApplication::translate("MainWindow", "\360\237\224\222", nullptr));
        passIcon->setStyleSheet(QCoreApplication::translate("MainWindow", "font-size:16px;color:rgba(255,255,255,115);background:rgba(255,255,255,15);border:1.5px solid rgba(255,255,255,38);border-right:none;border-radius:11px 0 0 11px;", nullptr));
        linePassword->setPlaceholderText(QCoreApplication::translate("MainWindow", "\342\200\242\342\200\242\342\200\242\342\200\242\342\200\242\342\200\242\342\200\242\342\200\242", nullptr));
        linePassword->setStyleSheet(QCoreApplication::translate("MainWindow", "QLineEdit{background:rgba(255,255,255,20);border:1.5px solid rgba(255,255,255,38);border-left:none;border-right:none;border-radius:0;padding:11px 14px;font-size:13px;color:white;}QLineEdit:focus{border:1.5px solid #3b82f6;background:rgba(59,130,246,25);}QLineEdit::placeholder{color:rgba(255,255,255,76);}", nullptr));
        btnTogglePassword->setText(QCoreApplication::translate("MainWindow", "\360\237\221\201", nullptr));
        btnTogglePassword->setStyleSheet(QCoreApplication::translate("MainWindow", "QPushButton{background:rgba(255,255,255,15);border:1.5px solid rgba(255,255,255,38);border-left:none;border-radius:0 11px 11px 0;font-size:16px;color:rgba(255,255,255,115);}QPushButton:hover{background:rgba(255,255,255,30);color:white;}QPushButton:checked{color:#60a5fa;}", nullptr));
        chkRemember->setText(QCoreApplication::translate("MainWindow", "Se souvenir de moi", nullptr));
        btnForgotPassword->setText(QCoreApplication::translate("MainWindow", "Mot de passe oubli\303\251 ?", nullptr));
        btnForgotPassword->setStyleSheet(QCoreApplication::translate("MainWindow", "QPushButton{background:transparent;color:rgba(255,255,255,97);border:none;font-size:12px;text-decoration:underline;}QPushButton:hover{color:#60a5fa;}", nullptr));
        loginErrMsg->setText(QString());
        loginErrMsg->setStyleSheet(QCoreApplication::translate("MainWindow", "QLabel{background:rgba(239,68,68,38);color:#fca5a5;border:1px solid rgba(239,68,68,76);border-radius:9px;padding:9px 14px;font-size:12px;font-weight:600;}", nullptr));
        btnLogin->setText(QCoreApplication::translate("MainWindow", "Se connecter", nullptr));
        btnLogin->setStyleSheet(QCoreApplication::translate("MainWindow", "QPushButton{background:qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 #2563eb,stop:1 #7c3aed);color:white;border:none;border-radius:12px;font-size:14px;font-weight:800;letter-spacing:1px;border-bottom:3px solid rgba(0,0,0,64);}QPushButton:hover{background:qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 #3b82f6,stop:1 #8b5cf6);}QPushButton:pressed{padding-top:3px;border-bottom:1px solid rgba(0,0,0,64);}", nullptr));
        registerLbl->setText(QCoreApplication::translate("MainWindow", "Pas encore de compte ?", nullptr));
        registerLbl->setStyleSheet(QCoreApplication::translate("MainWindow", "font-size:12px;color:rgba(255,255,255,89);", nullptr));
        btnRegister->setText(QCoreApplication::translate("MainWindow", "S'inscrire", nullptr));
        btnRegister->setStyleSheet(QCoreApplication::translate("MainWindow", "QPushButton{background:transparent;color:#60a5fa;border:none;font-size:12px;font-weight:700;}QPushButton:hover{color:#93c5fd;}", nullptr));
        sidebar->setStyleSheet(QCoreApplication::translate("MainWindow", "#sidebar{background-color:#03224c;}", nullptr));
        labelLogo->setStyleSheet(QCoreApplication::translate("MainWindow", "background-color: #03224c; color: white;", nullptr));
        labelLogo->setText(QCoreApplication::translate("MainWindow", "\342\232\223Fishtech", nullptr));
        btnDashboard->setText(QCoreApplication::translate("MainWindow", "\360\237\217\240 Tableau de bord", nullptr));
        btnQuais->setText(QCoreApplication::translate("MainWindow", "\342\232\223 Quais", nullptr));
        btnShips->setText(QCoreApplication::translate("MainWindow", "\360\237\232\242 Navires", nullptr));
        btnCaptures->setText(QCoreApplication::translate("MainWindow", "\360\237\220\237 Captures", nullptr));
        btnEmployees->setText(QCoreApplication::translate("MainWindow", "\360\237\221\244 Employ\303\251s", nullptr));
        btnSettings->setText(QCoreApplication::translate("MainWindow", "\342\232\231 Param\303\250tres", nullptr));
        sidebarSpacerWidget->setStyleSheet(QCoreApplication::translate("MainWindow", "background-color: #03224c;", nullptr));
        btnLogout->setText(QCoreApplication::translate("MainWindow", "\342\217\273 D\303\251connexion", nullptr));
        labelTitle->setStyleSheet(QCoreApplication::translate("MainWindow", "font-size: 28px; font-weight: bold; color: #03224c;", nullptr));
        labelTitle->setText(QCoreApplication::translate("MainWindow", "Gestion des Employ\303\251s", nullptr));
        lineSearch->setPlaceholderText(QCoreApplication::translate("MainWindow", "Rechercher un employ\303\251...", nullptr));
        btnSortByPosition->setText(QCoreApplication::translate("MainWindow", "\360\237\223\213 Tri Poste", nullptr));
        btnSortBySalary->setText(QCoreApplication::translate("MainWindow", "\360\237\222\260 Tri Salaire", nullptr));
        btnAnalytics->setText(QCoreApplication::translate("MainWindow", "\360\237\223\212 Statistiques", nullptr));
        btnAdd->setText(QCoreApplication::translate("MainWindow", "+ Ajouter", nullptr));
        statsBox->setTitle(QCoreApplication::translate("MainWindow", "\360\237\223\212 Statistiques par M\303\251tier", nullptr));
        statPecheur->setStyleSheet(QCoreApplication::translate("MainWindow", "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #dbeafe, stop:1 #bfdbfe); color: #03224c; font-weight: bold; border-radius: 8px; padding: 12px; border: 1px solid #93c5fd;", nullptr));
        statPecheur->setText(QCoreApplication::translate("MainWindow", "\360\237\216\243 P\303\252cheur: 0", nullptr));
        statMatelot->setStyleSheet(QCoreApplication::translate("MainWindow", "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #d1fae5, stop:1 #a7f3d0); color: #065f46; font-weight: bold; border-radius: 8px; padding: 12px; border: 1px solid #6ee7b7;", nullptr));
        statMatelot->setText(QCoreApplication::translate("MainWindow", "\342\233\265 Matelot: 0", nullptr));
        statDocker->setStyleSheet(QCoreApplication::translate("MainWindow", "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #fed7aa, stop:1 #fdba74); color: #92400e; font-weight: bold; border-radius: 8px; padding: 12px; border: 1px solid #fb923c;", nullptr));
        statDocker->setText(QCoreApplication::translate("MainWindow", "\360\237\223\246 Docker: 0", nullptr));
        statChefQuai->setStyleSheet(QCoreApplication::translate("MainWindow", "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #e9d5ff, stop:1 #d8b4fe); color: #6b21a8; font-weight: bold; border-radius: 8px; padding: 12px; border: 1px solid #d946ef;", nullptr));
        statChefQuai->setText(QCoreApplication::translate("MainWindow", "\360\237\221\224 Chef de quai: 0", nullptr));
        statSuperviseur->setStyleSheet(QCoreApplication::translate("MainWindow", "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #fecaca, stop:1 #fca5a5); color: #7c2d12; font-weight: bold; border-radius: 8px; padding: 12px; border: 1px solid #f87171;", nullptr));
        statSuperviseur->setText(QCoreApplication::translate("MainWindow", "\360\237\223\213 Superviseur: 0", nullptr));
        salaryStatsBox->setTitle(QCoreApplication::translate("MainWindow", "\360\237\222\260 Statistiques Salariales", nullptr));
        salaryAverage->setStyleSheet(QCoreApplication::translate("MainWindow", "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #dbeafe, stop:1 #bfdbfe); color: #03224c; font-weight: bold; border-radius: 8px; padding: 12px; border: 1px solid #93c5fd;", nullptr));
        salaryAverage->setText(QCoreApplication::translate("MainWindow", "\360\237\223\212 Salaire Moyen: 0 TND", nullptr));
        salaryHighest->setStyleSheet(QCoreApplication::translate("MainWindow", "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #d1fae5, stop:1 #a7f3d0); color: #065f46; font-weight: bold; border-radius: 8px; padding: 12px; border: 1px solid #6ee7b7;", nullptr));
        salaryHighest->setText(QCoreApplication::translate("MainWindow", "\360\237\223\210 Salaire Max: 0 TND", nullptr));
        salaryLowest->setStyleSheet(QCoreApplication::translate("MainWindow", "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #fed7aa, stop:1 #fdba74); color: #92400e; font-weight: bold; border-radius: 8px; padding: 12px; border: 1px solid #fb923c;", nullptr));
        salaryLowest->setText(QCoreApplication::translate("MainWindow", "\360\237\223\211 Salaire Min: 0 TND", nullptr));
        salaryTotal->setStyleSheet(QCoreApplication::translate("MainWindow", "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #e9d5ff, stop:1 #d8b4fe); color: #6b21a8; font-weight: bold; border-radius: 8px; padding: 12px; border: 1px solid #d946ef;", nullptr));
        salaryTotal->setText(QCoreApplication::translate("MainWindow", "\360\237\222\265 Total Mensuel: 0 TND", nullptr));
        QTableWidgetItem *___qtablewidgetitem = tableEmployees->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("MainWindow", "ID", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = tableEmployees->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("MainWindow", "Nom", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = tableEmployees->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("MainWindow", "Poste", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = tableEmployees->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("MainWindow", "Email", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = tableEmployees->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QCoreApplication::translate("MainWindow", "T\303\251l\303\251phone", nullptr));
        QTableWidgetItem *___qtablewidgetitem5 = tableEmployees->horizontalHeaderItem(5);
        ___qtablewidgetitem5->setText(QCoreApplication::translate("MainWindow", "Salaire", nullptr));
        QTableWidgetItem *___qtablewidgetitem6 = tableEmployees->horizontalHeaderItem(6);
        ___qtablewidgetitem6->setText(QCoreApplication::translate("MainWindow", "Edit", nullptr));
        QTableWidgetItem *___qtablewidgetitem7 = tableEmployees->horizontalHeaderItem(7);
        ___qtablewidgetitem7->setText(QCoreApplication::translate("MainWindow", "Delete", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
