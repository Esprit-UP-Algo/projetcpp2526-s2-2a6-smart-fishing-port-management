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
#include <QLabel>
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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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

void MainWindow::setupTableEmployees()
{
    QTableWidget* table = ui->tableEmployees;
    if (!table) return;

    table->verticalHeader()->setVisible(false);
    table->horizontalHeader()->setStretchLastSection(true);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
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

        QPushButton* btnEdit = new QPushButton("✏️");
        btnEdit->setToolTip("Edit");
        btnEdit->setStyleSheet(
            "QPushButton { "
            "background-color: #2196F3; "
            "color: white; "
            "border: none; "
            "border-radius: 5px; "
            "padding: 5px 10px; "
            "font-weight: bold; "
            "} "
            "QPushButton:hover { "
            "background-color: #1976D2; "
            "} "
            "QPushButton:pressed { "
            "background-color: #1565C0; "
            "}"
            );
        btnEdit->setMaximumWidth(40);
        connect(btnEdit, &QPushButton::clicked, this, [this, i]() {
            onEditEmployeeClicked(i);
        });
        table->setCellWidget(i, 6, btnEdit);

        QPushButton* btnDelete = new QPushButton("🗑️");
        btnDelete->setToolTip("Delete");
        btnDelete->setStyleSheet(
            "QPushButton { "
            "background-color: #F44336; "
            "color: white; "
            "border: none; "
            "border-radius: 5px; "
            "padding: 5px 10px; "
            "font-weight: bold; "
            "} "
            "QPushButton:hover { "
            "background-color: #E53935; "
            "} "
            "QPushButton:pressed { "
            "background-color: #D32F2F; "
            "}"
            );
        btnDelete->setMaximumWidth(40);
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
}

// Helper to hide employee-specific controls
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

// Helper to show employee-specific controls
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
    ui->labelTitle->setText("Gestion des Quais");
}

void MainWindow::onShowDashboard()
{
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
    ui->labelTitle->setText("Tableau de bord");
}

void MainWindow::onShowShips()
{
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
    ui->labelTitle->setText("Gestion des Navires");
}

void MainWindow::onShowCaptures()
{
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
    ui->labelTitle->setText("Gestion des Captures");
}

void MainWindow::onShowEmployeesContent()
{
    if (currentModuleWidget) {
        ui->contentLayout->removeWidget(currentModuleWidget);
        currentModuleWidget->hide();
        currentModuleWidget = nullptr;
    }
    showEmployeeControls(ui);
    ui->labelTitle->setText("Employees");
    loadEmployeesFromDb();
    updateEmployeeStats();
    updateSalaryStats();
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
    QString username = ui->lineUsername->text();
    QString password = ui->linePassword->text();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter username and password");
        return;
    }

    showEmployeesPage();
    onShowDashboard();
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
        showLoginPage();
    }
}

void MainWindow::onAddEmployeeClicked()
{
    AddEditEmployeeDialog dialog(this, false);
    if (dialog.exec() == QDialog::Accepted) {
        QTableWidget* table = ui->tableEmployees;
        if (!table) return;

        int newRow = table->rowCount();
        table->insertRow(newRow);

        table->setItem(newRow, 0, new QTableWidgetItem(dialog.getId()));
        table->setItem(newRow, 1, new QTableWidgetItem(dialog.getNom()));
        table->setItem(newRow, 2, new QTableWidgetItem(dialog.getPoste()));
        table->setItem(newRow, 3, new QTableWidgetItem(dialog.getEmail()));
        table->setItem(newRow, 4, new QTableWidgetItem(dialog.getTelephone()));
        table->setItem(newRow, 5, new QTableWidgetItem(QString::number(dialog.getSalaire(), 'f', 0) + " TND"));

        QPushButton* btnEdit = new QPushButton("✏️");
        btnEdit->setMaximumWidth(40);
        btnEdit->setStyleSheet(
            "QPushButton { background-color: #2196F3; color: white; border: none; border-radius: 5px; padding: 5px 10px; }"
            );
        connect(btnEdit, &QPushButton::clicked, this, [this, newRow]() {
            onEditEmployeeClicked(newRow);
        });
        table->setCellWidget(newRow, 6, btnEdit);

        QPushButton* btnDelete = new QPushButton("🗑️");
        btnDelete->setMaximumWidth(40);
        btnDelete->setStyleSheet(
            "QPushButton { background-color: #F44336; color: white; border: none; border-radius: 5px; padding: 5px 10px; }"
            );
        connect(btnDelete, &QPushButton::clicked, this, [this, newRow]() {
            onDeleteEmployeeClicked(newRow);
        });
        table->setCellWidget(newRow, 7, btnDelete);

        updateEmployeeStats();
        updateSalaryStats();
        QMessageBox::information(this, "Success", "Employee added successfully!");
    }
}

void MainWindow::onEditEmployeeClicked(int row)
{
    QTableWidget* table = ui->tableEmployees;
    if (!table || row < 0 || row >= table->rowCount())
        return;

    QString id = table->item(row, 0)->text();
    QString nom = table->item(row, 1)->text();
    QString poste = table->item(row, 2)->text();
    QString email = table->item(row, 3)->text();
    QString telephone = table->item(row, 4)->text();
    QString salaryText = table->item(row, 5)->text();
    salaryText.replace(" TND", "");
    double salary = salaryText.toDouble();

    AddEditEmployeeDialog dialog(this, true);
    dialog.setEmployeeData(id, nom, poste, email, telephone, salary);

    if (dialog.exec() == QDialog::Accepted) {
        table->item(row, 1)->setText(dialog.getNom());
        table->item(row, 2)->setText(dialog.getPoste());
        table->item(row, 3)->setText(dialog.getEmail());
        table->item(row, 4)->setText(dialog.getTelephone());
        table->item(row, 5)->setText(QString::number(dialog.getSalaire(), 'f', 0) + " TND");

        updateEmployeeStats();
        updateSalaryStats();
        QMessageBox::information(this, "Success", "Employee updated successfully!");
    }
}

void MainWindow::onDeleteEmployeeClicked(int row)
{
    QTableWidget* table = ui->tableEmployees;
    if (!table || row < 0 || row >= table->rowCount())
        return;

    QString nom = table->item(row, 1)->text();
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Confirm Delete",
        QString("Are you sure you want to delete %1?").arg(nom),
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        table->removeRow(row);
        updateEmployeeStats();
        updateSalaryStats();
        QMessageBox::information(this, "Success", "Employee deleted successfully!");
    }
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
        qDebug() << "Failed to connect to database for employee loading";
        return;
    }

    table->setRowCount(0);

    QSqlQuery query;
    if (!query.exec("SELECT id, nom, poste, email, telephone, salaire FROM employes ORDER BY id")) {
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

        QPushButton* btnEdit = new QPushButton("✏️");
        btnEdit->setToolTip("Edit");
        btnEdit->setStyleSheet(
            "QPushButton { "
            "background-color: #2196F3; "
            "color: white; "
            "border: none; "
            "border-radius: 5px; "
            "padding: 5px 10px; "
            "font-weight: bold; "
            "} "
            "QPushButton:hover { "
            "background-color: #1976D2; "
            "} "
            "QPushButton:pressed { "
            "background-color: #1565C0; "
            "}"
            );
        connect(btnEdit, &QPushButton::clicked, [this, rowCount]() { onEditEmployeeClicked(rowCount); });
        table->setCellWidget(rowCount, 6, btnEdit);

        QPushButton* btnDelete = new QPushButton("🗑️");
        btnDelete->setToolTip("Delete");
        btnDelete->setStyleSheet(
            "QPushButton { "
            "background-color: #F44336; "
            "color: white; "
            "border: none; "
            "border-radius: 5px; "
            "padding: 5px 10px; "
            "font-weight: bold; "
            "} "
            "QPushButton:hover { "
            "background-color: #DA190B; "
            "} "
            "QPushButton:pressed { "
            "background-color: #BA0000; "
            "}"
            );
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
        QMessageBox::warning(this, "Error", "No employee data available");
        return;
    }

    QDialog* analyticsDialog = new QDialog(this);
    analyticsDialog->setWindowTitle("📊 Employee Analytics & Statistics");
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
    QString fileName = QFileDialog::getSaveFileName(this, "Save Analytics PDF", "", "PDF Files (*.pdf)");

    if (fileName.isEmpty()) {
        return;
    }

    if (!fileName.endsWith(".pdf")) {
        fileName += ".pdf";
    }

    QPdfWriter pdfWriter(fileName);
    pdfWriter.setPageSize(QPageSize(QPageSize::A4));
    pdfWriter.setResolution(300);

    QPainter painter;
    if (!painter.begin(&pdfWriter)) {
        QMessageBox::critical(this, "Error", "Failed to create PDF file");
        return;
    }

    int pageWidth = pdfWriter.width();
    int margin = 80;
    int lineHeight = 40;

    QFont titleFont("Arial", 24, QFont::Bold);
    QFont headingFont("Arial", 14, QFont::Bold);
    QFont subHeadingFont("Arial", 12, QFont::Bold);
    QFont normalFont("Arial", 11);

    int y = 80;
    QTableWidget* table = ui->tableEmployees;

    double totalSalary = 0;
    double highestSalary = 0;
    double lowestSalary = std::numeric_limits<double>::max();
    QMap<QString, int> positionCount;
    QMap<QString, double> salaryByPosition;

    for (int i = 0; i < table->rowCount(); ++i) {
        QString poste = table->item(i, 2)->text();
        QString salaryText = table->item(i, 5)->text().replace(" TND", "");
        double salary = salaryText.toDouble();

        totalSalary += salary;
        if (salary > highestSalary) highestSalary = salary;
        if (salary < lowestSalary) lowestSalary = salary;
        positionCount[poste]++;
        salaryByPosition[poste] += salary;
    }

    int totalEmployees = table->rowCount();
    double averageSalary = totalEmployees > 0 ? totalSalary / totalEmployees : 0;

    // TITLE
    painter.setFont(titleFont);
    painter.drawText(margin, y, pageWidth - 2*margin, 50, Qt::AlignCenter, "Employee Analytics Report");
    y += 60;

    // DATE
    painter.setFont(normalFont);
    painter.drawText(margin, y, pageWidth - 2*margin, 30, Qt::AlignCenter,
                     "Generated: " + QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm"));
    y += 50;

    painter.drawLine(margin, y, pageWidth - margin, y);
    y += 40;

    painter.setFont(headingFont);
    painter.drawText(margin, y, pageWidth - 2*margin, 30, Qt::AlignLeft, "📊 Statistics Summary");
    y += 50;

    painter.setFont(normalFont);

    painter.drawText(margin + 40, y, pageWidth - 2*margin - 40, 25, Qt::AlignLeft,
                     "Total Employees: " + QString::number(totalEmployees));
    y += lineHeight;

    painter.drawText(margin + 40, y, pageWidth - 2*margin - 40, 25, Qt::AlignLeft,
                     "Average Salary: " + QString::number(static_cast<int>(averageSalary)) + " TND");
    y += lineHeight;

    painter.drawText(margin + 40, y, pageWidth - 2*margin - 40, 25, Qt::AlignLeft,
                     "Highest Salary: " + QString::number(static_cast<int>(highestSalary)) + " TND");
    y += lineHeight;

    painter.drawText(margin + 40, y, pageWidth - 2*margin - 40, 25, Qt::AlignLeft,
                     "Lowest Salary: " + QString::number(static_cast<int>(lowestSalary)) + " TND");
    y += lineHeight;

    painter.drawText(margin + 40, y, pageWidth - 2*margin - 40, 25, Qt::AlignLeft,
                     "Total Monthly Payroll: " + QString::number(static_cast<int>(totalSalary)) + " TND");
    y += 50;

    painter.drawLine(margin, y, pageWidth - margin, y);
    y += 40;

    painter.setFont(headingFont);
    painter.drawText(margin, y, pageWidth - 2*margin, 30, Qt::AlignLeft, "👤 Position Breakdown");
    y += 50;

    painter.setFont(normalFont);
    for (auto it = positionCount.begin(); it != positionCount.end(); ++it) {
        double avgPay = it.value() > 0 ? salaryByPosition[it.key()] / it.value() : 0;

        painter.setFont(subHeadingFont);
        painter.drawText(margin + 40, y, pageWidth - 2*margin - 40, 25, Qt::AlignLeft, it.key());
        y += 35;

        painter.setFont(normalFont);
        painter.drawText(margin + 60, y, pageWidth - 2*margin - 60, 25, Qt::AlignLeft,
                         "Employees: " + QString::number(it.value()));
        y += 30;

        painter.drawText(margin + 60, y, pageWidth - 2*margin - 60, 25, Qt::AlignLeft,
                         "Average: " + QString::number(static_cast<int>(avgPay)) + " TND");
        y += 30;

        painter.drawText(margin + 60, y, pageWidth - 2*margin - 60, 25, Qt::AlignLeft,
                         "Total: " + QString::number(static_cast<int>(salaryByPosition[it.key()])) + " TND");
        y += 50;
    }

    painter.end();

    QMessageBox::information(this, "✓ Success", "PDF generated successfully!\n\n" + fileName);
}

// =====================================================
// NOUVEAUX SLOTS POUR PLANNING ET AFFECTATION
// =====================================================

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
