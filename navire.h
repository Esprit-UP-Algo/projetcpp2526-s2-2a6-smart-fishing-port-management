#ifndef NAVIRE_H
#define NAVIRE_H

#include <QMainWindow>
#include <QModelIndex>
#include "connection.h"

class QSqlQueryModel;

QT_BEGIN_NAMESPACE
namespace Ui {
class NAVIRE;
}
QT_END_NAMESPACE

class NAVIRE : public QMainWindow
{
    Q_OBJECT

public:
    NAVIRE(QWidget *parent = nullptr);
    ~NAVIRE();

private:
    Ui::NAVIRE *ui;
    QSqlQueryModel *m_model;

    void initialiserTable();
    void rafraichirTable();
    void viderFormulaire();
    int idSelectionne() const;

private slots:
    void on_pushButton_ajouter_clicked();
    void on_pushButton_modifier_clicked();
    void on_pushButton_supprimer_clicked();
    void on_tableView_navires_clicked(const QModelIndex &index);
};
#endif // NAVIRE_H
