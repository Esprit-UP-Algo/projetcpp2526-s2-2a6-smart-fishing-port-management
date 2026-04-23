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
#include "connection.h"
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlError>

// Compatibility: provide the old 2-arg constructor that forwards to the new 4-arg one.
AddEditCaptureDialog::AddEditCaptureDialog(QWidget *parent, bool isEdit)
    : AddEditCaptureDialog(parent, isEdit, -1, false)
{
}

AddEditCaptureDialog::AddEditCaptureDialog(QWidget *parent, bool isEdit, bool allowExport)
    : AddEditCaptureDialog(parent, isEdit, -1, allowExport)
{
}

AddEditCaptureDialog::AddEditCaptureDialog(QWidget *parent, bool isEdit, int captureId, bool allowExport)
    : QDialog(parent), lineNavire(nullptr), dateEdit(nullptr), comboType(nullptr), spinQuantite(nullptr), isEditMode(isEdit), m_allowExport(allowExport), m_captureId(captureId)
{
    setWindowTitle(isEdit ? "Modifier la capture" : "Nouvelle capture");
        setModal(true);
        setMinimumSize(560, 420); // medium size

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(20);

    QLabel *titleLabel = new QLabel(isEdit ? "Modifier la capture" : "Nouvelle capture");
    titleLabel->setObjectName("titleLabel");
    titleLabel->setAlignment(Qt::AlignLeft);
    mainLayout->addWidget(titleLabel);

    // Form layout for better alignment
    QFormLayout *formLayout = new QFormLayout();
    formLayout->setLabelAlignment(Qt::AlignLeft);
    formLayout->setSpacing(15);
    formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    lineNavire = new QLineEdit(this);
    lineNavire->setPlaceholderText("Ex: Le Neptune");
    formLayout->addRow("Nom du Navire", lineNavire);

    dateEdit = new QDateEdit(this);
    dateEdit->setCalendarPopup(true);
    dateEdit->setDate(QDate::currentDate());
    dateEdit->setDisplayFormat("dd MMMM yyyy");
    formLayout->addRow("Date de Capture", dateEdit);

    comboType = new QComboBox(this);
    comboType->addItems({"Thon", "Sardine", "Maquereau", "Espadon", "Crevette", "Dorade", "Saumon", "Bar"});
    comboType->setEditable(true);
    formLayout->addRow("Type de Poisson", comboType);

    spinQuantite = new QSpinBox(this);
    spinQuantite->setRange(1, 1000000);
    spinQuantite->setSuffix(" kg");
    spinQuantite->setAlignment(Qt::AlignLeft);
    formLayout->addRow("Quantité", spinQuantite);

    mainLayout->addLayout(formLayout);

    // Spacer
    mainLayout->addStretch();

    // Buttons area
    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->setSpacing(12);

    if (m_allowExport) {
        QPushButton *exportBtn = new QPushButton("📄 Rapport PDF", this);
        exportBtn->setObjectName("btnSecondary");
        exportBtn->setCursor(Qt::PointingHandCursor);
        exportBtn->setMinimumHeight(40);
        connect(exportBtn, &QPushButton::clicked, this, &AddEditCaptureDialog::exportToPdf);
        bottomLayout->addWidget(exportBtn);
    }
    
    bottomLayout->addStretch();

    QPushButton *btnCancel = new QPushButton("✖️ Annuler", this);
    btnCancel->setObjectName("btnCancel");
    btnCancel->setCursor(Qt::PointingHandCursor);
    btnCancel->setMinimumHeight(40);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
    bottomLayout->addWidget(btnCancel);

    QPushButton *btnOk = new QPushButton(isEdit ? "🔄 Mettre à jour" : "💾 Enregistrer", this);
    btnOk->setObjectName("btnOk");
    btnOk->setCursor(Qt::PointingHandCursor);
    btnOk->setMinimumHeight(40);
    connect(btnOk, &QPushButton::clicked, this, &AddEditCaptureDialog::onAccepted);
    bottomLayout->addWidget(btnOk);

    mainLayout->addLayout(bottomLayout);

    lineNavire->setFocus();
}

QString AddEditCaptureDialog::getNavire() const { return lineNavire ? lineNavire->text() : QString(); }
QDate AddEditCaptureDialog::getDateCapture() const { return dateEdit ? dateEdit->date() : QDate::currentDate(); }
QString AddEditCaptureDialog::getTypePoisson() const { return comboType ? comboType->currentText() : QString(); }
int AddEditCaptureDialog::getQuantite() const { return spinQuantite ? spinQuantite->value() : 0; }

void AddEditCaptureDialog::setCaptureData(const QString &navire, const QDate &date, const QString &type, int quantite, const QString &/*extra*/)
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
    // Perform DB insert/update here
    Connection::instance()->createConnect();
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery q(db);

    if (isEditMode && m_captureId >= 0) {
        q.prepare("UPDATE captures SET Navire=?, Date_Capture=?, Type_Poisson=?, Quantite=? WHERE ID_Capture=?");
        q.addBindValue(lineNavire->text().trimmed());
        q.addBindValue(dateEdit->date().toString(Qt::ISODate));
        q.addBindValue(comboType->currentText().trimmed());
        q.addBindValue(spinQuantite->value());
        q.addBindValue(m_captureId);
        if (!q.exec()) {
            QMessageBox::critical(this, "Erreur", "Impossible de modifier la capture: " + q.lastError().text());
            return;
        }
        QMessageBox::information(this, "Succès", "Capture modifiée avec succès");
        accept();
        return;
    }

    // Insert new capture
    q.prepare("INSERT INTO captures (Navire, Date_Capture, Type_Poisson, Quantite) VALUES (?, ?, ?, ?)");
    q.addBindValue(lineNavire->text().trimmed());
    q.addBindValue(dateEdit->date().toString(Qt::ISODate));
    q.addBindValue(comboType->currentText().trimmed());
    q.addBindValue(spinQuantite->value());

    if (!q.exec()) {
        QMessageBox::critical(this, "Erreur", "Impossible d'ajouter la capture: " + q.lastError().text());
        return;
    }

    QMessageBox::information(this, "Succès", "Capture ajoutée avec succès");
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
