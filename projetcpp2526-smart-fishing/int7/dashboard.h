#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <QWidget>
#include <QLabel>
#include <QFrame>
#include <QGridLayout>
#include <QVBoxLayout>

#include "gestionquai.h"
#include "gestionnavires.h"
#include "gestioncaptures.h"
#include "gestionutilisateurs.h"

class Dashboard : public QWidget {
public:
    explicit Dashboard(GestionQuai *q, GestionNavires *n, GestionCaptures *c, GestionUtilisateurs *u, QWidget *parent = nullptr);
    void refresh();

private:
    // source pointers
    GestionQuai *gQuai;
    GestionNavires *gNavires;
    GestionCaptures *gCaptures;
    GestionUtilisateurs *gUsers;

    // UI labels
    QLabel *lblUsersTotal;
    QLabel *lblUsersAvgSalary;

    QLabel *lblNaviresTotal;
    QLabel *lblNaviresQuai;
    QLabel *lblNaviresMer;
    QLabel *lblNaviresInterdit;

    QLabel *lblCapturesTotal;
    QLabel *lblCapturesPoisson;
    QLabel *lblCapturesCrustace;
    QLabel *lblCapturesMollusque;

    QLabel *lblQuaisTotal;
    QLabel *lblQuaisNavires;
};

#endif // DASHBOARD_H

// Inline implementation to simplify linking when build system wasn't updated yet
inline Dashboard::Dashboard(GestionQuai *q, GestionNavires *n, GestionCaptures *c, GestionUtilisateurs *u, QWidget *parent)
    : QWidget(parent), gQuai(q), gNavires(n), gCaptures(c), gUsers(u)
{
    QVBoxLayout *main = new QVBoxLayout(this);
    main->setContentsMargins(8,8,8,8);
    main->setSpacing(8);

    QLabel *title = new QLabel("📊 Tableau de bord global");
    title->setStyleSheet("font-size:20px; font-weight:bold; color:#1e3a8a;");
    title->setAlignment(Qt::AlignCenter);
    main->addWidget(title);

    QFrame *frame = new QFrame;
    frame->setStyleSheet("background: white; border-radius:8px; padding:10px; border:1px solid #e2e8f0;");
    QGridLayout *grid = new QGridLayout(frame);
    grid->setSpacing(10);
    grid->setContentsMargins(8,8,8,8);

    // Users block
    QLabel *uTitle = new QLabel("👥 Utilisateurs");
    uTitle->setStyleSheet("font-weight:600; color:#1e293b;");
    lblUsersTotal = new QLabel("0");
    lblUsersTotal->setStyleSheet("font-size:18px; font-weight:bold; color:#1e3a8a;");
    lblUsersAvgSalary = new QLabel("Avg: 0");
    lblUsersAvgSalary->setStyleSheet("color:#64748b;");

    QVBoxLayout *uLayout = new QVBoxLayout;
    uLayout->addWidget(uTitle);
    uLayout->addWidget(lblUsersTotal);
    uLayout->addWidget(lblUsersAvgSalary);

    QFrame *uFrame = new QFrame;
    uFrame->setStyleSheet("background:#f8fafc; border-radius:6px; padding:8px;");
    uFrame->setLayout(uLayout);

    // Navires block
    QLabel *nTitle = new QLabel("🚢 Navires");
    nTitle->setStyleSheet("font-weight:600; color:#1e293b;");
    lblNaviresTotal = new QLabel("0"); lblNaviresTotal->setStyleSheet("font-size:18px; font-weight:bold; color:#1e3a8a;");
    lblNaviresQuai = new QLabel("À quai: 0"); lblNaviresQuai->setStyleSheet("color:#64748b;");
    lblNaviresMer = new QLabel("En mer: 0"); lblNaviresMer->setStyleSheet("color:#64748b;");
    lblNaviresInterdit = new QLabel("Interdit: 0"); lblNaviresInterdit->setStyleSheet("color:#64748b;");

    QVBoxLayout *nLayout = new QVBoxLayout;
    nLayout->addWidget(nTitle);
    nLayout->addWidget(lblNaviresTotal);
    nLayout->addWidget(lblNaviresQuai);
    nLayout->addWidget(lblNaviresMer);
    nLayout->addWidget(lblNaviresInterdit);

    QFrame *nFrame = new QFrame; nFrame->setStyleSheet("background:#f8fafc; border-radius:6px; padding:8px;"); nFrame->setLayout(nLayout);

    // Captures block
    QLabel *cTitle = new QLabel("🎣 Captures");
    cTitle->setStyleSheet("font-weight:600; color:#1e293b;");
    lblCapturesTotal = new QLabel("0"); lblCapturesTotal->setStyleSheet("font-size:18px; font-weight:bold; color:#1e3a8a;");
    lblCapturesPoisson = new QLabel("Poisson: 0"); lblCapturesPoisson->setStyleSheet("color:#64748b;");
    lblCapturesCrustace = new QLabel("Crustacés: 0"); lblCapturesCrustace->setStyleSheet("color:#64748b;");
    lblCapturesMollusque = new QLabel("Mollusques: 0"); lblCapturesMollusque->setStyleSheet("color:#64748b;");

    QVBoxLayout *cLayout = new QVBoxLayout;
    cLayout->addWidget(cTitle);
    cLayout->addWidget(lblCapturesTotal);
    cLayout->addWidget(lblCapturesPoisson);
    cLayout->addWidget(lblCapturesCrustace);
    cLayout->addWidget(lblCapturesMollusque);

    QFrame *cFrame = new QFrame; cFrame->setStyleSheet("background:#f8fafc; border-radius:6px; padding:8px;"); cFrame->setLayout(cLayout);

    // Quais block
    QLabel *qTitle = new QLabel("🏗️ Quais");
    qTitle->setStyleSheet("font-weight:600; color:#1e293b;");
    lblQuaisTotal = new QLabel("0"); lblQuaisTotal->setStyleSheet("font-size:18px; font-weight:bold; color:#1e3a8a;");
    lblQuaisNavires = new QLabel("Navires présents: 0"); lblQuaisNavires->setStyleSheet("color:#64748b;");

    QVBoxLayout *qLayout = new QVBoxLayout;
    qLayout->addWidget(qTitle);
    qLayout->addWidget(lblQuaisTotal);
    qLayout->addWidget(lblQuaisNavires);

    QFrame *qFrame = new QFrame; qFrame->setStyleSheet("background:#f8fafc; border-radius:6px; padding:8px;"); qFrame->setLayout(qLayout);

    grid->addWidget(uFrame, 0, 0);
    grid->addWidget(nFrame, 0, 1);
    grid->addWidget(cFrame, 1, 0);
    grid->addWidget(qFrame, 1, 1);

    main->addWidget(frame);
    main->addStretch();

    refresh();
}

inline void Dashboard::refresh()
{
    if (gUsers) {
        auto s = gUsers->getStats();
        lblUsersTotal->setText(QString::number(s.total));
        lblUsersAvgSalary->setText(QString("Avg: %1").arg(QString::number(s.avgSalaire, 'f', 2)));
    }
    if (gNavires) {
        auto s = gNavires->getStats();
        lblNaviresTotal->setText(QString::number(s.total));
        lblNaviresQuai->setText(QString("À quai: %1").arg(s.quai));
        lblNaviresMer->setText(QString("En mer: %1").arg(s.mer));
        lblNaviresInterdit->setText(QString("Interdit: %1").arg(s.interdit));
    }
    if (gCaptures) {
        auto s = gCaptures->getStats();
        lblCapturesTotal->setText(QString::number(s.total));
        lblCapturesPoisson->setText(QString("Poisson: %1").arg(s.poisson));
        lblCapturesCrustace->setText(QString("Crustacés: %1").arg(s.crustace));
        lblCapturesMollusque->setText(QString("Mollusques: %1").arg(s.mollusque));
    }
    if (gQuai) {
        auto s = gQuai->getStats();
        lblQuaisTotal->setText(QString::number(s.totalQuais));
        lblQuaisNavires->setText(QString("Navires présents: %1").arg(s.totalNaviresPresent));
    }
}
