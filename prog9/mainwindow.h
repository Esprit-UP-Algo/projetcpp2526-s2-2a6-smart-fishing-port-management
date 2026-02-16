#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QPushButton>
#include <QWidget>
#include <QPainter>
#include <QLabel>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QVector>
#include <QTextDocument>
#include <QFileDialog>
#include <QDesktopServices>
#include <QProgressBar>
#include <QGroupBox>
#include <QCheckBox>
#include <QRandomGenerator>
#include <QTimer>
#include <QPixmap>

// --- CLASSE CAMEMBERT (Existante) ---
class MonCamembert : public QWidget {
    Q_OBJECT
public:
    MonCamembert(QWidget *parent = nullptr) : QWidget(parent) {
        valeurOccupe = 0; valeurDispo = 0;
        setMinimumSize(300, 300);
    }
    void setValeurs(int occupe, int dispo) {
        valeurOccupe = occupe; valeurDispo = dispo; update();
    }
protected:
    void paintEvent(QPaintEvent *) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        int total = valeurOccupe + valeurDispo;
        if (total == 0) return;

        int angleOccupe = (valeurOccupe * 360 * 16) / total;
        int side = qMin(width(), height());
        QRectF rect((width() - side)/2.0 + 10, (height() - side)/2.0 + 10, side - 20, side - 20);

        painter.setBrush(QColor("#ef4444"));
        painter.setPen(Qt::NoPen);
        painter.drawPie(rect, 0, angleOccupe);
        painter.setBrush(QColor("#22c55e"));
        painter.drawPie(rect, angleOccupe, 360 * 16 - angleOccupe);

        painter.setPen(Qt::black);
        painter.setFont(QFont("Segoe UI", 10, QFont::Bold));
        painter.setBrush(QColor("#22c55e"));
        painter.drawRect(10, 10, 15, 15);
        painter.drawText(30, 23, "Disponible: " + QString::number(valeurDispo));
        painter.setBrush(QColor("#ef4444"));
        painter.drawRect(10, 35, 15, 15);
        painter.drawText(30, 48, "Occupé: " + QString::number(valeurOccupe));
    }
private:
    int valeurOccupe, valeurDispo;
};

// --- CLASSE HISTOGRAMME AMÉLIORÉ ---
class MonHistogramme : public QWidget {
    Q_OBJECT
public:
    MonHistogramme(QWidget *parent = nullptr) : QWidget(parent) {
        setMinimumHeight(300);
        showGrid = true;
        showValues = true;
    }
    void setDonnees(const QVector<QPair<QString, int>> &data) {
        donnees = data;
        update();
    }
    void setOptions(bool grid, bool values) {
        showGrid = grid;
        showValues = values;
        update();
    }
protected:
    void paintEvent(QPaintEvent *) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);

        if (donnees.isEmpty()) {
            p.drawText(rect(), Qt::AlignCenter, "Lancez la simulation...");
            return;
        }

        int margin = 50;
        int barWidth = (width() - 2 * margin) / donnees.size();
        int maxH = height() - 2 * margin;

        // Grille de fond
        if (showGrid) {
            p.setPen(QPen(QColor("#e2e8f0"), 1));
            for (int i = 0; i <= 10; ++i) {
                int y = height() - margin - (i * maxH / 10);
                p.drawLine(margin, y, width() - margin, y);
                p.drawText(margin - 30, y + 5, QString::number(i * 10) + "%");
            }
        }

        // Axes
        p.setPen(QPen(Qt::black, 2));
        p.drawLine(margin, margin, margin, height() - margin);
        p.drawLine(margin, height() - margin, width() - margin, height() - margin);

        // Trouver le maximum pour l'échelle
        int maxValue = 0;
        for (const auto &data : donnees) {
            if (data.second > maxValue) maxValue = data.second;
        }

        // Dessiner les barres
        for (int i = 0; i < donnees.size(); ++i) {
            int pourcent = donnees[i].second;
            int h = (pourcent * maxH) / 100;
            int x = margin + i * barWidth + 5;
            int y = height() - margin - h;

            // Gradient de couleur
            QLinearGradient gradient(x, y, x, y + h);
            if (pourcent > 90) {
                gradient.setColorAt(0, QColor("#ef4444"));
                gradient.setColorAt(1, QColor("#b91c1c"));
            } else if (pourcent > 60) {
                gradient.setColorAt(0, QColor("#f59e0b"));
                gradient.setColorAt(1, QColor("#d97706"));
            } else {
                gradient.setColorAt(0, QColor("#3b82f6"));
                gradient.setColorAt(1, QColor("#1d4ed8"));
            }

            p.setBrush(gradient);
            p.setPen(QPen(QColor("#1e293b"), 1));
            p.drawRect(x, y, barWidth - 10, h);

            // Valeurs au-dessus des barres
            if (showValues) {
                p.setPen(Qt::black);
                p.setFont(QFont("Segoe UI", 9, QFont::Bold));
                QString valueText = QString::number(pourcent) + "%";
                QRect textRect(x, y - 25, barWidth - 10, 20);
                p.drawText(textRect, Qt::AlignCenter, valueText);
            }

            // Labels des jours
            p.setPen(Qt::darkGray);
            p.setFont(QFont("Segoe UI", 8));
            p.drawText(QRect(x - 5, height() - margin + 5, barWidth + 10, 20),
                       Qt::AlignCenter, donnees[i].first);
        }

        // Titre de l'axe Y
        p.save();
        p.translate(15, height() / 2);
        p.rotate(-90);
        p.setPen(Qt::black);
        p.setFont(QFont("Segoe UI", 10, QFont::Bold));
        p.drawText(0, 0, "Taux d'occupation (%)");
        p.restore();
    }
private:
    QVector<QPair<QString, int>> donnees;
    bool showGrid;
    bool showValues;
};

// Structure pour les statistiques de simulation
struct StatsSimulation {
    int joursSaturation;
    int jourMaxOccupation;
    int maxOccupation;
    double moyenneOccupation;
    int joursCritiques; // jours > 80%
    QVector<int> tendance; // tendance sur 5 jours
};

// ------------------------------------------------

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
    void showStatistiques();
    void showProposition();
    void showSimulation();
    void showExportPDF();

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

    // Ajouter
    QLineEdit *idEdit;
    QLineEdit *nomEdit;
    QLineEdit *longueurEdit;
    QComboBox *dispoBox;
    QDateEdit *dateDebutEdit;
    QDateEdit *dateFinEdit;

    // Modifier
    QLineEdit *idEditM;
    QLineEdit *nomEditM;
    QLineEdit *longueurEditM;
    QComboBox *dispoBoxM;
    QDateEdit *dateDebutEditM;
    QDateEdit *dateFinEditM;

    // Proposition
    QLineEdit *longueurBateauEdit;
    QLabel *resultatLabel;

    // Simulation AMÉLIORÉE
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

    // Titre Fishtech
    QLabel *fishtechTitle;
};

#endif // MAINWINDOW_H
