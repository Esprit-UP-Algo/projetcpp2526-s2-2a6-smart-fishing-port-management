#ifndef STATS_DIALOG_H
#define STATS_DIALOG_H

#include <QDialog>
#include <QtCharts/QPieSeries>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSlice>
#include <QSqlDatabase>

class StatsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit StatsDialog(QSqlDatabase &db, QWidget *parent = nullptr);

private:
    void setupChart(QSqlDatabase &db, class QVBoxLayout *layout);
};

#endif // STATS_DIALOG_H
