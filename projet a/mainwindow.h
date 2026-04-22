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

private:
    void setupConnections();
    void setupTableEmployees();
    void loadEmployeesFromDb();
    void updateEmployeeStats();
    void updateSalaryStats();
    void generateAnalyticsPDF();

    QString analyticsStatsText;
    void* analyticsPieChart = nullptr;
    void* analyticsBarChart = nullptr;

    Ui::MainWindow *ui;
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
