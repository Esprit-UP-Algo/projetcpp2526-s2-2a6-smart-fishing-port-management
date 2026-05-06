#pragma once
#include <QString>

// ─── Professional Maritime Color Palette ────────────────────────────────────
namespace AppTheme {

// Core colors
constexpr auto BG        = "#f0f4f8";   // page background
constexpr auto SURFACE   = "#ffffff";   // card / panel
constexpr auto BORDER    = "#e2e8f0";   // subtle border
constexpr auto TEXT      = "#0f172a";   // primary text
constexpr auto MUTED     = "#64748b";   // secondary text
constexpr auto NAVY      = "#0a1628";   // header / dark accent
constexpr auto BLUE      = "#1d4ed8";   // primary action
constexpr auto BLUE_LIGHT= "#3b82f6";   // hover / secondary action
constexpr auto GREEN     = "#059669";   // success / à quai
constexpr auto AMBER     = "#d97706";   // warning / en mer
constexpr auto RED       = "#dc2626";   // danger / interdit
constexpr auto PURPLE    = "#7c3aed";   // chatbot / special

// ─── Shared widget stylesheet ────────────────────────────────────────────────
inline QString widgetStyle() {
    return R"(
        QWidget {
            background-color: #f0f4f8;
            font-family: 'Segoe UI', Arial, sans-serif;
            font-size: 13px;
            color: #0f172a;
        }
        QGroupBox {
            background: #ffffff;
            border: 1px solid #e2e8f0;
            border-radius: 10px;
            margin-top: 14px;
            padding: 12px 10px 10px 10px;
            font-weight: 700;
            color: #0a1628;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 12px;
            padding: 0 6px;
            color: #1d4ed8;
            font-size: 13px;
        }
        QLineEdit, QSpinBox, QDoubleSpinBox, QComboBox, QDateEdit {
            background: #ffffff;
            border: 1px solid #cbd5e1;
            border-radius: 6px;
            padding: 5px 8px;
            min-height: 32px;
            color: #0f172a;
        }
        QLineEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus,
        QComboBox:focus, QDateEdit:focus {
            border: 2px solid #1d4ed8;
        }
        QPushButton {
            background-color: #1d4ed8;
            color: #ffffff;
            border: none;
            border-radius: 7px;
            padding: 7px 18px;
            font-weight: 700;
            min-height: 34px;
        }
        QPushButton:hover  { background-color: #2563eb; }
        QPushButton:pressed{ background-color: #1e40af; padding-top: 9px; }
        QPushButton:disabled { background-color: #94a3b8; }
        QTableWidget, QTableView {
            background: #ffffff;
            gridline-color: #e2e8f0;
            border: 1px solid #e2e8f0;
            border-radius: 8px;
            selection-background-color: #dbeafe;
            selection-color: #1e3a8a;
        }
        QHeaderView::section {
            background-color: #0a1628;
            color: #ffffff;
            padding: 8px 6px;
            border: none;
            font-weight: 700;
            font-size: 12px;
        }
        QTableWidget::item, QTableView::item {
            padding: 6px;
            border-bottom: 1px solid #f1f5f9;
        }
        QTableWidget::item:hover, QTableView::item:hover {
            background-color: #f1f5f9;
        }
        QScrollBar:vertical {
            background: #f1f5f9; width: 8px; border-radius: 4px;
        }
        QScrollBar::handle:vertical {
            background: #94a3b8; border-radius: 4px; min-height: 30px;
        }
        QLabel { color: #0f172a; }
    )";
}

// ─── KPI card style ──────────────────────────────────────────────────────────
inline QString kpiCard(const QString &accentColor) {
    return QString(
        "background:#ffffff;"
        "border-radius:10px;"
        "border:1px solid #e2e8f0;"
        "border-left:4px solid %1;"
        "padding:10px;"
    ).arg(accentColor);
}

// ─── Action button variants ──────────────────────────────────────────────────
inline QString btnPrimary() {
    return "QPushButton{background:#1d4ed8;color:white;border:none;border-radius:7px;"
           "padding:7px 16px;font-weight:700;min-height:34px;}"
           "QPushButton:hover{background:#2563eb;}"
           "QPushButton:pressed{background:#1e40af;}";
}
inline QString btnDanger() {
    return "QPushButton{background:#dc2626;color:white;border:none;border-radius:7px;"
           "padding:7px 16px;font-weight:700;min-height:34px;}"
           "QPushButton:hover{background:#ef4444;}"
           "QPushButton:pressed{background:#b91c1c;}";
}
inline QString btnSuccess() {
    return "QPushButton{background:#059669;color:white;border:none;border-radius:7px;"
           "padding:7px 16px;font-weight:700;min-height:34px;}"
           "QPushButton:hover{background:#10b981;}"
           "QPushButton:pressed{background:#047857;}";
}
inline QString btnPurple() {
    return "QPushButton{background:#7c3aed;color:white;border:none;border-radius:7px;"
           "padding:7px 16px;font-weight:700;min-height:34px;}"
           "QPushButton:hover{background:#8b5cf6;}"
           "QPushButton:pressed{background:#6d28d9;}";
}
inline QString btnSlate() {
    return "QPushButton{background:#475569;color:white;border:none;border-radius:7px;"
           "padding:7px 16px;font-weight:700;min-height:34px;}"
           "QPushButton:hover{background:#64748b;}"
           "QPushButton:pressed{background:#334155;}";
}

} // namespace AppTheme
