#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "addedit_employeedialog.h"
#include "int7/gestionquai.h"
#include "int7/gestionnavires.h"
#include "int7/gestioncaptures.h"
#include "int7/dashboard.h"
#include "int7/gestionutilisateurs.h"
#include "planning.h"        // Nouveau
#include "affectation.h"     // Nouveau
#include "settingsdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    enum Page {
        LoginPage = 0,
        EmployeesPage = 1
    };

    void showLoginPage();
    void showEmployeesPage();

private slots:
    void onLoginClicked();
    void onForgotPasswordClicked();
    void onShowDashboard();
    void onLogoutClicked();
    void onAddEmployeeClicked();
    void onEditEmployeeClicked(int row);
    void onDeleteEmployeeClicked(int row);
    void onSearchEmployees(const QString &text);
    void onShowAnalytics();
    void onSortByPositionClicked();
    void onSortBySalaryClicked();
    void onShowQuais();
    void onShowShips();
    void onShowCaptures();
    void onShowEmployeesContent();
    void onShowPlanning();      // Nouveau
    void onShowAffectation();   // Nouveau
    void onShowSettings();

private:
    void setupConnections();
    void setupTableEmployees();
    void setupVisuals();
    void setupRoleBasedUI();
    bool canAccessQuais() const;
    bool canAccessDashboard() const;
    bool canAccessShips() const;
    bool canAccessCaptures() const;
    bool canAccessEmployees() const;
    bool canAccessSettings() const;
    void loadEmployeesFromDb();
    void updateEmployeeStats();
    void updateSalaryStats();
    void generateAnalyticsPDF();

    QString currentUserRole;

protected:
    void showEvent(QShowEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;

    QString analyticsStatsText;
    void* analyticsPieChart = nullptr;
    void* analyticsBarChart = nullptr;

    Ui::MainWindow *ui;
    QLabel *m_loginBg      = nullptr;
    QLabel *m_loginOverlay = nullptr;
    void updateLoginBackground();
    GestionQuai *quaiWidget = nullptr;
    GestionNavires *navireWidget = nullptr;
    GestionCaptures *captureWidget = nullptr;
    GestionUtilisateurs *utilWidget = nullptr;
    Dashboard *dashboardWidget = nullptr;
    PlanningWidget *planningWidget = nullptr;      // Nouveau
    AffectationWidget *affectationWidget = nullptr; // Nouveau
    QWidget *currentModuleWidget = nullptr;

};
#endif
