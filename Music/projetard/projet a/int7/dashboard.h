#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <QWidget>
#include <QLabel>
#include <QFrame>
#include <QProgressBar>
#include <QTimer>

#include "dashboard_widgets.h"
#include "gestionquai.h"
#include "gestionnavires.h"
#include "gestioncaptures.h"
#include "gestionutilisateurs.h"

class Dashboard : public QWidget {
    Q_OBJECT
public:
    explicit Dashboard(GestionQuai *q, GestionNavires *n, GestionCaptures *c,
                       GestionUtilisateurs *u, QWidget *parent = nullptr);
    void refresh();

private:
    GestionQuai         *gQuai;
    GestionNavires      *gNavires;
    GestionCaptures     *gCaptures;
    GestionUtilisateurs *gUsers;

    // KPI labels
    QLabel *kpiNaviresVal=nullptr,   *kpiNaviresSub=nullptr;
    QLabel *kpiCapturesVal=nullptr,  *kpiCapturesSub=nullptr;
    QLabel *kpiPersonnelVal=nullptr, *kpiPersonnelSub=nullptr;
    QLabel *kpiQuaisVal=nullptr,     *kpiQuaisSub=nullptr;

    // Donuts
    DonutWidget *donutNavires=nullptr, *donutCaptures=nullptr, *donutPersonnel=nullptr;

    // Donut row labels
    QLabel *statQuai=nullptr, *statMer=nullptr, *statInterdit=nullptr;
    QLabel *statPoisson=nullptr, *statCrustace=nullptr, *statMollusque=nullptr;
    QLabel *statPecheur=nullptr, *statDocker=nullptr, *statSuperviseur=nullptr;

    // Occupation
    QProgressBar *pbOccupation=nullptr;
    QLabel       *pbOccupLbl=nullptr, *pbOccupPct=nullptr;

    // Sparkline
    SparklineWidget *sparkline=nullptr;

    // Header
    QLabel *lblClock=nullptr, *lblDate=nullptr;

    // Helpers
    QFrame* makeKpiCard(const QString &icon, const QString &title, QColor accent,
                        QLabel **valLbl, QLabel **subLbl);
    QFrame* makeDonutCard(const QString &icon, const QString &title, QColor accent,
                          DonutWidget **donutOut,
                          const QStringList &rowIcons, const QStringList &rowLabels,
                          const QList<QColor> &rowColors, QList<QLabel*> *rowVals);
};

#endif // DASHBOARD_H
