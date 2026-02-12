#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "addedit_employeedialog.h"

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

    void onLogoutClicked();
    void onAddEmployeeClicked();
    void onEditEmployeeClicked(int row);
    void onDeleteEmployeeClicked(int row);
    void onSearchEmployees(const QString &text);
    void onShowAnalytics();

    void onSortByPositionClicked();
    void onSortBySalaryClicked();

private:
    void setupConnections();
    void setupTableEmployees();
    void updateEmployeeStats();
    void updateSalaryStats();
    void generateAnalyticsPDF();

    QString analyticsStatsText;
    void* analyticsPieChart = nullptr;
    void* analyticsBarChart = nullptr;

    Ui::MainWindow *ui;
};
#endif
