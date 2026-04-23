#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "addedit_employeedialog.h"
#include "addedit_capturedialog.h"
#include "addedit_shipdialog.h"
#include "forgotpassworddialog.h"
#include "connection.h"
#include "fishingzonesdialog.h"
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
#include <QBarSeries>
#include <QBarSet>
#include <QBarCategoryAxis>
#include <QValueAxis>
#include <QDialog>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QHBoxLayout>
#include <QPainter>
#include <QScrollArea>
#include <QPdfWriter>
#include <QFileDialog>
#include <QDateTime>
#include <QPageSize>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QDateEdit>
#include <QGroupBox>
#include <QRandomGenerator>
#include <cmath>
#include <QClipboard>
#include <QApplication>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QSequentialAnimationGroup>
#include <QParallelAnimationGroup>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    Connection::instance()->createConnect(); // Ensure DB connection
    
    setupTableEmployees();
    setupCapturesUi();
    setupTableCaptures();
    generateRandomCaptures();
    setupShipsUi();
    setupTableShips();
    
    setupConnections();
    
    showLoginPage();
    
    updateEmployeeStats();
    updateSalaryStats();
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
        btnEdit->setObjectName("btnEdit");
        btnEdit->setToolTip("Edit");
        btnEdit->setCursor(Qt::PointingHandCursor);
        btnEdit->setMaximumWidth(40);
        connect(btnEdit, &QPushButton::clicked, this, [this, i]() {
            onEditEmployeeClicked(i);
        });
        table->setCellWidget(i, 6, btnEdit);
        
        QPushButton* btnDelete = new QPushButton("🗑️");
        btnDelete->setObjectName("btnDelete");
        btnDelete->setToolTip("Delete");
        btnDelete->setCursor(Qt::PointingHandCursor);
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
    // Optional navigation/buttons present in the UI
    connect(ui->btnCaptures, &QPushButton::clicked, this, &MainWindow::showCapturesPage);
    connect(ui->btnEmployees, &QPushButton::clicked, this, &MainWindow::showEmployeesPage);
    connect(ui->btnQuais, &QPushButton::clicked, this, &MainWindow::showQuaisPage);
    connect(ui->btnShips, &QPushButton::clicked, this, &MainWindow::showShipsPage);
    connect(ui->btnDashboard, &QPushButton::clicked, this, &MainWindow::showEmployeesPage);
    
    // Connect sidebar buttons from Employees Page to switch to Captures
    connect(ui->btnCaptures, &QPushButton::clicked, this, &MainWindow::showCapturesPage);
    // Connect sidebar buttons from Employees Page to switch to Employees (redundant but safe)
    connect(ui->btnEmployees, &QPushButton::clicked, this, &MainWindow::showEmployeesPage);
    // Note: Sidebar buttons for Ships are connected in createSidebar()

    // Connect Quais button from sidebar
    if (ui->btnQuais) {
        connect(ui->btnQuais, &QPushButton::clicked, this, &MainWindow::showQuaisPage);
    }
}

void MainWindow::showLoginPage()
{
    ui->stackedWidget->setCurrentIndex(LoginPage);
    ui->lineEmail->clear();
    ui->linePassword->clear();
}

void MainWindow::showEmployeesPage()
{
    ui->stackedWidget->setCurrentIndex(EmployeesPage);
    updateEmployeeStats();
    updateSalaryStats();
}

void MainWindow::showCapturesPage()
{
    ui->stackedWidget->setCurrentIndex(CapturesPage);
    setupTableCaptures(); // Refresh data
    updateCaptureStats();
}

void MainWindow::showShipsPage()
{
    ui->stackedWidget->setCurrentIndex(ShipsPage);
    setupTableShips();
    updateShipStats();
}

void MainWindow::showQuaisPage()
{
    if (!gestionQuai) {
        setupQuaisUi();
    }
    ui->stackedWidget->setCurrentIndex(QuaisPage);
}

void MainWindow::setupQuaisUi()
{
    QWidget *quaisPage = new QWidget();
    QHBoxLayout *mainLayout = new QHBoxLayout(quaisPage);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QWidget *sidebar = createSidebar(quaisPage);
    mainLayout->addWidget(sidebar);

    gestionQuai = new GestionQuai(quaisPage);
    mainLayout->addWidget(gestionQuai, 1);

    ui->stackedWidget->addWidget(quaisPage);
}

void MainWindow::onAddQuaiClicked()
{
    if (gestionQuai) {
        gestionQuai->showAjouter();
    }
}

void MainWindow::onEditQuaiClicked()
{
    if (gestionQuai) {
        gestionQuai->showModifier();
    }
}

void MainWindow::onDeleteQuaiClicked()
{
    if (gestionQuai) {
        gestionQuai->showSupprimer();
    }
}

void MainWindow::onLoginClicked()
{
    QString username = ui->lineEmail->text();
    QString password = ui->linePassword->text();
    
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter username and password");
        return;
    }
    
    QMessageBox::information(this, "Success", "Login successful!");
    showEmployeesPage();
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
        
        // Delete button
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
        for (int j = 0; j < table->columnCount() - 2; ++j) { // Skip edit/delete columns
            QTableWidgetItem* item = table->item(i, j);
            if (item && item->text().contains(text, Qt::CaseInsensitive)) {
                match = true;
                break;
            }
        }
        table->setRowHidden(i, !match);
    }
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
        ui->salaryAvg->setText("📊 Salaire Moyen: 0 TND");
        ui->salaryMax->setText("📈 Salaire Max: 0 TND");
        ui->salaryMin->setText("📉 Salaire Min: 0 TND");
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
    
    ui->salaryAvg->setText(QString("📊 Salaire Moyen: %1 TND").arg(static_cast<int>(average)));
    ui->salaryMax->setText(QString("📈 Salaire Max: %1 TND").arg(static_cast<int>(highest)));
    ui->salaryMin->setText(QString("📉 Salaire Min: %1 TND").arg(static_cast<int>(lowest)));
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
    pdfWriter.setPageSize(QPageSize::A4);
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

// ==========================================================================================
// CAPTURES MODULE IMPLEMENTATION
// ==========================================================================================

void MainWindow::setupCapturesUi()
{
    QWidget *capturesPage = new QWidget();
    QHBoxLayout *mainLayout = new QHBoxLayout(capturesPage);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // 1. Sidebar
    QWidget *sidebar = createSidebar(capturesPage);
    mainLayout->addWidget(sidebar);

    // 2. Content Area - Same layout as employee management
    QVBoxLayout *contentLayout = new QVBoxLayout();
    
    // Header - Same as employee management
    QHBoxLayout *headerLayout = new QHBoxLayout();
    QLabel *title = new QLabel("Gestion des Captures", capturesPage);
    title->setObjectName("titleLabel");
    headerLayout->addWidget(title);
    headerLayout->addStretch();

    // Search bar with label - Same as employee management
    QLabel *searchLabel = new QLabel("Rechercher:", capturesPage);
    searchLabel->setObjectName("searchLabel"); // Will use default label or specialized if needed
    headerLayout->addWidget(searchLabel);

    QLineEdit *searchLine = new QLineEdit(capturesPage);
    searchLine->setPlaceholderText("Rechercher par navire ou poisson...");
    searchLine->setMinimumWidth(250);
    connect(searchLine, &QLineEdit::textChanged, this, &MainWindow::onSearchCaptures);
    headerLayout->addWidget(searchLine);

    // Sort buttons - Same placement as employee management
    QPushButton *btnSortDate = new QPushButton("📅 Trier Date", capturesPage);
    btnSortDate->setObjectName("btnSortDate");
    connect(btnSortDate, &QPushButton::clicked, this, &MainWindow::onSortCapturesByDate);
    headerLayout->addWidget(btnSortDate);

    QPushButton *btnSortQty = new QPushButton("⚖️ Trier Quantité", capturesPage);
    btnSortQty->setObjectName("btnSortQty");
    connect(btnSortQty, &QPushButton::clicked, this, &MainWindow::onSortCapturesByQuantity);
    headerLayout->addWidget(btnSortQty);

    // Stats button - Same as employee management (Analytics)
    QPushButton *btnStats = new QPushButton("📊 Statistiques", capturesPage);
    btnStats->setObjectName("btnStats");
    connect(btnStats, &QPushButton::clicked, this, &MainWindow::onShowCaptureStats);
    headerLayout->addWidget(btnStats);

    // Add button - Same style as employee management
    QPushButton *btnAdd = new QPushButton("➕ Ajouter", capturesPage);
    btnAdd->setObjectName("btnAdd");
    connect(btnAdd, &QPushButton::clicked, this, &MainWindow::onAddCaptureClicked);
    headerLayout->addWidget(btnAdd);

    // Bouton d'exportation PDF direct (visible et lisible)
    QPushButton *btnPdfExport = new QPushButton("📄 Exporter PDF", capturesPage);
    btnPdfExport->setObjectName("btnPdfExport");
    btnPdfExport->setStyleSheet("QPushButton { background-color: #1e40af; color: white; padding: 8px 15px; border-radius: 5px; font-weight: bold; min-height: 35px; }");
    btnPdfExport->setCursor(Qt::PointingHandCursor);
    connect(btnPdfExport, &QPushButton::clicked, this, &MainWindow::onExportCapturesPdf);

    headerLayout->addWidget(btnPdfExport);
    
    contentLayout->addLayout(headerLayout);

    // Advanced Features Buttons - Bottom Right of Captures Page
    QHBoxLayout *advancedLayout = new QHBoxLayout();
    advancedLayout->addStretch();
    
    // Button 1: Fraud/Illegal Fishing Detection
    QPushButton *btnFraudDetection = new QPushButton("🎮 Détection Fraude", capturesPage);
    btnFraudDetection->setStyleSheet(
        "QPushButton { "
        "background-color: #808080; "  // Gray color
        "color: white; "
        "padding: 10px 20px; "
        "border-radius: 8px; "
        "font-weight: bold; "
        "font-size: 13px; "
        "} "
        "QPushButton:hover { "
        "background-color: #606060; "
        "}"
    );
    btnFraudDetection->setToolTip("Système intelligent de détection de fraude ou de pêche illégale");
    connect(btnFraudDetection, &QPushButton::clicked, this, &MainWindow::onDetectFraud);
    advancedLayout->addWidget(btnFraudDetection);
    
    // Button 2: Fishing Zone Recommendation
    QPushButton *btnZoneRecommendation = new QPushButton("🎮 Zones de Pêche", capturesPage);
    btnZoneRecommendation->setStyleSheet(
        "QPushButton { "
        "background-color: #808080; "  // Gray color
        "color: white; "
        "padding: 10px 20px; "
        "border-radius: 8px; "
        "font-weight: bold; "
        "font-size: 13px; "
        "} "
        "QPushButton:hover { "
        "background-color: #606060; "
        "}"
    );
    btnZoneRecommendation->setToolTip("Recommandation intelligente des zones de pêche");
    connect(btnZoneRecommendation, &QPushButton::clicked, this, &MainWindow::onRecommendFishingZones);
    advancedLayout->addWidget(btnZoneRecommendation);
    
    contentLayout->addLayout(advancedLayout);

    // Stats Box - Same as employee management
    QGroupBox *statsBox = new QGroupBox("📊 Statistiques des Captures", capturesPage);
    QHBoxLayout *statsLayout = new QHBoxLayout(statsBox);
    
    // Create stats labels similar to employee stats
    statDiversity = new QLabel("🐟 Types Poissons: 0", statsBox);
    statTotalQuantity = new QLabel("⚖️ Quantité Totale: 0 kg", statsBox);
    statCaptureAverage = new QLabel("📊 Moyenne: 0 kg", statsBox);
    statCaptureMax = new QLabel("📈 Max: 0 kg", statsBox);
    statCaptureNavires = new QLabel("⚓ Navires: 0", statsBox);
    
    statDiversity->setProperty("type", "stat-blue");
    statTotalQuantity->setProperty("type", "stat-green");
    statCaptureAverage->setProperty("type", "stat-orange");
    statCaptureMax->setProperty("type", "stat-purple");
    statCaptureNavires->setProperty("type", "stat-blue");
    
    for (auto lbl : {statDiversity, statTotalQuantity, statCaptureAverage, statCaptureMax, statCaptureNavires}) {
        lbl->setObjectName("statCard");
        lbl->style()->unpolish(lbl);
        lbl->style()->polish(lbl);
    }
    
    statsLayout->addWidget(statDiversity);
    statsLayout->addWidget(statTotalQuantity);
    statsLayout->addWidget(statCaptureAverage);
    statsLayout->addWidget(statCaptureMax);
    statsLayout->addWidget(statCaptureNavires);
    contentLayout->addWidget(statsBox);

    // Table - Same style as employee management
    tableCaptures = new QTableWidget(capturesPage);
    tableCaptures->setObjectName("tableCaptures");
    tableCaptures->setColumnCount(6);
    tableCaptures->setHorizontalHeaderLabels({"ID", "Navire", "Date", "Type Poisson", "Quantité", "Actions"});
    // ... rest of table setup ...
    tableCaptures->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    tableCaptures->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    tableCaptures->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    tableCaptures->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    tableCaptures->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    tableCaptures->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    tableCaptures->horizontalHeader()->setStretchLastSection(false);
    tableCaptures->verticalHeader()->setVisible(false);
    tableCaptures->verticalHeader()->setDefaultSectionSize(60); 
    tableCaptures->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    tableCaptures->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    tableCaptures->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableCaptures->setSelectionMode(QAbstractItemView::SingleSelection);
    contentLayout->addWidget(tableCaptures);

    // Set preferred/minimum widths so data columns remain readable
    tableCaptures->setColumnWidth(1, 300); // Navire
    tableCaptures->setColumnWidth(2, 140); // Date
    tableCaptures->setColumnWidth(3, 220); // Type Poisson
    tableCaptures->setColumnWidth(4, 120); // Quantité

    mainLayout->addLayout(contentLayout);
    ui->stackedWidget->addWidget(capturesPage);
}

QWidget* MainWindow::createSidebar(QWidget* parent)
{
    QWidget *sidebar = new QWidget(parent);
    sidebar->setObjectName("sidebar");
    sidebar->setMinimumWidth(260);
    
    QVBoxLayout *layout = new QVBoxLayout(sidebar);
    layout->setSpacing(5);
    layout->setContentsMargins(0, 0, 0, 0);

    QLabel *logo = new QLabel("⚓ FISHTECH", sidebar);
    logo->setObjectName("logo");
    logo->setAlignment(Qt::AlignCenter);
    layout->addWidget(logo);

    auto addSidebarBtn = [&](const QString &text, const QString &objName = "") {
        QPushButton *btn = new QPushButton(text, sidebar);
        btn->setCheckable(true);
        btn->setCursor(Qt::PointingHandCursor);
        if (!objName.isEmpty()) btn->setObjectName(objName);
        layout->addWidget(btn);
        return btn;
    };

    QPushButton *btnDash = addSidebarBtn("📊 Tableau de bord");
    QPushButton *btnQuais = addSidebarBtn("⚓ Quais");
    QPushButton *btnNavires = addSidebarBtn("🚢 Navires");
    QPushButton *btnCap = addSidebarBtn("🐟 Captures");
    QPushButton *btnEmp = addSidebarBtn("👥 Employés");
    
    connect(btnEmp, &QPushButton::clicked, this, &MainWindow::showEmployeesPage);
    connect(btnCap, &QPushButton::clicked, this, &MainWindow::showCapturesPage);
    connect(btnNavires, &QPushButton::clicked, this, &MainWindow::showShipsPage);
    connect(btnQuais, &QPushButton::clicked, this, &MainWindow::showQuaisPage);

    layout->addStretch();

    QPushButton *btnLogout = new QPushButton("🚪 Déconnexion", sidebar);
    btnLogout->setObjectName("btnLogout");
    btnLogout->setCursor(Qt::PointingHandCursor);
    connect(btnLogout, &QPushButton::clicked, this, &MainWindow::onLogoutClicked);
    layout->addWidget(btnLogout);
    layout->addSpacing(20);

    return sidebar;
}

void MainWindow::setupTableCaptures()
{
    if (!tableCaptures) return;
    
    // Ensure database connection
    Connection::instance()->createConnect();
    QSqlDatabase db = QSqlDatabase::database();

    QSqlQuery q(db);
    q.exec("CREATE TABLE IF NOT EXISTS captures (ID_Capture INTEGER PRIMARY KEY AUTOINCREMENT, Navire TEXT, Date_Capture TEXT, Type_Poisson TEXT, Quantite INTEGER)");

    tableCaptures->setRowCount(0);
    if (!q.exec("SELECT ID_Capture, Navire, Date_Capture, Type_Poisson, Quantite FROM captures")) {
        qDebug() << "Error loading captures:" << q.lastError().text();
        return;
    }

    while (q.next()) {
        int row = tableCaptures->rowCount();
        tableCaptures->insertRow(row);
        int rawId = q.value(0).toInt();
        QString dispId = QString("CAP%1").arg(rawId, 3, 10, QChar('0'));
        QTableWidgetItem *idItem = new QTableWidgetItem(dispId);
        idItem->setData(Qt::UserRole, rawId);
        tableCaptures->setItem(row, 0, idItem);
        tableCaptures->setItem(row, 1, new QTableWidgetItem(q.value(1).toString()));
        tableCaptures->setItem(row, 2, new QTableWidgetItem(q.value(2).toString()));
        tableCaptures->setItem(row, 3, new QTableWidgetItem(q.value(3).toString()));
        tableCaptures->setItem(row, 4, new QTableWidgetItem(q.value(4).toString() + " kg"));

        // Actions column with Edit, Delete, and PDF buttons - Same as employee management
        QWidget *actionWidget = new QWidget();
        QHBoxLayout *actionLayout = new QHBoxLayout(actionWidget);
        actionLayout->setContentsMargins(0, 0, 0, 0);
        actionLayout->setSpacing(5);
        
        // Delete button
        QPushButton *btnDelete = new QPushButton("🗑️");
        btnDelete->setObjectName("btnDelete");
        btnDelete->setToolTip("Supprimer");
        btnDelete->setCursor(Qt::PointingHandCursor);
        btnDelete->setMaximumWidth(40);
        connect(btnDelete, &QPushButton::clicked, this, [this, row]() { onDeleteCaptureClicked(row); });

        // Edit button
        QPushButton *btnEdit = new QPushButton("✏️");
        btnEdit->setObjectName("btnEdit");
        btnEdit->setToolTip("Modifier");
        btnEdit->setCursor(Qt::PointingHandCursor);
        btnEdit->setMaximumWidth(40);
        connect(btnEdit, &QPushButton::clicked, this, [this, row]() { onEditCaptureClicked(row); });
        actionLayout->addWidget(btnEdit);

        // View button
        QPushButton *btnView = new QPushButton("👀");
        btnView->setObjectName("btnSecondary");
        btnView->setToolTip("Voir les détails");
        btnView->setCursor(Qt::PointingHandCursor);
        btnView->setMaximumWidth(40);
        connect(btnView, &QPushButton::clicked, this, [this, row]() {
            onViewCaptureClicked(row);
        });

        actionLayout->addWidget(btnDelete);
        actionLayout->addWidget(btnView);
        tableCaptures->setCellWidget(row, 5, actionWidget);
    }
    // Ensure ID and Actions columns fit their contents (buttons visible)
    tableCaptures->resizeColumnToContents(0);
    tableCaptures->resizeColumnToContents(5);
    // Ensure Actions column has a reasonable minimum width so buttons never overlap
    int minActionsW = qMax(tableCaptures->columnWidth(5), 180);
    tableCaptures->setColumnWidth(5, minActionsW);
    // Re-apply minimum widths for data columns to keep layout stable after load
    tableCaptures->setColumnWidth(1, qMax(tableCaptures->columnWidth(1), 300));
    tableCaptures->setColumnWidth(2, qMax(tableCaptures->columnWidth(2), 140));
    tableCaptures->setColumnWidth(3, qMax(tableCaptures->columnWidth(3), 220));
    tableCaptures->setColumnWidth(4, qMax(tableCaptures->columnWidth(4), 120));
    updateCaptureStats();
}

void MainWindow::generateRandomCaptures()
{
    // Ensure database connection
    Connection::instance()->createConnect();
    QSqlDatabase db = QSqlDatabase::database();

    // Check if table is empty
    QSqlQuery checkQuery(db);
    checkQuery.exec("SELECT COUNT(*) FROM captures");
    if (checkQuery.next() && checkQuery.value(0).toInt() > 0) {
        return; // Data already exists, don't add more
    }

    // Random data arrays
    QStringList navires = {"Le Grand Bleu", "L'Atlantide", "La Belle Hélène", "Le Requin Blanc", 
                          "Notre Dame des Mers", "Le Neptune", "L'Espérance", "La Star"};
    QStringList typesPoisson = {"Thon", "Sardine", "Maquereau", "Espadon", "Crevette", "Dorade", "Saumon", "Bar"};
    
    QRandomGenerator *generator = QRandomGenerator::global();
    
    // Generate 20 random captures
    for (int i = 0; i < 20; ++i) {
        QString navire = navires[generator->bounded(navires.size())];
        
        // Random date within last 30 days
        int daysAgo = generator->bounded(30);
        QDate date = QDate::currentDate().addDays(-daysAgo);
        
        QString typePoisson = typesPoisson[generator->bounded(typesPoisson.size())];
        int quantite = generator->bounded(50, 500); // 50-500 kg
        
        QSqlQuery insertQuery(db);
        insertQuery.prepare("INSERT INTO captures (Navire, Date_Capture, Type_Poisson, Quantite) VALUES (?, ?, ?, ?)");
        insertQuery.addBindValue(navire);
        insertQuery.addBindValue(date.toString(Qt::ISODate));
        insertQuery.addBindValue(typePoisson);
        insertQuery.addBindValue(quantite);
        insertQuery.exec();
    }
}

void MainWindow::onAddCaptureClicked()
{
    // Ensure database connection is established using Singleton pattern
    Connection::instance()->createConnect();
    
    // Get the database connection
    QSqlDatabase db = QSqlDatabase::database();
    
    AddEditCaptureDialog dlg(this, false);
    if (dlg.exec() == QDialog::Accepted) {
        QSqlQuery q(db);
        q.prepare("INSERT INTO captures (Navire, Date_Capture, Type_Poisson, Quantite) VALUES (?, ?, ?, ?)");
        q.addBindValue(dlg.getNavire());
        q.addBindValue(dlg.getDateCapture().toString(Qt::ISODate));
        q.addBindValue(dlg.getTypePoisson());
        q.addBindValue(dlg.getQuantite());
        
        if (q.exec()) {
            setupTableCaptures();
            QMessageBox::information(this, "Succès", "Capture ajoutée avec succès");
        } else {
            QMessageBox::critical(this, "Erreur", "Erreur lors de l'ajout: " + q.lastError().text());
        }
    }
}

void MainWindow::onOpenAddCaptureWithExportClicked()
{
    // Open the Add dialog with Export PDF enabled
    Connection::instance()->createConnect();
    QSqlDatabase db = QSqlDatabase::database();

    AddEditCaptureDialog dlg(this, false, true);
    if (dlg.exec() == QDialog::Accepted) {
        QSqlQuery q(db);
        q.prepare("INSERT INTO captures (Navire, Date_Capture, Type_Poisson, Quantite) VALUES (?, ?, ?, ?)");
        q.addBindValue(dlg.getNavire());
        q.addBindValue(dlg.getDateCapture().toString(Qt::ISODate));
        q.addBindValue(dlg.getTypePoisson());
        q.addBindValue(dlg.getQuantite());

        if (q.exec()) {
            setupTableCaptures();
            QMessageBox::information(this, "Succès", "Capture ajoutée avec succès");
        } else {
            QMessageBox::critical(this, "Erreur", "Erreur lors de l'ajout: " + q.lastError().text());
        }
    }
}

void MainWindow::onEditCaptureClicked(int row)
{
    int id = tableCaptures->item(row, 0)->data(Qt::UserRole).toInt();
    QString navire = tableCaptures->item(row, 1)->text();
    QDate date = QDate::fromString(tableCaptures->item(row, 2)->text(), Qt::ISODate);
    QString type = tableCaptures->item(row, 3)->text();
    int quantite = tableCaptures->item(row, 4)->text().replace(" kg", "").toInt();

    AddEditCaptureDialog dlg(this, true);
    dlg.setCaptureData(navire, date, type, quantite);

    if (dlg.exec() == QDialog::Accepted) {
        Connection::instance()->createConnect();
        QSqlDatabase db = QSqlDatabase::database();
        QSqlQuery q(db);
        q.prepare("UPDATE captures SET Navire=?, Date_Capture=?, Type_Poisson=?, Quantite=? WHERE ID_Capture=?");
        q.addBindValue(dlg.getNavire());
        q.addBindValue(dlg.getDateCapture().toString(Qt::ISODate));
        q.addBindValue(dlg.getTypePoisson());
        q.addBindValue(dlg.getQuantite());
        q.addBindValue(id);

        if (q.exec()) {
            setupTableCaptures();
            QMessageBox::information(this, "Succès", "Capture modifiée avec succès");
        } else {
            QMessageBox::critical(this, "Erreur", "Erreur lors de la modification");
        }
    }
}

void MainWindow::onDeleteCaptureClicked(int row)
{
    int id = tableCaptures->item(row, 0)->data(Qt::UserRole).toInt();
    if (QMessageBox::question(this, "Confirmation", "Voulez-vous vraiment supprimer cette capture ?", QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        Connection::instance()->createConnect();
        QSqlDatabase db = QSqlDatabase::database();
        QSqlQuery q(db);
        q.prepare("DELETE FROM captures WHERE ID_Capture = ?");
        q.addBindValue(id);
        if (q.exec()) setupTableCaptures();
    }
}

void MainWindow::onViewCaptureClicked(int row)
{
    if (!tableCaptures) return;
    if (row < 0 || row >= tableCaptures->rowCount()) return;
    QString id = tableCaptures->item(row, 0) ? tableCaptures->item(row, 0)->text() : "";
    QString navire = tableCaptures->item(row, 1) ? tableCaptures->item(row, 1)->text() : "";
    QString date = tableCaptures->item(row, 2) ? tableCaptures->item(row, 2)->text() : "";
    QString type = tableCaptures->item(row, 3) ? tableCaptures->item(row, 3)->text() : "";
    QString quantite = tableCaptures->item(row, 4) ? tableCaptures->item(row, 4)->text() : "";

    QDialog dlg(this);
    dlg.setWindowTitle("Détails de la capture");
    dlg.setModal(true);
    dlg.setMinimumSize(480, 340);

    QVBoxLayout *dlgLayout = new QVBoxLayout(&dlg);
    QLabel *hdr = new QLabel("<b style='font-size:16px;'>Détails de la capture</b>", &dlg);
    hdr->setAlignment(Qt::AlignCenter);
    dlgLayout->addWidget(hdr);

    QWidget *content = new QWidget(&dlg);
    content->setStyleSheet("background: #f7f9fc; border-radius:8px; padding:12px;");
    QFormLayout *form = new QFormLayout(content);
    form->setFormAlignment(Qt::AlignCenter);
    form->setLabelAlignment(Qt::AlignCenter);
    const char *keyStyle = "font-weight:600; color:#03224c;";
    const char *valStyle = "color:#07324a;";

    QLabel *k1 = new QLabel("ID Capture:"); k1->setStyleSheet(keyStyle); k1->setAlignment(Qt::AlignCenter);
    QLabel *v1 = new QLabel(id); v1->setStyleSheet(valStyle); v1->setAlignment(Qt::AlignCenter);
    form->addRow(k1, v1);

    QLabel *k2 = new QLabel("Navire:"); k2->setStyleSheet(keyStyle); k2->setAlignment(Qt::AlignCenter);
    QLabel *v2 = new QLabel(navire); v2->setStyleSheet(valStyle); v2->setAlignment(Qt::AlignCenter);
    form->addRow(k2, v2);

    QLabel *k3 = new QLabel("Date de capture:"); k3->setStyleSheet(keyStyle); k3->setAlignment(Qt::AlignCenter);
    QLabel *v3 = new QLabel(date); v3->setStyleSheet(valStyle); v3->setAlignment(Qt::AlignCenter);
    form->addRow(k3, v3);

    QLabel *k4 = new QLabel("Type de poisson:"); k4->setStyleSheet(keyStyle); k4->setAlignment(Qt::AlignCenter);
    QLabel *v4 = new QLabel(type); v4->setStyleSheet(valStyle); v4->setAlignment(Qt::AlignCenter);
    form->addRow(k4, v4);

    QLabel *k5 = new QLabel("Quantité:"); k5->setStyleSheet(keyStyle); k5->setAlignment(Qt::AlignCenter);
    QLabel *v5 = new QLabel(quantite); v5->setStyleSheet(valStyle); v5->setAlignment(Qt::AlignCenter);
    form->addRow(k5, v5);

    dlgLayout->addWidget(content);

    QHBoxLayout *btns = new QHBoxLayout();
    btns->addStretch();
    QDialogButtonBox *box = new QDialogButtonBox(QDialogButtonBox::Close, &dlg);
    box->button(QDialogButtonBox::Close)->setText("Fermer");
    box->button(QDialogButtonBox::Close)->setStyleSheet("QPushButton{background:#03224c;color:white;padding:8px 12px;border-radius:6px;}");
    btns->addWidget(box);
    dlgLayout->addLayout(btns);

    connect(box, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    dlg.exec();
}

void MainWindow::onSearchCaptures(const QString &text)
{
    for (int i = 0; i < tableCaptures->rowCount(); ++i) {
        bool match = false;
        for (int j = 1; j < 4; ++j) {
            if (tableCaptures->item(i, j)->text().contains(text, Qt::CaseInsensitive)) {
                match = true;
                break;
            }
        }
        tableCaptures->setRowHidden(i, !match);
    }
}

void MainWindow::onSortCapturesByDate()
{
    tableCaptures->sortItems(2, Qt::DescendingOrder);
}

void MainWindow::onSortCapturesByQuantity()
{
    // Custom sort for quantity (numeric)
    // For simplicity using string sort here, but ideally should subclass QTableWidgetItem
    tableCaptures->sortItems(4, Qt::DescendingOrder);
}

void MainWindow::updateCaptureStats()
{
    QSqlQuery q("SELECT COUNT(DISTINCT Type_Poisson), SUM(Quantite), AVG(Quantite), MAX(Quantite), COUNT(DISTINCT Navire) FROM captures");
    if (q.next()) {
        statDiversity->setText("🐟 Types Poissons: " + q.value(0).toString());
        statTotalQuantity->setText("⚖️ Quantité Totale: " + q.value(1).toString() + " kg");
        statCaptureAverage->setText("📊 Moyenne: " + QString::number(q.value(2).toDouble(), 'f', 0) + " kg");
        statCaptureMax->setText("📈 Max: " + q.value(3).toString() + " kg");
        statCaptureNavires->setText("⚓ Navires: " + q.value(4).toString());
    } else {
        statDiversity->setText("🐟 Types Poissons: 0");
        statTotalQuantity->setText("⚖️ Quantité Totale: 0 kg");
        statCaptureAverage->setText("📊 Moyenne: 0 kg");
        statCaptureMax->setText("📈 Max: 0 kg");
        statCaptureNavires->setText("⚓ Navires: 0");
    }
}

void MainWindow::updateShipStats()
{
    // Ship stats can be added here if needed
    // For now, just refresh the table
    setupTableShips();
}

void MainWindow::onShowCaptureStats()
{
    // Show a richer analytics dialog similar to employee analytics
    if (!tableCaptures || tableCaptures->rowCount() == 0) {
        QMessageBox::warning(this, "Attention", "Aucune donnée de capture disponible");
        return;
    }

    QDialog* analyticsDialog = new QDialog(this);
    analyticsDialog->setWindowTitle("📊 Statistiques des Captures");
    analyticsDialog->setGeometry(100, 100, 1000, 700);

    QVBoxLayout* layout = new QVBoxLayout(analyticsDialog);
    QHBoxLayout* chartsLayout = new QHBoxLayout();

    // Pie: Quantity by Fish Type
    QMap<QString, double> qtyByType;
    for (int i = 0; i < tableCaptures->rowCount(); ++i) {
        QString type = tableCaptures->item(i, 3)->text();
        QString qtyText = tableCaptures->item(i, 4)->text().replace(" kg", "");
        double q = qtyText.toDouble();
        qtyByType[type] += q;
    }

    QPieSeries* pieSeries = new QPieSeries();
    int colIdx = 0;
    QColor colors[] = {QColor(33,150,243), QColor(244,67,54), QColor(76,175,80), QColor(255,152,0), QColor(156,39,176)};
    for (auto it = qtyByType.begin(); it != qtyByType.end(); ++it) {
        QPieSlice* slice = pieSeries->append(it.key() + ": " + QString::number(it.value(), 'f', 0) + " kg", it.value());
        slice->setColor(colors[colIdx % 5]);
        colIdx++;
    }

    QChart* pieChart = new QChart();
    pieChart->addSeries(pieSeries);
    pieChart->setTitle("Quantité par Type de Poisson");
    pieChart->setAnimationOptions(QChart::SeriesAnimations);

    QChartView* pieChartView = new QChartView(pieChart);
    pieChartView->setRenderHint(QPainter::Antialiasing);
    pieChartView->setMinimumWidth(450);

    // Bar: Captures (count) by Navire
    QMap<QString, int> capturesByNavire;
    for (int i = 0; i < tableCaptures->rowCount(); ++i) {
        QString navire = tableCaptures->item(i, 1)->text();
        capturesByNavire[navire]++;
    }

    QBarSet* barSet = new QBarSet("Count");
    QStringList categories;
    for (auto it = capturesByNavire.begin(); it != capturesByNavire.end(); ++it) {
        categories << it.key();
        *barSet << it.value();
        barSet->setColor(QColor(33,150,243));
    }

    QBarSeries* barSeries = new QBarSeries();
    barSeries->append(barSet);

    QChart* barChart = new QChart();
    barChart->addSeries(barSeries);
    barChart->setTitle("Nombre de Captures par Navire");
    barChart->setAnimationOptions(QChart::SeriesAnimations);

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

    // Résumé des statistiques
    QString statsText = "<b>📈 STATISTIQUES DES CAPTURES</b><br><br>";
    int totalCaptures = tableCaptures->rowCount();
    double totalQty = 0;
    double maxQty = 0;
    double minQty = std::numeric_limits<double>::max();

    for (int i = 0; i < tableCaptures->rowCount(); ++i) {
        QString qtyText = tableCaptures->item(i, 4)->text().replace(" kg", "");
        double q = qtyText.toDouble();
        totalQty += q;
        if (q > maxQty) maxQty = q;
        if (q < minQty) minQty = q;
    }

    double avgQty = totalCaptures > 0 ? totalQty / totalCaptures : 0;

    statsText += QString("Total Captures: <b>%1</b><br>").arg(totalCaptures);
    statsText += QString("Quantité Totale: <b>%1 kg</b><br>").arg(static_cast<int>(totalQty));
    statsText += QString("Moyenne par Capture: <b>%1 kg</b><br>").arg(static_cast<int>(avgQty));
    statsText += QString("Max d'une Capture: <b>%1 kg</b><br>").arg(static_cast<int>(maxQty));
    statsText += QString("Min d'une Capture: <b>%1 kg</b><br><br>").arg((minQty==std::numeric_limits<double>::max()?0:static_cast<int>(minQty)));

    statsText += "<b>📊 Répartition par Type</b><br>";
    for (auto it = qtyByType.begin(); it != qtyByType.end(); ++it) {
        statsText += QString("%1: <b>%2 kg</b><br>").arg(it.key()).arg(static_cast<int>(it.value()));
    }

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
    QPushButton* pdfBtn = new QPushButton("📄 Exporter PDF");
    pdfBtn->setStyleSheet("QPushButton { background-color: #1e40af; color: white; padding: 10px; border-radius: 5px; font-weight: bold; }");
    connect(pdfBtn, &QPushButton::clicked, this, &MainWindow::generateCaptureAnalyticsPDF);
    buttonLayout->addWidget(pdfBtn);
    QPushButton* closeBtn = new QPushButton("Fermer");
    closeBtn->setStyleSheet("QPushButton { background-color: #03224c; color: white; padding: 10px; border-radius: 5px; font-weight: bold; }");
    connect(closeBtn, &QPushButton::clicked, analyticsDialog, &QDialog::accept);
    buttonLayout->addWidget(closeBtn);

    layout->addLayout(buttonLayout);
    analyticsDialog->exec();
}

// Advanced 1: Anomaly Detection
void MainWindow::onDetectAnomalies()
{
    // Calculate Mean and StdDev of diversity
    QList<int> diversities;
    QMap<QString, int> shipDiversity;
    
    QSqlQuery q("SELECT Navire, COUNT(DISTINCT Type_Poisson) FROM captures GROUP BY Navire");
    while (q.next()) {
        QString navire = q.value(0).toString();
        int div = q.value(1).toInt();
        diversities.append(div);
        shipDiversity[navire] = div;
    }

    if (diversities.isEmpty()) return;

    double sum = 0;
    for (int d : diversities) sum += d;
    double mean = sum / diversities.size();

    double sqSum = 0;
    for (int d : diversities) sqSum += (d - mean) * (d - mean);
    double stdDev = std::sqrt(sqSum / diversities.size());

    QString report = "<b>Analyse de Diversité (Moyenne: " + QString::number(mean, 'f', 1) + ")</b><br><br>";
    bool found = false;

    for (auto it = shipDiversity.begin(); it != shipDiversity.end(); ++it) {
        if (it.value() > mean + stdDev) {
            report += "🔴 <b>" + it.key() + "</b>: Diversité ÉLEVÉE (" + QString::number(it.value()) + ")<br>";
            found = true;
        } else if (it.value() < mean - stdDev) {
            report += "🔵 <b>" + it.key() + "</b>: Diversité FAIBLE (" + QString::number(it.value()) + ")<br>";
            found = true;
        }
    }

    if (!found) report += "Aucune anomalie détectée.";

    QMessageBox::information(this, "Détection d'Anomalies", report);
}

// Advanced 2: Comparison
void MainWindow::onComparePeriods()
{
    QDialog dlg(this);
    dlg.setWindowTitle("Comparer Périodes");
    QFormLayout *layout = new QFormLayout(&dlg);

    QDateEdit *start1 = new QDateEdit(QDate::currentDate().addMonths(-1));
    QDateEdit *end1 = new QDateEdit(QDate::currentDate());
    QDateEdit *start2 = new QDateEdit(QDate::currentDate().addMonths(-2));
    QDateEdit *end2 = new QDateEdit(QDate::currentDate().addMonths(-1));
    
    layout->addRow("Début Période 1:", start1);
    layout->addRow("Fin Période 1:", end1);
    layout->addRow("Début Période 2:", start2);
    layout->addRow("Fin Période 2:", end2);

    QDialogButtonBox *box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    layout->addWidget(box);
    connect(box, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(box, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() == QDialog::Accepted) {
        auto getSum = [](QDate s, QDate e) -> int {
            QSqlQuery q;
            q.prepare("SELECT SUM(Quantite) FROM captures WHERE Date_Capture BETWEEN ? AND ?");
            q.addBindValue(s.toString(Qt::ISODate));
            q.addBindValue(e.toString(Qt::ISODate));
            if (q.exec() && q.next()) return q.value(0).toInt();
            return 0;
        };

        int sum1 = getSum(start1->date(), end1->date());
        int sum2 = getSum(start2->date(), end2->date());

        QString msg = QString("Période 1: %1 kg\nPériode 2: %2 kg\n\nDifférence: %3 kg")
                      .arg(sum1).arg(sum2).arg(sum1 - sum2);
        QMessageBox::information(this, "Résultat Comparaison", msg);
    }
}

// ==========================================================================================
// ADVANCED FEATURES: FRAUD DETECTION & FISHING ZONE RECOMMENDATION
// ==========================================================================================

// Advanced Feature 1: Intelligent Fraud Detection / Illegal Fishing Detection
void MainWindow::onDetectFraud()
{
    if (!tableCaptures || tableCaptures->rowCount() == 0) {
        QMessageBox::warning(this, "Attention", "Aucune donnee de capture disponible pour l'analyse");
        return;
    }

    QDialog* fraudDialog = new QDialog(this);
    fraudDialog->setWindowTitle(" detection de Fraude / Peche Illegale");
    fraudDialog->setGeometry(100, 100, 900, 600);
    fraudDialog->setStyleSheet("background-color: white;");

    QVBoxLayout* layout = new QVBoxLayout(fraudDialog);

    // Title
    QLabel* title = new QLabel("<h2> Systeme Intelligent de Detection de Fraude</h2>", fraudDialog);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("color: #03224c; padding: 10px;");
    layout->addWidget(title);

    // Analysis results
    QString report = "<b> RAPPORT D'ANALYSE DE FRAUDE</b><br><br>";
    
    // Collect data
    QList<double> quantities;
    QMap<QString, QList<double>> shipQuantities;
    
    for (int i = 0; i < tableCaptures->rowCount(); ++i) {
        QString qtyText = tableCaptures->item(i, 4)->text().replace(" kg", "");
        double q = qtyText.toDouble();
        quantities.append(q);
        QString navire = tableCaptures->item(i, 1)->text();
        shipQuantities[navire].append(q);
    }

    // Calculate statistics
    double totalQty = 0;
    double maxQty = 0;
    double minQty = std::numeric_limits<double>::max();
    for (double q : quantities) {
        totalQty += q;
        if (q > maxQty) maxQty = q;
        if (q < minQty) minQty = q;
    }
    double avgQty = quantities.size() > 0 ? totalQty / quantities.size() : 0;

    // Calculate standard deviation
    double sqSum = 0;
    for (double q : quantities) {
        sqSum += (q - avgQty) * (q - avgQty);
    }
    double stdDev = quantities.size() > 1 ? std::sqrt(sqSum / quantities.size()) : 0;

    // Check for anomalies
    int suspiciousCount = 0;

    report += "<b> ANOMALIES DETECTEES:</b><br><br>";
    
    // Check 1: Unusually high quantities
    for (int i = 0; i < tableCaptures->rowCount(); ++i) {
        QString qtyText = tableCaptures->item(i, 4)->text().replace(" kg", "");
        double q = qtyText.toDouble();
        
        if (q > avgQty + 2 * stdDev) {
            QString navire = tableCaptures->item(i, 1)->text();
            QString type = tableCaptures->item(i, 3)->text();
            QString date = tableCaptures->item(i, 2)->text();
            report += QString(" Quantite suspecte: %1 kg (Navire: %2, Type: %3, Date: %4)")
                     .arg(q).arg(navire).arg(type).arg(date);
            report += "<br>--> La quantite depasse la moyenne de " + QString::number(static_cast<int>(q - avgQty)) + " kg de plus de 2 ecarts-types<br><br";
            suspiciousCount++;
        }
    }

    // Check 2: Ships with consistently high quantities
    for (auto it = shipQuantities.begin(); it != shipQuantities.end(); ++it) {
        double shipTotal = 0;
        double shipCount = it.value().size();
        for (double q : it.value()) {
            shipTotal += q;
        }
        double shipAvg = shipCount > 0 ? shipTotal / shipCount : 0;
        
        if (shipAvg > avgQty * 1.5) {
            report += QString(" Navire suspect: %1").arg(it.key());
            report += "<br>--> Moyenne des captures: " + QString::number(static_cast<int>(shipAvg)) + " kg (Moyenne globale: " + QString::number(static_cast<int>(avgQty)) + " kg)<br><br";
            suspiciousCount++;
        }
    }

    if (suspiciousCount == 0) {
        report += " Aucune anomalie majeure detectee<br>";
        report += "<br><i>Le systeme n'a trouve aucune indication de fraude ou de peche illegale.</i><br>";
    } else {
        report += "<br><b> RESUME:</b><br>";
        report += QString("--> <b>%1</b> anomalies potentielles detectees<br>").arg(suspiciousCount);
        report += "--> Une enquete plus approfondie est recommandee<br";
    }

    // Statistics section
    report += "<br><br><b> STATISTIQUES DE REFERENCE:</b><br>";
    report += QString("Moyenne des captures: <b>%1 kg</b><br>").arg(static_cast<int>(avgQty));
    report += QString("Ecart-type: <b>%1 kg</b><br>").arg(static_cast<int>(stdDev));
    report += QString("Seuil d'alerte: <b>%1 kg</b> (moyenne + 2xecart-type)<br>").arg(static_cast<int>(avgQty + 2*stdDev));

    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    
    QLabel* reportLabel = new QLabel(report);
    reportLabel->setStyleSheet("background-color: #f5f5f5; padding: 15px; border-radius: 8px; color: #03224c; font-size: 13px;");
    reportLabel->setWordWrap(true);
    reportLabel->setMinimumHeight(300);
    
    scrollArea->setWidget(reportLabel);
    layout->addWidget(scrollArea);

    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    
    QPushButton* closeBtn = new QPushButton("Fermer");
    closeBtn->setStyleSheet("QPushButton { background-color: #03224c; color: white; padding: 10px 20px; border-radius: 5px; font-weight: bold; }");
    connect(closeBtn, &QPushButton::clicked, fraudDialog, &QDialog::accept);
    buttonLayout->addWidget(closeBtn);
    
    layout->addLayout(buttonLayout);
    
    fraudDialog->exec();
}

// Advanced Feature 2: Intelligent Fishing Zone Recommendation
void MainWindow::onRecommendFishingZones()
{
    if (!tableCaptures || tableCaptures->rowCount() == 0) {
        QMessageBox::warning(this, "Attention", "Aucune donnee de capture disponible pour les recommandations");
        return;
    }

    QDialog* zoneDialog = new QDialog(this);
    zoneDialog->setWindowTitle(" Recommendation de Zones de Peche");
    zoneDialog->setGeometry(100, 100, 900, 650);
    zoneDialog->setStyleSheet("background-color: white;");

    QVBoxLayout* layout = new QVBoxLayout(zoneDialog);

    // Title
    QLabel* title = new QLabel("<h2> Systeme Intelligent de Recommendation de Zones de Peche</h2>", zoneDialog);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("color: #03224c; padding: 10px;");
    layout->addWidget(title);

    // Analyze historical data
    QMap<QString, double> fishByType;
    QMap<QString, int> catchCountByType;
    
    for (int i = 0; i < tableCaptures->rowCount(); ++i) {
        QString type = tableCaptures->item(i, 3)->text();
        QString qtyText = tableCaptures->item(i, 4)->text().replace(" kg", "");
        double q = qtyText.toDouble();
        
        fishByType[type] += q;
        catchCountByType[type]++;
    }

    // Calculate best zones based on fish types
    QString recommendation = "<b> ZONES DE PECHE RECOMMANDEES</b><br><br>";
    
    // Sort fish types by total quantity
    QList<QPair<QString, double>> sortedFish;
    for (auto it = fishByType.begin(); it != fishByType.end(); ++it) {
        sortedFish.append(qMakePair(it.key(), it.value()));
    }
    std::sort(sortedFish.begin(), sortedFish.end(), [](const QPair<QString, double>& a, const QPair<QString, double>& b) {
        return a.second > b.second;
    });

    recommendation += "<b> TOP 3 DES ZONES (par quantite totale):</b><br><br>";
    
    int rank = 1;
    for (auto it = sortedFish.begin(); it != sortedFish.end() && rank <= 3; ++it) {
        QString fishType = it->first;
        double totalQty = it->second;
        int catches = catchCountByType[fishType];
        
        double avgQty = catches > 0 ? totalQty / catches : 0;
        
        QString zoneName;
        if (fishType == "Thon") zoneName = "Zone Nord (Eau profonde)";
        else if (fishType == "Sardine") zoneName = "Zone Cotiere Est";
        else if (fishType == "Maquereau") zoneName = "Zone Centrale";
        else if (fishType == "Espadon") zoneName = "Zone Sud-Ouest";
        else if (fishType == "Crevette") zoneName = "Zone des Bancs";
        else if (fishType == "Dorade") zoneName = "Zone Cotiere";
        else if (fishType == "Saumon") zoneName = "Zone de Migration";
        else zoneName = "Zone de Peche Mixte " + QString::number(rank);
        
        recommendation += QString("<b>%1. %2</b><br>").arg(rank).arg(fishType);
        recommendation += QString("   Zone recommande: <b>%1</b><br>").arg(zoneName);
        recommendation += QString("   Quantite totale: <b>%1 kg</b><br>").arg(static_cast<int>(totalQty));
        recommendation += QString("   Nombre de captures: <b>%1</b><br>").arg(catches);
        recommendation += QString("   Moyenne par capture: <b>%1 kg</b><br><br>").arg(static_cast<int>(avgQty));
        
        rank++;
    }

    recommendation += "<b> RECOMMANDATIONS SAISONNIERES:</b><br><br>";
    
    // Check current season and recommend
    QDate currentDate = QDate::currentDate();
    QString season;
    int month = currentDate.month();
    
    if (month >= 3 && month <= 5) {
        season = "Printemps";
        recommendation += " <b>Saison: Printemps</b><br";
        recommendation += "--> Recommendation: Peche au maquereau et sardine pres des zones cotieres<br<br>";
    } else if (month >= 6 && month <= 8) {
        season = "Ete";
        recommendation += " <b>Saison: Ete</b><br";
        recommendation += "--> Recommendation: Peche au thon en eau profonde<br<br>";
    } else if (month >= 9 && month <= 11) {
        season = "Automne";
        recommendation += " <b>Saison: Automne</b><br";
        recommendation += "--> Recommendation: Peche a la dorade et au bar<br<br>";
    } else {
        season = "Hiver";
        recommendation += " <b>Saison: Hiver</b><br";
        recommendation += "--> Recommendation: Peche a la crevette dans les zones abritees<br<br>";
    }

    recommendation += "<b> ZONES A EVITER:</b><br><br>";
    
    // Find least productive zones
    QList<QPair<QString, double>> reverseSorted = sortedFish;
    std::sort(reverseSorted.begin(), reverseSorted.end(), [](const QPair<QString, double>& a, const QPair<QString, double>& b) {
        return a.second < b.second;
    });
    
    for (int i = 0; i < qMin(2, reverseSorted.size()); ++i) {
        recommendation += QString(" %1: Quantite faible (%2 kg) - Consider other zones<br>")
                         .arg(reverseSorted[i].first)
                         .arg(static_cast<int>(reverseSorted[i].second));
    }

    recommendation += "<br><b> RESUME:</b><br>";
    recommendation += QString("--> Total fish types analyzed: <b>%1</b><br>").arg(fishByType.size());
    recommendation += QString("--> Total captures in database: <b>%1</b><br>").arg(tableCaptures->rowCount());
    recommendation += "--> Based on historical performance data<br";

    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    
    QLabel* recLabel = new QLabel(recommendation);
    recLabel->setStyleSheet("background-color: #e8f5e9; padding: 15px; border-radius: 8px; color: #065f46; font-size: 13px;");
    recLabel->setWordWrap(true);
    recLabel->setMinimumHeight(350);
    
    scrollArea->setWidget(recLabel);
    layout->addWidget(scrollArea);

    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    
    QPushButton* applyBtn = new QPushButton(" Appliquer Recommandations");
    applyBtn->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; padding: 10px 20px; border-radius: 5px; font-weight: bold; }");
    connect(applyBtn, &QPushButton::clicked, [this, sortedFish]() {
        QStringList topFish;
        for (int i = 0; i < qMin(3, sortedFish.size()); ++i) {
            topFish.append(sortedFish[i].first);
        }
        QMessageBox::information(this, "Recommandations Appliquees", 
            "Les recommandations ont ete appliquees!\n\n"
            "Types de poissons prioritaires:\n" + topFish.join("\n") + 
            "\n\nLes navires seront informes des zones recommandees.");
    });
    buttonLayout->addWidget(applyBtn);
    
    QPushButton* closeBtn = new QPushButton("Fermer");
    closeBtn->setStyleSheet("QPushButton { background-color: #03224c; color: white; padding: 10px 20px; border-radius: 5px; font-weight: bold; }");
    connect(closeBtn, &QPushButton::clicked, zoneDialog, &QDialog::accept);
    buttonLayout->addWidget(closeBtn);
    
    layout->addLayout(buttonLayout);
    
    // Add a button to show recommended zones on the local 2D map dialog
    QPushButton* mapBtn = new QPushButton("Voir sur carte");
    mapBtn->setStyleSheet("QPushButton { background-color: #1976D2; color: white; padding: 10px 20px; border-radius: 5px; font-weight: bold; }");
    layout->addWidget(mapBtn);

    connect(mapBtn, &QPushButton::clicked, this, [this, sortedFish]() {
        // Build demo ZoneEntry list from top fish types
        QList<ZoneEntry> entries;
        QMap<QString, QPointF> coords;
        coords["Zone Nord (Eau profonde)"] = QPointF(80, 40);
        coords["Zone Cotiere Est"] = QPointF(220, 60);
        coords["Zone Centrale"] = QPointF(160, 140);
        coords["Zone Sud-Ouest"] = QPointF(300, 180);
        coords["Zone des Bancs"] = QPointF(60, 200);
        coords["Zone Cotiere"] = QPointF(260, 100);
        coords["Zone de Migration"] = QPointF(120, 30);

        int limitMap = qMin(3, sortedFish.size());
        for (int i = 0; i < limitMap; ++i) {
            QString fishType = sortedFish[i].first;
            QString zoneName;
            if (fishType == "Thon") zoneName = "Zone Nord (Eau profonde)";
            else if (fishType == "Sardine") zoneName = "Zone Cotiere Est";
            else if (fishType == "Maquereau") zoneName = "Zone Centrale";
            else if (fishType == "Espadon") zoneName = "Zone Sud-Ouest";
            else if (fishType == "Crevette") zoneName = "Zone des Bancs";
            else if (fishType == "Dorade") zoneName = "Zone Cotiere";
            else if (fishType == "Saumon") zoneName = "Zone de Migration";
            else zoneName = "Zone Mixte " + QString::number(i+1);

            QPointF pos = coords.contains(zoneName) ? coords[zoneName] : QPointF(100 + i*60, 80 + i*40);
            QString reason = QString("Type: %1, Quantite totale: %2 kg").arg(fishType).arg(static_cast<int>(sortedFish[i].second));
            entries.append({ zoneName, pos, reason });
        }

        FishingZonesDialog dlg(entries, this);
        dlg.exec();
    });
    
    zoneDialog->exec();
}

void MainWindow::generateCaptureAnalyticsPDF()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Enregistrer Rapport Captures (PDF)", "", "Fichiers PDF (*.pdf)");
    if (fileName.isEmpty()) return;
    if (!fileName.endsWith(".pdf")) fileName += ".pdf";

    QPdfWriter pdfWriter(fileName);
    pdfWriter.setPageSize(QPageSize::A4);
    pdfWriter.setResolution(300);

    QPainter painter;
    if (!painter.begin(&pdfWriter)) {
        QMessageBox::critical(this, "Erreur", "Impossible de créer le fichier PDF.");
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

    // Gather stats
    int totalCaptures = tableCaptures ? tableCaptures->rowCount() : 0;
    double totalQty = 0;
    double maxQty = 0;
    double minQty = std::numeric_limits<double>::max();
    QMap<QString, double> qtyByType;
    QMap<QString, int> capturesByNavire;

    for (int i = 0; tableCaptures && i < tableCaptures->rowCount(); ++i) {
        QString type = tableCaptures->item(i, 3)->text();
        QString qtyText = tableCaptures->item(i, 4)->text().replace(" kg", "");
        double q = qtyText.toDouble();
        totalQty += q;
        if (q > maxQty) maxQty = q;
        if (q < minQty) minQty = q;
        qtyByType[type] += q;
        capturesByNavire[tableCaptures->item(i, 1)->text()]++;
    }

    double avgQty = totalCaptures > 0 ? totalQty / totalCaptures : 0;

    // TITRE
    painter.setFont(titleFont);
    painter.drawText(margin, y, pageWidth - 2*margin, 50, Qt::AlignCenter, "Rapport Statistiques des Captures");
    y += 60;

    painter.setFont(normalFont);
    painter.drawText(margin, y, pageWidth - 2*margin, 30, Qt::AlignCenter,
                     "Généré le: " + QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm"));
    y += 50;

    painter.drawLine(margin, y, pageWidth - margin, y);
    y += 40;

    painter.setFont(headingFont);
    painter.drawText(margin, y, pageWidth - 2*margin, 30, Qt::AlignLeft, "📊 Statistics Summary");
    y += 50;

    painter.setFont(normalFont);
    painter.drawText(margin + 40, y, pageWidth - 2*margin - 40, 25, Qt::AlignLeft,
                     "Total Captures: " + QString::number(totalCaptures));
    y += lineHeight;
    painter.drawText(margin + 40, y, pageWidth - 2*margin - 40, 25, Qt::AlignLeft,
                     "Quantité Totale: " + QString::number(static_cast<int>(totalQty)) + " kg");
    y += lineHeight;
    painter.drawText(margin + 40, y, pageWidth - 2*margin - 40, 25, Qt::AlignLeft,
                     "Moyenne par Capture: " + QString::number(static_cast<int>(avgQty)) + " kg");
    y += lineHeight;
    painter.drawText(margin + 40, y, pageWidth - 2*margin - 40, 25, Qt::AlignLeft,
                     "Max d'une Capture: " + QString::number(static_cast<int>(maxQty)) + " kg");
    y += lineHeight;
    painter.drawText(margin + 40, y, pageWidth - 2*margin - 40, 25, Qt::AlignLeft,
                     "Min d'une Capture: " + QString::number((minQty==std::numeric_limits<double>::max()?0:static_cast<int>(minQty))) + " kg");
    y += 50;

    painter.drawLine(margin, y, pageWidth - margin, y);
    y += 40;

    painter.setFont(headingFont);
    painter.drawText(margin, y, pageWidth - 2*margin, 30, Qt::AlignLeft, "📊 Répartition par Type");
    y += 50;

    painter.setFont(normalFont);
    for (auto it = qtyByType.begin(); it != qtyByType.end(); ++it) {
        painter.drawText(margin + 40, y, pageWidth - 2*margin - 40, 25, Qt::AlignLeft,
                         it.key() + ": " + QString::number(static_cast<int>(it.value())) + " kg");
        y += 30;
        if (y > pdfWriter.height() - 200) { pdfWriter.newPage(); y = 80; }
    }

    painter.end();
    QMessageBox::information(this, "Succès", "PDF généré avec succès :\n\n" + fileName);
}

void MainWindow::onExportCapturesPdf()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Export Captures", "", "PDF Files (*.pdf)");
    if (fileName.isEmpty()) return;
    if (!fileName.endsWith(".pdf")) fileName += ".pdf";

    QPdfWriter writer(fileName);
    writer.setPageSize(QPageSize::A4);
    writer.setResolution(300); // Définition d'une résolution pour la lisibilité

    QPainter painter;
    if (!painter.begin(&writer)) return;

    int y = 150;
    int margin = 100;
    
    // Polices pour la lisibilité
    QFont titleFont("Arial", 20, QFont::Bold);
    QFont headerFont("Arial", 10, QFont::Bold);
    QFont normalFont("Arial", 9);

    painter.setFont(titleFont);
    painter.drawText(margin, y, "Rapport de la Liste des Captures");
    y += 100;
    
    painter.setFont(normalFont);
    painter.drawText(margin, y, "Généré le : " + QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm"));
    y += 150;

    // Simple table header
    painter.setFont(headerFont);
    painter.drawText(margin, y, "Navire");
    painter.drawText(1100, y, "Date");
    painter.drawText(1800, y, "Type de Poisson");
    painter.drawText(2600, y, "Quantité");
    y += 40;
    painter.drawLine(margin, y, writer.width() - margin, y);
    y += 80;

    painter.setFont(normalFont);
    for (int i = 0; i < tableCaptures->rowCount(); ++i) {
        if (tableCaptures->isRowHidden(i)) continue;
        
        painter.drawText(margin, y, tableCaptures->item(i, 1)->text());
        painter.drawText(1100, y, tableCaptures->item(i, 2)->text());
        painter.drawText(1800, y, tableCaptures->item(i, 3)->text());
        painter.drawText(2600, y, tableCaptures->item(i, 4)->text());
        y += 100;
        
        if (y > writer.height() - 200) {
            writer.newPage();
            y = 100;
        }
    }
    
    painter.end();
    QMessageBox::information(this, "Succès", "PDF exporté!");
}

// ==========================================================================================
// SHIPS MODULE IMPLEMENTATION
// ==========================================================================================

void MainWindow::setupShipsUi()
{
    QWidget *shipsPage = new QWidget();
    QHBoxLayout *mainLayout = new QHBoxLayout(shipsPage);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Sidebar
    QWidget *sidebar = createSidebar(shipsPage);
    mainLayout->addWidget(sidebar);

    // Content
    QVBoxLayout *contentLayout = new QVBoxLayout();

    // Header
    QHBoxLayout *headerLayout = new QHBoxLayout();
    QLabel *title = new QLabel("Gestion des Navires", shipsPage);
    title->setObjectName("titleLabel");
    headerLayout->addWidget(title);
    headerLayout->addStretch();

    QLineEdit *searchLine = new QLineEdit(shipsPage);
    searchLine->setPlaceholderText("Rechercher par nom ou immatriculation...");
    connect(searchLine, &QLineEdit::textChanged, this, &MainWindow::onSearchShips);
    headerLayout->addWidget(searchLine);

    QPushButton *btnAdd = new QPushButton("+ Ajouter", shipsPage);
    btnAdd->setObjectName("btnAdd");
    connect(btnAdd, &QPushButton::clicked, this, &MainWindow::onAddShipClicked);
    headerLayout->addWidget(btnAdd);

    contentLayout->addLayout(headerLayout);

    // Toolbar
    QHBoxLayout *toolbarLayout = new QHBoxLayout();
    QPushButton *btnSortCap = new QPushButton("⚖️ Tri Capacité", shipsPage);
    QPushButton *btnSortStat = new QPushButton("⚓ Tri Statut", shipsPage);
    QPushButton *btnStats = new QPushButton("📊 Statistiques", shipsPage);
    QPushButton *btnPdf = new QPushButton("📄 Rapport PDF", shipsPage);
    QPushButton *btnHistory = new QPushButton("📜 Historique", shipsPage);
    QPushButton *btnNotif = new QPushButton("🔔 Disponibilité", shipsPage);

    connect(btnSortCap, &QPushButton::clicked, this, &MainWindow::onSortShipsByCapacity);
    connect(btnSortStat, &QPushButton::clicked, this, &MainWindow::onSortShipsByStatus);
    connect(btnStats, &QPushButton::clicked, this, &MainWindow::onShowShipStats);
    connect(btnPdf, &QPushButton::clicked, this, &MainWindow::onGenerateShipPdf);
    connect(btnHistory, &QPushButton::clicked, this, &MainWindow::onShowShipHistory);
    connect(btnNotif, &QPushButton::clicked, this, &MainWindow::onCheckShipAvailability);

    toolbarLayout->addWidget(btnSortCap);
    toolbarLayout->addWidget(btnSortStat);
    toolbarLayout->addWidget(btnStats);
    toolbarLayout->addWidget(btnPdf);
    toolbarLayout->addWidget(btnHistory);
    toolbarLayout->addWidget(btnNotif);
    toolbarLayout->addStretch();
    contentLayout->addLayout(toolbarLayout);

    // Table
    tableShips = new QTableWidget(shipsPage);
    tableShips->setColumnCount(7);
    tableShips->setHorizontalHeaderLabels({"ID", "Nom", "Immatriculation", "Type", "Capacité", "Statut", "Actions"});
    tableShips->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableShips->verticalHeader()->setVisible(false);
    tableShips->verticalHeader()->setDefaultSectionSize(48);
    tableShips->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    tableShips->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    tableShips->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableShips->setSelectionMode(QAbstractItemView::SingleSelection);
    contentLayout->addWidget(tableShips);

    mainLayout->addLayout(contentLayout);
    ui->stackedWidget->addWidget(shipsPage);
}

void MainWindow::setupTableShips()
{
    if (!tableShips) return;

    // Ensure database connection
    Connection::instance()->createConnect();
    QSqlDatabase db = QSqlDatabase::database();

    QSqlQuery q(db);
    q.exec("CREATE TABLE IF NOT EXISTS navires (ID_Navire INTEGER PRIMARY KEY AUTOINCREMENT, Nom TEXT, Immatriculation TEXT, Type TEXT, Capacite INTEGER, Statut TEXT)");
    q.exec("CREATE TABLE IF NOT EXISTS navire_history (ID_History INTEGER PRIMARY KEY AUTOINCREMENT, ID_Navire INTEGER, Old_Statut TEXT, New_Statut TEXT, Date_Change TEXT)");

    tableShips->setRowCount(0);
    if (!q.exec("SELECT ID_Navire, Nom, Immatriculation, Type, Capacite, Statut FROM navires")) {
        qDebug() << "Error loading ships:" << q.lastError().text();
        return;
    }

    while (q.next()) {
        int row = tableShips->rowCount();
        tableShips->insertRow(row);
        tableShips->setItem(row, 0, new QTableWidgetItem(q.value(0).toString()));
        tableShips->setItem(row, 1, new QTableWidgetItem(q.value(1).toString()));
        tableShips->setItem(row, 2, new QTableWidgetItem(q.value(2).toString()));
        tableShips->setItem(row, 3, new QTableWidgetItem(q.value(3).toString()));
        tableShips->setItem(row, 4, new QTableWidgetItem(q.value(4).toString() + " T"));
        
        QTableWidgetItem *statusItem = new QTableWidgetItem(q.value(5).toString());
        if (q.value(5).toString() == "Interdit") statusItem->setForeground(Qt::red);
        else if (q.value(5).toString() == "En mer") statusItem->setForeground(Qt::blue);
        else statusItem->setForeground(Qt::darkGreen);
        tableShips->setItem(row, 5, statusItem);

        QWidget *actionWidget = new QWidget();
        QHBoxLayout *actionLayout = new QHBoxLayout(actionWidget);
        actionLayout->setContentsMargins(0, 0, 0, 0);
        
        QPushButton *btnEdit = new QPushButton("✏️");
        btnEdit->setObjectName("btnEdit");
        btnEdit->setCursor(Qt::PointingHandCursor);
        btnEdit->setMaximumWidth(40);
        connect(btnEdit, &QPushButton::clicked, this, [this, row]() { onEditShipClicked(row); });

        QPushButton *btnDelete = new QPushButton("🗑️");
        btnDelete->setObjectName("btnDelete");
        btnDelete->setCursor(Qt::PointingHandCursor);
        btnDelete->setMaximumWidth(40);
        connect(btnDelete, &QPushButton::clicked, this, [this, row]() { onDeleteShipClicked(row); });

        actionLayout->addWidget(btnEdit);
        actionLayout->addWidget(btnDelete);
        tableShips->setCellWidget(row, 6, actionWidget);    }
    tableShips->setColumnHidden(0, true);
}

void MainWindow::onAddShipClicked()
{
    // Ensure database connection is established using Singleton pattern
    Connection::instance()->createConnect();
    
    // Get the database connection
    QSqlDatabase db = QSqlDatabase::database();
    
    AddEditShipDialog dlg(this, false);
    if (dlg.exec() == QDialog::Accepted) {
        QSqlQuery q(db);
        q.prepare("INSERT INTO navires (Nom, Immatriculation, Type, Capacite, Statut) VALUES (?, ?, ?, ?, ?)");
        q.addBindValue(dlg.getNom());
        q.addBindValue(dlg.getImmatriculation());
        q.addBindValue(dlg.getType());
        q.addBindValue(dlg.getCapacite());
        q.addBindValue(dlg.getStatut());
        
        if (q.exec()) {
            setupTableShips();
            QMessageBox::information(this, "Succès", "Navire ajouté avec succès");
        } else {
            QMessageBox::critical(this, "Erreur", "Erreur lors de l'ajout: " + q.lastError().text());
        }
    }
}

void MainWindow::onEditShipClicked(int row)
{
    QString id = tableShips->item(row, 0)->text();
    QString nom = tableShips->item(row, 1)->text();
    QString immat = tableShips->item(row, 2)->text();
    QString type = tableShips->item(row, 3)->text();
    QString capacite = tableShips->item(row, 4)->text();
    QString oldStatut = tableShips->item(row, 5)->text();
    AddEditShipDialog dlg(this, true);
    dlg.setShipData(nom, immat, type, capacite, oldStatut);

    if (dlg.exec() == QDialog::Accepted) {
        QString newStatut = dlg.getStatut();
        
        Connection::instance()->createConnect();
        QSqlDatabase db = QSqlDatabase::database();
        QSqlQuery q(db);
        q.prepare("UPDATE navires SET Nom=?, Immatriculation=?, Type=?, Capacite=?, Statut=? WHERE ID_Navire=?");
        q.addBindValue(dlg.getNom());
        q.addBindValue(dlg.getImmatriculation());
        q.addBindValue(dlg.getType());
        q.addBindValue(dlg.getCapacite());
        q.addBindValue(newStatut);
        q.addBindValue(id);

        if (q.exec()) {
            // Advanced 1: History
            if (oldStatut != newStatut) {
                QSqlQuery h(db);
                h.prepare("INSERT INTO navire_history (ID_Navire, Old_Statut, New_Statut, Date_Change) VALUES (?, ?, ?, ?)");
                h.addBindValue(id);
                h.addBindValue(oldStatut);
                h.addBindValue(newStatut);
                h.addBindValue(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
                h.exec();
            }
            setupTableShips();
            QMessageBox::information(this, "Succès", "Navire modifié avec succès");
        } else {
            QMessageBox::critical(this, "Erreur", "Erreur lors de la modification");
        }
    }
}

void MainWindow::onDeleteShipClicked(int row)
{
    QString id = tableShips->item(row, 0)->text();
    if (QMessageBox::question(this, "Confirmation", "Supprimer ce navire ?") == QMessageBox::Yes) {
        Connection::instance()->createConnect();
        QSqlDatabase db = QSqlDatabase::database();
        QSqlQuery q(db);
        q.prepare("DELETE FROM navires WHERE ID_Navire = ?");
        q.addBindValue(id);
        if (q.exec()) setupTableShips();
    }
}

void MainWindow::onSearchShips(const QString &text)
{
    for (int i = 0; i < tableShips->rowCount(); ++i) {
        bool match = false;
        if (tableShips->item(i, 1)->text().contains(text, Qt::CaseInsensitive) || // Nom
            tableShips->item(i, 2)->text().contains(text, Qt::CaseInsensitive)) { // Immat
            match = true;
        }
        tableShips->setRowHidden(i, !match);
    }
}

void MainWindow::onSortShipsByCapacity()
{
    tableShips->sortItems(4, Qt::DescendingOrder);
}

void MainWindow::onSortShipsByStatus()
{
    tableShips->sortItems(5, Qt::AscendingOrder);
}

void MainWindow::onShowShipStats()
{
    // Show a richer analytics dialog similar to employee analytics
    if (!tableShips || tableShips->rowCount() == 0) {
        QMessageBox::warning(this, "Attention", "Aucune donnée de navire disponible");
        return;
    }

    QDialog* analyticsDialog = new QDialog(this);
    analyticsDialog->setWindowTitle("📊 Statistiques des Navires");
    analyticsDialog->setGeometry(100, 100, 1000, 700);

    QVBoxLayout* layout = new QVBoxLayout(analyticsDialog);
    QHBoxLayout* chartsLayout = new QHBoxLayout();

    // Pie: Ships by Type
    QMap<QString, int> shipsByType;
    for (int i = 0; i < tableShips->rowCount(); ++i) {
        QString type = tableShips->item(i, 3)->text();
        shipsByType[type]++;
    }

    QPieSeries* pieSeries = new QPieSeries();
    int colIdx = 0;
    QColor colors[] = {QColor(33,150,243), QColor(244,67,54), QColor(76,175,80), QColor(255,152,0), QColor(156,39,176)};
    for (auto it = shipsByType.begin(); it != shipsByType.end(); ++it) {
        QPieSlice* slice = pieSeries->append(it.key() + ": " + QString::number(it.value()), it.value());
        slice->setColor(colors[colIdx % 5]);
        colIdx++;
    }

    QChart* pieChart = new QChart();
    pieChart->addSeries(pieSeries);
    pieChart->setTitle("Navires par Type");
    pieChart->setAnimationOptions(QChart::SeriesAnimations);

    QChartView* pieChartView = new QChartView(pieChart);
    pieChartView->setRenderHint(QPainter::Antialiasing);
    pieChartView->setMinimumWidth(450);

    // Bar: Ships by Status
    QMap<QString, int> shipsByStatus;
    for (int i = 0; i < tableShips->rowCount(); ++i) {
        QString status = tableShips->item(i, 5)->text();
        shipsByStatus[status]++;
    }

    QBarSet* barSet = new QBarSet("Count");
    QStringList categories;
    for (auto it = shipsByStatus.begin(); it != shipsByStatus.end(); ++it) {
        categories << it.key();
        *barSet << it.value();
        barSet->setColor(QColor(33,150,243));
    }

    QBarSeries* barSeries = new QBarSeries();
    barSeries->append(barSet);

    QChart* barChart = new QChart();
    barChart->addSeries(barSeries);
    barChart->setTitle("Navires par Statut");
    barChart->setAnimationOptions(QChart::SeriesAnimations);

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

    // Statistics Summary
    QString statsText = "<b>📈 STATISTIQUES DES NAVIRES</b><br><br>";
    int totalShips = tableShips->rowCount();
    int totalCapacity = 0;
    int maxCapacity = 0;
    int minCapacity = std::numeric_limits<int>::max();

    for (int i = 0; i < tableShips->rowCount(); ++i) {
        QString capText = tableShips->item(i, 4)->text().replace(" T", "");
        int cap = capText.toInt();
        totalCapacity += cap;
        if (cap > maxCapacity) maxCapacity = cap;
        if (cap < minCapacity) minCapacity = cap;
    }

    double avgCapacity = totalShips > 0 ? static_cast<double>(totalCapacity) / totalShips : 0;

    statsText += QString("Total Navires: <b>%1</b><br>").arg(totalShips);
    statsText += QString("Capacité Totale: <b>%1 T</b><br>").arg(totalCapacity);
    statsText += QString("Capacité Moyenne: <b>%1 T</b><br>").arg(static_cast<int>(avgCapacity));
    statsText += QString("Capacité Max: <b>%1 T</b><br>").arg(maxCapacity);
    statsText += QString("Capacité Min: <b>%1 T</b><br><br>").arg(minCapacity == std::numeric_limits<int>::max() ? 0 : minCapacity);

    statsText += "<b>📊 Répartition par Type</b><br>";
    for (auto it = shipsByType.begin(); it != shipsByType.end(); ++it) {
        statsText += QString("%1: <b>%2</b> navires<br>").arg(it.key()).arg(it.value());
    }
    
    statsText += "<br><b>⚓ Répartition par Statut</b><br>";
    for (auto it = shipsByStatus.begin(); it != shipsByStatus.end(); ++it) {
        QString statusIcon = "";
        if (it.key() == "Disponible") statusIcon = "✅ ";
        else if (it.key() == "En mer") statusIcon = "🌊 ";
        else if (it.key() == "Maintenance") statusIcon = "🔧 ";
        else if (it.key() == "Interdit") statusIcon = "🚫 ";
        statsText += QString("%1%2: <b>%3</b> navires<br>").arg(statusIcon).arg(it.key()).arg(it.value());
    }

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
    QPushButton* pdfBtn = new QPushButton("📄 Exporter PDF");
    pdfBtn->setStyleSheet("QPushButton { background-color: #1e40af; color: white; padding: 10px; border-radius: 5px; font-weight: bold; }");
    connect(pdfBtn, &QPushButton::clicked, this, &MainWindow::generateShipAnalyticsPDF);
    buttonLayout->addWidget(pdfBtn);
    QPushButton* closeBtn = new QPushButton("Fermer");
    closeBtn->setStyleSheet("QPushButton { background-color: #03224c; color: white; padding: 10px; border-radius: 5px; font-weight: bold; }");
    connect(closeBtn, &QPushButton::clicked, analyticsDialog, &QDialog::accept);
    buttonLayout->addWidget(closeBtn);

    layout->addLayout(buttonLayout);
    analyticsDialog->exec();
}

void MainWindow::generateShipAnalyticsPDF()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Enregistrer Rapport Navires (PDF)", "", "Fichiers PDF (*.pdf)");
    if (fileName.isEmpty()) return;
    if (!fileName.endsWith(".pdf")) fileName += ".pdf";

    QPdfWriter pdfWriter(fileName);
    pdfWriter.setPageSize(QPageSize::A4);
    pdfWriter.setResolution(300);

    QPainter painter;
    if (!painter.begin(&pdfWriter)) {
        QMessageBox::critical(this, "Erreur", "Impossible de créer le fichier PDF.");
        return;
    }

    int pageWidth = pdfWriter.width();
    int margin = 80;
    int lineHeight = 40;

    QFont titleFont("Arial", 24, QFont::Bold);
    QFont headingFont("Arial", 14, QFont::Bold);
    QFont normalFont("Arial", 11);

    int y = 80;

    // Gather stats
    int totalShips = tableShips ? tableShips->rowCount() : 0;
    int totalCapacity = 0;
    int maxCapacity = 0;
    int minCapacity = std::numeric_limits<int>::max();
    QMap<QString, int> shipsByType;
    QMap<QString, int> shipsByStatus;

    for (int i = 0; tableShips && i < tableShips->rowCount(); ++i) {
        QString type = tableShips->item(i, 3)->text();
        QString status = tableShips->item(i, 5)->text();
        QString capText = tableShips->item(i, 4)->text().replace(" T", "");
        int cap = capText.toInt();
        
        totalCapacity += cap;
        if (cap > maxCapacity) maxCapacity = cap;
        if (cap < minCapacity) minCapacity = cap;
        shipsByType[type]++;
        shipsByStatus[status]++;
    }

    double avgCapacity = totalShips > 0 ? static_cast<double>(totalCapacity) / totalShips : 0;

    // TITLE
    painter.setFont(titleFont);
    painter.drawText(margin, y, pageWidth - 2*margin, 50, Qt::AlignCenter, "Rapport Statistiques des Navires");
    y += 60;

    painter.setFont(normalFont);
    painter.drawText(margin, y, pageWidth - 2*margin, 30, Qt::AlignCenter,
                     "Généré le: " + QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm"));
    y += 50;

    painter.drawLine(margin, y, pageWidth - margin, y);
    y += 40;

    painter.setFont(headingFont);
    painter.drawText(margin, y, pageWidth - 2*margin, 30, Qt::AlignLeft, "📊 Statistiques Résumé");
    y += 50;

    painter.setFont(normalFont);
    painter.drawText(margin + 40, y, pageWidth - 2*margin - 40, 25, Qt::AlignLeft,
                     "Total Navires: " + QString::number(totalShips));
    y += lineHeight;
    painter.drawText(margin + 40, y, pageWidth - 2*margin - 40, 25, Qt::AlignLeft,
                     "Capacité Totale: " + QString::number(totalCapacity) + " T");
    y += lineHeight;
    painter.drawText(margin + 40, y, pageWidth - 2*margin - 40, 25, Qt::AlignLeft,
                     "Capacité Moyenne: " + QString::number(static_cast<int>(avgCapacity)) + " T");
    y += lineHeight;
    painter.drawText(margin + 40, y, pageWidth - 2*margin - 40, 25, Qt::AlignLeft,
                     "Capacité Max: " + QString::number(maxCapacity) + " T");
    y += lineHeight;
    painter.drawText(margin + 40, y, pageWidth - 2*margin - 40, 25, Qt::AlignLeft,
                     "Capacité Min: " + QString::number(minCapacity == std::numeric_limits<int>::max() ? 0 : minCapacity) + " T");
    y += 50;

    painter.drawLine(margin, y, pageWidth - margin, y);
    y += 40;

    painter.setFont(headingFont);
    painter.drawText(margin, y, pageWidth - 2*margin, 30, Qt::AlignLeft, "🚢 Répartition par Type");
    y += 50;

    painter.setFont(normalFont);
    for (auto it = shipsByType.begin(); it != shipsByType.end(); ++it) {
        painter.drawText(margin + 40, y, pageWidth - 2*margin - 40, 25, Qt::AlignLeft,
                         it.key() + ": " + QString::number(it.value()) + " navires");
        y += 30;
    }
    
    y += 20;
    painter.drawLine(margin, y, pageWidth - margin, y);
    y += 40;
    
    painter.setFont(headingFont);
    painter.drawText(margin, y, pageWidth - 2*margin, 30, Qt::AlignLeft, "⚓ Répartition par Statut");
    y += 50;
    
    painter.setFont(normalFont);
    for (auto it = shipsByStatus.begin(); it != shipsByStatus.end(); ++it) {
        painter.drawText(margin + 40, y, pageWidth - 2*margin - 40, 25, Qt::AlignLeft,
                         it.key() + ": " + QString::number(it.value()) + " navires");
        y += 30;
    }

    painter.end();
    QMessageBox::information(this, "Succès", "PDF généré avec succès :\n\n" + fileName);
}

void MainWindow::onGenerateShipPdf()
{
    int row = tableShips->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Attention", "Veuillez sélectionner un navire pour générer le rapport.");
        return;
    }

    QString nom = tableShips->item(row, 1)->text();
    QString immat = tableShips->item(row, 2)->text();
    QString type = tableShips->item(row, 3)->text();
    QString cap = tableShips->item(row, 4)->text();
    QString stat = tableShips->item(row, 5)->text();

    QString fileName = QFileDialog::getSaveFileName(this, "Rapport Navire", nom + "_Rapport.pdf", "PDF Files (*.pdf)");
    if (fileName.isEmpty()) return;
    if (!fileName.endsWith(".pdf")) fileName += ".pdf";

    QPdfWriter writer(fileName);
    writer.setPageSize(QPageSize::A4);
    QPainter painter(&writer);

    painter.drawText(100, 100, "Rapport du Navire: " + nom);
    painter.drawText(100, 300, "Immatriculation: " + immat);
    painter.drawText(100, 500, "Type: " + type);
    painter.drawText(100, 700, "Capacité: " + cap);
    painter.drawText(100, 900, "Statut Actuel: " + stat);
    painter.drawText(100, 1100, "Date du rapport: " + QDateTime::currentDateTime().toString());

    painter.end();
    QMessageBox::information(this, "Succès", "Rapport PDF généré !");
}

// Advanced 1: History
void MainWindow::onShowShipHistory()
{
    int row = tableShips->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Attention", "Veuillez sélectionner un navire.");
        return;
    }
    QString id = tableShips->item(row, 0)->text();
    QString nom = tableShips->item(row, 1)->text();

    QDialog dlg(this);
    dlg.setWindowTitle("Historique: " + nom);
    dlg.resize(500, 300);
    QVBoxLayout *layout = new QVBoxLayout(&dlg);

    QTableWidget *histTable = new QTableWidget();
    histTable->setColumnCount(3);
    histTable->setHorizontalHeaderLabels({"Ancien Statut", "Nouveau Statut", "Date"});
    histTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QSqlQuery q;
    q.prepare("SELECT Old_Statut, New_Statut, Date_Change FROM navire_history WHERE ID_Navire = ? ORDER BY Date_Change DESC");
    q.addBindValue(id);
    q.exec();

    while (q.next()) {
        int r = histTable->rowCount();
        histTable->insertRow(r);
        histTable->setItem(r, 0, new QTableWidgetItem(q.value(0).toString()));
        histTable->setItem(r, 1, new QTableWidgetItem(q.value(1).toString()));
        histTable->setItem(r, 2, new QTableWidgetItem(q.value(2).toString()));
    }
    layout->addWidget(histTable);
    dlg.exec();
}

// Advanced 2: Notification System
void MainWindow::onCheckShipAvailability()
{
    QSqlQuery q("SELECT COUNT(*) FROM navires WHERE Statut IN ('Interdit', 'En mer', 'Maintenance')");
    if (q.next()) {
        int count = q.value(0).toInt();
        QString msg = QString("%1 navire(s) sont actuellement indisponibles (En mer, Interdit ou Maintenance).").arg(count);
        
        QSystemTrayIcon *trayIcon = new QSystemTrayIcon(this);
        trayIcon->setIcon(QIcon::fromTheme("dialog-warning")); // Use a standard icon or resource
        trayIcon->show();
        trayIcon->showMessage("Alerte Disponibilité Navires", msg, QSystemTrayIcon::Warning, 5000);
        
        // Also show message box for immediate feedback
        QMessageBox::warning(this, "Disponibilité Navires", msg);
    }
}
