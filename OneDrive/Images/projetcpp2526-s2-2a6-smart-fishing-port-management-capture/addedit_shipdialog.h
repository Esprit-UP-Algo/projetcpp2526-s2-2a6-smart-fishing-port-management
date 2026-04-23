#ifndef ADDEDIT_SHIPDIALOG_H
#define ADDEDIT_SHIPDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>

class AddEditShipDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AddEditShipDialog(QWidget *parent = nullptr, bool isEdit = false);
    ~AddEditShipDialog();

    QString getImmatriculation() const;
    QString getNom() const;
    QString getType() const;
    QString getCapacite() const;
    QString getStatut() const;

    void setShipData(const QString &immat, const QString &nom, const QString &type,
                     const QString &capacite, const QString &statut);

private slots:
    void onAccepted();

private:
    QLineEdit *lineImmatriculation;
    QLineEdit *lineNom;
    QComboBox *comboType;
    QLineEdit *lineCapacite;
    QComboBox *comboStatut;
    bool isEditMode;
};

#endif
