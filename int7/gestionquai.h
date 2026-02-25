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

// ============ Camembert pour les statistiques ============
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

// ============ Histogramme pour la simulation ============
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
    void rechercherQuai(const QString &texte);
    void trierQuais(int index);
    void trouverMeilleurQuai();
    void lancerSimulation();
    void exporterCSV();
    void genererScenarioAleatoire();

    // Mise à jour automatique de l'état
    void onPlacesChanged(const QString &);
    void onNaviresChanged(const QString &);
    void onTravauxChanged(int);

private:
    void configurerInterface();
    void configurerPages();
    void configurerAjouter();
    void configurerConsulter();
    void configurerStatistiques();
    void configurerProposition();
    void configurerSimulation();
    void configurerExportCSV();
    void configurerSupprimer();
    void configurerModifier();
    void appliquerStyles();

    void syncTables();
    void updateStats();
    QString genererContenuCSV();
    QString calculerEtat(int places, int navires, bool travaux);
    StatsSimulation analyserResultats(const QVector<QPair<QString, int>>& resultats);
    QVector<int> simulerArriveesBateaux(int jours, double moyenne, double ecartType, bool variationsJournalieres);
    QString genererRecommandations(const StatsSimulation& stats);

    // Widgets principaux
    QStackedWidget *pages;
    QWidget *pageAjouter;
    QWidget *pageModifier;
    QWidget *pageSupprimer;
    QWidget *pageConsulter;
    QWidget *pageStatistiques;
    QWidget *pageProposition;
    QWidget *pageSimulation;
    QWidget *pageExportCSV;

    // Boutons de la barre d'outils (seront créés dans configurerInterface)
    QPushButton *btnAjouter;
    QPushButton *btnModifier;
    QPushButton *btnSupprimer;
    QPushButton *btnConsulter;
    QPushButton *btnStatistiques;
    QPushButton *btnProposition;
    QPushButton *btnSimulation;
    QPushButton *btnExportCSV;

    // Tableaux
    QTableWidget *tableConsulter;
    QTableWidget *tableSupprimer;
    QTableWidget *tableModifier;

    // Recherche et tri
    QLineEdit *searchEdit;
    QComboBox *sortBox;

    // Camembert
    MonCamembert *monGraphique;

    // Ajouter
    QLineEdit *idEdit;
    QLineEdit *nomEdit;
    QLineEdit *placesEdit;
    QLineEdit *naviresEdit;
    QLineEdit *adresseEdit;
    QCheckBox *travauxCheck;
    QPushButton *btnAnnulerAjout;

    // Modifier
    QLineEdit *idEditM;
    QLineEdit *nomEditM;
    QLineEdit *placesEditM;
    QLineEdit *naviresEditM;
    QLineEdit *adresseEditM;
    QCheckBox *travauxCheckM;
    QPushButton *btnAnnulerModif;

    // Proposition
    QLineEdit *longueurBateauEdit;
    QLabel *resultatLabel;

    // Simulation
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

    // Export CSV
    QLineEdit *nomFichierEdit;
    QComboBox *formatCSVBox;
    QLabel *lblPreviewCSV;
    QPushButton *btnGenererCSV;
};

#endif // GESTIONQUAI_H
