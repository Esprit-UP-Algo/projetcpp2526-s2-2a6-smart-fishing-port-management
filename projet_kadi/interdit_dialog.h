#ifndef INTERDIT_DIALOG_H
#define INTERDIT_DIALOG_H

#include <QDialog>
#include <QSqlDatabase>

class QSqlQueryModel;
class QTableView;
class QComboBox;

class InterditDialog : public QDialog
{
    Q_OBJECT
public:
    explicit InterditDialog(QSqlDatabase &db, QWidget *parent = nullptr);

private slots:
    void on_pushButton_mettre_a_jour_clicked();

private:
    QSqlDatabase m_db;
    QSqlQueryModel *m_model;
    QTableView *m_view;
    QComboBox *m_comboStatut;
    
    void rafraichir();
    int idSelectionne() const;
};

#endif // INTERDIT_DIALOG_H
