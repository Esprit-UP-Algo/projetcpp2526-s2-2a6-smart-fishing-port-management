#include "capturesdialog.h"
#include "addedit_capturedialog.h"
#include "connection.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QHeaderView>
#include <QFormLayout>
#include <QLineEdit>
#include <QDateEdit>
#include <QSpinBox>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <algorithm>
#include <limits>

CapturesDialog::CapturesDialog(QWidget *parent)
    : QWidget(parent), table(nullptr), lineSearch(nullptr), 
      statTotal(nullptr), statTotalQuantite(nullptr), statMoyenneQuantite(nullptr),
      statMaxQuantite(nullptr), statNavireCount(nullptr)
{
    setupUI();

    Connection::instance()->createconnect();
    ensureTableExists();
    loadData();
}

CapturesDialog::~CapturesDialog()
{
}

void CapturesDialog::setupUI()
{
    QVBoxLayout *main = new QVBoxLayout(this);

    // Search bar
    QHBoxLayout *searchLayout = new QHBoxLayout();
    QLabel *searchLabel = new QLabel("Recherche:", this);
    lineSearch = new QLineEdit(this);
    lineSearch->setPlaceholderText("Rechercher par navire, type de poisson...");
    searchLayout->addWidget(searchLabel);
    searchLayout->addWidget(lineSearch);
    main->addLayout(searchLayout);

    // Stats section
    QHBoxLayout *statsLayout = new QHBoxLayout();
    statTotal = new QLabel("Total: 0", this);
    statTotalQuantite = new QLabel("Quantite Totale: 0", this);
    statMoyenneQuantite = new QLabel("Moyenne: 0", this);
    statMaxQuantite = new QLabel("Max: 0", this);
    statNavireCount = new QLabel("Navires: 0", this);
    
    statTotal->setStyleSheet("background-color: #e3f2fd; padding: 8px; border-radius: 5px; font-weight: bold;");
    statTotalQuantite->setStyleSheet("background-color: #e8f5e9; padding: 8px; border-radius: 5px; font-weight: bold;");
    statMoyenneQuantite->setStyleSheet("background-color: #fff3e0; padding: 8px; border-radius: 5px; font-weight: bold;");
    statMaxQuantite->setStyleSheet("background-color: #fce4ec; padding: 8px; border-radius: 5px; font-weight: bold;");
    statNavireCount->setStyleSheet("background-color: #f3e5f5; padding: 8px; border-radius: 5px; font-weight: bold;");
    
    statsLayout->addWidget(statTotal);
    statsLayout->addWidget(statTotalQuantite);
    statsLayout->addWidget(statMoyenneQuantite);
    statsLayout->addWidget(statMaxQuantite);
    statsLayout->addWidget(statNavireCount);
    main->addLayout(statsLayout);

    // Table
    table = new QTableWidget(this);
    table->setColumnCount(6);
    table->setHorizontalHeaderLabels({"ID", "Navire", "Date_Capture", "Type_Poisson", "Quantite", "Actions"});
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    main->addWidget(table, 1);

    // Buttons
    QHBoxLayout *buttons = new QHBoxLayout();
    QPushButton *btnAdd = new QPushButton("Ajouter", this);
    QPushButton *btnEdit = new QPushButton("Modifier", this);
    QPushButton *btnDelete = new QPushButton("Supprimer", this);
    QPushButton *btnSortNavire = new QPushButton("Trier par Navire", this);
    QPushButton *btnSortDate = new QPushButton("Trier par Date", this);
    QPushButton *btnSortQuantite = new QPushButton("Trier par Quantite", this);
    QPushButton *btnClose = new QPushButton("Fermer", this);
    
    btnAdd->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; padding: 8px; border-radius: 5px; font-weight: bold; }");
    btnEdit->setStyleSheet("QPushButton { background-color: #2196F3; color: white; padding: 8px; border-radius: 5px; font-weight: bold; }");
    btnDelete->setStyleSheet("QPushButton { background-color: #F44336; color: white; padding: 8px; border-radius: 5px; font-weight: bold; }");
    btnSortNavire->setStyleSheet("QPushButton { background-color: #9C27B0; color: white; padding: 8px; border-radius: 5px; }");
    btnSortDate->setStyleSheet("QPushButton { background-color: #FF9800; color: white; padding: 8px; border-radius: 5px; }");
    btnSortQuantite->setStyleSheet("QPushButton { background-color: #00BCD4; color: white; padding: 8px; border-radius: 5px; }");
    btnClose->setStyleSheet("QPushButton { background-color: #607D8B; color: white; padding: 8px; border-radius: 5px; }");
    
    buttons->addWidget(btnAdd);
    buttons->addWidget(btnEdit);
    buttons->addWidget(btnDelete);
    buttons->addStretch();
    buttons->addWidget(btnSortNavire);
    buttons->addWidget(btnSortDate);
    buttons->addWidget(btnSortQuantite);
    buttons->addStretch();
    buttons->addWidget(btnClose);
    main->addLayout(buttons);

    connect(btnAdd, &QPushButton::clicked, this, &CapturesDialog::onAdd);
    connect(btnEdit, &QPushButton::clicked, this, &CapturesDialog::onEdit);
    connect(btnDelete, &QPushButton::clicked, this, &CapturesDialog::onDelete);
    connect(btnSortNavire, &QPushButton::clicked, this, &CapturesDialog::onSortByNavire);
    connect(btnSortDate, &QPushButton::clicked, this, &CapturesDialog::onSortByDate);
    connect(btnSortQuantite, &QPushButton::clicked, this, &CapturesDialog::onSortByQuantite);
    connect(btnClose, &QPushButton::clicked, this, &CapturesDialog::closeClicked);
    connect(lineSearch, &QLineEdit::textChanged, this, &CapturesDialog::onSearch);
}

void CapturesDialog::ensureTableExists()
{
    QSqlQuery q;
    const QString create =
        "CREATE TABLE IF NOT EXISTS captures ("
        "ID_Capture INTEGER PRIMARY KEY AUTOINCREMENT, "
        "Navire TEXT, "
        "Date_Capture TEXT, "
        "Type_Poisson TEXT, "
        "Quantite INTEGER)";
    if (!q.exec(create)) {
        qDebug() << "Failed to create captures table:" << q.lastError().text();
    }
}

void CapturesDialog::loadData()
{
    table->setRowCount(0);
    QSqlQuery q;
    if (!q.exec("SELECT ID_Capture, Navire, Date_Capture, Type_Poisson, Quantite FROM captures ORDER BY ID_Capture DESC")) {
        qDebug() << "Failed to query captures:" << q.lastError().text();
        return;
    }

    while (q.next()) {
        int row = table->rowCount();
        table->insertRow(row);
        for (int col = 0; col < 5; ++col) {
            // Column 0 is ID, we will format it as CAP### and store raw id in UserRole
            if (col == 0) {
                int rawId = q.value(0).toInt();
                QString disp = QString("CAP%1").arg(rawId, 3, 10, QChar('0'));
                QTableWidgetItem *idItem = new QTableWidgetItem(disp);
                idItem->setData(Qt::UserRole, rawId);
                table->setItem(row, 0, idItem);
            } else {
                QTableWidgetItem *item = new QTableWidgetItem(q.value(col).toString());
                table->setItem(row, col, item);
            }
        }
        
        // Actions column with edit button
        QPushButton *btnEdit = new QPushButton("Edit");
        btnEdit->setToolTip("Modifier");
        btnEdit->setStyleSheet("QPushButton { background-color: #2196F3; color: white; border: none; border-radius: 3px; padding: 3px 8px; }");
        btnEdit->setMaximumWidth(40);
        connect(btnEdit, &QPushButton::clicked, this, [this, row]() {
            table->selectRow(row);
            onEdit();
        });
        table->setCellWidget(row, 5, btnEdit);
    }
    table->setColumnHidden(0, false);
    updateStats();
}

int CapturesDialog::selectedCaptureId() const
{
    QModelIndexList sel = table->selectionModel()->selectedRows();
    if (sel.isEmpty()) return -1;
    int row = sel.first().row();
    QTableWidgetItem *idItem = table->item(row, 0);
    if (!idItem) return -1;
    if (idItem->data(Qt::UserRole).isValid()) return idItem->data(Qt::UserRole).toInt();
    // fallback to parsing digits from the display text (e.g., CAP001 -> 1)
    QString txt = idItem->text();
    QString digits;
    for (QChar c : txt) if (c.isDigit()) digits.append(c);
    return digits.isEmpty() ? -1 : digits.toInt();
}

void CapturesDialog::onSearch(const QString &text)
{
    for (int i = 0; i < table->rowCount(); ++i) {
        bool match = false;
        for (int j = 1; j < table->columnCount() - 1; ++j) {
            QTableWidgetItem *item = table->item(i, j);
            if (item && item->text().contains(text, Qt::CaseInsensitive)) {
                match = true;
                break;
            }
        }
        table->setRowHidden(i, !match);
    }
}

void CapturesDialog::onSortByNavire()
{
    QList<QStringList> rows;
    for (int i = 0; i < table->rowCount(); ++i) {
        QStringList row;
        for (int j = 0; j < table->columnCount() - 1; ++j) {
            row << (table->item(i, j) ? table->item(i, j)->text() : "");
        }
        rows.append(row);
    }
    
    std::sort(rows.begin(), rows.end(), [](const QStringList& a, const QStringList& b) {
        return a[1] < b[1]; // Sort by Navire
    });
    
    for (int i = 0; i < rows.size(); ++i) {
        for (int j = 0; j < rows[i].size(); ++j) {
            if (table->item(i, j)) {
                table->item(i, j)->setText(rows[i][j]);
            }
        }
    }
    
    QMessageBox::information(this, "Tri", "Captures triees par Navire");
}

void CapturesDialog::onSortByDate()
{
    QList<QStringList> rows;
    for (int i = 0; i < table->rowCount(); ++i) {
        QStringList row;
        for (int j = 0; j < table->columnCount() - 1; ++j) {
            row << (table->item(i, j) ? table->item(i, j)->text() : "");
        }
        rows.append(row);
    }
    
    std::sort(rows.begin(), rows.end(), [](const QStringList& a, const QStringList& b) {
        return a[2] > b[2]; // Sort by Date (newest first)
    });
    
    for (int i = 0; i < rows.size(); ++i) {
        for (int j = 0; j < rows[i].size(); ++j) {
            if (table->item(i, j)) {
                table->item(i, j)->setText(rows[i][j]);
            }
        }
    }
    
    QMessageBox::information(this, "Tri", "Captures triees par Date");
}

void CapturesDialog::onSortByQuantite()
{
    QList<QStringList> rows;
    for (int i = 0; i < table->rowCount(); ++i) {
        QStringList row;
        for (int j = 0; j < table->columnCount() - 1; ++j) {
            row << (table->item(i, j) ? table->item(i, j)->text() : "");
        }
        rows.append(row);
    }
    
    std::sort(rows.begin(), rows.end(), [](const QStringList& a, const QStringList& b) {
        return a[4].toInt() > b[4].toInt(); // Sort by Quantite (descending)
    });
    
    for (int i = 0; i < rows.size(); ++i) {
        for (int j = 0; j < rows[i].size(); ++j) {
            if (table->item(i, j)) {
                table->item(i, j)->setText(rows[i][j]);
            }
        }
    }
    
    QMessageBox::information(this, "Tri", "Captures triees par Quantite (decroissant)");
}

void CapturesDialog::updateStats()
{
    int total = table->rowCount();
    int totalQuantite = 0;
    int maxQuantite = 0;
    QStringList navires;
    
    for (int i = 0; i < table->rowCount(); ++i) {
        int quantite = table->item(i, 4) ? table->item(i, 4)->text().toInt() : 0;
        totalQuantite += quantite;
        if (quantite > maxQuantite) maxQuantite = quantite;
        
        QString navire = table->item(i, 1) ? table->item(i, 1)->text() : "";
        if (!navire.isEmpty() && !navires.contains(navire)) {
            navires.append(navire);
        }
    }
    
    int moyenne = total > 0 ? totalQuantite / total : 0;
    
    statTotal->setText(QString("Total: %1").arg(total));
    statTotalQuantite->setText(QString("Quantite Totale: %1").arg(totalQuantite));
    statMoyenneQuantite->setText(QString("Moyenne: %1").arg(moyenne));
    statMaxQuantite->setText(QString("Max: %1").arg(maxQuantite));
    statNavireCount->setText(QString("Navires: %1").arg(navires.size()));
}

void CapturesDialog::onAdd()
{
    AddEditCaptureDialog dlg(this, false);
    dlg.setWindowTitle("Ajouter une capture");
    
    if (dlg.exec() == QDialog::Accepted) {
        QSqlQuery q;
        q.prepare("INSERT INTO captures (Navire, Date_Capture, Type_Poisson, Quantite) VALUES (?, ?, ?, ?)");
        q.addBindValue(dlg.getNavire());
        q.addBindValue(dlg.getDateCapture().toString(Qt::ISODate));
        q.addBindValue(dlg.getTypePoisson());
        q.addBindValue(dlg.getQuantite());
        if (!q.exec()) {
            QMessageBox::critical(this, "Erreur", "Impossible d'ajouter la capture: " + q.lastError().text());
        } else {
            loadData();
            QMessageBox::information(this, "Succès", "Capture ajoutée avec succès !");
        }
    }
}

void CapturesDialog::onEdit()
{
    int id = selectedCaptureId();
    if (id < 0) {
        QMessageBox::warning(this, "Selection requise", "Veuillez selectionner une capture a modifier.");
        return;
    }

    QSqlQuery q;
    q.prepare("SELECT Navire, Date_Capture, Type_Poisson, Quantite FROM captures WHERE ID_Capture = ?");
    q.addBindValue(id);
    if (!q.exec() || !q.next()) {
        QMessageBox::critical(this, "Erreur", "Impossible de charger la capture: " + q.lastError().text());
        return;
    }

    QString curNavire = q.value(0).toString();
    QDate curDate = QDate::fromString(q.value(1).toString(), Qt::ISODate);
    QString curType = q.value(2).toString();
    int curQuant = q.value(3).toInt();

    AddEditCaptureDialog dlg(this, true);
    dlg.setCaptureData(curNavire, curDate, curType, curQuant);
    dlg.setWindowTitle("Modifier la capture");

    if (dlg.exec() == QDialog::Accepted) {
        QSqlQuery u;
        u.prepare("UPDATE captures SET Navire = ?, Date_Capture = ?, Type_Poisson = ?, Quantite = ? WHERE ID_Capture = ?");
        u.addBindValue(dlg.getNavire());
        u.addBindValue(dlg.getDateCapture().toString(Qt::ISODate));
        u.addBindValue(dlg.getTypePoisson());
        u.addBindValue(dlg.getQuantite());
        u.addBindValue(id);
        if (!u.exec()) {
            QMessageBox::critical(this, "Erreur", "Impossible de modifier la capture: " + u.lastError().text());
        } else {
            loadData();
            QMessageBox::information(this, "Succès", "Capture modifiée avec succès !");
        }
    }
}

void CapturesDialog::onDelete()
{
    int id = selectedCaptureId();
    if (id < 0) {
        QMessageBox::warning(this, "Selection requise", "Veuillez selectionner une capture a supprimer.");
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(this, "Confirmer", "Supprimer la capture selectionnee ?", QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) return;

    QSqlQuery q;
    q.prepare("DELETE FROM captures WHERE ID_Capture = ?");
    q.addBindValue(id);
    if (!q.exec()) {
        QMessageBox::critical(this, "Erreur", "Impossible de supprimer la capture: " + q.lastError().text());
    } else {
        loadData();
        QMessageBox::information(this, "Succes", "Capture supprimee avec succes!");
    }
}
