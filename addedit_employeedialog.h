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

    // Getters
    QString getId() const;
    QString getNom() const;
    QString getPoste() const;
    QString getEmail() const;
    QString getTelephone() const;
    double getSalaire() const;

    // Setters for edit mode
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
};

#endif // ADDEDIT_EMPLOYEEDIALOG_H
