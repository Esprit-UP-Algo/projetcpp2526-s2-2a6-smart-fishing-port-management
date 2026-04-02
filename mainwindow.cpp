#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialogs/addedit_employeedialog.h"
#include "dialogs/forgotpassworddialog.h"
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
#include <QLabel>
#include <QHBoxLayout>
#include <QPainter>
#include <QScrollArea>
#include <QPdfWriter>
#include <QFileDialog>
#include <QDateTime>
#include <QLineSeries>
#include <QSqlRecord>
#include <QPageSize>
#include <QCryptographicHash>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>
#include "int7/gestionquai.h"
#include "int7/gestionnavires.h"
#include "int7/gestioncaptures.h"
#include "connection.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    setupTableEmployees();
    loadEmployeesFromDb();
    
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
    table->setRowCount(0);
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
    if (!quaiWidget) quaiWidget = new GestionQuai(this);
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
    if (!quaiWidget) quaiWidget = new GestionQuai(this);
    if (!navireWidget) navireWidget = new GestionNavires(this);
    if (!captureWidget) captureWidget = new GestionCaptures(this);
    if (!utilWidget) utilWidget = new GestionUtilisateurs(this);

    utilWidget->loadFromDb();
    navireWidget->loadFromDb();
    captureWidget->loadFromDb();
    quaiWidget->loadFromDb();

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
    if (!navireWidget) navireWidget = new GestionNavires(this);
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
    if (!captureWidget) captureWidget = new GestionCaptures(this);
    if (currentModuleWidget == captureWidget) return;
    if (currentModuleWidget) {
        ui->contentLayout->removeWidget(currentModuleWidget);
        currentModuleWidget->hide();
    }
    hideEmployeeControls(ui);

    Connection conn;
    if (conn.createconnect()) {
        captureWidget->loadFromDb();
    } else {
        qDebug() << "[MainWindow] Could not connect to DB when opening Captures; falling back to local data.";
    }
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

void MainWindow::initializePageVisibility()
{
    SessionManager &session = SessionManager::getInstance();
    
    // Hide all navigation buttons by default
    ui->btnEmployees->setVisible(false);
    ui->btnCaptures->setVisible(false);
    ui->btnShips->setVisible(false);
    ui->btnQuais->setVisible(false);
    ui->btnAnalytics->setVisible(false);
    ui->btnDashboard->setVisible(false);
    
    // Show buttons based on accessible pages
    QList<SessionManager::AccessPage> pages = session.getAccessiblePages();
    
    for (auto page : pages) {
        switch (page) {
            case SessionManager::PageEmployees:
                ui->btnEmployees->setVisible(true);
                break;
            case SessionManager::PageCaptures:
                ui->btnCaptures->setVisible(true);
                break;
            case SessionManager::PageNavires:
                ui->btnShips->setVisible(true);
                break;
            case SessionManager::PageQuai:
                ui->btnQuais->setVisible(true);
                break;
            case SessionManager::PageAnalytics:
                ui->btnAnalytics->setVisible(true);
                break;
            case SessionManager::PageDashboard:
                ui->btnDashboard->setVisible(true);
                break;
            default:
                break;
        }
    }
}

void MainWindow::updateMenuForRole()
{
    SessionManager &session = SessionManager::getInstance();
    QString role = session.getCurrentRoleString();
    QString user = session.getCurrentUser().nom;
    
    // Update title bar with role information
    this->setWindowTitle(QString("FishTech - %1 (%2)").arg(user, role));
    
    // Update label showing current user
    ui->labelTitle->setText(QString("connecté en tant que: %1").arg(role));
}


void MainWindow::onLoginClicked()
{
    QString username = ui->lineUsername->text().trimmed();
    QString password = ui->linePassword->text();
    
    // Remove any surrounding quotes that might have been added
    if (password.startsWith('"') && password.endsWith('"')) {
        password = password.mid(1, password.length() - 2);
    }
    if (password.startsWith("'") && password.endsWith("'")) {
        password = password.mid(1, password.length() - 2);
    }
    password = password.trimmed();
    
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "❌ Login Failed", "Please enter username and password");
        return;
    }
    
    // Attempt authentication using SessionManager
    SessionManager &session = SessionManager::getInstance();
    if (!session.authenticate(username, password)) {
        // Show error with debug info
        QMessageBox::warning(this, "❌ Login Failed", 
            QString("Invalid credentials for: %1\n\n"
                   "Password: %2\n\n"
                   "Check the Output panel for the SQL command to fix the database.\n"
                   "Or verify that PASSWORD_HASH column has the correct hash.").arg(username, password));
        ui->linePassword->clear();
        return;
    }
    
    // Authentication successful
    Employe user = session.getCurrentUser();
    QMessageBox::information(this, "✅ Login Successful", 
                           QString("Welcome %1 (%2)!").arg(user.nom, session.getCurrentRoleString()));
    
    // Initialize page visibility based on role
    initializePageVisibility();
    updateMenuForRole();
    
    // Navigate to dashboard or appropriate page
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
        // Clear session
        SessionManager::getInstance().logout();
        
        // Reset UI
        this->setWindowTitle("FishTech");
        ui->labelTitle->setText("Not logged in");
        
        // Return to login page
        showLoginPage();
        QMessageBox::information(this, "✅ Logged Out", "You have been successfully logged out.");
    }
}

QString MainWindow::generateNextEmployeeId()
{
    QTableWidget* table = ui->tableEmployees;
    int maxNum = 0;
    

    for (int row = 0; row < table->rowCount(); ++row) {
        QTableWidgetItem* item = table->item(row, 0);
        if (item) {
            QString id = item->text();
            if (id.startsWith("E")) {
                bool ok;
                int num = id.mid(1).toInt(&ok);
                if (ok && num > maxNum) {
                    maxNum = num;
                }
            }
        }
    }
    
    return "E" + QString::number(maxNum + 1);
}

void MainWindow::onAddEmployeeClicked()
{
    AddEditEmployeeDialog dialog(this, false);
    
    // Collecter les emails et téléphones existants
    QStringList existingEmails, existingPhones;
    if (ui->tableEmployees) {
        for (int i = 0; i < ui->tableEmployees->rowCount(); ++i) {
            QTableWidgetItem* emailItem = ui->tableEmployees->item(i, 3);
            QTableWidgetItem* phoneItem = ui->tableEmployees->item(i, 4);
            if (emailItem) existingEmails << emailItem->text();
            if (phoneItem) existingPhones << phoneItem->text();
        }
    }
    
    dialog.setExistingEmails(existingEmails);
    dialog.setExistingPhones(existingPhones);
    
    if (dialog.exec() == QDialog::Accepted) {
        QTableWidget* table = ui->tableEmployees;
        if (!table) return;
        

        QSqlDatabase db = QSqlDatabase::database();
        if (!db.isOpen()) {
            Connection conn;
            if (!conn.createconnect()) {
                QMessageBox::warning(this, "❌ Erreur de connexion", "Impossible de se connecter à la base de données. Employé non enregistré.");
                return;
            }
            db = QSqlDatabase::database();
        }

        if (!db.transaction()) {
            qDebug() << "Failed to start transaction for INSERT:" << db.lastError().text();
        }
        

        QString newId = generateNextEmployeeId();
        
        QSqlQuery query(db);
        QString passwordHash;
        QString inputPassword = dialog.getPassword().trimmed();
        if (!inputPassword.isEmpty()) {
            passwordHash = QString(QCryptographicHash::hash(inputPassword.toUtf8(), QCryptographicHash::Sha256).toHex());
        } else {
            // Use default old hash if no password provided (should not happen for add mode as validation ensures password)
            passwordHash = "482c811da5d5b4bc6d497ffa98491e38";
        }

        query.prepare("INSERT INTO employes (id, nom, poste, email, telephone, salaire, genre, password_hash) VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
        query.addBindValue(newId);
        query.addBindValue(dialog.getNom());
        query.addBindValue(dialog.getPoste());
        query.addBindValue(dialog.getEmail());
        query.addBindValue(dialog.getTelephone());
        query.addBindValue(dialog.getSalaire());
        query.addBindValue(dialog.getGenre());
        query.addBindValue(passwordHash);
        
        if (!query.exec()) {
            QMessageBox::critical(this, "❌ Erreur BD", "Échec de l'ajout de l'employé: " + query.lastError().text());
            db.rollback();
            return;
        }
        
        if (!db.commit()) {
            QMessageBox::critical(this, "❌ Erreur BD", "Impossible de valider les modifications");
            db.rollback();
            return;
        }
        

        loadEmployeesFromDb();
        updateEmployeeStats();
        updateSalaryStats();
        QMessageBox::information(this, "✅ Succès", "Employé ajouté avec succès !");
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
    
    // Fetch genre from database
    QString genre = "M";
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) {
        Connection conn;
        if (conn.createconnect()) {
            db = QSqlDatabase::database();
        }
    }
    
    if (db.isOpen()) {
        QSqlQuery query(db);
        if (query.exec(QString("SELECT genre FROM employes WHERE id = '%1'").arg(id))) {
            if (query.next()) {
                genre = query.value(0).toString();
            }
        }
    }
    
    AddEditEmployeeDialog dialog(this, true);
    
    // Collecter les emails et téléphones existants (sauf celui en cours d'édition)
    QStringList existingEmails, existingPhones;
    for (int i = 0; i < table->rowCount(); ++i) {
        if (i != row) { // Exclure la ligne courante
            QTableWidgetItem* emailItem = table->item(i, 3);
            QTableWidgetItem* phoneItem = table->item(i, 4);
            if (emailItem) existingEmails << emailItem->text();
            if (phoneItem) existingPhones << phoneItem->text();
        }
    }
    
    dialog.setExistingEmails(existingEmails);
    dialog.setExistingPhones(existingPhones);
    
    dialog.setEmployeeData(id, nom, poste, email, telephone, salary, genre);
    
    if (dialog.exec() == QDialog::Accepted) {

        if (!db.isOpen()) {
            Connection conn;
            if (!conn.createconnect()) {
                QMessageBox::warning(this, "❌ Erreur de connexion", "Impossible de se connecter à la base de données. Changements non enregistrés.");
                return;
            }
            db = QSqlDatabase::database();
        }
        
        QString idStr = id.trimmed();
        if (idStr.isEmpty()) {
            QMessageBox::warning(this, "❌ Erreur", "ID d'employé invalide");
            return;
        }
        
        if (!db.transaction()) {
            qDebug() << "Failed to start transaction for UPDATE:" << db.lastError().text();
        }
        QSqlQuery query(db);
        QString newPassword = dialog.getPassword().trimmed();
        if (!newPassword.isEmpty()) {
            QString passwordHash = QString(QCryptographicHash::hash(newPassword.toUtf8(), QCryptographicHash::Sha256).toHex());
            query.prepare("UPDATE employes SET nom = ?, poste = ?, email = ?, telephone = ?, salaire = ?, genre = ?, password_hash = ? WHERE id = ?");
            query.addBindValue(dialog.getNom());
            query.addBindValue(dialog.getPoste());
            query.addBindValue(dialog.getEmail());
            query.addBindValue(dialog.getTelephone());
            query.addBindValue(dialog.getSalaire());
            query.addBindValue(dialog.getGenre());
            query.addBindValue(passwordHash);
            query.addBindValue(idStr);
        } else {
            query.prepare("UPDATE employes SET nom = ?, poste = ?, email = ?, telephone = ?, salaire = ?, genre = ? WHERE id = ?");
            query.addBindValue(dialog.getNom());
            query.addBindValue(dialog.getPoste());
            query.addBindValue(dialog.getEmail());
            query.addBindValue(dialog.getTelephone());
            query.addBindValue(dialog.getSalaire());
            query.addBindValue(dialog.getGenre());
            query.addBindValue(idStr);
        }
        
        if (!query.exec()) {
            QMessageBox::critical(this, "❌ Erreur BD", "Échec de la modification: " + query.lastError().text());
            db.rollback();
            return;
        }
        int affected = query.numRowsAffected();
        qDebug() << "Update rows affected:" << affected;
        
        if (!db.commit()) {
            QMessageBox::critical(this, "❌ Erreur BD", "Impossible de valider les modifications");
            db.rollback();
            return;
        }
        

        table->item(row, 1)->setText(dialog.getNom());
        table->item(row, 2)->setText(dialog.getPoste());
        table->item(row, 3)->setText(dialog.getEmail());
        table->item(row, 4)->setText(dialog.getTelephone());
        table->item(row, 5)->setText(QString::number(dialog.getSalaire(), 'f', 0) + " TND");
        
        updateEmployeeStats();
        updateSalaryStats();
        QMessageBox::information(this, "✅ Succès", "Employé modifié avec succès !");
    }
}

void MainWindow::onDeleteEmployeeClicked(int row)
{
    QTableWidget* table = ui->tableEmployees;
    if (!table || row < 0 || row >= table->rowCount())
        return;
    
    QString id = table->item(row, 0)->text();
    QString nom = table->item(row, 1)->text();
    QString poste = table->item(row, 2)->text();
    
    QMessageBox msgBox(QMessageBox::Warning, "⚠️ Confirmation de suppression",
                       QString("Êtes-vous sûr de vouloir supprimer cet employé ?\n\n"
                              "ID: %1\n"
                              "Nom: %2\n"
                              "Poste: %3\n\n"
                              "Cette action est irréversible.").arg(id, nom, poste),
                       QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    
    if (msgBox.exec() == QMessageBox::Yes) {

        QSqlDatabase db = QSqlDatabase::database();
        if (!db.isOpen()) {
            Connection conn;
            if (!conn.createconnect()) {
                QMessageBox::warning(this, "❌ Erreur de connexion", "Impossible de se connecter à la base de données. Employé non supprimé.");
                return;
            }
            db = QSqlDatabase::database();
        }
        if (!db.transaction()) {
            qDebug() << "Failed to start transaction for DELETE:" << db.lastError().text();
        }
        
        if (!table->item(row, 0)) {
            QMessageBox::warning(this, "❌ Erreur", "Données d'employé invalides");
            return;
        }
        
        QString idStr = table->item(row, 0)->text().trimmed();
        if (idStr.isEmpty()) {
            QMessageBox::warning(this, "❌ Erreur", "ID d'employé invalide");
            return;
        }
        
        qDebug() << "Deleting employee with ID:" << idStr;
        
        QSqlQuery query(db);
        query.prepare("DELETE FROM employes WHERE id = ?");
        query.addBindValue(idStr);
        
        if (!query.exec()) {
            QMessageBox::critical(this, "❌ Erreur BD", "Échec de la suppression: " + query.lastError().text());
            qDebug() << "Delete query failed. Bound value was:" << idStr;
            db.rollback();
            return;
        }
        
        qDebug() << "Delete query executed successfully. Rows affected:" << query.numRowsAffected();
        
        if (!db.commit()) {
            QMessageBox::critical(this, "❌ Erreur BD", "Impossible de valider la suppression:" + db.lastError().text());
            qDebug() << "Commit failed:" << db.lastError().text();
            db.rollback();
            return;
        }
        
        qDebug() << "Commit successful";
        

        table->removeRow(row);
        updateEmployeeStats();
        updateSalaryStats();
        

        qDebug() << "Reloading employees from database after deletion";
        loadEmployeesFromDb();
        updateEmployeeStats();
        updateSalaryStats();
        QMessageBox::information(this, "✅ Succès", QString("Employé '%1' supprimé avec succès !").arg(nom));
    }
}

void MainWindow::onSearchEmployees(const QString &text)
{
    QTableWidget* table = ui->tableEmployees;
    if (!table) return;
    
    int visibleCount = 0;
    QString searchText = text.trimmed().toLower();
    
    for (int i = 0; i < table->rowCount(); ++i) {
        bool match = searchText.isEmpty(); // Si recherche vide, afficher tous
        
        if (!match) {
            for (int j = 0; j < table->columnCount() - 1; ++j) { // Exclure colonne Actions
                QTableWidgetItem* item = table->item(i, j);
                if (item && item->text().toLower().contains(searchText, Qt::CaseInsensitive)) {
                    match = true;
                    break;
                }
            }
        }
        
        table->setRowHidden(i, !match);
        if (match) visibleCount++;
    }
    
    qDebug() << "Recherche:" << text << "- Résultats:" << visibleCount << "/" << table->rowCount();
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
    if (!query.exec("SELECT id, nom, poste, email, telephone, salaire, genre FROM employes ORDER BY id")) {
        // Try without genre column for backwards compatibility
        if (!query.exec("SELECT id, nom, poste, email, telephone, salaire FROM employes ORDER BY id")) {
            qDebug() << "Error loading employees:" << query.lastError().text();
            return;
        }
    }
    

    int rowCount = 0;
    while (query.next()) {
        table->insertRow(rowCount);
        
        QString id = query.value(0).toString();
        QString nom = query.value(1).toString();
        QString poste = query.value(2).toString();
        QString email = query.value(3).toString();
        QString telephone = query.value(4).toString();
        double salaire = query.value(5).toDouble();
        QString genre = query.record().count() > 6 ? query.value(6).toString() : "M";
        
        table->setItem(rowCount, 0, new QTableWidgetItem(id));
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
        connect(btnEdit, &QPushButton::clicked, [this, id]() { 
            QTableWidget* table = ui->tableEmployees;
            for (int i = 0; i < table->rowCount(); ++i) {
                QTableWidgetItem* item = table->item(i, 0);
                if (item && item->text() == id) {
                    onEditEmployeeClicked(i);
                    return;
                }
            }
        });
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
        connect(btnDelete, &QPushButton::clicked, [this, id]() { 
            QTableWidget* table = ui->tableEmployees;
            for (int i = 0; i < table->rowCount(); ++i) {
                QTableWidgetItem* item = table->item(i, 0);
                if (item && item->text() == id) {
                    onDeleteEmployeeClicked(i);
                    return;
                }
            }
        });
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
    
    QLayout* chartsLayout;
    QVBoxLayout* chartsMainLayout = new QVBoxLayout();
    
    QHBoxLayout* chartsLayoutRow1 = new QHBoxLayout();
    QHBoxLayout* chartsLayoutRow2 = new QHBoxLayout();
    
    // Calculate total salary first for use in all charts
    double totalSalary = 0.0;
    
    QMap<QString, double> salaryByPosition;
    for (int i = 0; i < table->rowCount(); ++i) {
        QString poste = table->item(i, 2)->text();
        QString salaryText = table->item(i, 5)->text().replace(" TND", "");
        double salary = salaryText.toDouble();
        salaryByPosition[poste] += salary;
        totalSalary += salary;
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
    pieChartView->setMinimumWidth(400);
    pieChartView->setMaximumHeight(350);
    
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
    barChartView->setMinimumWidth(400);
    barChartView->setMaximumHeight(350);
    
    // Gender Parity Pie Chart
    QMap<QString, int> genderCount;
    for (int i = 0; i < table->rowCount(); ++i) {
        // Try to read genre from database (6th column if available)
        QString genre = "M";  // Default
        // For now, we'll use default since genre column might not be in table display
        genderCount[genre]++;
    }
    
    // Better approach: Query database for gender info
    QMap<QString, int> genderCountDb;
    QSqlDatabase db = QSqlDatabase::database();
    if (db.isOpen()) {
        QSqlQuery query(db);
        if (query.exec("SELECT genre FROM employes")) {
            while (query.next()) {
                QString genre = query.value(0).toString();
                genderCountDb[genre.isEmpty() ? "M" : genre]++;
            }
        }
    }
    if (!genderCountDb.isEmpty()) {
        genderCount = genderCountDb;
    } else {
        // Fallback: default all to M for now
        genderCount["M"] = table->rowCount();
    }
    
    QPieSeries* genderPieSeries = new QPieSeries();
    QColor genderColors[] = {QColor(100, 150, 220), QColor(255, 100, 150)};  // Blue for M, Pink for F
    int genderColorIdx = 0;
    
    for (auto it = genderCount.begin(); it != genderCount.end(); ++it) {
        QString genderLabel = (it.key() == "F") ? "Female" : "Male";
        int count = it.value();
        double percentage = (static_cast<double>(count) / table->rowCount()) * 100.0;
        QPieSlice* slice = genderPieSeries->append(
            QString("%1: %2 (%3%)").arg(genderLabel).arg(count).arg(static_cast<int>(percentage)),
            count
        );
        slice->setColor(genderColors[genderColorIdx % 2]);
        genderColorIdx++;
    }
    
    QChart* genderChart = new QChart();
    genderChart->addSeries(genderPieSeries);
    genderChart->setTitle("Employee Gender Parity");
    genderChart->setAnimationOptions(QChart::SeriesAnimations);
    
    QChartView* genderChartView = new QChartView(genderChart);
    genderChartView->setRenderHint(QPainter::Antialiasing);
    genderChartView->setMinimumWidth(400);
    genderChartView->setMaximumHeight(350);
    
    // Budget Trend Line Chart
    QLineSeries* budgetSeries = new QLineSeries();
    budgetSeries->setName("Monthly Budget");
    
    double dailyPayroll = totalSalary / 30.0;  // Estimate daily payroll
    for (int month = 0; month < 12; ++month) {
        double projectedBudget = totalSalary * (month + 1);
        budgetSeries->append(month, projectedBudget);
    }
    
    QChart* budgetChart = new QChart();
    budgetChart->addSeries(budgetSeries);
    budgetChart->setTitle("12-Month Budget Projection");
    budgetChart->setAnimationOptions(QChart::SeriesAnimations);
    
    QValueAxis* budgetAxisX = new QValueAxis();
    budgetAxisX->setLabelFormat("%i");
    budgetAxisX->setRange(0, 11);
    budgetAxisX->setTitleText("Month");
    budgetChart->addAxis(budgetAxisX, Qt::AlignBottom);
    budgetSeries->attachAxis(budgetAxisX);
    
    QValueAxis* budgetAxisY = new QValueAxis();
    budgetAxisY->setTitleText("Cumulative Budget (TND)");
    budgetChart->addAxis(budgetAxisY, Qt::AlignLeft);
    budgetSeries->attachAxis(budgetAxisY);
    
    QChartView* budgetChartView = new QChartView(budgetChart);
    budgetChartView->setRenderHint(QPainter::Antialiasing);
    budgetChartView->setMinimumWidth(400);
    budgetChartView->setMaximumHeight(350);
    
    chartsLayoutRow1->addWidget(pieChartView);
    chartsLayoutRow1->addWidget(barChartView);
    
    chartsLayoutRow2->addWidget(genderChartView);
    chartsLayoutRow2->addWidget(budgetChartView);
    
    chartsMainLayout->addLayout(chartsLayoutRow1);
    chartsMainLayout->addLayout(chartsLayoutRow2);
    chartsLayout = chartsMainLayout;
    
    QString statsText = "<b>📈 SALARY STATISTICS</b><br><br>";
    
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
    

    painter.setFont(titleFont);
    painter.drawText(margin, y, pageWidth - 2*margin, 50, Qt::AlignCenter, "Employee Analytics Report");
    y += 60;
    

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

