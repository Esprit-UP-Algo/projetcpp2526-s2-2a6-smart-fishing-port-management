/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFormLayout>
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
    QVBoxLayout *loginLayout;
    QLabel *loginAvatar;
    QLabel *loginTitle;
    QLabel *loginSubtitle;
    QSpacerItem *loginSpacerTop;
    QFormLayout *loginFormLayout;
    QLabel *loginEmailLabel;
    QLineEdit *lineUsername;
    QLabel *loginPasswordLabel;
    QLineEdit *linePassword;
    QSpacerItem *loginSpacerBottom;
    QPushButton *btnLogin;
    QPushButton *btnForgotPassword;
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
    QHBoxLayout *statsLayout;
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
        loginLayout = new QVBoxLayout(loginPage);
        loginLayout->setObjectName("loginLayout");
        loginLayout->setContentsMargins(40, 30, 40, 30);
        loginAvatar = new QLabel(loginPage);
        loginAvatar->setObjectName("loginAvatar");
        loginAvatar->setAlignment(Qt::AlignCenter);
        loginAvatar->setText("⛴");
        loginAvatar->setStyleSheet("font-size: 80px; color: #01c9ff; margin-bottom: 8px;");
        loginLayout->addWidget(loginAvatar);

        loginTitle = new QLabel(loginPage);
        loginTitle->setObjectName("loginTitle");
        loginTitle->setAlignment(Qt::AlignCenter);

        loginLayout->addWidget(loginTitle);

        loginSubtitle = new QLabel(loginPage);
        loginSubtitle->setObjectName("loginSubtitle");
        loginSubtitle->setAlignment(Qt::AlignCenter);

        loginLayout->addWidget(loginSubtitle);

        loginSpacerTop = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        loginLayout->addItem(loginSpacerTop);

        loginFormLayout = new QFormLayout();
        loginFormLayout->setObjectName("loginFormLayout");
        loginFormLayout->setHorizontalSpacing(15);
        loginFormLayout->setVerticalSpacing(20);
        loginEmailLabel = new QLabel(loginPage);
        loginEmailLabel->setObjectName("loginEmailLabel");

        loginFormLayout->setWidget(0, QFormLayout::LabelRole, loginEmailLabel);

        lineUsername = new QLineEdit(loginPage);
        lineUsername->setObjectName("lineUsername");
        lineUsername->setMinimumHeight(35);

        loginFormLayout->setWidget(0, QFormLayout::FieldRole, lineUsername);

        loginPasswordLabel = new QLabel(loginPage);
        loginPasswordLabel->setObjectName("loginPasswordLabel");

        loginFormLayout->setWidget(1, QFormLayout::LabelRole, loginPasswordLabel);

        linePassword = new QLineEdit(loginPage);
        linePassword->setObjectName("linePassword");
        linePassword->setEchoMode(QLineEdit::Password);
        linePassword->setMinimumHeight(35);

        loginFormLayout->setWidget(1, QFormLayout::FieldRole, linePassword);


        loginLayout->addLayout(loginFormLayout);

        loginSpacerBottom = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        loginLayout->addItem(loginSpacerBottom);

        btnLogin = new QPushButton(loginPage);
        btnLogin->setObjectName("btnLogin");
        btnLogin->setMinimumHeight(45);

        loginLayout->addWidget(btnLogin);

        btnForgotPassword = new QPushButton(loginPage);
        btnForgotPassword->setObjectName("btnForgotPassword");
        btnForgotPassword->setMinimumHeight(35);

        loginLayout->addWidget(btnForgotPassword);

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
        sidebar->setAutoFillBackground(true);
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
        btnSortByPosition->setMaximumWidth(120);

        headerLayout->addWidget(btnSortByPosition);

        btnSortBySalary = new QPushButton(employeesPage);
        btnSortBySalary->setObjectName("btnSortBySalary");
        btnSortBySalary->setMaximumWidth(120);

        headerLayout->addWidget(btnSortBySalary);

        btnAnalytics = new QPushButton(employeesPage);
        btnAnalytics->setObjectName("btnAnalytics");
        btnAnalytics->setMaximumWidth(120);

        headerLayout->addWidget(btnAnalytics);

        btnAdd = new QPushButton(employeesPage);
        btnAdd->setObjectName("btnAdd");

        headerLayout->addWidget(btnAdd);


        contentLayout->addLayout(headerLayout);

        statsBox = new QGroupBox(employeesPage);
        statsBox->setObjectName("statsBox");
        statsLayout = new QHBoxLayout(statsBox);
        statsLayout->setSpacing(10);
        statsLayout->setObjectName("statsLayout");
        statPecheur = new QLabel(statsBox);
        statPecheur->setObjectName("statPecheur");
        statPecheur->setAlignment(Qt::AlignCenter);

        statsLayout->addWidget(statPecheur);

        statMatelot = new QLabel(statsBox);
        statMatelot->setObjectName("statMatelot");
        statMatelot->setAlignment(Qt::AlignCenter);

        statsLayout->addWidget(statMatelot);

        statDocker = new QLabel(statsBox);
        statDocker->setObjectName("statDocker");
        statDocker->setAlignment(Qt::AlignCenter);

        statsLayout->addWidget(statDocker);

        statChefQuai = new QLabel(statsBox);
        statChefQuai->setObjectName("statChefQuai");
        statChefQuai->setAlignment(Qt::AlignCenter);

        statsLayout->addWidget(statChefQuai);

        statSuperviseur = new QLabel(statsBox);
        statSuperviseur->setObjectName("statSuperviseur");
        statSuperviseur->setAlignment(Qt::AlignCenter);

        statsLayout->addWidget(statSuperviseur);


        contentLayout->addWidget(statsBox);

        salaryStatsBox = new QGroupBox(employeesPage);
        salaryStatsBox->setObjectName("salaryStatsBox");
        salaryStatsLayout = new QHBoxLayout(salaryStatsBox);
        salaryStatsLayout->setSpacing(10);
        salaryStatsLayout->setObjectName("salaryStatsLayout");
        salaryAverage = new QLabel(salaryStatsBox);
        salaryAverage->setObjectName("salaryAverage");
        salaryAverage->setAlignment(Qt::AlignCenter);

        salaryStatsLayout->addWidget(salaryAverage);

        salaryHighest = new QLabel(salaryStatsBox);
        salaryHighest->setObjectName("salaryHighest");
        salaryHighest->setAlignment(Qt::AlignCenter);

        salaryStatsLayout->addWidget(salaryHighest);

        salaryLowest = new QLabel(salaryStatsBox);
        salaryLowest->setObjectName("salaryLowest");
        salaryLowest->setAlignment(Qt::AlignCenter);

        salaryStatsLayout->addWidget(salaryLowest);

        salaryTotal = new QLabel(salaryStatsBox);
        salaryTotal->setObjectName("salaryTotal");
        salaryTotal->setAlignment(Qt::AlignCenter);

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
        loginTitle->setText(QCoreApplication::translate("MainWindow", "\342\232\223Fishtech", nullptr));
        loginTitle->setStyleSheet(QCoreApplication::translate("MainWindow", "font-size: 28px; font-weight: bold; color: #03224c;", nullptr));
        loginSubtitle->setText(QCoreApplication::translate("MainWindow", "Connexion au syst\303\250me", nullptr));
        loginSubtitle->setStyleSheet(QCoreApplication::translate("MainWindow", "color: #e0f0ff; font-size: 14px;", nullptr));
        loginEmailLabel->setText(QCoreApplication::translate("MainWindow", "Email", nullptr));
        lineUsername->setPlaceholderText(QCoreApplication::translate("MainWindow", "Entrer votre email", nullptr));
        loginPasswordLabel->setText(QCoreApplication::translate("MainWindow", "Mot de passe", nullptr));
        linePassword->setPlaceholderText(QCoreApplication::translate("MainWindow", "Entrer votre mot de passe", nullptr));
        btnLogin->setText(QCoreApplication::translate("MainWindow", "Se connecter", nullptr));
        btnForgotPassword->setText(QCoreApplication::translate("MainWindow", "Mot de passe oubli\303\251?", nullptr));
        sidebar->setStyleSheet(QCoreApplication::translate("MainWindow", "#sidebar { background-color: #03224c; }", nullptr));
        labelLogo->setText(QCoreApplication::translate("MainWindow", "\342\232\223Fishtech", nullptr));
        labelLogo->setStyleSheet(QCoreApplication::translate("MainWindow", "background-color: #03224c; color: white;", nullptr));
        btnDashboard->setText(QCoreApplication::translate("MainWindow", "\360\237\217\240 Tableau de bord", nullptr));
        btnQuais->setText(QCoreApplication::translate("MainWindow", "\342\232\223 Quais", nullptr));
        btnShips->setText(QCoreApplication::translate("MainWindow", "\360\237\232\242 Navires", nullptr));
        btnCaptures->setText(QCoreApplication::translate("MainWindow", "\360\237\220\237 Captures", nullptr));
        btnEmployees->setText(QCoreApplication::translate("MainWindow", "\360\237\221\244 Employ\303\251s", nullptr));
        btnSettings->setText(QCoreApplication::translate("MainWindow", "\342\232\231 Param\303\250tres", nullptr));
        sidebarSpacerWidget->setStyleSheet(QCoreApplication::translate("MainWindow", "background-color: #03224c;", nullptr));
        btnLogout->setText(QCoreApplication::translate("MainWindow", "\342\217\273 D\303\251connexion", nullptr));
        labelTitle->setText(QCoreApplication::translate("MainWindow", "Gestion des Employ\303\251s", nullptr));
        labelTitle->setStyleSheet(QCoreApplication::translate("MainWindow", "font-size: 28px; font-weight: bold; color: #03224c;", nullptr));
        lineSearch->setPlaceholderText(QCoreApplication::translate("MainWindow", "Rechercher un employ\303\251...", nullptr));
        btnSortByPosition->setText(QCoreApplication::translate("MainWindow", "\360\237\223\213 Tri Poste", nullptr));
        btnSortBySalary->setText(QCoreApplication::translate("MainWindow", "\360\237\222\260 Tri Salaire", nullptr));
        btnAnalytics->setText(QCoreApplication::translate("MainWindow", "\360\237\223\212 Statistiques", nullptr));
        btnAdd->setText(QCoreApplication::translate("MainWindow", "+ Ajouter", nullptr));
        statsBox->setTitle(QCoreApplication::translate("MainWindow", "\360\237\223\212 Statistiques par M\303\251tier", nullptr));
        statPecheur->setText(QCoreApplication::translate("MainWindow", "\360\237\216\243 P\303\252cheur: 0", nullptr));
        statPecheur->setStyleSheet(QCoreApplication::translate("MainWindow", "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #dbeafe, stop:1 #bfdbfe); color: #03224c; font-weight: bold; border-radius: 8px; padding: 12px; border: 1px solid #93c5fd;", nullptr));
        statMatelot->setText(QCoreApplication::translate("MainWindow", "\342\233\265 Matelot: 0", nullptr));
        statMatelot->setStyleSheet(QCoreApplication::translate("MainWindow", "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #d1fae5, stop:1 #a7f3d0); color: #065f46; font-weight: bold; border-radius: 8px; padding: 12px; border: 1px solid #6ee7b7;", nullptr));
        statDocker->setText(QCoreApplication::translate("MainWindow", "\360\237\223\246 Docker: 0", nullptr));
        statDocker->setStyleSheet(QCoreApplication::translate("MainWindow", "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #fed7aa, stop:1 #fdba74); color: #92400e; font-weight: bold; border-radius: 8px; padding: 12px; border: 1px solid #fb923c;", nullptr));
        statChefQuai->setText(QCoreApplication::translate("MainWindow", "\360\237\221\224 Chef de quai: 0", nullptr));
        statChefQuai->setStyleSheet(QCoreApplication::translate("MainWindow", "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #e9d5ff, stop:1 #d8b4fe); color: #6b21a8; font-weight: bold; border-radius: 8px; padding: 12px; border: 1px solid #d946ef;", nullptr));
        statSuperviseur->setText(QCoreApplication::translate("MainWindow", "\360\237\223\213 Superviseur: 0", nullptr));
        statSuperviseur->setStyleSheet(QCoreApplication::translate("MainWindow", "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #fecaca, stop:1 #fca5a5); color: #7c2d12; font-weight: bold; border-radius: 8px; padding: 12px; border: 1px solid #f87171;", nullptr));
        salaryStatsBox->setTitle(QCoreApplication::translate("MainWindow", "\360\237\222\260 Statistiques Salariales", nullptr));
        salaryAverage->setText(QCoreApplication::translate("MainWindow", "\360\237\223\212 Salaire Moyen: 0 TND", nullptr));
        salaryAverage->setStyleSheet(QCoreApplication::translate("MainWindow", "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #dbeafe, stop:1 #bfdbfe); color: #03224c; font-weight: bold; border-radius: 8px; padding: 12px; border: 1px solid #93c5fd;", nullptr));
        salaryHighest->setText(QCoreApplication::translate("MainWindow", "\360\237\223\210 Salaire Max: 0 TND", nullptr));
        salaryHighest->setStyleSheet(QCoreApplication::translate("MainWindow", "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #d1fae5, stop:1 #a7f3d0); color: #065f46; font-weight: bold; border-radius: 8px; padding: 12px; border: 1px solid #6ee7b7;", nullptr));
        salaryLowest->setText(QCoreApplication::translate("MainWindow", "\360\237\223\211 Salaire Min: 0 TND", nullptr));
        salaryLowest->setStyleSheet(QCoreApplication::translate("MainWindow", "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #fed7aa, stop:1 #fdba74); color: #92400e; font-weight: bold; border-radius: 8px; padding: 12px; border: 1px solid #fb923c;", nullptr));
        salaryTotal->setText(QCoreApplication::translate("MainWindow", "\360\237\222\265 Total Mensuel: 0 TND", nullptr));
        salaryTotal->setStyleSheet(QCoreApplication::translate("MainWindow", "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #e9d5ff, stop:1 #d8b4fe); color: #6b21a8; font-weight: bold; border-radius: 8px; padding: 12px; border: 1px solid #d946ef;", nullptr));
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
