#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QStackedLayout>
#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>

class GestionQuai;
class GestionNavires;
class GestionCaptures;
class GestionUtilisateurs;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onLoginClicked();
    void onLogoutClicked();
    void showGestionQuais();
    void showDashboard();
    void showGestionNavires();
    void showGestionCaptures();
    void showGestionUtilisateurs();

private:
    void setupLoginPage();
    void setupMainInterface();
    void setupRoleBasedUI();
    bool canAccessQuais() const;
    bool canAccessDashboard() const;
    bool canAccessNavires() const;
    bool canAccessCaptures() const;
    bool canAccessUtilisateurs() const;

    QStackedLayout *stackedLayout;
    QWidget *loginPage;
    QWidget *mainPage;
    QString currentUserRole;

    QWidget *menuWidget;
    QPushButton *btnQuais;
    QPushButton *btnDashboard;
    QPushButton *btnNavires;
    QPushButton *btnCaptures;
    QPushButton *btnUtilisateurs;
    QPushButton *btnLogout;

    QStackedWidget *contentStack;

    GestionQuai *gestionQuaiWidget;
    class Dashboard* dashboardWidget;
    GestionNavires *gestionNaviresWidget;
    GestionCaptures *gestionCapturesWidget;
    GestionUtilisateurs *gestionUtilisateursWidget;

    QLineEdit *userEdit;
    QLineEdit *passEdit;
};

#endif
