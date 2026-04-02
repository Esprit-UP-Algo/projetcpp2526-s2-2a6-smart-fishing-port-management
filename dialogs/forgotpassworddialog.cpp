#include "forgotpassworddialog.h"
#include <QFormLayout>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QRegularExpression>
#include <QMessageBox>

ForgotPasswordDialog::ForgotPasswordDialog(QWidget *parent)
    : QDialog(parent), contentWidget(nullptr), lineName(nullptr), lineId(nullptr), linePoste(nullptr), lineEmail(nullptr), linePhone(nullptr)
{
    this->setWindowTitle("Mot de passe oublié");

    QFormLayout *layout = new QFormLayout(this);

    lineName = new QLineEdit(this);
    lineId = new QLineEdit(this);
    linePoste = new QLineEdit(this);
    lineEmail = new QLineEdit(this);
    linePhone = new QLineEdit(this);

    layout->addRow("Nom:", lineName);
    layout->addRow("ID:", lineId);
    layout->addRow("Poste:", linePoste);
    layout->addRow("Email:", lineEmail);
    layout->addRow("Téléphone:", linePhone);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    layout->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::accepted, this, &ForgotPasswordDialog::onAccepted);
    connect(buttons, &QDialogButtonBox::rejected, this, &ForgotPasswordDialog::reject);
}

ForgotPasswordDialog::~ForgotPasswordDialog()
{
}

QString ForgotPasswordDialog::getName() const { return lineName ? lineName->text() : QString(); }
QString ForgotPasswordDialog::getId() const { return lineId ? lineId->text() : QString(); }
QString ForgotPasswordDialog::getPoste() const { return linePoste ? linePoste->text() : QString(); }
QString ForgotPasswordDialog::getEmail() const { return lineEmail ? lineEmail->text() : QString(); }
QString ForgotPasswordDialog::getTelephone() const { return linePhone ? linePhone->text() : QString(); }

void ForgotPasswordDialog::onAccepted()
{
    if (lineName->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Validation", "Veuillez entrer votre nom.");
        lineName->setFocus();
        return;
    }
    if (lineEmail->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Validation", "Veuillez entrer votre email.");
        lineEmail->setFocus();
        return;
    }
    QRegularExpression emailRx("^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}$");
    if (!emailRx.match(lineEmail->text().trimmed()).hasMatch()) {
        QMessageBox::warning(this, "Validation", "Veuillez entrer un email valide.");
        lineEmail->setFocus();
        return;
    }

    accept();
}
