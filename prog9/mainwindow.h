#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void showAjouter();
    void showModifier();
    void showSupprimer();
    void showConsulter();

    void ajouterQuai();
    void supprimerQuai();
    void remplirFormulaireDepuisTable(int row, int col);
    void modifierQuai();

private:
    QStackedWidget *pages;

    QWidget *pageAjouter;
    QWidget *pageModifier;
    QWidget *pageSupprimer;
    QWidget *pageConsulter;

    QPushButton *btnAdd;
    QPushButton *btnEdit;
    QPushButton *btnDelete;
    QPushButton *btnView;

    QTableWidget *tableConsulter;
    QTableWidget *tableSupprimer;
    QTableWidget *tableModifier;

    QLineEdit *idEdit;
    QLineEdit *nomEdit;
    QLineEdit *longueurEdit;
    QComboBox *dispoBox;

    QLineEdit *idEditM;
    QLineEdit *nomEditM;
    QLineEdit *longueurEditM;
    QComboBox *dispoBoxM;
};

#endif // MAINWINDOW_H
