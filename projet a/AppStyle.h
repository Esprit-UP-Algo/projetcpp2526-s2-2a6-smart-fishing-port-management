#pragma once
#include <QString>

inline QString appStyleSheet() {
    return R"(

/* ═══════════════════════════════════════════
   GLOBAL
═══════════════════════════════════════════ */
QMainWindow, QWidget {
    background-color: #f0f4f8;
    font-family: "Segoe UI", Arial, sans-serif;
    font-size: 13px;
    color: #1e293b;
}

/* ═══════════════════════════════════════════
   LOGIN PAGE
═══════════════════════════════════════════ */
QWidget#loginPage {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
        stop:0 #0a1628, stop:0.5 #0d2d5e, stop:1 #1a4a8a);
}

QWidget#loginCard {
    background: rgba(255,255,255,0.97);
    border-radius: 20px;
    border: 1px solid rgba(255,255,255,0.3);
}

QLabel#loginTitle {
    font-size: 32px;
    font-weight: 800;
    color: #ffffff;
    letter-spacing: 1px;
}

QLabel#loginSubtitle {
    font-size: 14px;
    color: #64748b;
    font-weight: 400;
}

QLabel#loginEmailLabel, QLabel#loginPasswordLabel {
    font-size: 12px;
    font-weight: 700;
    color: #374151;
    letter-spacing: 0.5px;
}

/* ═══════════════════════════════════════════
   INPUT FIELDS
═══════════════════════════════════════════ */
QLineEdit {
    background-color: #f8fafc;
    border: 2px solid #e2e8f0;
    border-radius: 10px;
    padding: 10px 14px;
    font-size: 13px;
    color: #ffffff;
    selection-background-color: #3b82f6;
}
QLineEdit:focus {
    border: 2px solid #3b82f6;
    background-color: #eff6ff;
}
QLineEdit:hover {
    border: 2px solid #93c5fd;
}

/* ═══════════════════════════════════════════
   BUTTONS — BASE
═══════════════════════════════════════════ */
QPushButton {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #2563eb, stop:1 #1d4ed8);
    color: white;
    border: none;
    border-radius: 10px;
    padding: 10px 18px;
    font-weight: 700;
    font-size: 13px;
    letter-spacing: 0.3px;
}
QPushButton:hover {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #3b82f6, stop:1 #2563eb);
    border-bottom: 3px solid #1e40af;
}
QPushButton:pressed {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #1d4ed8, stop:1 #1e3a8a);
    border-bottom: 1px solid #1e3a8a;
    padding-top: 12px;
}
QPushButton:disabled {
    background: #cbd5e1;
    color: #94a3b8;
}

/* Login button */
QPushButton#btnLogin {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #0ea5e9, stop:1 #0284c7);
    font-size: 15px;
    font-weight: 800;
    border-radius: 12px;
    padding: 14px;
    letter-spacing: 1px;
}
QPushButton#btnLogin:hover {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #38bdf8, stop:1 #0ea5e9);
    border-bottom: 3px solid #0369a1;
}
QPushButton#btnLogin:pressed {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #0284c7, stop:1 #0369a1);
}

/* Forgot password */
QPushButton#btnForgotPassword {
    background: transparent;
    color: #3b82f6;
    border: none;
    font-size: 12px;
    font-weight: 600;
    text-decoration: underline;
    padding: 4px;
}
QPushButton#btnForgotPassword:hover {
    color: #1d4ed8;
}

/* Logout */
QPushButton#btnLogout {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #ef4444, stop:1 #dc2626);
    border-radius: 10px;
    margin: 8px;
}
QPushButton#btnLogout:hover {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #f87171, stop:1 #ef4444);
    border-bottom: 3px solid #b91c1c;
}

/* Add button */
QPushButton#btnAdd {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #10b981, stop:1 #059669);
    border-radius: 10px;
}
QPushButton#btnAdd:hover {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #34d399, stop:1 #10b981);
    border-bottom: 3px solid #047857;
}

/* Cancel */
QPushButton#btnCancel {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #6b7280, stop:1 #4b5563);
}
QPushButton#btnCancel:hover {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #9ca3af, stop:1 #6b7280);
}

/* ═══════════════════════════════════════════
   SIDEBAR
═══════════════════════════════════════════ */
QWidget#sidebar {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #0a1628, stop:0.6 #0d2d5e, stop:1 #0a1628);
    border-right: 1px solid #1e3a8a;
}

QLabel#labelLogo {
    font-size: 20px;
    font-weight: 900;
    color: white;
    padding: 24px 16px 20px 16px;
    background: rgba(255,255,255,0.05);
    border-bottom: 2px solid #1e40af;
    letter-spacing: 2px;
}

QWidget#sidebar QPushButton {
    background: transparent;
    color: #94a3b8;
    border: none;
    border-radius: 10px;
    text-align: left;
    padding: 12px 16px;
    font-size: 13px;
    font-weight: 600;
    margin: 2px 8px;
    letter-spacing: 0.2px;
}
QWidget#sidebar QPushButton:hover {
    background: rgba(59,130,246,0.15);
    color: #e2e8f0;
    border-left: 3px solid #3b82f6;
    padding-left: 13px;
}
QWidget#sidebar QPushButton:pressed {
    background: rgba(59,130,246,0.25);
    color: white;
}
QWidget#sidebar QPushButton:checked {
    background: rgba(59,130,246,0.2);
    color: white;
    border-left: 3px solid #60a5fa;
}

/* ═══════════════════════════════════════════
   HEADER / TITLE
═══════════════════════════════════════════ */
QLabel#labelTitle {
    font-size: 26px;
    font-weight: 800;
    color: #ffffff;
    padding: 8px 0px;
    letter-spacing: 0.5px;
}

/* ═══════════════════════════════════════════
   TABLE
═══════════════════════════════════════════ */
QTableWidget {
    background: white;
    border: 1px solid #e2e8f0;
    border-radius: 12px;
    gridline-color: #f1f5f9;
    selection-background-color: #dbeafe;
    selection-color: #ffffff;
    alternate-background-color: #f8fafc;
}
QTableWidget::item {
    padding: 8px 10px;
    color: #ffffff;
    border-bottom: 1px solid #f1f5f9;
}
QTableWidget::item:hover {
    background-color: #eff6ff;
}
QTableWidget::item:selected {
    background-color: #dbeafe;
    color: #ffffff;
}
QHeaderView::section {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #1e3a8a, stop:1 #1d4ed8);
    color: white;
    padding: 10px 8px;
    border: none;
    font-weight: 700;
    font-size: 12px;
    letter-spacing: 0.5px;
}
QHeaderView::section:first {
    border-top-left-radius: 10px;
}
QHeaderView::section:last {
    border-top-right-radius: 10px;
}

/* ═══════════════════════════════════════════
   GROUPBOX / CARDS
═══════════════════════════════════════════ */
QGroupBox {
    background: white;
    border: 1px solid #e2e8f0;
    border-radius: 14px;
    margin-top: 18px;
    padding: 16px 12px 12px 12px;
    font-weight: 700;
    color: #ffffff;
}
QGroupBox::title {
    subcontrol-origin: margin;
    left: 14px;
    padding: 0 8px;
    color: #1d4ed8;
    font-size: 13px;
    font-weight: 800;
    background: transparent;
}

/* ═══════════════════════════════════════════
   SCROLLBAR
═══════════════════════════════════════════ */
QScrollBar:vertical {
    background: #f1f5f9;
    width: 8px;
    border-radius: 4px;
}
QScrollBar::handle:vertical {
    background: #94a3b8;
    border-radius: 4px;
    min-height: 30px;
}
QScrollBar::handle:vertical:hover {
    background: #3b82f6;
}
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }

QScrollBar:horizontal {
    background: #f1f5f9;
    height: 8px;
    border-radius: 4px;
}
QScrollBar::handle:horizontal {
    background: #94a3b8;
    border-radius: 4px;
    min-width: 30px;
}
QScrollBar::handle:horizontal:hover { background: #3b82f6; }
QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0; }

/* ═══════════════════════════════════════════
   COMBOBOX
═══════════════════════════════════════════ */
QComboBox {
    background: #f8fafc;
    border: 2px solid #e2e8f0;
    border-radius: 10px;
    padding: 8px 12px;
    font-size: 13px;
    color: #ffffff;
}
QComboBox:focus { border: 2px solid #3b82f6; }
QComboBox::drop-down { border: none; width: 24px; }
QComboBox QAbstractItemView {
    background: #0d2d5e;
    border: 1px solid #e2e8f0;
    border-radius: 8px;
    selection-background-color: #dbeafe;
    color: #ffffff;
}

/* ═══════════════════════════════════════════
   SPINBOX
═══════════════════════════════════════════ */
QSpinBox, QDoubleSpinBox {
    background: #f8fafc;
    border: 2px solid #e2e8f0;
    border-radius: 10px;
    padding: 8px 12px;
    font-size: 13px;
    color: #ffffff;
}
QSpinBox:focus, QDoubleSpinBox:focus { border: 2px solid #3b82f6; }

/* ═══════════════════════════════════════════
   DIALOG
═══════════════════════════════════════════ */
QDialog {
    background: #f8fafc;
    border-radius: 16px;
}

/* ═══════════════════════════════════════════
   MESSAGEBOX
═══════════════════════════════════════════ */
QMessageBox {
    background: #0d2d5e;
}
QMessageBox QPushButton {
    min-width: 80px;
    padding: 8px 16px;
    border-radius: 8px;
}

/* ═══════════════════════════════════════════
   TOOLTIP
═══════════════════════════════════════════ */
QToolTip {
    background: #1e293b;
    color: white;
    border: none;
    border-radius: 6px;
    padding: 6px 10px;
    font-size: 12px;
}

/* ═══════════════════════════════════════════
   FRAME / CARDS
═══════════════════════════════════════════ */
QFrame#pageFrame {
    background: white;
    border-radius: 14px;
    border: 1px solid #e2e8f0;
}

/* ═══════════════════════════════════════════
   LABEL STATS
═══════════════════════════════════════════ */
QLabel#statCard {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
        stop:0 #1d4ed8, stop:1 #2563eb);
    color: white;
    border-radius: 12px;
    padding: 12px;
    font-weight: 700;
}

)";
}
