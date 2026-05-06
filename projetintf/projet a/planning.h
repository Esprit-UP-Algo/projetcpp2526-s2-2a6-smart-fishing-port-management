#ifndef PLANNING_H
#define PLANNING_H

#include <QWidget>
#include <QDate>
#include <QVector>
#include <QPainter>
#include <QPushButton>
#include <QLabel>
#include <QColor>

struct EvenementCalendrier {
    QDate date;
    QString texte;      // Description courte
    QString type;       // "QuaiTravaux"
    QString couleur;
};

class PlanningWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PlanningWidget(QWidget *parent = nullptr);
    void refresh();      // recharge les données et redessine

protected:
    void paintEvent(QPaintEvent *) override;
    void resizeEvent(QResizeEvent *) override;

private slots:
    void moisPrecedent();
    void moisSuivant();
    void exporterCSV();

private:
    void chargerEvenements();
    void dessinerCalendrier(QPainter &painter);
    void dessinerLegende(QPainter &painter);
    QColor couleurPourQuai(int idQuai) const;

    QPushButton *btnPrev, *btnNext, *btnExport;
    QLabel *labelMois;
    int moisCourant, anneeCourante;
    QVector<EvenementCalendrier> evenements;
};

#endif // PLANNING_H
