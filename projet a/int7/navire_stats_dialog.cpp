#include "navire_stats_dialog.h"
#include <QVBoxLayout>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QtCharts/QChart>
#include <QPainter>

NavireStatsDialog::NavireStatsDialog(QSqlDatabase db, QWidget *parent) : QDialog(parent)
{
    setWindowTitle(tr("Statistiques des Navires"));
    resize(600, 450);
    QVBoxLayout *layout = new QVBoxLayout(this);
    setupChart(db, layout);
}

void NavireStatsDialog::setupChart(QSqlDatabase &db, QVBoxLayout *layout)
{
    QPieSeries *series = new QPieSeries();

    QStringList statuts = {"à quai", "en mer", "interdit"};
    for (const QString &s : statuts) {
        QSqlQuery query(db);
        query.prepare("SELECT COUNT(*) FROM navire WHERE statut = :statut");
        query.bindValue(":statut", s);
        if (query.exec() && query.next()) {
            int count = query.value(0).toInt();
            if (count > 0)
                series->append(s + " (" + QString::number(count) + ")", count);
        }
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle(tr("Répartition des navires par statut"));
    chart->setAnimationOptions(QChart::AllAnimations);
    chart->legend()->setAlignment(Qt::AlignRight);

    for (QPieSlice *slice : series->slices()) {
        slice->setLabelVisible(true);
        slice->setExploded(true);
    }

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    layout->addWidget(chartView);
}
