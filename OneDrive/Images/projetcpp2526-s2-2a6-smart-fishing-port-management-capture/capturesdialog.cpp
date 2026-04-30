#include "capturesdialog.h"
#include "addedit_capturedialog.h"
#include "connection.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include "fishingzonesdialog.h"
#ifdef HAVE_QT_WEBENGINE
#include "fishingzones3ddialog.h"
#endif
#include <QHeaderView>
#include <QFormLayout>
#include <QLineEdit>
#include <QDateEdit>
#include <QDate>
#include <QSpinBox>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <algorithm>
#include <limits>
#include <QRandomGenerator> // For dummy ship data

CapturesDialog::CapturesDialog(QWidget *parent)
    : QWidget(parent), table(nullptr), lineSearch(nullptr), 
      statTotal(nullptr), statTotalQuantite(nullptr), statMoyenneQuantite(nullptr),
      statMaxQuantite(nullptr), statNavireCount(nullptr)
{
    setupUI();

    Connection::instance()->createConnect();
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

    // Filters: type and date range
    QHBoxLayout *filterLayout = new QHBoxLayout();
    QLabel *lblType = new QLabel("Type:", this);
    comboTypeFilter = new QComboBox(this);
    comboTypeFilter->addItem("Tous");
    QLabel *lblFrom = new QLabel("De:", this);
    dateFrom = new QDateEdit(this);
    dateFrom->setCalendarPopup(true);
    QLabel *lblTo = new QLabel("A:", this);
    dateTo = new QDateEdit(this);
    dateTo->setCalendarPopup(true);
    dateTo->setDate(QDate::currentDate());
    dateFrom->setDate(QDate::currentDate().addDays(-30));
    filterLayout->addWidget(lblType);
    filterLayout->addWidget(comboTypeFilter);
    filterLayout->addWidget(lblFrom);
    filterLayout->addWidget(dateFrom);
    filterLayout->addWidget(lblTo);
    filterLayout->addWidget(dateTo);
    filterLayout->addStretch();
    main->addLayout(filterLayout);

    // Stats section
    QHBoxLayout *statsLayout = new QHBoxLayout();
    statTotal = new QLabel("Total: 0", this);
    statTotalQuantite = new QLabel("Quantite Totale: 0", this);
    statMoyenneQuantite = new QLabel("Moyenne: 0", this);
    statMaxQuantite = new QLabel("Max: 0", this);
    statNavireCount = new QLabel("Navires: 0", this);
    
    statTotal->setProperty("type", "stat-blue");
    statTotalQuantite->setProperty("type", "stat-green");
    statMoyenneQuantite->setProperty("type", "stat-orange");
    statMaxQuantite->setProperty("type", "stat-purple");
    statNavireCount->setProperty("type", "stat-blue");
    
    for (auto lbl : {statTotal, statTotalQuantite, statMoyenneQuantite, statMaxQuantite, statNavireCount}) {
        lbl->setObjectName("statCard");
        lbl->style()->unpolish(lbl);
        lbl->style()->polish(lbl);
    }
    
    statsLayout->addWidget(statTotal);
    statsLayout->addWidget(statTotalQuantite);
    statsLayout->addWidget(statMoyenneQuantite);
    statsLayout->addWidget(statMaxQuantite);
    statsLayout->addWidget(statNavireCount);
    main->addLayout(statsLayout);

    // Table
    table = new QTableWidget(this);
    table->setObjectName("tableCapturesDialog");
    table->setColumnCount(6);
    table->setHorizontalHeaderLabels({"ID", "Navire", "Date_Capture", "Type_Poisson", "Quantite", "Actions"});
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->verticalHeader()->setVisible(false);
    table->verticalHeader()->setDefaultSectionSize(60);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    main->addWidget(table, 1);

    // Buttons
    QHBoxLayout *buttons = new QHBoxLayout();
    buttons->setSpacing(10);
    
    QPushButton *btnAdd = new QPushButton("➕ Ajouter", this);
    btnAdd->setObjectName("btnAdd");
    
    QPushButton *btnEdit = new QPushButton("✏️ Modifier", this);
    btnEdit->setObjectName("btnEdit");
    
    QPushButton *btnDelete = new QPushButton("🗑️ Supprimer", this);
    btnDelete->setObjectName("btnDelete");
    
    QPushButton *btnSortNavire = new QPushButton("Trier par Navire", this);
    btnSortNavire->setObjectName("btnSecondary");
    
    QPushButton *btnSortDate = new QPushButton("Trier par Date", this);
    btnSortDate->setObjectName("btnSecondary");
    
    QPushButton *btnSortQuantite = new QPushButton("Trier par Quantite", this);
    btnSortQuantite->setObjectName("btnSecondary");
    
    QPushButton *btnClose = new QPushButton("✖️ Fermer", this);
    btnClose->setObjectName("btnCancel");
    
    QPushButton *btnZones = new QPushButton("🌐 Zone de pêche", this);
    btnZones->setObjectName("btnSecondary");
    
    buttons->addWidget(btnAdd);
    buttons->addWidget(btnEdit);
    buttons->addWidget(btnDelete);
    buttons->addStretch();
    buttons->addWidget(btnSortNavire);
    buttons->addWidget(btnSortDate);
    buttons->addWidget(btnSortQuantite);
    buttons->addWidget(btnZones);
    buttons->addStretch();
    buttons->addWidget(btnClose);
    main->addLayout(buttons);

    connect(btnAdd, &QPushButton::clicked, this, &CapturesDialog::onAdd);
    connect(btnEdit, &QPushButton::clicked, this, &CapturesDialog::onEdit);
    connect(btnDelete, &QPushButton::clicked, this, &CapturesDialog::onDelete);
    connect(btnSortNavire, &QPushButton::clicked, this, &CapturesDialog::onSortByNavire);
    connect(btnSortDate, &QPushButton::clicked, this, &CapturesDialog::onSortByDate);
    connect(btnSortQuantite, &QPushButton::clicked, this, &CapturesDialog::onSortByQuantite);
    connect(btnZones, &QPushButton::clicked, this, &CapturesDialog::onShowZones);
    connect(btnClose, &QPushButton::clicked, this, &CapturesDialog::closeClicked);
    connect(lineSearch, &QLineEdit::textChanged, this, &CapturesDialog::onSearch);
}

void CapturesDialog::onShowZones()
{
    // Enhanced recommendation: compute per-navire metrics from captures and rank by a simple score
    QSqlQuery q;
    if (!q.exec("SELECT Navire, Date_Capture, Type_Poisson, Quantite FROM captures")) {
        QMessageBox::warning(this, "Zones", "Impossible de lire les captures: " + q.lastError().text());
        return;
    }

    struct Metrics {
        int totalAll = 0;
        int totalLast30 = 0;
        QMap<QString,int> typeCounts;
    };

    QMap<QString, Metrics> map;
    QDate from = dateFrom ? dateFrom->date() : QDate::currentDate().addDays(-30);
    QDate to = dateTo ? dateTo->date() : QDate::currentDate();
    if (from > to) std::swap(from, to);
    QString selectedType = comboTypeFilter ? comboTypeFilter->currentText() : QString("Tous");
    QDate cutoff = QDate::currentDate().addDays(-30);

    while (q.next()) {
        QString nav = q.value(0).toString();
        QDate d = QDate::fromString(q.value(1).toString(), Qt::ISODate);
        QString type = q.value(2).toString();
        int qty = q.value(3).toInt();

        // apply filters: type and date range
        if (!selectedType.isEmpty() && selectedType != "Tous" && type != selectedType) continue;
        if (d.isValid() && (d < from || d > to)) continue;

        Metrics &m = map[nav];
        m.totalAll += qty;
        if (d.isValid() && d >= cutoff) m.totalLast30 += qty;
        m.typeCounts[type] = m.typeCounts.value(type, 0) + 1;
    }

    // compute score and prepare list
    struct ZoneScore { QString nav; double score; int totalLast30; int totalAll; QString topType; int topTypeCount; };
    QList<ZoneScore> scores;
    for (auto it = map.constBegin(); it != map.constEnd(); ++it) {
        const QString &nav = it.key();
        const Metrics &m = it.value();
        // find most common type for this navire
        QString bestType;
        int bestCount = 0;
        for (auto tIt = m.typeCounts.constBegin(); tIt != m.typeCounts.constEnd(); ++tIt) {
            if (tIt.value() > bestCount) { bestType = tIt.key(); bestCount = tIt.value(); }
        }
        // simple scoring: recent activity weighted higher
        double score = m.totalLast30 * 3 + m.totalAll * 1 + bestCount * 2;
        scores.append({nav, score, m.totalLast30, m.totalAll, bestType, bestCount});
    }

    std::sort(scores.begin(), scores.end(), [](const ZoneScore &a, const ZoneScore &b){ return a.score > b.score; });

    // Try to read real coordinates (lat/lon) from `navire` table if available.
    QMap<QString, QPointF> coordsLatLon; // x=lon, y=lat
    QSqlQuery infoQ;
    bool hasLat = false, hasLon = false;
    if (infoQ.exec("PRAGMA table_info(navire)")) {
        while (infoQ.next()) {
            QString col = infoQ.value(1).toString().toLower();
            if (col == "lat" || col == "latitude") hasLat = true;
            if (col == "lon" || col == "lng" || col == "longitude") hasLon = true;
        }
    }

    if (hasLat && hasLon) {
        QSqlQuery q2;
        if (q2.exec("SELECT nom, lat, lon FROM navire")) {
            while (q2.next()) {
                QString nom = q2.value(0).toString();
                double lat = q2.value(1).toDouble();
                double lon = q2.value(2).toDouble();
                coordsLatLon[nom] = QPointF(lon, lat);
            }
        }
    }

    // Local lightweight zone representation used for both 3D and 2D dialogs
    struct LocalZone { QString name; double lat; double lon; QString reason; };
    QList<LocalZone> localZones;
    int limit = qMin(3, scores.size());
    for (int i = 0; i < limit; ++i) {
        const ZoneScore &z = scores[i];
        QString reason = QString("Activite recente: %1, Total: %2, Poisson frequent: %3 (%4)")
                         .arg(z.totalLast30).arg(z.totalAll).arg(z.topType.isEmpty() ? QString("N/A") : z.topType).arg(z.topTypeCount);

        LocalZone lz;
        lz.name = z.nav;
        lz.reason = reason;
        if (coordsLatLon.contains(z.nav)) {
            lz.lon = coordsLatLon[z.nav].x();
            lz.lat = coordsLatLon[z.nav].y();
        } else {
            double baseLon = 10.0 + (i + 1) * 0.5;
            double baseLat = 36.5 + (i + 1) * 0.3;
            lz.lon = baseLon; lz.lat = baseLat;
        }
        localZones.append(lz);
    }

    if (localZones.isEmpty()) {
        QMessageBox::information(this, "Zones", "Aucune capture disponible pour recommander des zones.");
        return;
    }

#ifdef HAVE_QT_WEBENGINE
    // Convert to ZoneEntry3D and open WebEngine 3D dialog
    QList<ZoneEntry3D> entries3d;
    for (const LocalZone &lz : localZones) {
        ZoneEntry3D e; e.name = lz.name; e.lat = lz.lat; e.lon = lz.lon; e.reason = lz.reason;
        entries3d.append(e);
    }
    FishingZones3DDialog dlg(entries3d, this);
    dlg.exec();
#else
    // Fallback: convert to 2D ZoneEntry and show the existing 2D dialog
    QList<ZoneEntry> entries2d;
    for (const LocalZone &lz : localZones) {
        ZoneEntry e; e.name = lz.name; e.pos = QPointF(lz.lon, lz.lat); e.reason = lz.reason;
        entries2d.append(e);
    }
    FishingZonesDialog dlg(entries2d, this);
    dlg.exec();
#endif
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

    QSet<QString> types;

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
                if (col == 3) types.insert(item->text());
                table->setItem(row, col, item);
            }
        }
        
        // Actions column with edit button
        QWidget *actionWidget = new QWidget();
        QHBoxLayout *actionLayout = new QHBoxLayout(actionWidget);
        actionLayout->setContentsMargins(2, 2, 2, 2);
        actionLayout->setSpacing(4);

        QPushButton *btnEdit = new QPushButton("✏️");
        btnEdit->setStyleSheet("QPushButton { background-color: #2196F3; color: white; border-radius: 4px; padding: 4px; }");
        btnEdit->setMaximumWidth(40);
        int recordId = q.value(0).toInt();
        connect(btnEdit, &QPushButton::clicked, this, [this, recordId]() { editCaptureById(recordId); });
        actionLayout->addWidget(btnEdit);

        QPushButton *btnDelete = new QPushButton("🗑️");
        btnDelete->setStyleSheet("QPushButton { background-color: #F44336; color: white; border-radius: 4px; padding: 4px; }");
        btnDelete->setMaximumWidth(40);
        connect(btnDelete, &QPushButton::clicked, this, [this, recordId]() {
            // Logic to delete by ID could be added here similar to editCaptureById
            QMessageBox::StandardButton reply = QMessageBox::question(this, "Confirmer", "Supprimer cette capture ?");
            if (reply == QMessageBox::Yes) {
                QSqlQuery dq;
                dq.prepare("DELETE FROM captures WHERE ID_Capture = ?");
                dq.addBindValue(recordId);
                dq.exec();
                loadData();
            }
        });
        actionLayout->addWidget(btnDelete);

        QPushButton *btnView = new QPushButton("👀");
        btnView->setStyleSheet("QPushButton { background-color: #ffffff; border: 1px solid #ccc; border-radius: 4px; padding: 4px; }");
        btnView->setMaximumWidth(40);
        actionLayout->addWidget(btnView);

        table->setCellWidget(row, 5, actionWidget);
    }
    table->setColumnHidden(0, false);
    updateStats();

    // populate type filter combobox with distinct types
    if (comboTypeFilter) {
        comboTypeFilter->blockSignals(true);
        comboTypeFilter->clear();
        comboTypeFilter->addItem("Tous");
        QList<QString> listTypes = types.values();
        std::sort(listTypes.begin(), listTypes.end());
        for (const QString &t : listTypes) comboTypeFilter->addItem(t);
        comboTypeFilter->blockSignals(false);
    }
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

void CapturesDialog::editCaptureById(int id)
{
    if (id < 0) {
        QMessageBox::warning(this, "Selection requise", "Identifiant invalide pour la modification.");
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
