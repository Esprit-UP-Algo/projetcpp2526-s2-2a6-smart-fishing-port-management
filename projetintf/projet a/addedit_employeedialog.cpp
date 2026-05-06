#include "addedit_employeedialog.h"
#include <QFormLayout>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QMessageBox>
#include <QLabel>
#include <QRegularExpression>

AddEditEmployeeDialog::AddEditEmployeeDialog(QWidget *parent, bool isEdit)
    : QDialog(parent), lineId(nullptr), lineNom(nullptr), comboPoste(nullptr),
      lineEmail(nullptr), lineTelephone(nullptr), lineSalaire(nullptr), isEditMode(isEdit)
{
    this->setWindowTitle(isEdit ? "Edit Employee" : "Add New Employee");
    this->setModal(true);
    this->setMinimumWidth(400);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QLabel *titleLabel = new QLabel(isEdit ? "<b>Edit Employe Information</b>" : "<b>Add New Employe</b>");
    titleLabel->setStyleSheet("font-size: 14px; color: #03224c;");
    mainLayout->addWidget(titleLabel);

    QFormLayout *formLayout = new QFormLayout();

    lineId = new QLineEdit(this);
    lineId->setReadOnly(isEdit);
    formLayout->addRow("Employee ID:", lineId);

    lineNom = new QLineEdit(this);
    formLayout->addRow("Full Name:", lineNom);

    comboPoste = new QComboBox(this);
    comboPoste->addItems({"Pêcheur", "Matelot", "Docker", "Chef de quai", "Superviseur"});
    formLayout->addRow("Position:", comboPoste);

    lineEmail = new QLineEdit(this);
    lineEmail->setPlaceholderText("name@example.com");
    formLayout->addRow("Email:", lineEmail);

    lineTelephone = new QLineEdit(this);
    lineTelephone->setPlaceholderText("Phone number");
    formLayout->addRow("Phone:", lineTelephone);

    lineSalaire = new QLineEdit(this);
    lineSalaire->setPlaceholderText("0");
    formLayout->addRow("Salary (TND):", lineSalaire);

    mainLayout->addLayout(formLayout);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    buttonBox->button(QDialogButtonBox::Ok)->setText("Save");
    buttonBox->button(QDialogButtonBox::Cancel)->setText("Cancel");
    
    connect(buttonBox, &QDialogButtonBox::accepted, this, &AddEditEmployeeDialog::onAccepted);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    mainLayout->addWidget(buttonBox);
}

AddEditEmployeeDialog::~AddEditEmployeeDialog()
{
}

bool AddEditEmployeeDialog::isValidEmail(const QString &email) const
{
    QRegularExpression re(R"(^[\w\.\-]+@[\w\-]+(\.[\w\-]+)+$)");
    return re.match(email.trimmed()).hasMatch();
}

bool AddEditEmployeeDialog::isValidPhone(const QString &phone) const
{
    QString p = phone;
    p.remove(QRegularExpression("[ \-\(\)]+"));
    // allow optional + and 6-15 digits
    QRegularExpression re(R"(^\+?\d{6,15}$)");
    return re.match(p.trimmed()).hasMatch();
}

bool AddEditEmployeeDialog::isValidSalary(const QString &salary, double &outValue) const
{
    QString s = salary;
    s.replace(" TND", "");
    bool ok = false;
    double v = s.toDouble(&ok);
    if (!ok) return false;
    outValue = v;
    return v >= 0.0;
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
    QString id = lineId->text().trimmed();
    QString nom = lineNom->text().trimmed();
    QString email = lineEmail->text().trimmed();
    QString phone = lineTelephone->text().trimmed();
    QString salaryStr = lineSalaire->text().trimmed();

    if (id.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter Employe ID");
        lineId->setFocus();
        return;
    }

    if (nom.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter Full Name");
        lineNom->setFocus();
        return;
    }

    if (email.isEmpty() || !isValidEmail(email)) {
        QMessageBox::warning(this, "Error", "Please enter a valid Email address");
        lineEmail->setFocus();
        return;
    }

    if (phone.isEmpty() || !isValidPhone(phone)) {
        QMessageBox::warning(this, "Error", "Please enter a valid Phone number (digits only, optional +)");
        lineTelephone->setFocus();
        return;
    }

    double salaryVal = 0.0;
    if (salaryStr.isEmpty() || !isValidSalary(salaryStr, salaryVal)) {
        QMessageBox::warning(this, "Error", "Please enter a valid non-negative Salary");
        lineSalaire->setFocus();
        return;
    }

    // Optional: normalize salary display
    lineSalaire->setText(QString::number(salaryVal, 'f', 0));

    accept();
}
