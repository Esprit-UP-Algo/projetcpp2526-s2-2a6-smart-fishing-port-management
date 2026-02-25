#ifndef GESTIONQUAI_H
#define GESTIONQUAI_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QStackedWidget>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QProgressBar>
#include <QVector>
#include <QPair>
#include <QRandomGenerator>
#include <QTimer>
#include <QPainter>
#include <QCoreApplication>
#include <QGroupBox>
#include <QGridLayout>
#include <QFont>

// ============ Camembert pour les statistiques ============
class MonCamembert : public QWidget {
    Q_OBJECT
public:
    explicit MonCamembert(QWidget *parent = nullptr);
    void setValeurs(int occupe, int dispo);
protected:
    void paintEvent(QPaintEvent *) override;
private:
    int valeurOccupe;
    int valeurDispo;
};

// ============ Histogramme pour la simulation ============
class MonHistogramme : public QWidget {
    Q_OBJECT
public:
    explicit MonHistogramme(QWidget *parent = nullptr);
    void setDonnees(const QVector<QPair<QString, int>> &data);
    void setOptions(bool grid, bool values);
protected:
    void paintEvent(QPaintEvent *) override;
private:
    QVector<QPair<QString, int>> donnees;
    bool showGrid;
    bool showValues;
};

// ============ Structure pour les résultats de simulation ============
struct StatsSimulation {
    int joursSaturation;
    int jourMaxOccupation;
    int maxOccupation;
    double moyenneOccupation;
    int joursCritiques;
    QVector<int> tendance;
};

// ============ Classe principale de gestion des quais ============
class GestionQuai : public QWidget
{
    Q_OBJECT

public:
    explicit GestionQuai(QWidget *parent = nullptr);
    ~GestionQuai();

    // Public methods to navigate to different pages
    void showAjouter() { showMainPage(); }
    void showModifier() { showMainPage(); }
    void showSupprimer() { showMainPage(); }

    struct Summary {
        int total = 0;
        int totalPlaces = 0;
        int totalNavires = 0;
        int dispos = 0;
        int moyenne = 0;
        int countDisponible = 0;
        int countCritique = 0;
        int countPlein = 0;
        int countTravaux = 0;
    };

    Summary getSummary() const;

private slots:
    void loadData();
    void onSearch(const QString &text);
    void onAdd();
    void onEdit();
    void onDelete();
    void onSortByNom();
    void onSortByPlaces();
    void onSortByNavires();
    void updateStats();
    void onTrouverMeilleur();
    void onLancerSimulation();
    void onScenarioAleatoire();
    void showPropositionPage();
    void showSimulationPage();
    void showMainPage();

private:
    void setupUI();
    QString calculerEtat(int places, int navires, bool travaux);
    StatsSimulation analyserResultats(const QVector<int> &arrivees);
    QVector<int> simulerArriveesBateaux(int jours, double moyenne, double ecartType, bool variationsJournalieres);
    QString genererRecommandations(const StatsSimulation &stats);

    // UI Elements
    QTableWidget *table;
    QLineEdit *lineSearch;
    QLabel *statTotal;
    QLabel *statTotalPlaces;
    QLabel *statTotalNavires;
    QLabel *statMoyenneOccupation;
    QLabel *statDispos;
    QStackedWidget *pages;
    QLineEdit *longueurBateauEdit;
    QLabel *resultatLabel;
    QSpinBox *spinJours;
    QDoubleSpinBox *spinMoyenneArrivees;
    QDoubleSpinBox *spinEcartType;
    QCheckBox *checkVariationsJournalieres;
    QCheckBox *checkWeekend;
    QCheckBox *checkEffetSaison;
    QProgressBar *progressSimulation;
    MonHistogramme *histoWidget;
    QLabel *lblResultatSimul;
    QLabel *lblStatsDetaillees;
    QWidget *pageConsulter;
    QWidget *pageProposition;
    QWidget *pageSimulation;

signals:
    void dataChanged();
};

#endif // GESTIONQUAI_H
