/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.10.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
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
    QWidget *loginContainer;
    QVBoxLayout *verticalLayout_2;
    QSpacerItem *verticalSpacer_2;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QWidget *loginCard;
    QVBoxLayout *verticalLayout_3;
    QLabel *loginTitle;
    QLabel *loginSubtitle;
    QSpacerItem *verticalSpacer_4;
    QLabel *labelEmail;
    QLineEdit *lineEmail;
    QLabel *labelPassword;
    QLineEdit *linePassword;
    QPushButton *btnLogin;
    QPushButton *btnForgotPassword;
    QSpacerItem *verticalSpacer_3;
    QSpacerItem *horizontalSpacer_2;
    QSpacerItem *verticalSpacer;
    QWidget *employeesPage;
    QHBoxLayout *mainLayout;
    QWidget *sidebar;
    QVBoxLayout *sidebarLayout;
    QLabel *logo;
    QPushButton *btnDashboard;
    QPushButton *btnQuais;
    QPushButton *btnShips;
    QPushButton *btnCaptures;
    QPushButton *btnEmployees;
    QPushButton *btnSettings;
    QSpacerItem *spacerItem;
    QPushButton *btnLogout;
    QWidget *contentArea;
    QVBoxLayout *contentLayout;
    QHBoxLayout *headerLayout;
    QLabel *pageTitle;
    QSpacerItem *spacerItem1;
    QLineEdit *lineSearch;
    QPushButton *btnAdd;
    QHBoxLayout *quickStatsLayout;
    QLabel *statTotal;
    QLabel *statPresent;
    QLabel *statAbsent;
    QGroupBox *groupStats;
    QHBoxLayout *statsLayout;
    QLabel *statPecheur;
    QLabel *statMatelot;
    QLabel *statDocker;
    QLabel *statChefQuai;
    QLabel *statSuperviseur;
    QGroupBox *groupSalary;
    QHBoxLayout *salaryLayout;
    QLabel *salaryAvg;
    QLabel *salaryMax;
    QLabel *salaryMin;
    QTableWidget *tableEmployees;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1200, 700);
        MainWindow->setMinimumSize(QSize(900, 600));
        stackedWidget = new QStackedWidget(MainWindow);
        stackedWidget->setObjectName("stackedWidget");
        loginPage = new QWidget();
        loginPage->setObjectName("loginPage");
        loginLayout = new QVBoxLayout(loginPage);
        loginLayout->setSpacing(0);
        loginLayout->setObjectName("loginLayout");
        loginLayout->setContentsMargins(0, 0, 0, 0);
        loginContainer = new QWidget(loginPage);
        loginContainer->setObjectName("loginContainer");
        verticalLayout_2 = new QVBoxLayout(loginContainer);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_2->addItem(verticalSpacer_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        loginCard = new QWidget(loginContainer);
        loginCard->setObjectName("loginCard");
        loginCard->setMinimumSize(QSize(450, 500));
        loginCard->setMaximumSize(QSize(450, 500));
        verticalLayout_3 = new QVBoxLayout(loginCard);
        verticalLayout_3->setSpacing(20);
        verticalLayout_3->setObjectName("verticalLayout_3");
        verticalLayout_3->setContentsMargins(40, 40, 40, 40);
        loginTitle = new QLabel(loginCard);
        loginTitle->setObjectName("loginTitle");
        loginTitle->setAlignment(Qt::AlignCenter);

        verticalLayout_3->addWidget(loginTitle);

        loginSubtitle = new QLabel(loginCard);
        loginSubtitle->setObjectName("loginSubtitle");
        loginSubtitle->setAlignment(Qt::AlignCenter);

        verticalLayout_3->addWidget(loginSubtitle);

        verticalSpacer_4 = new QSpacerItem(20, 20, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);

        verticalLayout_3->addItem(verticalSpacer_4);

        labelEmail = new QLabel(loginCard);
        labelEmail->setObjectName("labelEmail");

        verticalLayout_3->addWidget(labelEmail);

        lineEmail = new QLineEdit(loginCard);
        lineEmail->setObjectName("lineEmail");

        verticalLayout_3->addWidget(lineEmail);

        labelPassword = new QLabel(loginCard);
        labelPassword->setObjectName("labelPassword");

        verticalLayout_3->addWidget(labelPassword);

        linePassword = new QLineEdit(loginCard);
        linePassword->setObjectName("linePassword");
        linePassword->setEchoMode(QLineEdit::Password);

        verticalLayout_3->addWidget(linePassword);

        btnLogin = new QPushButton(loginCard);
        btnLogin->setObjectName("btnLogin");
        btnLogin->setMinimumSize(QSize(0, 50));
        btnLogin->setCursor(Qt::PointingHandCursor);

        verticalLayout_3->addWidget(btnLogin);

        btnForgotPassword = new QPushButton(loginCard);
        btnForgotPassword->setObjectName("btnForgotPassword");

        verticalLayout_3->addWidget(btnForgotPassword);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_3->addItem(verticalSpacer_3);


        horizontalLayout->addWidget(loginCard);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        verticalLayout_2->addLayout(horizontalLayout);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);


        loginLayout->addWidget(loginContainer);

        stackedWidget->addWidget(loginPage);
        employeesPage = new QWidget();
        employeesPage->setObjectName("employeesPage");
        mainLayout = new QHBoxLayout(employeesPage);
        mainLayout->setSpacing(0);
        mainLayout->setObjectName("mainLayout");
        mainLayout->setContentsMargins(0, 0, 0, 0);
        sidebar = new QWidget(employeesPage);
        sidebar->setObjectName("sidebar");
        sidebar->setMinimumSize(QSize(250, 0));
        sidebar->setMaximumSize(QSize(250, 16777215));
        sidebarLayout = new QVBoxLayout(sidebar);
        sidebarLayout->setSpacing(5);
        sidebarLayout->setObjectName("sidebarLayout");
        sidebarLayout->setContentsMargins(0, 0, 0, 0);
        logo = new QLabel(sidebar);
        logo->setObjectName("logo");
        logo->setAlignment(Qt::AlignCenter);

        sidebarLayout->addWidget(logo);

        btnDashboard = new QPushButton(sidebar);
        btnDashboard->setObjectName("btnDashboard");
        btnDashboard->setCheckable(true);
        btnDashboard->setChecked(false);

        sidebarLayout->addWidget(btnDashboard);

        btnQuais = new QPushButton(sidebar);
        btnQuais->setObjectName("btnQuais");
        btnQuais->setCheckable(true);

        sidebarLayout->addWidget(btnQuais);

        btnShips = new QPushButton(sidebar);
        btnShips->setObjectName("btnShips");
        btnShips->setCheckable(true);

        sidebarLayout->addWidget(btnShips);

        btnCaptures = new QPushButton(sidebar);
        btnCaptures->setObjectName("btnCaptures");
        btnCaptures->setCheckable(true);

        sidebarLayout->addWidget(btnCaptures);

        btnEmployees = new QPushButton(sidebar);
        btnEmployees->setObjectName("btnEmployees");
        btnEmployees->setCheckable(true);
        btnEmployees->setChecked(true);

        sidebarLayout->addWidget(btnEmployees);

        btnSettings = new QPushButton(sidebar);
        btnSettings->setObjectName("btnSettings");
        btnSettings->setCheckable(true);

        sidebarLayout->addWidget(btnSettings);

        spacerItem = new QSpacerItem(0, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        sidebarLayout->addItem(spacerItem);

        btnLogout = new QPushButton(sidebar);
        btnLogout->setObjectName("btnLogout");

        sidebarLayout->addWidget(btnLogout);


        mainLayout->addWidget(sidebar);

        contentArea = new QWidget(employeesPage);
        contentArea->setObjectName("contentArea");
        contentArea->setMinimumSize(QSize(650, 0));
        contentLayout = new QVBoxLayout(contentArea);
        contentLayout->setSpacing(15);
        contentLayout->setObjectName("contentLayout");
        contentLayout->setContentsMargins(25, 25, 25, 25);
        headerLayout = new QHBoxLayout();
        headerLayout->setObjectName("headerLayout");
        pageTitle = new QLabel(contentArea);
        pageTitle->setObjectName("pageTitle");

        headerLayout->addWidget(pageTitle);

        spacerItem1 = new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        headerLayout->addItem(spacerItem1);

        lineSearch = new QLineEdit(contentArea);
        lineSearch->setObjectName("lineSearch");
        lineSearch->setMinimumSize(QSize(250, 0));

        headerLayout->addWidget(lineSearch);

        btnAdd = new QPushButton(contentArea);
        btnAdd->setObjectName("btnAdd");
        btnAdd->setMinimumSize(QSize(140, 0));

        headerLayout->addWidget(btnAdd);


        contentLayout->addLayout(headerLayout);

        quickStatsLayout = new QHBoxLayout();
        quickStatsLayout->setObjectName("quickStatsLayout");
        statTotal = new QLabel(contentArea);
        statTotal->setObjectName("statTotal");
        statTotal->setAlignment(Qt::AlignCenter);

        quickStatsLayout->addWidget(statTotal);

        statPresent = new QLabel(contentArea);
        statPresent->setObjectName("statPresent");
        statPresent->setAlignment(Qt::AlignCenter);

        quickStatsLayout->addWidget(statPresent);

        statAbsent = new QLabel(contentArea);
        statAbsent->setObjectName("statAbsent");
        statAbsent->setAlignment(Qt::AlignCenter);

        quickStatsLayout->addWidget(statAbsent);


        contentLayout->addLayout(quickStatsLayout);

        groupStats = new QGroupBox(contentArea);
        groupStats->setObjectName("groupStats");
        statsLayout = new QHBoxLayout(groupStats);
        statsLayout->setObjectName("statsLayout");
        statPecheur = new QLabel(groupStats);
        statPecheur->setObjectName("statPecheur");
        statPecheur->setAlignment(Qt::AlignCenter);

        statsLayout->addWidget(statPecheur);

        statMatelot = new QLabel(groupStats);
        statMatelot->setObjectName("statMatelot");
        statMatelot->setAlignment(Qt::AlignCenter);

        statsLayout->addWidget(statMatelot);

        statDocker = new QLabel(groupStats);
        statDocker->setObjectName("statDocker");
        statDocker->setAlignment(Qt::AlignCenter);

        statsLayout->addWidget(statDocker);

        statChefQuai = new QLabel(groupStats);
        statChefQuai->setObjectName("statChefQuai");
        statChefQuai->setAlignment(Qt::AlignCenter);

        statsLayout->addWidget(statChefQuai);

        statSuperviseur = new QLabel(groupStats);
        statSuperviseur->setObjectName("statSuperviseur");
        statSuperviseur->setAlignment(Qt::AlignCenter);

        statsLayout->addWidget(statSuperviseur);


        contentLayout->addWidget(groupStats);

        groupSalary = new QGroupBox(contentArea);
        groupSalary->setObjectName("groupSalary");
        salaryLayout = new QHBoxLayout(groupSalary);
        salaryLayout->setObjectName("salaryLayout");
        salaryAvg = new QLabel(groupSalary);
        salaryAvg->setObjectName("salaryAvg");
        salaryAvg->setAlignment(Qt::AlignCenter);

        salaryLayout->addWidget(salaryAvg);

        salaryMax = new QLabel(groupSalary);
        salaryMax->setObjectName("salaryMax");
        salaryMax->setAlignment(Qt::AlignCenter);

        salaryLayout->addWidget(salaryMax);

        salaryMin = new QLabel(groupSalary);
        salaryMin->setObjectName("salaryMin");
        salaryMin->setAlignment(Qt::AlignCenter);

        salaryLayout->addWidget(salaryMin);


        contentLayout->addWidget(groupSalary);

        tableEmployees = new QTableWidget(contentArea);
        if (tableEmployees->columnCount() < 7)
            tableEmployees->setColumnCount(7);
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
        tableEmployees->setObjectName("tableEmployees");
        tableEmployees->setAlternatingRowColors(true);
        tableEmployees->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableEmployees->setSortingEnabled(true);
        tableEmployees->setColumnCount(7);
        tableEmployees->horizontalHeader()->setStretchLastSection(true);

        contentLayout->addWidget(tableEmployees);


        mainLayout->addWidget(contentArea);

        stackedWidget->addWidget(employeesPage);
        MainWindow->setCentralWidget(stackedWidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "\342\232\223 FishTech - Gestion Portuaire", nullptr));
        MainWindow->setStyleSheet(QCoreApplication::translate("MainWindow", "\n"
"    * {\n"
"        font-family: 'Segoe UI', 'Roboto', 'Helvetica Neue', sans-serif;\n"
"        outline: none;\n"
"    }\n"
"    \n"
"    QMainWindow, QDialog {\n"
"        background-color: #f8fafc;\n"
"    }\n"
"    \n"
"    QLabel {\n"
"        color: #334155;\n"
"        font-size: 14px;\n"
"    }\n"
"    \n"
"    QLabel#pageTitle {\n"
"        font-size: 32px;\n"
"        font-weight: 700;\n"
"        color: #0f172a;\n"
"        padding: 10px 0;\n"
"        letter-spacing: -0.5px;\n"
"    }\n"
"    \n"
"    QLabel#sectionTitle {\n"
"        font-size: 20px;\n"
"        font-weight: 600;\n"
"        color: #1e293b;\n"
"        margin-top: 10px;\n"
"    }\n"
"    \n"
"    QLineEdit, QComboBox, QSpinBox, QDoubleSpinBox, QDateEdit, QTextEdit {\n"
"        background-color: #ffffff;\n"
"        border: 1px solid #e2e8f0;\n"
"        border-radius: 8px;\n"
"        padding: 10px 15px;\n"
"        font-size: 14px;\n"
"        color: #1e293b;\n"
"        selection-background-color: #3b82f6;\n"
"    }\n"
"  "
                        "  \n"
"    QLineEdit:hover, QComboBox:hover, QSpinBox:hover {\n"
"        border: 1px solid #cbd5e1;\n"
"    }\n"
"    \n"
"    QLineEdit:focus, QComboBox:focus, QSpinBox:focus {\n"
"        border: 2px solid #3b82f6;\n"
"        background-color: #ffffff;\n"
"    }\n"
"    \n"
"    QPushButton {\n"
"        background-color: #1e40af;\n"
"        color: white;\n"
"        border: none;\n"
"        border-radius: 8px;\n"
"        padding: 12px 24px;\n"
"        font-weight: 600;\n"
"        font-size: 14px;\n"
"    }\n"
"    \n"
"    QPushButton:hover {\n"
"        background-color: #1d4ed8;\n"
"    }\n"
"    \n"
"    QPushButton:pressed {\n"
"        background-color: #1e3a8a;\n"
"        padding-top: 13px;\n"
"        padding-bottom: 11px;\n"
"    }\n"
"    \n"
"    QPushButton:disabled {\n"
"        background-color: #cbd5e1;\n"
"        color: #94a3b8;\n"
"    }\n"
"    \n"
"    QPushButton#btnAdd {\n"
"        background-color: #10b981;\n"
"    }\n"
"    QPushButton#btnAdd:hover {\n"
"        background-co"
                        "lor: #059669;\n"
"    }\n"
"    \n"
"    QPushButton#btnEdit {\n"
"        background-color: #3b82f6;\n"
"        padding: 8px 16px;\n"
"    }\n"
"    \n"
"    QPushButton#btnDelete {\n"
"        background-color: #ef4444;\n"
"        padding: 8px 16px;\n"
"    }\n"
"    \n"
"    QPushButton#btnSecondary {\n"
"        background-color: transparent;\n"
"        border: 1px solid #e2e8f0;\n"
"        color: #475569;\n"
"    }\n"
"    QPushButton#btnSecondary:hover {\n"
"        background-color: #f1f5f9;\n"
"        border-color: #cbd5e1;\n"
"    }\n"
"\n"
"    QWidget#sidebar {\n"
"        background-color: #0f172a;\n"
"        border: none;\n"
"    }\n"
"    \n"
"    QLabel#logo {\n"
"        color: #ffffff;\n"
"        font-size: 24px;\n"
"        font-weight: 800;\n"
"        padding: 40px 20px;\n"
"        border: none;\n"
"    }\n"
"    \n"
"    QPushButton#sidebarBtn, QWidget#sidebar QPushButton {\n"
"        background-color: transparent;\n"
"        color: #94a3b8;\n"
"        text-align: left;\n"
"    "
                        "    padding: 14px 24px;\n"
"        margin: 2px 12px;\n"
"        border-radius: 10px;\n"
"        font-size: 15px;\n"
"        font-weight: 500;\n"
"        border: none;\n"
"    }\n"
"    \n"
"    QPushButton#sidebarBtn:hover, QWidget#sidebar QPushButton:hover {\n"
"        background-color: #1e293b;\n"
"        color: #f8fafc;\n"
"    }\n"
"    \n"
"    QPushButton#sidebarBtn:checked, QWidget#sidebar QPushButton[active=\"true\"] {\n"
"        background-color: #1e40af;\n"
"        color: #ffffff;\n"
"        font-weight: 600;\n"
"    }\n"
"    \n"
"    QPushButton#btnLogout {\n"
"        background-color: rgba(239, 68, 68, 0.1);\n"
"        color: #f87171;\n"
"        margin-top: 20px;\n"
"    }\n"
"    \n"
"    QPushButton#btnLogout:hover {\n"
"        background-color: #ef4444;\n"
"        color: #ffffff;\n"
"    }\n"
"    \n"
"    QTableWidget {\n"
"        background-color: #ffffff;\n"
"        border: 1px solid #e2e8f0;\n"
"        border-radius: 12px;\n"
"        gridline-color: #f1f5f9;\n"
"        s"
                        "election-background-color: #eff6ff;\n"
"        selection-color: #1e40af;\n"
"        outline: none;\n"
"    }\n"
"    \n"
"    QTableWidget::item {\n"
"        padding: 16px;\n"
"        border-bottom: 1px solid #f1f5f9;\n"
"        color: #334155;\n"
"    }\n"
"    \n"
"    QHeaderView::section {\n"
"        background-color: #f8fafc;\n"
"        color: #64748b;\n"
"        padding: 15px;\n"
"        border: none;\n"
"        border-bottom: 2px solid #e2e8f0;\n"
"        font-weight: 700;\n"
"        font-size: 12px;\n"
"        text-transform: uppercase;\n"
"        letter-spacing: 0.5px;\n"
"    }\n"
"    \n"
"    QLabel.statCard {\n"
"        background-color: #ffffff;\n"
"        border: 1px solid #e2e8f0;\n"
"        border-radius: 16px;\n"
"        padding: 24px;\n"
"        font-weight: 600;\n"
"    }\n"
"    \n"
"    QLabel[type=\"stat-blue\"] {\n"
"        background-color: #eff6ff;\n"
"        color: #1e40af;\n"
"        border: 1px solid #bfdbfe;\n"
"    }\n"
"    QLabel[type=\"stat-green\"] {\n"
""
                        "        background-color: #ecfdf5;\n"
"        color: #065f46;\n"
"        border: 1px solid #a7f3d0;\n"
"    }\n"
"    QLabel[type=\"stat-orange\"] {\n"
"        background-color: #fff7ed;\n"
"        color: #9a3412;\n"
"        border: 1px solid #ffedd5;\n"
"    }\n"
"    QLabel[type=\"stat-purple\"] {\n"
"        background-color: #faf5ff;\n"
"        color: #581c87;\n"
"        border: 1px solid #f3e8ff;\n"
"    }\n"
"\n"
"    QScrollBar:vertical {\n"
"        border: none;\n"
"        background: #f1f5f9;\n"
"        width: 8px;\n"
"        margin: 0;\n"
"    }\n"
"    QScrollBar::handle:vertical {\n"
"        background: #cbd5e1;\n"
"        border-radius: 4px;\n"
"        min-height: 40px;\n"
"    }\n"
"    QScrollBar::handle:vertical:hover {\n"
"        background: #94a3b8;\n"
"    }\n"
"        background: #94a3b8;\n"
"    }\n"
"   ", nullptr));
        loginContainer->setStyleSheet(QCoreApplication::translate("MainWindow", "\n"
"         #loginContainer {\n"
"             background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #0f172a, stop:1 #1e293b);\n"
"         }\n"
"         #loginCard {\n"
"             background-color: white;\n"
"             border-radius: 20px;\n"
"         }\n"
"         #loginTitle {\n"
"             font-size: 32px;\n"
"             font-weight: 800;\n"
"             color: #1e40af;\n"
"         }\n"
"         #loginSubtitle {\n"
"             font-size: 14px;\n"
"             color: #64748b;\n"
"         }\n"
"        ", nullptr));
        loginTitle->setText(QCoreApplication::translate("MainWindow", "\342\232\223 FishTech", nullptr));
        loginSubtitle->setText(QCoreApplication::translate("MainWindow", "Connectez-vous pour continuer", nullptr));
        labelEmail->setText(QCoreApplication::translate("MainWindow", "Adresse Email", nullptr));
        labelEmail->setStyleSheet(QCoreApplication::translate("MainWindow", "font-weight: 600; color: #475569;", nullptr));
        lineEmail->setPlaceholderText(QCoreApplication::translate("MainWindow", "votre@email.com", nullptr));
        labelPassword->setText(QCoreApplication::translate("MainWindow", "Mot de passe", nullptr));
        labelPassword->setStyleSheet(QCoreApplication::translate("MainWindow", "font-weight: 600; color: #475569;", nullptr));
        linePassword->setPlaceholderText(QCoreApplication::translate("MainWindow", "\342\200\242\342\200\242\342\200\242\342\200\242\342\200\242\342\200\242\342\200\242\342\200\242", nullptr));
        btnLogin->setText(QCoreApplication::translate("MainWindow", "Se connecter", nullptr));
        btnForgotPassword->setText(QCoreApplication::translate("MainWindow", "Mot de passe oubli\303\251 ?", nullptr));
        btnForgotPassword->setStyleSheet(QCoreApplication::translate("MainWindow", "background: transparent; color: #3b82f6; font-weight: 600;", nullptr));
        logo->setText(QCoreApplication::translate("MainWindow", "\342\232\223 FishTech", nullptr));
        btnDashboard->setText(QCoreApplication::translate("MainWindow", "\360\237\223\212 Tableau de bord", nullptr));
        btnQuais->setText(QCoreApplication::translate("MainWindow", "\342\232\223 Quais", nullptr));
        btnShips->setText(QCoreApplication::translate("MainWindow", "\360\237\232\242 Navires", nullptr));
        btnCaptures->setText(QCoreApplication::translate("MainWindow", "\360\237\220\237 Captures", nullptr));
        btnEmployees->setText(QCoreApplication::translate("MainWindow", "\360\237\221\245 Employ\303\251s", nullptr));
        btnSettings->setText(QCoreApplication::translate("MainWindow", "\342\232\231 Param\303\250tres", nullptr));
        btnLogout->setText(QCoreApplication::translate("MainWindow", "\360\237\232\252 D\303\251connexion", nullptr));
        pageTitle->setText(QCoreApplication::translate("MainWindow", "Gestion des Employ\303\251s", nullptr));
        lineSearch->setPlaceholderText(QCoreApplication::translate("MainWindow", "\360\237\224\215 Rechercher un employ\303\251...", nullptr));
        btnAdd->setText(QCoreApplication::translate("MainWindow", "\342\236\225 Nouvel employ\303\251", nullptr));
        statTotal->setText(QCoreApplication::translate("MainWindow", "\360\237\223\212 Total employ\303\251s: 24", nullptr));
        statTotal->setObjectName(QCoreApplication::translate("MainWindow", "statCard", nullptr));
        statTotal->setProperty("type", QVariant(QCoreApplication::translate("MainWindow", "stat-blue", nullptr)));
        statPresent->setText(QCoreApplication::translate("MainWindow", "\342\234\205 Pr\303\251sents: 18", nullptr));
        statPresent->setObjectName(QCoreApplication::translate("MainWindow", "statCard", nullptr));
        statPresent->setProperty("type", QVariant(QCoreApplication::translate("MainWindow", "stat-green", nullptr)));
        statAbsent->setText(QCoreApplication::translate("MainWindow", "\342\235\214 Absents: 6", nullptr));
        statAbsent->setObjectName(QCoreApplication::translate("MainWindow", "statCard", nullptr));
        statAbsent->setProperty("type", QVariant(QCoreApplication::translate("MainWindow", "stat-orange", nullptr)));
        groupStats->setTitle(QCoreApplication::translate("MainWindow", "R\303\251partition par poste", nullptr));
        statPecheur->setText(QCoreApplication::translate("MainWindow", "\360\237\216\243 P\303\252cheurs: 8", nullptr));
        statPecheur->setObjectName(QCoreApplication::translate("MainWindow", "statCard", nullptr));
        statPecheur->setProperty("type", QVariant(QCoreApplication::translate("MainWindow", "stat-orange", nullptr)));
        statMatelot->setText(QCoreApplication::translate("MainWindow", "\342\233\265 Matelots: 5", nullptr));
        statMatelot->setObjectName(QCoreApplication::translate("MainWindow", "statCard", nullptr));
        statMatelot->setProperty("type", QVariant(QCoreApplication::translate("MainWindow", "stat-green", nullptr)));
        statDocker->setText(QCoreApplication::translate("MainWindow", "\360\237\223\246 Dockers: 4", nullptr));
        statDocker->setObjectName(QCoreApplication::translate("MainWindow", "statCard", nullptr));
        statDocker->setProperty("type", QVariant(QCoreApplication::translate("MainWindow", "stat-purple", nullptr)));
        statChefQuai->setText(QCoreApplication::translate("MainWindow", "\360\237\221\224 Chefs de quai: 4", nullptr));
        statChefQuai->setObjectName(QCoreApplication::translate("MainWindow", "statCard", nullptr));
        statChefQuai->setProperty("type", QVariant(QCoreApplication::translate("MainWindow", "stat-blue", nullptr)));
        statSuperviseur->setText(QCoreApplication::translate("MainWindow", "\360\237\223\213 Superviseurs: 3", nullptr));
        statSuperviseur->setObjectName(QCoreApplication::translate("MainWindow", "statCard", nullptr));
        statSuperviseur->setProperty("type", QVariant(QCoreApplication::translate("MainWindow", "stat-blue", nullptr)));
        groupSalary->setTitle(QCoreApplication::translate("MainWindow", "Statistiques salariales", nullptr));
        salaryAvg->setText(QCoreApplication::translate("MainWindow", "\360\237\223\212 Salaire moyen: 2,450 TND", nullptr));
        salaryAvg->setObjectName(QCoreApplication::translate("MainWindow", "statCard", nullptr));
        salaryAvg->setProperty("type", QVariant(QCoreApplication::translate("MainWindow", "stat-blue", nullptr)));
        salaryMax->setText(QCoreApplication::translate("MainWindow", "\360\237\223\210 Salaire max: 4,200 TND", nullptr));
        salaryMax->setObjectName(QCoreApplication::translate("MainWindow", "statCard", nullptr));
        salaryMax->setProperty("type", QVariant(QCoreApplication::translate("MainWindow", "stat-green", nullptr)));
        salaryMin->setText(QCoreApplication::translate("MainWindow", "\360\237\223\211 Salaire min: 1,350 TND", nullptr));
        salaryMin->setObjectName(QCoreApplication::translate("MainWindow", "statCard", nullptr));
        salaryMin->setProperty("type", QVariant(QCoreApplication::translate("MainWindow", "stat-orange", nullptr)));
        QTableWidgetItem *___qtablewidgetitem = tableEmployees->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("MainWindow", "ID", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = tableEmployees->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("MainWindow", "Nom complet", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = tableEmployees->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("MainWindow", "Poste", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = tableEmployees->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("MainWindow", "Email", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = tableEmployees->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QCoreApplication::translate("MainWindow", "T\303\251l\303\251phone", nullptr));
        QTableWidgetItem *___qtablewidgetitem5 = tableEmployees->horizontalHeaderItem(5);
        ___qtablewidgetitem5->setText(QCoreApplication::translate("MainWindow", "Salaire (TND)", nullptr));
        QTableWidgetItem *___qtablewidgetitem6 = tableEmployees->horizontalHeaderItem(6);
        ___qtablewidgetitem6->setText(QCoreApplication::translate("MainWindow", "Actions", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
