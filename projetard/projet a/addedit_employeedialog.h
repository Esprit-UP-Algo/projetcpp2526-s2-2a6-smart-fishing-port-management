#ifndef ADDEDIT_EMPLOYEEDIALOG_H
#define ADDEDIT_EMPLOYEEDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>

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

    void setEmployeeData(const QString &id, const QString &nom, const QString &poste,
                        const QString &email, const QString &telephone, double salary);

private slots:
    void onAccepted();

private:
    QLineEdit *lineId;
    QLineEdit *lineNom;
    QComboBox *comboPoste;
    QLineEdit *lineEmail;
    QLineEdit *lineTelephone;
    QLineEdit *lineSalaire;
    bool isEditMode;
    bool isValidEmail(const QString &email) const;
    bool isValidPhone(const QString &phone) const;
    bool isValidSalary(const QString &salary, double &outValue) const;
};

#endif
