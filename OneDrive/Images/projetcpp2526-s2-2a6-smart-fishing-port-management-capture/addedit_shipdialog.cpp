#include "addedit_shipdialog.h"
#include <QFormLayout>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QMessageBox>
#include <QLabel>
#include <QDoubleValidator>
#include <QIntValidator>

AddEditShipDialog::AddEditShipDialog(QWidget *parent, bool isEdit)
    : QDialog(parent), isEditMode(isEdit)
{
    setWindowTitle(isEdit ? "Modifier le navire" : "Nouveau navire");
    setModal(true);
    setMinimumSize(500, 550);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(32, 32, 32, 32);
    mainLayout->setSpacing(24);

    QLabel *titleLabel = new QLabel(isEdit ? "Informations du navire" : "Ajouter un nouveau navire");
    titleLabel->setObjectName("titleLabel");
    mainLayout->addWidget(titleLabel);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->setLabelAlignment(Qt::AlignLeft);
    formLayout->setSpacing(15);
    formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    lineImmatriculation = new QLineEdit(this);
    lineImmatriculation->setReadOnly(isEdit);
    lineImmatriculation->setPlaceholderText("Ex: PRT-12345");
    formLayout->addRow("Immatriculation", lineImmatriculation);

    lineNom = new QLineEdit(this);
    lineNom->setPlaceholderText("Nom du bâtiment");
    formLayout->addRow("Nom du navire", lineNom);

    comboType = new QComboBox(this);
    comboType->addItems({"Pêche industrielle", "Pêche côtière", "Palangrier", "Senneur", "Chalutier"});
    comboType->setEditable(true);
    formLayout->addRow("Type de pêche", comboType);

    lineCapacite = new QLineEdit(this);
    lineCapacite->setPlaceholderText("0");
    lineCapacite->setValidator(new QIntValidator(1, 10000, this));
    formLayout->addRow("Capacité (Tonnes)", lineCapacite);

    comboStatut = new QComboBox(this);
    comboStatut->addItems({"Disponible", "En mer", "Maintenance", "Interdit"});
    formLayout->addRow("Statut actuel", comboStatut);

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
    connect(btnOk, &QPushButton::clicked, this, &AddEditShipDialog::onAccepted);
    buttonLayout->addWidget(btnOk);

    mainLayout->addLayout(buttonLayout);
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

    if (lineCapacite->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer une capacité valide (nombre entier).");
        return;
    }

    accept();
}
