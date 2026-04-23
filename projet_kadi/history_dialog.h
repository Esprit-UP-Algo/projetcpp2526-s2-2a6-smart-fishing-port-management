#ifndef HISTORY_DIALOG_H
#define HISTORY_DIALOG_H

#include <QDialog>
#include <QSqlDatabase>

class QSqlQueryModel;
class QTableView;
class QPushButton;

class HistoryDialog : public QDialog
{
    Q_OBJECT
public:
    explicit HistoryDialog(QSqlDatabase &db, QWidget *parent = nullptr);

private slots:
    void on_pushButton_restaurer_clicked();
    void on_pushButton_supprimer_def_clicked();

private:
    QSqlDatabase m_db;
    QSqlQueryModel *m_model;
    QTableView *m_view;
    
    void rafraichir();
    int idSelectionne() const;
};

#endif // HISTORY_DIALOG_H
