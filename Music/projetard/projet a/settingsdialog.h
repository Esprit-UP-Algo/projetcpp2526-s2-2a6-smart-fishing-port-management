#pragma once

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QSlider>
#include <QComboBox>
#include <QFrame>
#include <QButtonGroup>
#include <QSettings>
#include <QApplication>
#include <QFont>

// ─── SettingsDialog ───────────────────────────────────────────────────────────
// Persiste via QSettings("FishTech","App").
// Appelé depuis MainWindow::onShowSettings().
// Émet themeChangeRequested(stylesheet, fontSize) à l'application.
// ─────────────────────────────────────────────────────────────────────────────
class SettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget *parent = nullptr);

    // Accessors used by MainWindow to apply after accept()
    bool    nightMode()  const;
    int     fontSize()   const;
    QString language()   const;
    int     density()    const;   // 0=Normal 1=Compact 2=Confortable
    QString accentHex()  const;

    // Build and return the full application stylesheet
    QString buildStyleSheet() const;

private:
    // ── Appearance ──────────────────────────────────────────
    QCheckBox  *m_chkNight;

    // ── Density ─────────────────────────────────────────────
    QPushButton *m_btnNormal, *m_btnCompact, *m_btnComfort;
    QButtonGroup *m_densityGroup;

    // ── Font size ────────────────────────────────────────────
    QSlider *m_sliderFont;
    QLabel  *m_lblFontPreview;

    // ── Language ─────────────────────────────────────────────
    QComboBox *m_cmbLang;

    // ── Accent color ─────────────────────────────────────────
    QButtonGroup *m_accentGroup;
    static constexpr const char* ACCENTS[] = {
        "#1d4ed8",  // Bleu (défaut)
        "#059669",  // Vert
        "#7c3aed",  // Violet
        "#d97706",  // Orange
        "#dc2626",  // Rouge
        "#0891b2",  // Cyan
    };
    static constexpr const char* ACCENT_NAMES[] = {
        "Bleu","Vert","Violet","Orange","Rouge","Cyan"
    };

    void load();
    void save() const;

    // ── UI helpers ────────────────────────────────────────────
    static QFrame* section(const QString &title);
    static QFrame* hline();
    QCheckBox* toggle(const QString &label, const QString &tip, bool checked = false);
};

// ─────────────────────────────────────────────────────────────────────────────
// Inline implementation
// ─────────────────────────────────────────────────────────────────────────────

inline QFrame* SettingsDialog::hline() {
    auto *f = new QFrame;
    f->setFrameShape(QFrame::HLine);
    f->setStyleSheet("background:#e2e8f0;border:none;max-height:1px;margin:2px 0;");
    return f;
}

inline QFrame* SettingsDialog::section(const QString &title) {
    auto *f = new QFrame;
    f->setStyleSheet("QFrame{background:#f8fafc;border-radius:10px;border:1px solid #e2e8f0;}");
    auto *vl = new QVBoxLayout(f);
    vl->setContentsMargins(16,12,16,12);
    vl->setSpacing(10);
    auto *lbl = new QLabel(title);
    lbl->setStyleSheet("font-size:11px;font-weight:700;color:#64748b;letter-spacing:1px;background:transparent;border:none;");
    vl->addWidget(lbl);
    vl->addWidget(hline());
    return f;
}

inline QCheckBox* SettingsDialog::toggle(const QString &label, const QString &tip, bool checked) {
    auto *cb = new QCheckBox(label);
    cb->setChecked(checked);
    cb->setToolTip(tip);
    cb->setStyleSheet(
        "QCheckBox{font-size:13px;font-weight:600;color:#1e293b;spacing:10px;background:transparent;border:none;}"
        "QCheckBox::indicator{width:40px;height:22px;border-radius:11px;border:2px solid #cbd5e1;background:#e2e8f0;}"
        "QCheckBox::indicator:checked{background:#1d4ed8;border:2px solid #1d4ed8;}"
        "QCheckBox::indicator:hover{border:2px solid #3b82f6;}"
    );
    return cb;
}

inline SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("Paramètres");
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setFixedWidth(440);
    setStyleSheet(
        "QDialog{background:#ffffff;}"
        "QLabel{color:#1e293b;background:transparent;border:none;}"
        "QComboBox{background:#f8fafc;border:1px solid #e2e8f0;border-radius:8px;padding:6px 10px;font-size:13px;color:#1e293b;min-height:32px;}"
        "QComboBox:focus{border:2px solid #1d4ed8;}"
        "QComboBox::drop-down{border:none;width:20px;}"
        "QComboBox QAbstractItemView{background:white;border:1px solid #e2e8f0;selection-background-color:#dbeafe;color:#1e293b;}"
        "QSlider::groove:horizontal{background:#e2e8f0;height:6px;border-radius:3px;}"
        "QSlider::handle:horizontal{background:#1d4ed8;width:18px;height:18px;border-radius:9px;margin:-6px 0;}"
        "QSlider::sub-page:horizontal{background:#1d4ed8;border-radius:3px;}"
        "QPushButton{background:#f1f5f9;color:#475569;border:1px solid #e2e8f0;border-radius:8px;padding:7px 16px;font-size:12px;font-weight:600;}"
        "QPushButton:hover{background:#e2e8f0;color:#1e293b;}"
        "QPushButton:checked{background:#1d4ed8;color:white;border:1px solid #1d4ed8;}"
    );

    auto *main = new QVBoxLayout(this);
    main->setContentsMargins(20,20,20,20);
    main->setSpacing(12);

    // ── Title bar ─────────────────────────────────────────────
    {
        auto *h = new QHBoxLayout;
        auto *ico = new QLabel("⚙️");
        ico->setStyleSheet("font-size:22px;background:transparent;border:none;");
        auto *t = new QLabel("Paramètres");
        t->setStyleSheet("font-size:18px;font-weight:800;color:#0f172a;background:transparent;border:none;");
        auto *sub = new QLabel("Personnalisez votre expérience");
        sub->setStyleSheet("font-size:12px;color:#64748b;background:transparent;border:none;");
        auto *vl = new QVBoxLayout; vl->setSpacing(1);
        vl->addWidget(t); vl->addWidget(sub);
        h->addWidget(ico); h->addLayout(vl,1);
        main->addLayout(h);
        main->addWidget(hline());
    }

    // ── Apparence ─────────────────────────────────────────────
    {
        auto *sec = section("APPARENCE");
        auto *vl  = qobject_cast<QVBoxLayout*>(sec->layout());

        m_chkNight = toggle("Mode nuit",
            "Thème sombre — réduit la fatigue oculaire en environnement peu éclairé");

        auto *row = new QHBoxLayout;
        row->setSpacing(8);
        auto *moonIco = new QLabel("🌙");
        moonIco->setStyleSheet("font-size:16px;background:transparent;border:none;");
        auto *desc = new QLabel("Basculer vers un thème sombre");
        desc->setStyleSheet("font-size:12px;color:#64748b;background:transparent;border:none;");
        row->addWidget(moonIco); row->addWidget(desc,1); row->addWidget(m_chkNight);
        vl->addLayout(row);

        main->addWidget(sec);
    }

    // ── Densité d'affichage ───────────────────────────────────
    {
        auto *sec = section("DENSITÉ D'AFFICHAGE");
        auto *vl  = qobject_cast<QVBoxLayout*>(sec->layout());

        auto *row = new QHBoxLayout; row->setSpacing(8);
        m_btnCompact  = new QPushButton("Compact");
        m_btnNormal   = new QPushButton("Normal");
        m_btnComfort  = new QPushButton("Confortable");
        m_btnCompact->setCheckable(true);
        m_btnNormal->setCheckable(true);
        m_btnComfort->setCheckable(true);
        m_btnNormal->setChecked(true);

        m_densityGroup = new QButtonGroup(this);
        m_densityGroup->setExclusive(true);
        m_densityGroup->addButton(m_btnCompact, 1);
        m_densityGroup->addButton(m_btnNormal,  0);
        m_densityGroup->addButton(m_btnComfort, 2);

        row->addWidget(m_btnCompact,1);
        row->addWidget(m_btnNormal,1);
        row->addWidget(m_btnComfort,1);
        vl->addLayout(row);

        auto *hint = new QLabel("Ajuste les marges et espacements dans toute l'interface");
        hint->setStyleSheet("font-size:11px;color:#94a3b8;background:transparent;border:none;");
        vl->addWidget(hint);

        main->addWidget(sec);
    }

    // ── Taille de police ──────────────────────────────────────
    {
        auto *sec = section("TAILLE DE POLICE");
        auto *vl  = qobject_cast<QVBoxLayout*>(sec->layout());

        auto *row = new QHBoxLayout; row->setSpacing(10);
        auto *small = new QLabel("A");
        small->setStyleSheet("font-size:10px;color:#94a3b8;background:transparent;border:none;");
        m_sliderFont = new QSlider(Qt::Horizontal);
        m_sliderFont->setRange(11,16);
        m_sliderFont->setValue(13);
        m_sliderFont->setTickInterval(1);
        m_sliderFont->setSingleStep(1);
        auto *big = new QLabel("A");
        big->setStyleSheet("font-size:17px;color:#94a3b8;background:transparent;border:none;");
        m_lblFontPreview = new QLabel("Aperçu — 13px");
        m_lblFontPreview->setStyleSheet("font-size:13px;color:#1e293b;background:transparent;border:none;");
        m_lblFontPreview->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

        connect(m_sliderFont, &QSlider::valueChanged, this, [this](int v){
            m_lblFontPreview->setStyleSheet(QString("font-size:%1px;color:#1e293b;background:transparent;border:none;").arg(v));
            m_lblFontPreview->setText(QString("Aperçu — %1px").arg(v));
        });

        row->addWidget(small);
        row->addWidget(m_sliderFont,1);
        row->addWidget(big);
        row->addWidget(m_lblFontPreview);
        vl->addLayout(row);

        main->addWidget(sec);
    }

    // ── Langue ────────────────────────────────────────────────
    {
        auto *sec = section("LANGUE DE L'INTERFACE");
        auto *vl  = qobject_cast<QVBoxLayout*>(sec->layout());

        auto *row = new QHBoxLayout; row->setSpacing(10);
        auto *ico = new QLabel("🌍");
        ico->setStyleSheet("font-size:18px;background:transparent;border:none;");
        m_cmbLang = new QComboBox;
        m_cmbLang->addItem("🇫🇷  Français",  "fr");
        m_cmbLang->addItem("🇬🇧  English",   "en");
        m_cmbLang->addItem("🇸🇦  العربية",   "ar");
        m_cmbLang->addItem("🇪🇸  Español",   "es");
        row->addWidget(ico); row->addWidget(m_cmbLang,1);
        vl->addLayout(row);

        auto *hint = new QLabel("Nécessite un redémarrage pour prendre effet");
        hint->setStyleSheet("font-size:11px;color:#94a3b8;background:transparent;border:none;");
        vl->addWidget(hint);

        main->addWidget(sec);
    }

    // ── Couleur d'accent ──────────────────────────────────────
    {
        auto *sec = section("COULEUR D'ACCENT");
        auto *vl  = qobject_cast<QVBoxLayout*>(sec->layout());

        auto *row = new QHBoxLayout; row->setSpacing(8);
        m_accentGroup = new QButtonGroup(this);
        m_accentGroup->setExclusive(true);

        for (int i = 0; i < 6; ++i) {
            auto *btn = new QPushButton;
            btn->setFixedSize(32,32);
            btn->setCheckable(true);
            btn->setToolTip(ACCENT_NAMES[i]);
            btn->setStyleSheet(QString(
                "QPushButton{background:%1;border-radius:16px;border:3px solid transparent;}"
                "QPushButton:checked{border:3px solid #0f172a;}"
                "QPushButton:hover{border:3px solid #64748b;}"
            ).arg(ACCENTS[i]));
            m_accentGroup->addButton(btn, i);
            row->addWidget(btn);
        }
        row->addStretch();
        // Default: first button checked
        qobject_cast<QPushButton*>(m_accentGroup->button(0))->setChecked(true);
        vl->addLayout(row);

        main->addWidget(sec);
    }

    // ── Action buttons ────────────────────────────────────────
    {
        main->addWidget(hline());
        auto *row = new QHBoxLayout; row->setSpacing(8);

        auto *btnCancel = new QPushButton("Annuler");
        btnCancel->setStyleSheet(
            "QPushButton{background:#f1f5f9;color:#475569;border:1px solid #e2e8f0;"
            "border-radius:8px;padding:9px 20px;font-size:13px;font-weight:600;}"
            "QPushButton:hover{background:#e2e8f0;}"
        );

        auto *btnApply = new QPushButton("Appliquer");
        btnApply->setStyleSheet(
            "QPushButton{background:#1d4ed8;color:white;border:none;"
            "border-radius:8px;padding:9px 24px;font-size:13px;font-weight:700;}"
            "QPushButton:hover{background:#2563eb;}"
            "QPushButton:pressed{background:#1e40af;}"
        );
        btnApply->setDefault(true);

        row->addStretch();
        row->addWidget(btnCancel);
        row->addWidget(btnApply);
        main->addLayout(row);

        connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
        connect(btnApply,  &QPushButton::clicked, this, [this]{
            save();
            accept();
        });
    }

    load();
}

// ─── Accessors ────────────────────────────────────────────────────────────────
inline bool    SettingsDialog::nightMode() const { return m_chkNight->isChecked(); }
inline int     SettingsDialog::fontSize()  const { return m_sliderFont->value(); }
inline QString SettingsDialog::language()  const { return m_cmbLang->currentData().toString(); }
inline int     SettingsDialog::density()   const { return m_densityGroup->checkedId(); }
inline QString SettingsDialog::accentHex() const {
    int id = m_accentGroup->checkedId();
    return (id >= 0 && id < 6) ? QString(ACCENTS[id]) : QString(ACCENTS[0]);
}

// ─── Persistence ─────────────────────────────────────────────────────────────
inline void SettingsDialog::save() const {
    QSettings s("FishTech", "App");
    s.setValue("nightMode",  m_chkNight->isChecked());
    s.setValue("density",    m_densityGroup->checkedId());
    s.setValue("fontSize",   m_sliderFont->value());
    s.setValue("language",   m_cmbLang->currentIndex());
    s.setValue("accentId",   m_accentGroup->checkedId());
}

inline void SettingsDialog::load() {
    QSettings s("FishTech", "App");
    m_chkNight->setChecked(s.value("nightMode", false).toBool());

    int den = s.value("density", 0).toInt();
    if (auto *b = qobject_cast<QPushButton*>(m_densityGroup->button(den)))
        b->setChecked(true);

    m_sliderFont->setValue(s.value("fontSize", 13).toInt());
    m_cmbLang->setCurrentIndex(s.value("language", 0).toInt());

    int acc = s.value("accentId", 0).toInt();
    if (auto *b = qobject_cast<QPushButton*>(m_accentGroup->button(acc)))
        b->setChecked(true);
}

// ─── buildStyleSheet ──────────────────────────────────────────────────────────
inline QString SettingsDialog::buildStyleSheet() const
{
    const QString accent  = accentHex();
    const QString accentH = [&]{ // hover: lighten accent slightly
        // Simple approach: use a fixed lighter variant per accent
        static const char* hovers[] = {
            "#2563eb","#10b981","#8b5cf6","#f59e0b","#ef4444","#06b6d4"
        };
        int id = m_accentGroup->checkedId();
        return QString(id >= 0 && id < 6 ? hovers[id] : hovers[0]);
    }();

    const bool  night   = nightMode();
    const int   fs      = fontSize();
    const int   den     = density(); // 0=Normal 1=Compact 2=Confortable
    const int   pad     = (den == 1) ? 5 : (den == 2) ? 12 : 8;   // item padding

    const QString bg      = night ? "#0f172a" : "#f0f4f8";
    const QString surface = night ? "#1e293b" : "#ffffff";
    const QString border  = night ? "#334155" : "#e2e8f0";
    const QString text    = night ? "#f1f5f9" : "#1e293b";
    const QString muted   = night ? "#94a3b8" : "#64748b";
    const QString inputBg = night ? "#1e293b" : "#f8fafc";
    const QString hdrBg   = night ? "#0f172a" : "#0a1628";
    const QString selBg   = night ? "#1e3a8a" : "#dbeafe";
    const QString selText = night ? "#ffffff"  : "#1e293b";

    return QString(R"(
QMainWindow, QWidget {
    background-color: %1;
    font-family: "Segoe UI", Arial, sans-serif;
    font-size: %2px;
    color: %3;
}
QWidget#loginPage {
    background: qlineargradient(x1:0,y1:0,x2:1,y2:1,
        stop:0 #0a1628, stop:0.5 #0d2d5e, stop:1 #1a4a8a);
}
QWidget#loginCard {
    background: rgba(255,255,255,0.97);
    border-radius: 20px;
    border: 1px solid rgba(255,255,255,0.3);
}
QWidget#sidebar {
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
        stop:0 #0a1628, stop:0.6 #0d2d5e, stop:1 #0a1628);
    border-right: 1px solid #1e3a8a;
}
QWidget#sidebar QPushButton {
    background: transparent; color: #94a3b8;
    border: none; border-radius: 10px;
    text-align: left; padding: %4px 16px;
    font-size: %2px; font-weight: 600; margin: 2px 8px;
}
QWidget#sidebar QPushButton:hover {
    background: rgba(59,130,246,0.18); color: #e2e8f0;
    border-left: 3px solid %5; padding-left: 13px;
}
QWidget#sidebar QPushButton:pressed {
    background: rgba(59,130,246,0.30); color: white;
}
QLabel#labelLogo {
    font-size: 20px; font-weight: 900; color: white;
    padding: 22px 16px; background: rgba(255,255,255,0.06);
    border-bottom: 2px solid %5; letter-spacing: 2px;
}
QLabel#labelTitle {
    font-size: 22px; font-weight: 800; color: %3;
    padding: 8px 4px; letter-spacing: 0.5px;
}
QGroupBox {
    background: %6; border: 1px solid %7;
    border-radius: 12px; margin-top: 16px;
    padding: %4px 12px 12px 12px;
    font-weight: 700; color: %3;
}
QGroupBox::title {
    subcontrol-origin: margin; left: 14px;
    padding: 0 8px; color: %5;
    font-size: %2px; font-weight: 800; background: %6;
}
QTableWidget {
    background: %6; border: 1px solid %7;
    border-radius: 12px; gridline-color: %7;
    selection-background-color: %8;
    selection-color: %9; color: %3;
    alternate-background-color: %10;
}
QTableWidget::item { padding: %4px 10px; color: %3; border-bottom: 1px solid %7; }
QTableWidget::item:hover { background: %8; }
QTableWidget::item:selected { background: %5; color: white; }
QHeaderView::section {
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
        stop:0 %11, stop:1 %5);
    color: white; padding: %4px 8px; border: none;
    font-weight: 700; font-size: %2px;
}
QHeaderView::section:first { border-top-left-radius: 10px; }
QHeaderView::section:last  { border-top-right-radius: 10px; }
QLineEdit, QSpinBox, QDoubleSpinBox {
    background: %12; border: 1px solid %7;
    border-radius: 8px; padding: %4px 10px;
    font-size: %2px; color: %3;
}
QLineEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus {
    border: 2px solid %5;
}
QComboBox {
    background: %12; border: 1px solid %7;
    border-radius: 8px; padding: %4px 10px;
    font-size: %2px; color: %3;
}
QComboBox:focus { border: 2px solid %5; }
QComboBox::drop-down { border: none; width: 20px; }
QComboBox QAbstractItemView {
    background: %6; border: 1px solid %7;
    selection-background-color: %8; color: %3;
}
QPushButton {
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
        stop:0 %13, stop:1 %5);
    color: white; border: none; border-radius: 8px;
    padding: %4px 18px; font-weight: 700; font-size: %2px;
}
QPushButton:hover {
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
        stop:0 %13, stop:1 %13);
    border-bottom: 2px solid %5;
}
QPushButton:pressed { background: %5; padding-top: %14px; }
QPushButton:disabled { background: #cbd5e1; color: #94a3b8; }
QPushButton#btnLogin {
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
        stop:0 #0ea5e9, stop:1 #0284c7);
    font-size: 15px; font-weight: 800; border-radius: 12px; padding: 14px;
}
QPushButton#btnLogin:hover { background: #38bdf8; }
QPushButton#btnLogout {
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
        stop:0 #ef4444, stop:1 #dc2626);
    border-radius: 10px; margin: 4px 8px;
}
QPushButton#btnLogout:hover { background: #f87171; border-bottom: 3px solid #b91c1c; }
QPushButton#btnAdd {
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
        stop:0 #10b981, stop:1 #059669);
    border-radius: 8px;
}
QPushButton#btnAdd:hover { background: #34d399; border-bottom: 3px solid #047857; }
QPushButton#btnForgotPassword {
    background: transparent; color: %5; border: none;
    font-size: 12px; font-weight: 600; text-decoration: underline; padding: 4px;
}
QScrollBar:vertical { background: %7; width: 8px; border-radius: 4px; }
QScrollBar::handle:vertical { background: %15; border-radius: 4px; min-height: 30px; }
QScrollBar::handle:vertical:hover { background: %5; }
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
QScrollBar:horizontal { background: %7; height: 8px; border-radius: 4px; }
QScrollBar::handle:horizontal { background: %15; border-radius: 4px; min-width: 30px; }
QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0; }
QDialog { background: %6; }
QMessageBox { background: %6; }
QMessageBox QPushButton { min-width: 80px; padding: 8px 16px; border-radius: 8px; }
QToolTip { background: #1e293b; color: white; border: none; border-radius: 6px; padding: 6px 10px; font-size: 12px; }
    )")
    .arg(bg)                    // %1  bg
    .arg(fs)                    // %2  font-size
    .arg(text)                  // %3  text color
    .arg(pad)                   // %4  padding
    .arg(accent)                // %5  accent
    .arg(surface)               // %6  surface
    .arg(border)                // %7  border
    .arg(selBg)                 // %8  selection bg
    .arg(selText)               // %9  selection text
    .arg(night ? "#162032" : "#f8fafc")  // %10 alt row
    .arg(hdrBg)                 // %11 header bg
    .arg(inputBg)               // %12 input bg
    .arg(accentH)               // %13 accent hover
    .arg(pad + 2)               // %14 pressed padding
    .arg(night ? "#475569" : "#cbd5e1"); // %15 scrollbar handle
}
