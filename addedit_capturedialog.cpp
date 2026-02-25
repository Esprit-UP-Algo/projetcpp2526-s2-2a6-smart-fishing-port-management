#include "addedit_capturedialog.h"
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QMessageBox>
#include <QLabel>
#include <QFileDialog>
#include <QPdfWriter>
#include <QPainter>
#include <QLineEdit>
#include <QCalendarWidget>
#include <QListView>

// Compatibility: provide the old 2-arg constructor that forwards to the new 3-arg one.
AddEditCaptureDialog::AddEditCaptureDialog(QWidget *parent, bool isEdit)
    : AddEditCaptureDialog(parent, isEdit, false)
{
}

AddEditCaptureDialog::AddEditCaptureDialog(QWidget *parent, bool isEdit, bool allowExport)
    : QDialog(parent), lineNavire(nullptr), dateEdit(nullptr), comboType(nullptr), spinQuantite(nullptr), isEditMode(isEdit), m_allowExport(allowExport)
{
    setWindowTitle(isEdit ? "Modifier la capture" : "Nouvelle capture");
        setModal(true);
        setMinimumSize(560, 420); // medium size

        // Centralized, polished field styling used across capture form widgets
        const QString commonFieldStyle = R"STYLE(
QLineEdit, QComboBox, QDateEdit, QSpinBox {
    background: #ffffff;
    color: #07324a;
    border: 1px solid #d6e0ea;
    padding: 10px;
    border-radius: 10px;
}
QLineEdit:focus, QComboBox:focus, QDateEdit:focus, QSpinBox:focus {
    border: 1px solid #1e88e5;
}
QComboBox::drop-down { subcontrol-origin: padding; subcontrol-position: top right; width: 30px; }
QListView { background: #ffffff; color: #07324a; selection-background-color: #cfe8ff; selection-color: #07324a; }
QListView::item { padding: 6px 10px; }
QSpinBox::up-button, QSpinBox::down-button { width: 18px; }
)STYLE";

        setStyleSheet(QString("QDialog { background: #ffffff; }") + commonFieldStyle);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QLabel *titleLabel = new QLabel(isEdit ? "<b>Modifier la capture</b>" : "<b>Nouvelle capture</b>");
    titleLabel->setStyleSheet("font-size: 16px; color: #03224c; margin-bottom:10px;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    // Container with soft background and padding
    QWidget *container = new QWidget(this);
    container->setStyleSheet("background: #f7f9fc; border-radius: 10px; padding: 16px;");
    QVBoxLayout *containerLayout = new QVBoxLayout(container);
    containerLayout->setContentsMargins(8, 8, 8, 8);
    containerLayout->setSpacing(12);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->setLabelAlignment(Qt::AlignRight);
    formLayout->setFormAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    formLayout->setHorizontalSpacing(18);
    formLayout->setVerticalSpacing(10);

    lineNavire = new QLineEdit(this);
    lineNavire->setPlaceholderText("Ex: Le Neptune");
    lineNavire->setMinimumWidth(320);
    lineNavire->setToolTip("Nom du navire qui a effectué la capture");
    // Uses centralized stylesheet from the dialog for consistent appearance
    QPalette pLine = lineNavire->palette();
    pLine.setColor(QPalette::Text, QColor("#07324a"));
    pLine.setColor(QPalette::PlaceholderText, QColor("#9aa6b2"));
    lineNavire->setPalette(pLine);
    formLayout->addRow("Navire:", lineNavire);

    dateEdit = new QDateEdit(this);
    dateEdit->setCalendarPopup(true);
    dateEdit->setDate(QDate::currentDate());
    dateEdit->setDisplayFormat("dd/MM/yyyy");
    dateEdit->setToolTip("Date de la capture");
    // Widget uses centralized styling; keep calendar popup customizations below
    QPalette pDate = dateEdit->palette();
    pDate.setColor(QPalette::Text, QColor("#07324a"));
    dateEdit->setPalette(pDate);
    // Ensure internal line edit (used to show the date) has visible text colors
    if (QLineEdit *dline = dateEdit->findChild<QLineEdit*>()) {
        dline->setStyleSheet("QLineEdit { background: transparent; color: #07324a; }");
        QPalette pd = dline->palette();
        pd.setColor(QPalette::Text, QColor("#07324a"));
        pd.setColor(QPalette::PlaceholderText, QColor("#9aa6b2"));
        dline->setPalette(pd);
    }
    // Strong calendar styling for the popup
    QCalendarWidget *cal = new QCalendarWidget(this);
    cal->setFirstDayOfWeek(Qt::Monday);
    cal->setStyleSheet(R"(
QCalendarWidget { background: #ffffff; color: #07324a; border: 1px solid #d6e0ea; }
QCalendarWidget QToolButton { color: #07324a; background: transparent; }
QCalendarWidget QAbstractItemView { selection-background-color: #cfe8ff; selection-color: #07324a; background: #ffffff; color: #07324a; }
QCalendarWidget QWidget { color: #07324a; }
)" );
    cal->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    dateEdit->setCalendarWidget(cal);
    formLayout->addRow("Date:", dateEdit);

    comboType = new QComboBox(this);
    comboType->addItems({"Thon", "Sardine", "Maquereau", "Espadon", "Crevette", "Dorade", "Saumon", "Bar"});
    comboType->setEditable(true);
    comboType->setInsertPolicy(QComboBox::NoInsert);
    comboType->setToolTip("Type de poisson (sélectionner ou saisir)");
    // Appearance is handled by the dialog's centralized stylesheet
    if (comboType->isEditable() && comboType->lineEdit()) {
        comboType->lineEdit()->setStyleSheet("QLineEdit { background: transparent; color: #07324a; }");
        QPalette pCombo = comboType->lineEdit()->palette();
        pCombo.setColor(QPalette::Text, QColor("#07324a"));
        pCombo.setColor(QPalette::PlaceholderText, QColor("#9aa6b2"));
        comboType->lineEdit()->setPalette(pCombo);
    }
    // Ensure dropdown list items are visible
    if (comboType->view()) {
        QListView *lv = qobject_cast<QListView*>(comboType->view());
        if (lv) {
            lv->setStyleSheet(R"(
QListView { background: #ffffff; color: #07324a; selection-background-color: #cfe8ff; selection-color: #07324a; }
QListView::item { padding: 6px 10px; }
)");
            QPalette pv = lv->palette();
            pv.setColor(QPalette::Text, QColor("#07324a"));
            pv.setColor(QPalette::Base, QColor("#ffffff"));
            pv.setColor(QPalette::Highlight, QColor("#cfe8ff"));
            pv.setColor(QPalette::HighlightedText, QColor("#07324a"));
            lv->setPalette(pv);
        } else {
            comboType->view()->setStyleSheet("QListView { background: #ffffff; color: #07324a; selection-background-color: #cfe8ff; selection-color: #07324a; }");
        }
    }
    formLayout->addRow("Type:", comboType);

    spinQuantite = new QSpinBox(this);
    spinQuantite->setRange(1, 1000000);
    spinQuantite->setSuffix(" kg");
    spinQuantite->setToolTip("Quantité en kilogrammes");
    spinQuantite->setAlignment(Qt::AlignCenter);
    // Uses centralized styling from the dialog for consistent appearance
    QPalette pSpin = spinQuantite->palette();
    pSpin.setColor(QPalette::Text, QColor("#07324a"));
    spinQuantite->setPalette(pSpin);
    // Ensure spinbox internal lineedit shows text and suffix clearly
    #if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    if (QLineEdit *sline = spinQuantite->findChild<QLineEdit*>()) {
        sline->setStyleSheet("QLineEdit { background: transparent; color: #07324a; }");
        QPalette ps = sline->palette();
        ps.setColor(QPalette::Text, QColor("#07324a"));
        sline->setPalette(ps);
    }
    #endif
    // Up/down button width and arrow colors are set through the dialog stylesheet
    formLayout->addRow("Quantité:", spinQuantite);

    containerLayout->addLayout(formLayout);
    mainLayout->addWidget(container);

    // Buttons area with optional Export PDF
    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->setContentsMargins(4, 8, 4, 4);
    bottomLayout->addStretch();

    if (m_allowExport) {
        QPushButton *exportBtn = new QPushButton("📄 Exporter PDF", this);
        exportBtn->setStyleSheet("QPushButton{ background:#1e40af; color:white; padding:8px 12px; border-radius:6px; font-weight:600; }");
        exportBtn->setMinimumWidth(120);
        connect(exportBtn, &QPushButton::clicked, this, &AddEditCaptureDialog::exportToPdf);
        bottomLayout->addWidget(exportBtn);
    }

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    buttonBox->button(QDialogButtonBox::Ok)->setText("Enregistrer");
    buttonBox->button(QDialogButtonBox::Cancel)->setText("Annuler");
    buttonBox->button(QDialogButtonBox::Ok)->setStyleSheet("QPushButton{ background:#4CAF50; color:white; padding:8px 12px; border-radius:6px; font-weight:600; }");
    buttonBox->button(QDialogButtonBox::Cancel)->setStyleSheet("QPushButton{ background:#9E9E9E; color:white; padding:8px 12px; border-radius:6px; }");
    connect(buttonBox, &QDialogButtonBox::accepted, this, &AddEditCaptureDialog::onAccepted);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    bottomLayout->addWidget(buttonBox);

    mainLayout->addLayout(bottomLayout);

    // UX fine-tuning
    lineNavire->setFocus();
    setTabOrder(lineNavire, dateEdit);
    setTabOrder(dateEdit, comboType);
    setTabOrder(comboType, spinQuantite);
}

QString AddEditCaptureDialog::getNavire() const { return lineNavire ? lineNavire->text() : QString(); }
QDate AddEditCaptureDialog::getDateCapture() const { return dateEdit ? dateEdit->date() : QDate::currentDate(); }
QString AddEditCaptureDialog::getTypePoisson() const { return comboType ? comboType->currentText() : QString(); }
int AddEditCaptureDialog::getQuantite() const { return spinQuantite ? spinQuantite->value() : 0; }

void AddEditCaptureDialog::setCaptureData(const QString &navire, const QDate &date, const QString &type, int quantite)
{
    if (lineNavire) lineNavire->setText(navire);
    if (dateEdit) dateEdit->setDate(date);
    if (comboType) comboType->setCurrentText(type);
    if (spinQuantite) spinQuantite->setValue(quantite);
}

void AddEditCaptureDialog::onAccepted()
{
    if (!lineNavire || lineNavire->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Champ requis", "Veuillez saisir le nom du navire.");
        return;
    }
    if (!comboType || comboType->currentText().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Champ requis", "Veuillez saisir le type de poisson.");
        return;
    }
    accept();
}

void AddEditCaptureDialog::exportToPdf()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Enregistrer PDF", "", "PDF Files (*.pdf)");
    if (fileName.isEmpty()) return;
    if (!fileName.endsWith(".pdf")) fileName += ".pdf";

    QPdfWriter writer(fileName);
    writer.setPageSize(QPageSize::A4);
    writer.setResolution(300);

    QPainter painter;
    if (!painter.begin(&writer)) {
        QMessageBox::critical(this, "Erreur", "Impossible de créer le fichier PDF.");
        return;
    }

    int margin = 60;
    int y = 80;
    QFont titleF("Arial", 16, QFont::Bold);
    QFont normalF("Arial", 11);

    painter.setFont(titleF);
    painter.drawText(margin, y, writer.width() - 2*margin, 30, Qt::AlignCenter, "Détails de la capture");
    y += 50;

    painter.setFont(normalF);
    painter.drawText(margin, y, writer.width() - 2*margin, 25, Qt::AlignLeft, QString("Navire: %1").arg(getNavire()));
    y += 30;
    painter.drawText(margin, y, writer.width() - 2*margin, 25, Qt::AlignLeft, QString("Date: %1").arg(getDateCapture().toString("dd/MM/yyyy")));
    y += 30;
    painter.drawText(margin, y, writer.width() - 2*margin, 25, Qt::AlignLeft, QString("Type: %1").arg(getTypePoisson()));
    y += 30;
    painter.drawText(margin, y, writer.width() - 2*margin, 25, Qt::AlignLeft, QString("Quantité: %1 kg").arg(getQuantite()));

    painter.end();
    QMessageBox::information(this, "✓ Succès", "PDF exporté: " + fileName);
}
