#ifndef NAVIRE_STATS_DIALOG_H
#define NAVIRE_STATS_DIALOG_H

#include <QDialog>
#include <QtCharts/QPieSeries>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSlice>
#include <QSqlDatabase>

class NavireStatsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit NavireStatsDialog(QSqlDatabase db, QWidget *parent = nullptr);

private:
    void setupChart(QSqlDatabase &db, class QVBoxLayout *layout);
};

#endif // NAVIRE_STATS_DIALOG_H
