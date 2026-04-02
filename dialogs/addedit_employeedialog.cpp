#include "addedit_employeedialog.h"
#include "database/constants.h"
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QMessageBox>
#include <QLabel>
#include <QRegularExpression>

AddEditEmployeeDialog::AddEditEmployeeDialog(QWidget *parent, bool isEdit)
    : QDialog(parent), lineId(nullptr), lineNom(nullptr), comboPoste(nullptr), comboGenre(nullptr),
      lineEmail(nullptr), lineTelephone(nullptr), spinSalaire(nullptr), 
      isEditMode(isEdit), isIdEditable(!isEdit)
{
    this->setWindowTitle(isEdit ? "Modifier l'employé" : "Ajouter un nouvel employé");
    this->setModal(true);
    this->setMinimumWidth(450);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QLabel *titleLabel = new QLabel(isEdit ? "<b>Modifier les informations de l'employé</b>" : "<b>Ajouter un nouvel employé</b>");
    titleLabel->setStyleSheet("font-size: 14px; color: #03224c;");
    mainLayout->addWidget(titleLabel);

    QFormLayout *formLayout = new QFormLayout();

    lineId = new QLineEdit(this);
    lineId->setReadOnly(!isIdEditable);
    lineId->setPlaceholderText("ID automatique");
    lineId->setMaxLength(50);
    formLayout->addRow("ID Employé:", lineId);

    lineNom = new QLineEdit(this);
    lineNom->setPlaceholderText("Nom complet (2-100 caractères)");
    lineNom->setMaxLength(100);
    formLayout->addRow("Nom Complet:", lineNom);

    comboPoste = new QComboBox(this);
    // Use all positions from constants.h
    for (const QString &pos : Config::POSITIONS) {
        comboPoste->addItem(pos);
    }
    formLayout->addRow("Poste:", comboPoste);

    comboGenre = new QComboBox(this);
    comboGenre->addItems({"M", "F"});
    comboGenre->setMaxVisibleItems(2);
    formLayout->addRow("Genre:", comboGenre);

    lineEmail = new QLineEdit(this);
    lineEmail->setPlaceholderText("exemple@domaine.com");
    lineEmail->setMaxLength(120);
    formLayout->addRow("Email:", lineEmail);

    linePassword = new QLineEdit(this);
    linePassword->setEchoMode(QLineEdit::Password);
    linePassword->setPlaceholderText("Mot de passe (laissez vide pour ne pas changer)");
    linePassword->setMaxLength(255);
    formLayout->addRow("Mot de passe:", linePassword);

    lineTelephone = new QLineEdit(this);
    lineTelephone->setPlaceholderText("ex: 0600000000 ou +216600000000");
    lineTelephone->setMaxLength(20);
    formLayout->addRow("Téléphone:", lineTelephone);

    spinSalaire = new QDoubleSpinBox(this);
    spinSalaire->setRange(0, 1000000);
    spinSalaire->setValue(0);
    spinSalaire->setSuffix(" TND");
    spinSalaire->setDecimals(0);
    formLayout->addRow("Salaire (TND):", spinSalaire);

    mainLayout->addLayout(formLayout);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    buttonBox->button(QDialogButtonBox::Ok)->setText(isEdit ? "✔️ Modifier" : "💾 Ajouter");
    buttonBox->button(QDialogButtonBox::Cancel)->setText("❌ Annuler");
    
    connect(buttonBox, &QDialogButtonBox::accepted, this, &AddEditEmployeeDialog::onAccepted);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    mainLayout->addWidget(buttonBox);
}

AddEditEmployeeDialog::~AddEditEmployeeDialog()
{
}

bool AddEditEmployeeDialog::isValidEmail(const QString &email) const
{
    QString e = email.trimmed().toLower();
    QRegularExpression re(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
    return re.match(e).hasMatch() && e.length() <= 120;
}

bool AddEditEmployeeDialog::isValidPhone(const QString &phone) const
{
    QString p = phone.trimmed();
    p.remove(QRegularExpression(R"([\s\-\(\)\.])"));
    QRegularExpression re(R"(^\+?[0-9]{6,15}$)");
    return re.match(p).hasMatch();
}

bool AddEditEmployeeDialog::isValidNom(const QString &nom) const
{
    QString n = nom.trimmed();
    if (n.length() < 2 || n.length() > 100) return false;
    QRegularExpression re(R"(^[a-zA-ZÀ-ÿ\s\-']+$)");
    return re.match(n).hasMatch();
}

bool AddEditEmployeeDialog::isValidSalary(const QString &salary, double &outValue) const
{
    QString s = salary;
    s.replace(" TND", "").replace(" ", "");
    bool ok = false;
    double v = s.toDouble(&ok);
    if (!ok) return false;
    outValue = v;
    return v >= 0.0 && v <= 1000000.0;
}

bool AddEditEmployeeDialog::isEmailUnique(const QString &email) const
{
    QString e = email.trimmed().toLower();
    for (const QString &existingEmail : existingEmails) {
        if (existingEmail.trimmed().toLower() == e) {
            // Si mode édition et c'est l'email original, c'est ok
            if (isEditMode && originalEmail.trimmed().toLower() == e)
                continue;
            return false;
        }
    }
    return true;
}

bool AddEditEmployeeDialog::isPhoneUnique(const QString &phone) const
{
    QString p = phone.trimmed();
    p.remove(QRegularExpression(R"([\s\-\(\)\.])"));
    
    for (const QString &existingPhone : existingPhones) {
        QString ep = existingPhone.trimmed();
        ep.remove(QRegularExpression(R"([\s\-\(\)\.])"));
        if (ep == p) {
            // Si mode édition et c'est le téléphone original, c'est ok
            if (isEditMode && originalPhone.trimmed().remove(QRegularExpression(R"([\s\-\(\)\.])")) == p)
                continue;
            return false;
        }
    }
    return true;
}

QString AddEditEmployeeDialog::getId() const
{
    return lineId ? lineId->text().trimmed() : QString();
}

QString AddEditEmployeeDialog::getNom() const
{
    return lineNom ? lineNom->text().trimmed() : QString();
}

QString AddEditEmployeeDialog::getPoste() const
{
    return comboPoste ? comboPoste->currentText() : QString();
}

QString AddEditEmployeeDialog::getEmail() const
{
    return lineEmail ? lineEmail->text().trimmed() : QString();
}

QString AddEditEmployeeDialog::getTelephone() const
{
    return lineTelephone ? lineTelephone->text().trimmed() : QString();
}

double AddEditEmployeeDialog::getSalaire() const
{
    return spinSalaire ? spinSalaire->value() : 0.0;
}

QString AddEditEmployeeDialog::getGenre() const
{
    return comboGenre ? comboGenre->currentText() : "M";
}

QString AddEditEmployeeDialog::getPassword() const
{
    return linePassword ? linePassword->text() : QString();
}

void AddEditEmployeeDialog::setEmployeeData(const QString &id, const QString &nom, const QString &poste,
                                            const QString &email, const QString &telephone, double salary, const QString &genre, const QString &password)
{
    if (lineId) lineId->setText(id);
    if (lineNom) lineNom->setText(nom);
    if (comboPoste) comboPoste->setCurrentText(poste);
    if (comboGenre) comboGenre->setCurrentText(genre.isEmpty() ? "M" : genre);
    if (lineEmail) lineEmail->setText(email);
    if (lineTelephone) lineTelephone->setText(telephone);
    if (spinSalaire) spinSalaire->setValue(salary);
    if (linePassword) linePassword->setText(password);
    
    originalEmail = email;
    originalPhone = telephone;
}

void AddEditEmployeeDialog::setExistingEmails(const QStringList &emails)
{
    existingEmails = emails;
}

void AddEditEmployeeDialog::setExistingPhones(const QStringList &phones)
{
    existingPhones = phones;
}

void AddEditEmployeeDialog::onAccepted()
{
    if (validateForm()) {
        accept();
    }
}

bool AddEditEmployeeDialog::validateForm()
{
    // Validation ID
    if (!isEditMode && lineId->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Erreur de validation", 
            "❌ L'ID est obligatoire pour un nouvel employé.");
        lineId->setFocus();
        return false;
    }

    // Validation Nom
    QString nom = lineNom->text();
    if (nom.trimmed().isEmpty()) {
        QMessageBox::warning(this, "Erreur de validation", 
            "❌ Le nom est obligatoire.");
        lineNom->setFocus();
        return false;
    }
    if (!isValidNom(nom)) {
        QMessageBox::warning(this, "Erreur de validation", 
            "❌ Le nom doit contenir entre 2 et 100 caractères et ne peut contenir que des lettres, espaces, tirets et apostrophes.");
        lineNom->setFocus();
        return false;
    }

    // Validation Email
    QString email = lineEmail->text();
    if (email.trimmed().isEmpty()) {
        QMessageBox::warning(this, "Erreur de validation", 
            "❌ L'email est obligatoire.");
        lineEmail->setFocus();
        return false;
    }
    if (!isValidEmail(email)) {
        QMessageBox::warning(this, "Erreur de validation", 
            "❌ L'email est invalide. Format attendu: exemple@domaine.com");
        lineEmail->setFocus();
        return false;
    }
    if (!isEmailUnique(email)) {
        QMessageBox::warning(this, "Erreur de validation", 
            "❌ Cet email est déjà utilisé par un autre employé.");
        lineEmail->setFocus();
        return false;
    }

    // Validation Téléphone
    QString phone = lineTelephone->text();
    if (phone.trimmed().isEmpty()) {
        QMessageBox::warning(this, "Erreur de validation", 
            "❌ Le téléphone est obligatoire.");
        lineTelephone->setFocus();
        return false;
    }

    // Validation Mot de passe
    QString password = linePassword->text();
    if (!isEditMode && password.trimmed().isEmpty()) {
        QMessageBox::warning(this, "Erreur de validation",
            "❌ Le mot de passe est obligatoire lors de la création d'un nouvel employé.");
        linePassword->setFocus();
        return false;
    }

    if (!password.trimmed().isEmpty() && password.trimmed().length() < 6) {
        QMessageBox::warning(this, "Erreur de validation",
            "❌ Le mot de passe doit contenir au moins 6 caractères.");
        linePassword->setFocus();
        return false;
    }
    if (!isValidPhone(phone)) {
        QMessageBox::warning(this, "Erreur de validation", 
            "❌ Le téléphone est invalide. Entrez au moins 6 chiffres (ex: 0600000000 ou +216600000000).");
        lineTelephone->setFocus();
        return false;
    }
    if (!isPhoneUnique(phone)) {
        QMessageBox::warning(this, "Erreur de validation", 
            "❌ Ce numéro de téléphone est déjà utilisé par un autre employé.");
        lineTelephone->setFocus();
        return false;
    }

    // Validation Salaire
    double salary = spinSalaire->value();
    if (salary < 0 || salary > 1000000) {
        QMessageBox::warning(this, "Erreur de validation", 
            "❌ Le salaire doit être entre 0 et 1 000 000 TND.");
        spinSalaire->setFocus();
        return false;
    }

    return true;
}
