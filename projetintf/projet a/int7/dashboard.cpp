#include "dashboard.h"
#include <QScrollArea>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLocale>
#include <QDateTime>
#include <QDate>

// ─────────────────────────────────────────────────────────────────────────────
// makeKpiCard
// ─────────────────────────────────────────────────────────────────────────────
QFrame* Dashboard::makeKpiCard(const QString &icon, const QString &title,
                                QColor accent, QLabel **valLbl, QLabel **subLbl)
{
    auto *card = new QFrame;
    card->setStyleSheet(
        "QFrame{background:white;border-radius:16px;border:1px solid #e2e8f0;}"
        "QLabel{background:transparent;border:none;}");
    card->setMinimumHeight(130);

    auto *vl = new QVBoxLayout(card);
    vl->setContentsMargins(16,14,16,0); vl->setSpacing(4);

    auto *hdr = new QHBoxLayout; hdr->setSpacing(10);
    auto *badge = new QLabel(icon);
    badge->setFixedSize(42,42); badge->setAlignment(Qt::AlignCenter);
    badge->setStyleSheet(QString("font-size:20px;border-radius:10px;background:%1;")
                             .arg(accent.name()+"22"));
    auto *ttl = new QLabel(title);
    ttl->setStyleSheet("font-size:11px;font-weight:700;color:#64748b;letter-spacing:0.5px;");
    hdr->addWidget(badge); hdr->addWidget(ttl,1);
    vl->addLayout(hdr);

    *valLbl = new QLabel("—");
    (*valLbl)->setStyleSheet(QString(
        "font-size:36px;font-weight:900;color:%1;letter-spacing:-1px;").arg(accent.name()));
    vl->addWidget(*valLbl);

    *subLbl = new QLabel("...");
    (*subLbl)->setStyleSheet("font-size:11px;color:#94a3b8;");
    vl->addWidget(*subLbl);
    vl->addStretch();

    auto *wave = new WaveWidget(accent, card);
    vl->addWidget(wave);
    return card;
}

// ─────────────────────────────────────────────────────────────────────────────
// makeDonutCard
// ─────────────────────────────────────────────────────────────────────────────
QFrame* Dashboard::makeDonutCard(const QString &icon, const QString &title,
                                  QColor accent, DonutWidget **donutOut,
                                  const QStringList &rowIcons, const QStringList &rowLabels,
                                  const QList<QColor> &rowColors, QList<QLabel*> *rowVals)
{
    auto *card = new QFrame;
    card->setStyleSheet(
        "QFrame{background:white;border-radius:16px;border:1px solid #e2e8f0;}"
        "QLabel{background:transparent;border:none;}");

    auto *vl = new QVBoxLayout(card);
    vl->setContentsMargins(14,12,14,12); vl->setSpacing(8);

    auto *hdr = new QHBoxLayout;
    auto *ttl = new QLabel(icon + "  " + title);
    ttl->setStyleSheet(QString("font-size:12px;font-weight:700;color:%1;").arg(accent.name()));
    auto *voir = new QLabel("<span style='color:#2563eb;font-size:11px;'>Voir tout</span>");
    hdr->addWidget(ttl,1); hdr->addWidget(voir);
    vl->addLayout(hdr);

    auto *sep = new QFrame; sep->setFrameShape(QFrame::HLine);
    sep->setStyleSheet("background:#e2e8f0;border:none;max-height:1px;");
    vl->addWidget(sep);

    auto *body = new QHBoxLayout; body->setSpacing(10);
    auto *rowsW = new QVBoxLayout; rowsW->setSpacing(6);
    rowVals->clear();
    for (int i = 0; i < rowLabels.size(); ++i) {
        auto *row = new QHBoxLayout; row->setSpacing(6);
        auto *ico = new QLabel(rowIcons[i]);
        ico->setStyleSheet("font-size:14px;");
        auto *lbl = new QLabel(rowLabels[i]);
        lbl->setStyleSheet("font-size:12px;color:#475569;");
        auto *val = new QLabel("—");
        val->setStyleSheet(QString("font-size:14px;font-weight:800;color:%1;")
                               .arg(rowColors[i].name()));
        val->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
        row->addWidget(ico); row->addWidget(lbl,1); row->addWidget(val);
        rowsW->addLayout(row);
        rowVals->append(val);
    }
    rowsW->addStretch();
    *donutOut = new DonutWidget;
    body->addLayout(rowsW,1); body->addWidget(*donutOut);
    vl->addLayout(body);
    return card;
}


// ─────────────────────────────────────────────────────────────────────────────
// Constructor
// ─────────────────────────────────────────────────────────────────────────────
Dashboard::Dashboard(GestionQuai *q, GestionNavires *n, GestionCaptures *c,
                     GestionUtilisateurs *u, QWidget *parent)
    : QWidget(parent), gQuai(q), gNavires(n), gCaptures(c), gUsers(u)
{
    auto *scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true); scroll->setFrameShape(QFrame::NoFrame);
    scroll->setStyleSheet(
        "QScrollArea{background:#f0f4f8;border:none;}"
        "QScrollBar:vertical{background:#f1f5f9;width:6px;border-radius:3px;}"
        "QScrollBar::handle:vertical{background:#cbd5e1;border-radius:3px;}");

    auto *root = new QWidget; root->setStyleSheet("background:#f0f4f8;");
    scroll->setWidget(root);
    auto *outer = new QVBoxLayout(this); outer->setContentsMargins(0,0,0,0);
    outer->addWidget(scroll);

    auto *main = new QVBoxLayout(root);
    main->setContentsMargins(24,20,24,24); main->setSpacing(18);

    // ── HEADER ───────────────────────────────────────────────────────────────
    {
        auto *hdr = new QHBoxLayout;
        auto *left = new QVBoxLayout; left->setSpacing(2);
        auto *title = new QLabel("Tableau de bord");
        title->setStyleSheet("font-size:24px;font-weight:900;color:#0f172a;background:transparent;");
        auto *sub = new QLabel("Vue d'ensemble du port — FishTech Maritime");
        sub->setStyleSheet("font-size:12px;color:#64748b;background:transparent;");
        left->addWidget(title); left->addWidget(sub);

        auto *right = new QHBoxLayout; right->setSpacing(14);
        right->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

        auto *clockBlock = new QVBoxLayout; clockBlock->setSpacing(1);
        clockBlock->setAlignment(Qt::AlignRight);
        lblClock = new QLabel;
        lblClock->setStyleSheet("font-size:26px;font-weight:900;color:#1d4ed8;background:transparent;letter-spacing:1px;");
        lblClock->setAlignment(Qt::AlignRight);
        lblDate = new QLabel;
        lblDate->setStyleSheet("font-size:11px;color:#64748b;background:transparent;");
        lblDate->setAlignment(Qt::AlignRight);
        clockBlock->addWidget(lblClock); clockBlock->addWidget(lblDate);

        auto *liveCol = new QVBoxLayout; liveCol->setSpacing(2);
        liveCol->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
        auto *liveBadge = new QLabel("● LIVE");
        liveBadge->setStyleSheet("background:#dcfce7;color:#16a34a;border-radius:10px;padding:3px 10px;font-size:11px;font-weight:700;border:none;");
        auto *liveSub = new QLabel("Données en temps réel");
        liveSub->setStyleSheet("font-size:10px;color:#64748b;background:transparent;");
        liveSub->setAlignment(Qt::AlignRight);
        liveCol->addWidget(liveBadge); liveCol->addWidget(liveSub);

        right->addLayout(clockBlock); right->addLayout(liveCol);
        hdr->addLayout(left,1); hdr->addLayout(right);
        main->addLayout(hdr);
    }

    // ── KPI CARDS ────────────────────────────────────────────────────────────
    {
        auto *grid = new QGridLayout; grid->setSpacing(14);
        grid->addWidget(makeKpiCard("🚢","NAVIRES TOTAL", QColor("#1d4ed8"),&kpiNaviresVal,  &kpiNaviresSub),  0,0);
        grid->addWidget(makeKpiCard("🎣","capture",      QColor("#059669"),&kpiCapturesVal, &kpiCapturesSub), 0,1);
        grid->addWidget(makeKpiCard("👥","PERSONNEL",     QColor("#7c3aed"),&kpiPersonnelVal,&kpiPersonnelSub),0,2);
        grid->addWidget(makeKpiCard("⚓","QUAIS",         QColor("#d97706"),&kpiQuaisVal,    &kpiQuaisSub),    0,3);
        main->addLayout(grid);
    }

    // ── DONUT ROW ─────────────────────────────────────────────────────────────
    {
        auto *row = new QHBoxLayout; row->setSpacing(14);
        QList<QLabel*> navVals, capVals, perVals;

        row->addWidget(makeDonutCard("🚢","STATUT DES NAVIRES",QColor("#1d4ed8"),&donutNavires,
            {"🟢","🔵","🔴"},{"À quai","En mer","Interdit"},
            {QColor("#059669"),QColor("#1d4ed8"),QColor("#dc2626")},&navVals),1);

        row->addWidget(makeDonutCard("🎣","RÉPARTITION DES capture",QColor("#059669"),&donutCaptures,
            {"🐟","🦞","🦑"},{"Poisson","Crustacés","Mollusques"},
            {QColor("#0ea5e9"),QColor("#f59e0b"),QColor("#8b5cf6")},&capVals),1);

        row->addWidget(makeDonutCard("👥","RÉPARTITION DU PERSONNEL",QColor("#7c3aed"),&donutPersonnel,
            {"🎣","📦","📋"},{"Pêcheurs","Dockers","Superviseurs"},
            {QColor("#0ea5e9"),QColor("#f59e0b"),QColor("#7c3aed")},&perVals),1);

        if (navVals.size()==3){statQuai=navVals[0];statMer=navVals[1];statInterdit=navVals[2];}
        if (capVals.size()==3){statPoisson=capVals[0];statCrustace=capVals[1];statMollusque=capVals[2];}
        if (perVals.size()==3){statPecheur=perVals[0];statDocker=perVals[1];statSuperviseur=perVals[2];}
        main->addLayout(row);
    }

    // ── OCCUPATION + SPARKLINE ────────────────────────────────────────────────
    {
        auto *row = new QHBoxLayout; row->setSpacing(14);

        // Occupation
        {
            auto *card = new QFrame;
            card->setStyleSheet("QFrame{background:white;border-radius:16px;border:1px solid #e2e8f0;}QLabel{background:transparent;border:none;}");
            auto *vl = new QVBoxLayout(card); vl->setContentsMargins(16,14,16,14); vl->setSpacing(10);
            auto *ttl = new QLabel("⚓  TAUX D'OCCUPATION DES QUAIS");
            ttl->setStyleSheet("font-size:12px;font-weight:700;color:#0f172a;");
            vl->addWidget(ttl);
            auto *sep = new QFrame; sep->setFrameShape(QFrame::HLine);
            sep->setStyleSheet("background:#e2e8f0;border:none;max-height:1px;");
            vl->addWidget(sep);
            pbOccupPct = new QLabel("0%");
            pbOccupPct->setStyleSheet("font-size:38px;font-weight:900;color:#1d4ed8;");
            pbOccupPct->setAlignment(Qt::AlignCenter);
            auto *occLbl = new QLabel("Occupation");
            occLbl->setStyleSheet("font-size:11px;color:#94a3b8;"); occLbl->setAlignment(Qt::AlignCenter);
            vl->addWidget(pbOccupPct); vl->addWidget(occLbl);
            pbOccupation = new QProgressBar;
            pbOccupation->setRange(0,100); pbOccupation->setValue(0);
            pbOccupation->setTextVisible(false); pbOccupation->setFixedHeight(10);
            pbOccupation->setStyleSheet(
                "QProgressBar{background:#e2e8f0;border-radius:5px;border:none;}"
                "QProgressBar::chunk{background:qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 #1d4ed8,stop:1 #059669);border-radius:5px;}");
            pbOccupLbl = new QLabel("0%");
            pbOccupLbl->setStyleSheet("font-size:11px;font-weight:700;color:#1d4ed8;");
            pbOccupLbl->setAlignment(Qt::AlignRight);
            vl->addWidget(pbOccupation); vl->addWidget(pbOccupLbl);
            auto *leg = new QHBoxLayout; leg->setSpacing(14);
            auto mkLeg=[](const QString&col,const QString&txt){
                auto*h=new QHBoxLayout;h->setSpacing(4);
                auto*d=new QLabel("●");d->setStyleSheet(QString("color:%1;font-size:10px;background:transparent;border:none;").arg(col));
                auto*l=new QLabel(txt);l->setStyleSheet("font-size:11px;color:#64748b;background:transparent;border:none;");
                h->addWidget(d);h->addWidget(l);return h;};
            leg->addLayout(mkLeg("#1d4ed8","Navires présents"));
            leg->addLayout(mkLeg("#e2e8f0","Places libres"));
            leg->addStretch(); vl->addLayout(leg);
            row->addWidget(card,1);
        }

        // Sparkline
        {
            auto *card = new QFrame;
            card->setStyleSheet("QFrame{background:white;border-radius:16px;border:1px solid #e2e8f0;}QLabel{background:transparent;border:none;}");
            auto *vl = new QVBoxLayout(card); vl->setContentsMargins(16,14,16,14); vl->setSpacing(8);
            auto *ttl = new QLabel("⚓  ACTIVITÉ DES capture (7 DERNIERS JOURS)");
            ttl->setStyleSheet("font-size:12px;font-weight:700;color:#0f172a;");
            vl->addWidget(ttl);
            auto *sep = new QFrame; sep->setFrameShape(QFrame::HLine);
            sep->setStyleSheet("background:#e2e8f0;border:none;max-height:1px;");
            vl->addWidget(sep);
            sparkline = new SparklineWidget;
            QList<int> def={3,5,4,7,6,8,5}; QStringList lbl;
            for(int i=6;i>=0;--i) lbl<<QDate::currentDate().addDays(-i).toString("dd MMM");
            sparkline->setData(def,lbl);
            vl->addWidget(sparkline,1);
            row->addWidget(card,2);
        }
        main->addLayout(row);
    }

    // ── FOOTER ───────────────────────────────────────────────────────────────
    {
        auto *footer = new QFrame;
        footer->setStyleSheet(
            "QFrame{background:qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 #1e3a8a,stop:1 #1d4ed8);border-radius:14px;border:none;}"
            "QLabel{background:transparent;border:none;}");
        auto *fl = new QHBoxLayout(footer); fl->setContentsMargins(20,14,20,14); fl->setSpacing(0);
        auto mkInfo=[](const QString&icon,const QString&t,const QString&s){
            auto*h=new QHBoxLayout;h->setSpacing(10);
            auto*i=new QLabel(icon);i->setStyleSheet("font-size:18px;");
            auto*tv=new QVBoxLayout;tv->setSpacing(1);
            auto*tl=new QLabel(t);tl->setStyleSheet("font-size:12px;font-weight:700;color:white;");
            auto*sl=new QLabel(s);sl->setStyleSheet("font-size:10px;color:rgba(255,255,255,0.7);");
            tv->addWidget(tl);tv->addWidget(sl);h->addWidget(i);h->addLayout(tv);return h;};
        fl->addLayout(mkInfo("📍","Port de Tunis, Tunisie","Coordonnées GPS  36.8065° N, 10.1815° E"));
        fl->addStretch();
        fl->addLayout(mkInfo("📡","Données en temps réel","Système mis à jour il y a quelques secondes"));
        fl->addStretch();
        fl->addLayout(mkInfo("🔒","Sécurité","Système sécurisé SSL / HTTPS"));
        fl->addStretch();
        fl->addLayout(mkInfo("🚢","FishTech — Gestion Portuaire Intelligente","Solution fiable pour une gestion portuaire moderne."));
        main->addWidget(footer);
    }

    // ── CLOCK ────────────────────────────────────────────────────────────────
    auto *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this](){
        QLocale fr(QLocale::French);
        QDateTime now=QDateTime::currentDateTime();
        lblClock->setText(now.toString("hh:mm:ss"));
        lblDate->setText(fr.toString(now,"dddd dd MMMM yyyy"));
    });
    timer->start(1000);
    QLocale fr(QLocale::French);
    QDateTime now=QDateTime::currentDateTime();
    lblClock->setText(now.toString("hh:mm:ss"));
    lblDate->setText(fr.toString(now,"dddd dd MMMM yyyy"));

    refresh();
}


// ─────────────────────────────────────────────────────────────────────────────
// refresh()
// ─────────────────────────────────────────────────────────────────────────────
void Dashboard::refresh()
{
    if (gNavires) {
        auto s=gNavires->getStats();
        kpiNaviresVal->setText(QString::number(s.total));
        kpiNaviresSub->setText(QString("%1 à quai · %2 en mer · %3 interdit").arg(s.quai).arg(s.mer).arg(s.interdit));
        if(statQuai)    statQuai->setText(QString::number(s.quai));
        if(statMer)     statMer->setText(QString::number(s.mer));
        if(statInterdit)statInterdit->setText(QString::number(s.interdit));
        if(donutNavires){
            donutNavires->setSlices({{"À quai",s.quai,QColor("#059669")},{"En mer",s.mer,QColor("#1d4ed8")},{"Interdit",s.interdit,QColor("#dc2626")}});
            donutNavires->setTotal(s.total);
        }
    }
    if (gCaptures) {
        auto s=gCaptures->getStats();
        kpiCapturesVal->setText(QString::number(s.total));
        kpiCapturesSub->setText(QString("%1 poisson · %2 crustacés · %3 mollusques").arg(s.poisson).arg(s.crustace).arg(s.mollusque));
        if(statPoisson)  statPoisson->setText(QString::number(s.poisson));
        if(statCrustace) statCrustace->setText(QString::number(s.crustace));
        if(statMollusque)statMollusque->setText(QString::number(s.mollusque));
        if(donutCaptures){
            donutCaptures->setSlices({{"Poisson",s.poisson,QColor("#0ea5e9")},{"Crustacés",s.crustace,QColor("#f59e0b")},{"Mollusques",s.mollusque,QColor("#8b5cf6")}});
            donutCaptures->setTotal(s.total);
        }
        if(sparkline){
            QList<int> data; QStringList labels;
            for(int i=6;i>=0;--i){labels<<QDate::currentDate().addDays(-i).toString("dd MMM");data<<0;}
            sparkline->setData(data,labels);
        }
    }
    if (gUsers) {
        auto s=gUsers->getStats();
        kpiPersonnelVal->setText(QString::number(s.total));
        kpiPersonnelSub->setText(QString("Salaire moy. %1 TND").arg(QString::number(s.avgSalaire,'f',0)));
        if(statPecheur)    statPecheur->setText(QString::number(s.pecheur));
        if(statDocker)     statDocker->setText(QString::number(s.docker));
        if(statSuperviseur)statSuperviseur->setText(QString::number(s.superviseur));
        if(donutPersonnel){
            donutPersonnel->setSlices({{"Pêcheurs",s.pecheur,QColor("#0ea5e9")},{"Dockers",s.docker,QColor("#f59e0b")},{"Superviseurs",s.superviseur,QColor("#7c3aed")}});
            donutPersonnel->setTotal(s.total);
        }
    }
    if (gQuai) {
        auto s=gQuai->getStats();
        kpiQuaisVal->setText(QString::number(s.totalQuais));
        kpiQuaisSub->setText(QString("%1 navire(s) présent(s)").arg(s.totalNaviresPresent));
        int pct=(s.totalQuais>0)?qMin(100,s.totalNaviresPresent*100/qMax(1,s.totalQuais)):0;
        if(pbOccupation)pbOccupation->setValue(pct);
        if(pbOccupLbl)  pbOccupLbl->setText(QString("%1%").arg(pct));
        if(pbOccupPct)  pbOccupPct->setText(QString("%1%").arg(pct));
    }
}
