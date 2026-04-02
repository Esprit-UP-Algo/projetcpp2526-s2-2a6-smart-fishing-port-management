#ifndef ADDEDIT_EMPLOYEEDIALOG_H
#define ADDEDIT_EMPLOYEEDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QString>
#include "employe.h"

class AddEditEmployeeDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AddEditEmployeeDialog(QWidget *parent = nullptr, bool isEdit = false);
    ~AddEditEmployeeDialog();

    QString getId() const;
    QString getNom() const;
    QString getPoste() const;
    QString getEmail() const;
    QString getTelephone() const;
    double getSalaire() const;
    QString getGenre() const;
    QString getPassword() const;

    void setEmployeeData(const QString &id, const QString &nom, const QString &poste,
                        const QString &email, const QString &telephone, double salary, const QString &genre = "M", const QString &password = "");
    
    void setExistingEmails(const QStringList &emails);
    void setExistingPhones(const QStringList &phones);

private slots:
    void onAccepted();

private:
    bool validateForm();
    bool isValidEmail(const QString &email) const;
    bool isValidPhone(const QString &phone) const;
    bool isValidNom(const QString &nom) const;
    bool isValidSalary(const QString &salary, double &outValue) const;
    bool isEmailUnique(const QString &email) const;
    bool isPhoneUnique(const QString &phone) const;

    QLineEdit *lineId;
    QLineEdit *lineNom;
    QComboBox *comboPoste;
    QComboBox *comboGenre;
    QLineEdit *lineEmail;
    QLineEdit *lineTelephone;
    QLineEdit *linePassword;
    QDoubleSpinBox *spinSalaire;
    bool isEditMode;
    bool isIdEditable;
    
    QStringList existingEmails;
    QStringList existingPhones;
    QString originalEmail;
    QString originalPhone;
};

#endif // ADDEDIT_EMPLOYEEDIALOG_H
