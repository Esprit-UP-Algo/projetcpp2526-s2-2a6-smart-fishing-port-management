#include "BarriereWidget.h"
#include <QPainterPath>
#include <QLinearGradient>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPainter>
#include <QDateTime>
#include <QRandomGenerator>
#include <QFrame>
#include <QApplication>

// ── helper ───────────────────────────────────────────────────────────────────

static QPushButton *mkBtn(const QString &t, const QString &bg, const QString &hov)
{
    auto *b = new QPushButton(t);
    b->setMinimumHeight(40);
    b->setCursor(Qt::PointingHandCursor);
    b->setStyleSheet(QString(
        "QPushButton{background:%1;color:white;border:none;border-radius:10px;"
        "padding:0 16px;font-weight:700;font-size:13px;}"
        "QPushButton:hover{background:%2;border-bottom:3px solid rgba(0,0,0,.2);}"
        "QPushButton:pressed{padding-top:3px;}").arg(bg,hov));
    return b;
}

// ── constructor ──────────────────────────────────────────────────────────────

BarriereWidget::BarriereWidget(const QList<QString> &navires, QWidget *parent)
    : QWidget(parent), m_navires(navires)
{
    setMinimumSize(700, 520);
    setStyleSheet("QWidget{background:#f0f4f8;font-family:'Segoe UI';font-size:13px;}"
                  "QListWidget{background:white;border:1px solid #e2e8f0;border-radius:8px;}"
                  "QListWidget::item{padding:6px 10px;border-bottom:1px solid #f1f5f9;}"
                  "QListWidget::item:selected{background:#dbeafe;color:#1e293b;}");

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(16,16,16,16);
    root->setSpacing(12);

    // ── title ────────────────────────────────────────────────
    auto *titleRow = new QHBoxLayout;
    auto *title = new QLabel("⚓  Détection des Navires & Contrôle Barrière");
    title->setStyleSheet("font-size:18px;font-weight:900;color:#0a1628;");
    titleRow->addWidget(title);
    titleRow->addStretch();

    // Arduino status
    m_statusLbl = new QLabel("🔴  Arduino non connecté");
    m_statusLbl->setStyleSheet("font-size:12px;font-weight:700;color:#ef4444;"
                               "background:#fee2e2;border-radius:8px;padding:4px 10px;");
    titleRow->addWidget(m_statusLbl);
    root->addLayout(titleRow);

    // ── main grid ────────────────────────────────────────────
    auto *grid = new QHBoxLayout;
    grid->setSpacing(12);

    // Left: barrier visual + controls
    auto *leftFrame = new QFrame;
    leftFrame->setStyleSheet("QFrame{background:white;border-radius:14px;"
                             "border:1px solid #e2e8f0;}");
    auto *leftBox = new QVBoxLayout(leftFrame);
    leftBox->setContentsMargins(16,16,16,16);
    leftBox->setSpacing(10);

    auto *barriereLbl = new QLabel("🚧  État de la Barrière");
    barriereLbl->setStyleSheet("font-size:14px;font-weight:800;color:#1e293b;");
    leftBox->addWidget(barriereLbl);

    // Animated barrier visual (custom painted label)
    m_barriereViz = new QLabel;
    m_barriereViz->setFixedSize(280, 160);
    m_barriereViz->setAlignment(Qt::AlignCenter);
    leftBox->addWidget(m_barriereViz, 0, Qt::AlignCenter);

    // Buttons
    auto *btnRow = new QHBoxLayout;
    m_btnOuvrir = mkBtn("🔓  Ouvrir", "#10b981", "#059669");
    m_btnFermer = mkBtn("🔒  Fermer", "#ef4444", "#dc2626");
    m_btnAuto   = mkBtn("🤖  Auto",   "#8b5cf6", "#7c3aed");
    btnRow->addWidget(m_btnOuvrir);
    btnRow->addWidget(m_btnFermer);
    btnRow->addWidget(m_btnAuto);
    leftBox->addLayout(btnRow);

    // Arduino connect button
    auto *btnConnect = mkBtn("🔌  Connecter Arduino", "#3b82f6", "#2563eb");
    leftBox->addWidget(btnConnect);

    grid->addWidget(leftFrame, 1);

    // Right: navire list + log
    auto *rightBox = new QVBoxLayout;
    rightBox->setSpacing(8);

    auto *navLbl = new QLabel("🚢  Navires détectés");
    navLbl->setStyleSheet("font-size:13px;font-weight:800;color:#1e293b;");
    rightBox->addWidget(navLbl);

    m_navireList = new QListWidget;
    m_navireList->setMaximumHeight(180);
    for (const QString &n : m_navires)
        m_navireList->addItem("🚢  " + n);
    rightBox->addWidget(m_navireList);

    auto *logLbl = new QLabel("📋  Journal des événements");
    logLbl->setStyleSheet("font-size:13px;font-weight:800;color:#1e293b;");
    rightBox->addWidget(logLbl);

    m_logList = new QListWidget;
    rightBox->addWidget(m_logList, 1);

    grid->addLayout(rightBox, 1);
    root->addLayout(grid, 1);

    // ── connections ──────────────────────────────────────────
    connect(m_btnOuvrir, &QPushButton::clicked, this, &BarriereWidget::onOuvrir);
    connect(m_btnFermer, &QPushButton::clicked, this, &BarriereWidget::onFermer);
    connect(m_btnAuto,   &QPushButton::clicked, this, &BarriereWidget::onAutoToggle);
    connect(m_navireList,&QListWidget::currentRowChanged, this, &BarriereWidget::onNavireSelected);
    connect(btnConnect,  &QPushButton::clicked, this, [this]() {
        int r = m_arduino.connect_arduino();
        m_connected = (r == 0);
        if (m_connected) {
            m_statusLbl->setText("🟢  Arduino connecté");
            m_statusLbl->setStyleSheet("font-size:12px;font-weight:700;color:#059669;"
                                       "background:#d1fae5;border-radius:8px;padding:4px 10px;");
            log("Arduino connecté sur " + m_arduino.getarduino_port_name());
        } else {
            m_statusLbl->setText("🔴  Arduino non trouvé");
            log("Échec connexion Arduino (code " + QString::number(r) + ")");
        }
    });

    // ── animation timer ──────────────────────────────────────
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &BarriereWidget::onTick);
    m_timer->start(40);

    updateBarriereVisual();
    log("Système de détection démarré");
}

// ── slots ─────────────────────────────────────────────────────────────────────

void BarriereWidget::onOuvrir()
{
    m_barriereOpen = true;
    sendCmd("O"); // 'O' = Open
    log("Barrière OUVERTE manuellement");
    updateBarriereVisual();
}

void BarriereWidget::onFermer()
{
    m_barriereOpen = false;
    sendCmd("F"); // 'F' = Fermer
    log("Barrière FERMÉE manuellement");
    updateBarriereVisual();
}

void BarriereWidget::onAutoToggle()
{
    m_autoMode = !m_autoMode;
    m_btnAuto->setText(m_autoMode ? "🤖  Auto ON" : "🤖  Auto");
    m_btnAuto->setStyleSheet(m_btnAuto->styleSheet());
    log(m_autoMode ? "Mode automatique ACTIVÉ" : "Mode automatique DÉSACTIVÉ");
}

void BarriereWidget::onNavireSelected(int row)
{
    if (row < 0 || row >= m_navires.size()) return;
    m_detectedIdx = row;
    log("Navire détecté : " + m_navires[row]);

    if (m_autoMode) {
        // Auto: open barrier when ship detected
        m_barriereOpen = true;
        sendCmd("O");
        log("→ Barrière ouverte automatiquement pour " + m_navires[row]);
        updateBarriereVisual();

        // Auto-close after 5 seconds
        QTimer::singleShot(5000, this, [this]() {
            if (m_autoMode) {
                m_barriereOpen = false;
                sendCmd("F");
                log("→ Barrière fermée automatiquement");
                updateBarriereVisual();
            }
        });
    }
}

void BarriereWidget::onTick()
{
    m_tick++;
    // Simulate random detection every ~8s in auto mode
    if (m_autoMode && !m_navires.isEmpty() && m_tick % 200 == 0) {
        int idx = QRandomGenerator::global()->bounded(m_navires.size());
        m_navireList->setCurrentRow(idx);
    }
    updateBarriereVisual();
}

// ── sendCmd ───────────────────────────────────────────────────────────────────

void BarriereWidget::sendCmd(const QByteArray &cmd)
{
    if (m_connected)
        m_arduino.write_to_arduino(cmd);
    else
        log("(simulation) Commande envoyée : " + QString(cmd));
}

// ── log ───────────────────────────────────────────────────────────────────────

void BarriereWidget::log(const QString &msg)
{
    if (!m_logList) return;
    QString ts = QDateTime::currentDateTime().toString("HH:mm:ss");
    auto *item = new QListWidgetItem("[" + ts + "]  " + msg);
    if (msg.contains("OUVERTE") || msg.contains("ouverte"))
        item->setForeground(QColor("#059669"));
    else if (msg.contains("FERMÉE") || msg.contains("fermée"))
        item->setForeground(QColor("#dc2626"));
    else if (msg.contains("détecté"))
        item->setForeground(QColor("#1d4ed8"));
    m_logList->insertItem(0, item);
    if (m_logList->count() > 50) m_logList->takeItem(m_logList->count()-1);
}

// ── updateBarriereVisual ──────────────────────────────────────────────────────

void BarriereWidget::updateBarriereVisual()
{
    if (!m_barriereViz) return;

    QPixmap pix(280, 160);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);

    // Water channel
    QLinearGradient water(0,60,0,130);
    water.setColorAt(0, QColor("#0ea5e9"));
    water.setColorAt(1, QColor("#0369a1"));
    p.fillRect(0, 60, 280, 70, water);

    // Animated waves
    p.setPen(QPen(QColor(255,255,255,60), 1.2));
    for (int row = 0; row < 3; row++) {
        QPainterPath wave;
        double phase = m_tick * 0.1 + row * 1.2;
        wave.moveTo(0, 80 + row*15);
        for (int x = 0; x <= 280; x += 5)
            wave.lineTo(x, 80 + row*15 + 4*qSin(x*0.08 + phase));
        p.drawPath(wave);
    }

    // Shore (left and right)
    p.fillRect(0,   0, 60, 160, QColor("#d4a574"));
    p.fillRect(220, 0, 60, 160, QColor("#d4a574"));

    // Barrier posts
    p.fillRect(52, 40, 8, 80, QColor("#374151"));
    p.fillRect(220, 40, 8, 80, QColor("#374151"));

    // Barrier arm
    if (m_barriereOpen) {
        // Arm raised (vertical)
        double angle = -80.0 + qMin(80.0, (m_tick % 20) * 4.0);
        p.save();
        p.translate(56, 60);
        p.rotate(angle);
        QLinearGradient armG(0,0,100,0);
        armG.setColorAt(0, QColor("#10b981"));
        armG.setColorAt(0.5, QColor("#fbbf24"));
        armG.setColorAt(1, QColor("#10b981"));
        p.fillRect(0, -5, 100, 10, armG);
        p.restore();
    } else {
        // Arm horizontal (closed)
        QLinearGradient armG(60,0,220,0);
        armG.setColorAt(0, QColor("#ef4444"));
        armG.setColorAt(0.5, QColor("#fbbf24"));
        armG.setColorAt(1, QColor("#ef4444"));
        p.fillRect(60, 55, 160, 10, armG);

        // Stripes
        p.setPen(Qt::NoPen);
        for (int i = 0; i < 8; i++) {
            p.setBrush(i%2==0 ? QColor(239,68,68,180) : QColor(251,191,36,180));
            p.drawRect(60+i*20, 55, 20, 10);
        }
    }

    // Status text
    p.setPen(m_barriereOpen ? QColor("#059669") : QColor("#dc2626"));
    p.setFont(QFont("Segoe UI", 11, QFont::Bold));
    p.drawText(QRect(0,130,280,24), Qt::AlignCenter,
               m_barriereOpen ? "🔓  BARRIÈRE OUVERTE" : "🔒  BARRIÈRE FERMÉE");

    // Boat icon if detected
    if (m_detectedIdx >= 0 && m_barriereOpen) {
        double bx = 80 + (m_tick % 60) * 1.2;
        if (bx < 200) {
            p.setFont(QFont("Segoe UI", 18));
            p.drawText(QRectF(bx, 68, 30, 30), Qt::AlignCenter, "🚢");
        }
    }

    p.end();
    m_barriereViz->setPixmap(pix);
}
