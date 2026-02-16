#ifndef GESTIONQUAI_H
#define GESTIONQUAI_H

#include <QWidget>
#include <QStackedWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QVector>
#include <QProgressBar>
#include <QCheckBox>
#include <QRandomGenerator>
#include <QTimer>
#include <QPainter>
#include <QFont>
#include <QPen>
#include <QLinearGradient>
#include <QMessageBox>
#include <QFileDialog>
#include <QDesktopServices>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QDir>
#include <QTextStream>
#include <QFile>
#include <cmath>
#include <limits>

class MonCamembert : public QWidget {
    Q_OBJECT
public:
    MonCamembert(QWidget *parent = nullptr);
    void setValeurs(int occupe, int dispo);
protected:
    void paintEvent(QPaintEvent *) override;
private:
    int valeurOccupe;
    int valeurDispo;
};

class MonHistogramme : public QWidget {
    Q_OBJECT
public:
    MonHistogramme(QWidget *parent = nullptr);
    void setDonnees(const QVector<QPair<QString, int>> &data);
    void setOptions(bool grid, bool values);
protected:
    void paintEvent(QPaintEvent *) override;
private:
    QVector<QPair<QString, int>> donnees;
    bool showGrid;
    bool showValues;
};

struct StatsSimulation {
    int joursSaturation;
    int jourMaxOccupation;
    int maxOccupation;
    double moyenneOccupation;
    int joursCritiques;
    QVector<int> tendance;
};

class GestionQuai : public QWidget
{
    Q_OBJECT

public:
    GestionQuai(QWidget *parent = nullptr);

private slots:
    void showAjouter();
    void showModifier();
    void showSupprimer();
    void showConsulter();
    void showStatistiques();
    void showProposition();
    void showSimulation();
    void showExportCSV();
    void ajouterQuai();
    void supprimerQuai();
    void remplirFormulaireDepuisTable(int row, int col);
    void modifierQuai();
    void onEtatChanged(const QString &etat);
    void onEtatChangedM(const QString &etat);
    void rechercherQuai(const QString &texte);
    void trierQuais(int index);
    void trouverMeilleurQuai();
    void lancerSimulation();
    void exporterCSV();
    void genererScenarioAleatoire();

private:
    void syncTables();
    void updateStats();
    QString genererContenuCSV();
    StatsSimulation analyserResultats(const QVector<QPair<QString, int>>& resultats);
    QVector<int> simulerArriveesBateaux(int jours, double moyenne, double ecartType, bool variationsJournalieres);
    QString genererRecommandations(const StatsSimulation& stats);

    QStackedWidget *pages;
    QWidget *pageAjouter;
    QWidget *pageModifier;
    QWidget *pageSupprimer;
    QWidget *pageConsulter;
    QWidget *pageStatistiques;
    QWidget *pageProposition;
    QWidget *pageSimulation;
    QWidget *pageExportCSV;

    QPushButton *btnAdd;
    QPushButton *btnEdit;
    QPushButton *btnDelete;
    QPushButton *btnView;
    QPushButton *btnStats;
    QPushButton *btnProp;
    QPushButton *btnSimul;
    QPushButton *btnCSV;

    QTableWidget *tableConsulter;
    QTableWidget *tableSupprimer;
    QTableWidget *tableModifier;

    QLineEdit *searchEdit;
    QComboBox *sortBox;
    MonCamembert *monGraphique;

    QLineEdit *idEdit;
    QLineEdit *nomEdit;
    QLineEdit *longueurEdit;
    QComboBox *dispoBox;
    QDateEdit *dateDebutEdit;
    QDateEdit *dateFinEdit;

    QLineEdit *idEditM;
    QLineEdit *nomEditM;
    QLineEdit *longueurEditM;
    QComboBox *dispoBoxM;
    QDateEdit *dateDebutEditM;
    QDateEdit *dateFinEditM;

    QLineEdit *longueurBateauEdit;
    QLabel *resultatLabel;

    QSpinBox *spinJours;
    QDoubleSpinBox *spinMoyenneArrivees;
    QDoubleSpinBox *spinEcartType;
    QSpinBox *spinDureeSejour;
    QCheckBox *checkVariationsJournalieres;
    QCheckBox *checkWeekend;
    QCheckBox *checkEffetSaison;
    QProgressBar *progressSimulation;
    MonHistogramme *histoWidget;
    QLabel *lblResultatSimul;
    QLabel *lblStatsDetaillees;
    QPushButton *btnScenarioAleatoire;

    QLineEdit *nomFichierEdit;
    QComboBox *formatCSVBox;
    QLabel *lblPreviewCSV;
    QPushButton *btnGenererCSV;
};

#endif
