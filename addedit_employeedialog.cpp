#include "addedit_employeedialog.h"
#include <QFormLayout>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QMessageBox>
#include <QLabel>

AddEditEmployeeDialog::AddEditEmployeeDialog(QWidget *parent, bool isEdit)
    : QDialog(parent), lineId(nullptr), lineNom(nullptr), comboPoste(nullptr),
      lineEmail(nullptr), lineTelephone(nullptr), lineSalaire(nullptr), isEditMode(isEdit)
{
    this->setWindowTitle(isEdit ? "Modifier l'employé" : "Nouvel employé");
    this->setModal(true);
    this->setMinimumSize(500, 600);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(32, 32, 32, 32);
    mainLayout->setSpacing(24);

    QLabel *titleLabel = new QLabel(isEdit ? "Informations de l'employé" : "Ajouter un nouvel employé");
    titleLabel->setObjectName("titleLabel");
    mainLayout->addWidget(titleLabel);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->setLabelAlignment(Qt::AlignLeft);
    formLayout->setSpacing(15);
    formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    lineId = new QLineEdit(this);
    lineId->setReadOnly(isEdit);
    lineId->setPlaceholderText("ID unique...");
    formLayout->addRow("ID Employé", lineId);

    lineNom = new QLineEdit(this);
    lineNom->setPlaceholderText("Nom et Prénom");
    formLayout->addRow("Nom complet", lineNom);

    comboPoste = new QComboBox(this);
    comboPoste->addItems({"Pêcheur", "Matelot", "Docker", "Chef de quai", "Superviseur"});
    formLayout->addRow("Poste / Fonction", comboPoste);

    lineEmail = new QLineEdit(this);
    lineEmail->setPlaceholderText("exemple@port.com");
    formLayout->addRow("Adresse Email", lineEmail);

    lineTelephone = new QLineEdit(this);
    lineTelephone->setPlaceholderText("+216 -- --- ---");
    formLayout->addRow("Téléphone", lineTelephone);

    lineSalaire = new QLineEdit(this);
    lineSalaire->setPlaceholderText("0.00");
    formLayout->addRow("Salaire (TND)", lineSalaire);

    mainLayout->addLayout(formLayout);
    mainLayout->addStretch();

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(12);
    buttonLayout->addStretch();

    QPushButton *btnCancel = new QPushButton("Annuler", this);
    btnCancel->setObjectName("btnCancel");
    btnCancel->setCursor(Qt::PointingHandCursor);
    btnCancel->setMinimumHeight(45);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
    buttonLayout->addWidget(btnCancel);

    QPushButton *btnOk = new QPushButton(isEdit ? "Mettre à jour" : "Enregistrer", this);
    btnOk->setObjectName("btnOk");
    btnOk->setCursor(Qt::PointingHandCursor);
    btnOk->setMinimumHeight(45);
    connect(btnOk, &QPushButton::clicked, this, &AddEditEmployeeDialog::onAccepted);
    buttonLayout->addWidget(btnOk);

    mainLayout->addLayout(buttonLayout);
}

AddEditEmployeeDialog::~AddEditEmployeeDialog()
{
}

QString AddEditEmployeeDialog::getId() const
{
    return lineId ? lineId->text() : QString();
}

QString AddEditEmployeeDialog::getNom() const
{
    return lineNom ? lineNom->text() : QString();
}

QString AddEditEmployeeDialog::getPoste() const
{
    return comboPoste ? comboPoste->currentText() : QString();
}

QString AddEditEmployeeDialog::getEmail() const
{
    return lineEmail ? lineEmail->text() : QString();
}

QString AddEditEmployeeDialog::getTelephone() const
{
    return lineTelephone ? lineTelephone->text() : QString();
}

double AddEditEmployeeDialog::getSalaire() const
{
    QString text = lineSalaire ? lineSalaire->text() : "0";
    text.replace(" TND", "");
    return text.toDouble();
}

void AddEditEmployeeDialog::setEmployeeData(const QString &id, const QString &nom, const QString &poste,
                                            const QString &email, const QString &telephone, double salary)
{
    if (lineId) lineId->setText(id);
    if (lineNom) lineNom->setText(nom);
    if (comboPoste) comboPoste->setCurrentText(poste);
    if (lineEmail) lineEmail->setText(email);
    if (lineTelephone) lineTelephone->setText(telephone);
    if (lineSalaire) lineSalaire->setText(QString::number(salary, 'f', 0));
}

void AddEditEmployeeDialog::onAccepted()
{
    if (lineId->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter Employee ID");
        lineId->setFocus();
        return;
    }

    if (lineNom->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter Full Name");
        lineNom->setFocus();
        return;
    }

    if (lineEmail->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter Email");
        lineEmail->setFocus();
        return;
    }

    if (lineTelephone->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter Phone number");
        lineTelephone->setFocus();
        return;
    }

    if (lineSalaire->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter Salary");
        lineSalaire->setFocus();
        return;
    }

    accept();
}
