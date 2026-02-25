#include "addedit_shipdialog.h"
#include <QFormLayout>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QMessageBox>
#include <QLabel>

AddEditShipDialog::AddEditShipDialog(QWidget *parent, bool isEdit)
    : QDialog(parent), isEditMode(isEdit)
{
    setWindowTitle(isEdit ? "Modifier Navire" : "Ajouter Navire");
    setModal(true);
    setMinimumWidth(400);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QLabel *titleLabel = new QLabel(isEdit ? "<b>Modifier les informations</b>" : "<b>Nouveau Navire</b>");
    titleLabel->setStyleSheet("font-size: 14px; color: #03224c;");
    mainLayout->addWidget(titleLabel);

    QFormLayout *formLayout = new QFormLayout();

    lineImmatriculation = new QLineEdit(this);
    lineImmatriculation->setReadOnly(isEdit);
    lineImmatriculation->setPlaceholderText("Immatriculation");
    formLayout->addRow("Immatriculation:", lineImmatriculation);

    lineNom = new QLineEdit(this);
    lineNom->setPlaceholderText("Nom du navire");
    formLayout->addRow("Nom:", lineNom);

    comboType = new QComboBox(this);
    comboType->addItems({"Pêche industrielle", "Pêche côtière", "Palangrier", "Senneur", "Chalutier"});
    comboType->setEditable(true);
    formLayout->addRow("Type:", comboType);

    lineCapacite = new QLineEdit(this);
    lineCapacite->setPlaceholderText("Capacité en tonnes");
    formLayout->addRow("Capacité (T):", lineCapacite);

    comboStatut = new QComboBox(this);
    comboStatut->addItems({"Disponible", "En mer", "Maintenance", "Interdit"});
    formLayout->addRow("Statut:", comboStatut);

    mainLayout->addLayout(formLayout);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    buttonBox->button(QDialogButtonBox::Ok)->setText("Save");
    buttonBox->button(QDialogButtonBox::Cancel)->setText("Cancel");
    
    connect(buttonBox, &QDialogButtonBox::accepted, this, &AddEditShipDialog::onAccepted);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    mainLayout->addWidget(buttonBox);
}

AddEditShipDialog::~AddEditShipDialog()
{
}

QString AddEditShipDialog::getImmatriculation() const
{
    return lineImmatriculation ? lineImmatriculation->text() : QString();
}

QString AddEditShipDialog::getNom() const
{
    return lineNom ? lineNom->text() : QString();
}

QString AddEditShipDialog::getType() const
{
    return comboType ? comboType->currentText() : QString();
}

QString AddEditShipDialog::getCapacite() const
{
    return lineCapacite ? lineCapacite->text() : QString();
}

QString AddEditShipDialog::getStatut() const
{
    return comboStatut ? comboStatut->currentText() : QString();
}

void AddEditShipDialog::setShipData(const QString &immat, const QString &nom, const QString &type,
                                    const QString &capacite, const QString &statut)
{
    if (lineImmatriculation) lineImmatriculation->setText(immat);
    if (lineNom) lineNom->setText(nom);
    if (comboType) comboType->setCurrentText(type);
    if (lineCapacite) lineCapacite->setText(capacite);
    if (comboStatut) comboStatut->setCurrentText(statut);
}

void AddEditShipDialog::onAccepted()
{
    if (lineImmatriculation->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer l'immatriculation");
        return;
    }

    if (lineNom->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer le nom du navire");
        return;
    }

    accept();
}
