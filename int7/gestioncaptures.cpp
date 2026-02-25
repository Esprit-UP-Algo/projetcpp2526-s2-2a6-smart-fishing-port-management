#include "gestioncaptures.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGridLayout>
#include <QHeaderView>
#include <QFileDialog>
#include <QTextStream>
#include <QDesktopServices>

GestionCaptures::GestionCaptures(QWidget *parent) : QWidget(parent), modeModification(false)
{
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataDir);
    m_jsonPath = QDir(dataDir).filePath("captures.json");

    configurerInterface();
    chargerCaptures();
    mettreAJourStatistiques();
}

GestionCaptures::~GestionCaptures()
{
    sauvegarderCaptures();
}

void GestionCaptures::configurerInterface()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Barre d'outils (match gestionutilisateurs)
    QHBoxLayout *toolbar = new QHBoxLayout;
    toolbar->setContentsMargins(8, 8, 8, 8);
    toolbar->setSpacing(8);

    btnAjouter = new QPushButton("➕ Ajouter");
    btnModifier = new QPushButton("✏️ Modifier");
    btnSupprimer = new QPushButton("🗑️ Supprimer");
    btnConsulter = new QPushButton("📋 Consulter");
    btnExporter = new QPushButton("📥 Exporter CSV");
    btnActualiser = new QPushButton("🔄 Actualiser");

    QList<QPushButton*> toolButtons = {btnAjouter, btnModifier, btnSupprimer, btnConsulter, btnExporter, btnActualiser};
    foreach(QPushButton *btn, toolButtons) {
        btn->setMinimumHeight(35);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet("background-color: #3b82f6; color: white; border-radius: 5px; padding: 6px 12px; font-weight: bold;");
        toolbar->addWidget(btn);
    }
    toolbar->addStretch();

    mainLayout->addLayout(toolbar);

    // Cadre principal
    QWidget *mainFrame = new QWidget;
    mainFrame->setObjectName("pageFrame");
    mainFrame->setStyleSheet("background: #1e3a8a; border-radius: 8px; margin: 8px;");

    pages = new QStackedWidget(mainFrame);

    configurerTableauBord();
    configurerFormulaire();

    pages->addWidget(pageTableauBord);
    pages->addWidget(pageFormulaire);

    QVBoxLayout *frameLayout = new QVBoxLayout(mainFrame);
    frameLayout->addWidget(pages);
    frameLayout->setContentsMargins(15, 15, 15, 15);

    mainLayout->addWidget(mainFrame);

    connect(btnAjouter, &QPushButton::clicked, this, &GestionCaptures::viderFormulaire);
    connect(btnModifier, &QPushButton::clicked, this, &GestionCaptures::modifierCapture);
    connect(btnSupprimer, &QPushButton::clicked, this, &GestionCaptures::supprimerCapture);
    connect(btnConsulter, &QPushButton::clicked, this, &GestionCaptures::afficherStatistiques);
    connect(btnExporter, &QPushButton::clicked, this, &GestionCaptures::exporterCSV);
    connect(btnActualiser, &QPushButton::clicked, this, &GestionCaptures::actualiserTable);

    appliquerStyles();
    afficherStatistiques();
}

void GestionCaptures::configurerTableauBord()
{
    pageTableauBord = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(pageTableauBord);
        layout->setContentsMargins(8, 0, 8, 8);
        layout->setSpacing(0);

    QLabel *titre = new QLabel("Gestion des captures de poissons");
    titre->setStyleSheet("font-size: 22px; font-weight: bold; color: #1e3a8a;");

    QFrame *frameStats = new QFrame;
    frameStats->setStyleSheet("background-color: white; border-radius: 8px; border: 1px solid #e2e8f0; padding: 8px;");

    QVBoxLayout *statsInnerLayout = new QVBoxLayout(frameStats);
    statsInnerLayout->setContentsMargins(0,0,0,0);
    statsInnerLayout->setSpacing(4);

    QLabel *titreStats = new QLabel("📊 Statistiques des captures");
        titreStats->setStyleSheet("font-size: 16px; font-weight: bold; color: #1e293b;");

    QHBoxLayout *statsLayout = new QHBoxLayout;
        statsLayout->setSpacing(4);

    // compact stat widgets: numeric label (member) + description label
    auto makeStatWidget = [&](QLabel *&valueLabel, const QString &desc, const QString &color) -> QWidget* {
        QWidget *w = new QWidget;
        QVBoxLayout *l = new QVBoxLayout(w);
            l->setContentsMargins(4,4,4,4);
            l->setSpacing(2);

        valueLabel = new QLabel("0");
        valueLabel->setAlignment(Qt::AlignCenter);
        valueLabel->setStyleSheet(QString("font-size: 18px; font-weight: bold; color: %1;").arg(color));

        QLabel *descLabel = new QLabel(desc);
        descLabel->setAlignment(Qt::AlignCenter);
        descLabel->setStyleSheet("font-size: 13px; color: #64748b;");

        l->addWidget(valueLabel);
        l->addWidget(descLabel);

        w->setStyleSheet("background: #f1f5f9; border-radius: 6px; padding: 4px; border-left: 4px solid #3b82f6;");
        return w;
    };

    statsLayout->addWidget(makeStatWidget(labelNbTotal, "📊 Total", "#1e3a8a"));
    statsLayout->addWidget(makeStatWidget(labelNbPoisson, "🐟 Poissons", "#0891b2"));
    statsLayout->addWidget(makeStatWidget(labelNbCrustace, "🦐 Crustacés", "#f59e0b"));
    statsLayout->addWidget(makeStatWidget(labelNbMollusque, "🐚 Mollusques", "#10b981"));
    statsLayout->addStretch();

    statsInnerLayout->addWidget(titreStats);
    statsInnerLayout->addLayout(statsLayout);

    QHBoxLayout *actionsLayout = new QHBoxLayout;

    lineRecherche = new QLineEdit;
    lineRecherche->setPlaceholderText("🔍 Rechercher par navire, type de poisson ou agent...");
        lineRecherche->setFixedWidth(280);
        lineRecherche->setStyleSheet("padding: 6px; border: 1px solid #cbd5e1; border-radius: 6px; background: white;");

    actionsLayout->addWidget(lineRecherche);
    actionsLayout->addStretch();

    tableCaptures = new QTableWidget;
    tableCaptures->setColumnCount(10);
    QStringList headers = {"ID", "Navire", "Type", "Icône", "Quantité", "Date", "Heure", "Agent", "Statut", "Actions"};
    tableCaptures->setHorizontalHeaderLabels(headers);
    tableCaptures->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableCaptures->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableCaptures->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableCaptures->verticalHeader()->setVisible(false);
    tableCaptures->setMinimumHeight(250);
    tableCaptures->setStyleSheet("background: white; border: 1px solid #e2e8f0; border-radius: 6px;");

    layout->addWidget(titre);
    layout->addWidget(frameStats);
    layout->addLayout(actionsLayout);
    layout->addWidget(tableCaptures);

    connect(lineRecherche, &QLineEdit::textChanged, this, &GestionCaptures::rechercherCapture);
}

void GestionCaptures::configurerFormulaire()
{
    pageFormulaire = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(pageFormulaire);
        layout->setContentsMargins(8, 8, 8, 8); // réduit
        layout->setSpacing(8); // réduit

    QLabel *titre = new QLabel("➕ Ajouter une capture");
    titre->setStyleSheet("font-size: 20px; font-weight: bold; color: #1e3a8a;"); // réduit
    titre->setObjectName("titreFormulaire");

    QFrame *formFrame = new QFrame;
    formFrame->setStyleSheet("background-color: white; border-radius: 8px; border: 1px solid #e2e8f0; padding: 20px;"); // réduit

    QGridLayout *formLayout = new QGridLayout(formFrame);
        formLayout->setHorizontalSpacing(10); // réduit
        formLayout->setVerticalSpacing(5); // réduit

    editCaptureId = new QLineEdit;
    editCaptureId->setVisible(false);

    formLayout->addWidget(new QLabel("🚢 Navire:"), 0, 0);
    editShipName = new QLineEdit;
    editShipName->setPlaceholderText("Nom du navire");
    editShipName->setMinimumHeight(35); // réduit
    formLayout->addWidget(editShipName, 0, 1);

    formLayout->addWidget(new QLabel("👤 Agent:"), 0, 2);
    editAgent = new QLineEdit;
    editAgent->setPlaceholderText("Nom de l'agent");
    editAgent->setMinimumHeight(35);
    formLayout->addWidget(editAgent, 0, 3);

    formLayout->addWidget(new QLabel("🐟 Type:"), 1, 0);
    comboFishType = new QComboBox;
    comboFishType->addItems({"Poisson", "Crustacé", "Mollusque", "Requin", "Thon", "Saumon", "Bar", "Crabe", "Homard", "Crevette", "Huître", "Moule"});
    comboFishType->setMinimumHeight(35);
    formLayout->addWidget(comboFishType, 1, 1);

    formLayout->addWidget(new QLabel("🎯 Icône:"), 1, 2);
    editFishIcon = new QLineEdit;
    editFishIcon->setText("🐟");
    editFishIcon->setMinimumHeight(35);
    editFishIcon->setMaximumWidth(60); // réduit
    formLayout->addWidget(editFishIcon, 1, 3);

    formLayout->addWidget(new QLabel("⚖️ Quantité:"), 2, 0);
    spinQuantity = new QSpinBox;
    spinQuantity->setRange(1, 10000);
    spinQuantity->setValue(100);
    spinQuantity->setSuffix(" kg");
    spinQuantity->setMinimumHeight(35);
    formLayout->addWidget(spinQuantity, 2, 1);

    formLayout->addWidget(new QLabel("📍 Zone:"), 2, 2);
    editZone = new QLineEdit;
    editZone->setPlaceholderText("Zone de pêche");
    editZone->setMinimumHeight(35);
    formLayout->addWidget(editZone, 2, 3);

    formLayout->addWidget(new QLabel("📅 Date:"), 3, 0);
    dateCaptureDate = new QDateEdit;
    dateCaptureDate->setDate(QDate::currentDate());
    dateCaptureDate->setCalendarPopup(true);
    dateCaptureDate->setMinimumHeight(35);
    formLayout->addWidget(dateCaptureDate, 3, 1);

    formLayout->addWidget(new QLabel("⏰ Heure:"), 3, 2);
    editTime = new QLineEdit;
    editTime->setText(QTime::currentTime().toString("HH:mm"));
    editTime->setMinimumHeight(35);
    formLayout->addWidget(editTime, 3, 3);

    formLayout->addWidget(new QLabel("🎣 Méthode:"), 4, 0);
    comboMethod = new QComboBox;
    comboMethod->addItems({"Filet", "Ligne", "Casier", "Chalut", "Palangre", "Harpon", "Autre"});
    comboMethod->setMinimumHeight(35);
    formLayout->addWidget(comboMethod, 4, 1);

    formLayout->addWidget(new QLabel("📊 Statut:"), 4, 2);
    comboStatus = new QComboBox;
    comboStatus->addItems({"Enregistré", "Validé", "En transit", "Livré", "En attente"});
    comboStatus->setMinimumHeight(35);
    formLayout->addWidget(comboStatus, 4, 3);

    formLayout->addWidget(new QLabel("🗺️ Latitude:"), 5, 0);
    spinLatitude = new QDoubleSpinBox;
    spinLatitude->setRange(-90, 90);
    spinLatitude->setDecimals(6);
    spinLatitude->setValue(48.8566);
    spinLatitude->setMinimumHeight(35);
    formLayout->addWidget(spinLatitude, 5, 1);

    formLayout->addWidget(new QLabel("🗺️ Longitude:"), 5, 2);
    spinLongitude = new QDoubleSpinBox;
    spinLongitude->setRange(-180, 180);
    spinLongitude->setDecimals(6);
    spinLongitude->setValue(2.3522);
    spinLongitude->setMinimumHeight(35);
    formLayout->addWidget(spinLongitude, 5, 3);

    formLayout->addWidget(new QLabel("💬 Commentaires:"), 6, 0);
    textComments = new QTextEdit;
    textComments->setMaximumHeight(80); // réduit
    textComments->setPlaceholderText("Commentaires supplémentaires...");
    formLayout->addWidget(textComments, 6, 1, 1, 3);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnSave = new QPushButton("💾 Enregistrer");
    btnCancel = new QPushButton("❌ Annuler");

    btnSave->setMinimumHeight(40); // réduit
    btnCancel->setMinimumHeight(40);
    btnSave->setCursor(Qt::PointingHandCursor);
    btnCancel->setCursor(Qt::PointingHandCursor);
        btnSave->setStyleSheet("background-color: #3b82f6; color: white; font-weight: bold; font-size: 14px; padding: 6px; border-radius: 6px;"); // réduit
    btnCancel->setStyleSheet("background-color: #ef4444; color: white; font-weight: bold; font-size: 14px; padding: 8px; border-radius: 6px;");

    btnLayout->addStretch();
    btnLayout->addWidget(btnSave);
    btnLayout->addWidget(btnCancel);
    btnLayout->addStretch();

    layout->addWidget(titre);
    layout->addWidget(formFrame);
    layout->addLayout(btnLayout);
    layout->addStretch();

    connect(btnSave, &QPushButton::clicked, this, &GestionCaptures::ajouterCapture);
    connect(btnCancel, &QPushButton::clicked, this, &GestionCaptures::afficherStatistiques);
}

void GestionCaptures::chargerCaptures()
{
    listeCaptures.clear();

    QFile file(m_jsonPath);
    if (!file.exists()) {
        Capture c1;
        c1.shipName = "Neptune";
        c1.fishType = "Thon";
        c1.quantity = 250;
        c1.agent = "Jean";
        listeCaptures.append(c1);

        Capture c2;
        c2.shipName = "Atlantis";
        c2.fishType = "Crabe";
        c2.quantity = 120;
        c2.agent = "Marie";
        c2.status = "Validé";
        listeCaptures.append(c2);

        Capture c3;
        c3.shipName = "Océan";
        c3.fishType = "Saumon";
        c3.quantity = 180;
        c3.agent = "Pierre";
        c3.status = "En transit";
        listeCaptures.append(c3);

        sauvegarderCaptures();
        return;
    }

    if (!file.open(QIODevice::ReadOnly)) return;

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isArray()) return;

    QJsonArray array = doc.array();
    for (const QJsonValue &val : array) {
        if (val.isObject()) {
            Capture c;
            c.fromJson(val.toObject());
            listeCaptures.append(c);
        }
    }
}

void GestionCaptures::sauvegarderCaptures()
{
    QJsonArray array;
    for (const Capture &c : listeCaptures) {
        array.append(c.toJson());
    }

    QJsonDocument doc(array);
    QFile file(m_jsonPath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
    }
}

void GestionCaptures::actualiserTable()
{
    if (!tableCaptures) return;

    tableCaptures->setRowCount(0);
    for (int idx = 0; idx < listeCaptures.size(); idx++) {
        const Capture &c = listeCaptures[idx];
        int row = tableCaptures->rowCount();
        tableCaptures->insertRow(row);

        tableCaptures->setItem(row, 0, new QTableWidgetItem(c.captureId.left(8)));
        tableCaptures->setItem(row, 1, new QTableWidgetItem(c.shipName));
        tableCaptures->setItem(row, 2, new QTableWidgetItem(c.fishType));
        tableCaptures->setItem(row, 3, new QTableWidgetItem(c.fishIcon));
        tableCaptures->setItem(row, 4, new QTableWidgetItem(QString::number(c.quantity) + " kg"));
        tableCaptures->setItem(row, 5, new QTableWidgetItem(c.captureDate.toString("dd/MM/yyyy")));
        tableCaptures->setItem(row, 6, new QTableWidgetItem(c.time));
        tableCaptures->setItem(row, 7, new QTableWidgetItem(c.agent));

        QTableWidgetItem *statusItem = new QTableWidgetItem(c.status);
        statusItem->setTextAlignment(Qt::AlignCenter);
        if (c.status == "Enregistré") {
            statusItem->setBackground(QColor("#fef3c7"));
            statusItem->setForeground(QColor("#92400e"));
        } else if (c.status == "Validé") {
            statusItem->setBackground(QColor("#d1fae5"));
            statusItem->setForeground(QColor("#065f46"));
        } else if (c.status == "En transit") {
            statusItem->setBackground(QColor("#dbeafe"));
            statusItem->setForeground(QColor("#1e40af"));
        } else if (c.status == "Livré") {
            statusItem->setBackground(QColor("#e0e7ff"));
            statusItem->setForeground(QColor("#4338ca"));
        }
        tableCaptures->setItem(row, 8, statusItem);

        QWidget *actionWidget = new QWidget;
        QHBoxLayout *actionLayout = new QHBoxLayout(actionWidget);
        actionLayout->setContentsMargins(3, 3, 3, 3);
        actionLayout->setSpacing(5);

        QPushButton *btnEdit = new QPushButton("✏️");
        btnEdit->setToolTip("Modifier");
        btnEdit->setMaximumWidth(35);
        btnEdit->setMinimumHeight(30);
        btnEdit->setStyleSheet("background-color: #3b82f6; color: white; border-radius: 5px; font-size: 14px; padding: 3px;");
        btnEdit->setCursor(Qt::PointingHandCursor);

        QPushButton *btnDelete = new QPushButton("🗑️");
        btnDelete->setToolTip("Supprimer");
        btnDelete->setMaximumWidth(35);
        btnDelete->setMinimumHeight(30);
        btnDelete->setStyleSheet("background-color: #ef4444; color: white; border-radius: 5px; font-size: 14px; padding: 3px;");
        btnDelete->setCursor(Qt::PointingHandCursor);

        actionLayout->addWidget(btnEdit);
        actionLayout->addWidget(btnDelete);
        actionLayout->addStretch();

        tableCaptures->setCellWidget(row, 9, actionWidget);
        tableCaptures->setRowHeight(row, 40);

        connect(btnEdit, &QPushButton::clicked, this, [this, idx]() {
            if (idx >= 0 && idx < listeCaptures.size()) {
                // Popup to edit capture
                QMessageBox::information(this, "Modifier", "Modification de la capture " + listeCaptures[idx].captureId);
            }
        });

        connect(btnDelete, &QPushButton::clicked, this, [this, idx]() {
            if (idx >= 0 && idx < listeCaptures.size() &&
                QMessageBox::question(this, "Confirmation", "Voulez-vous vraiment supprimer cette capture ?",
                                    QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
                listeCaptures.removeAt(idx);
                sauvegarderCaptures();
                actualiserTable();
                QMessageBox::information(this, "Succès", "Capture supprimée avec succès");
            }
        });
    }

    mettreAJourStatistiques();
}

void GestionCaptures::mettreAJourStatistiques()
{
    int total = listeCaptures.size();
    int poisson = 0, crustace = 0, mollusque = 0;

    for (const Capture &c : listeCaptures) {
        QString type = c.fishType.toLower();
        if (type == "poisson")
            poisson++;
        else if (type == "crustace")
            crustace++;
        else if (type == "mollusque")
            mollusque++;
    }

    if (labelNbTotal) labelNbTotal->setText(QString::number(total));
    if (labelNbPoisson) labelNbPoisson->setText(QString::number(poisson));
    if (labelNbCrustace) labelNbCrustace->setText(QString::number(crustace));
    if (labelNbMollusque) labelNbMollusque->setText(QString::number(mollusque));
}

GestionCaptures::CapturesStats GestionCaptures::getStats() const {
    GestionCaptures::CapturesStats s{0,0,0,0};
    s.total = listeCaptures.size();
    for (const Capture &c : listeCaptures) {
        QString type = c.fishType.toLower();
        if (type == "poisson") s.poisson++;
        else if (type == "crustace") s.crustace++;
        else if (type == "mollusque") s.mollusque++;
    }
    return s;
}

QString GestionCaptures::genererCaptureId()
{
    return "CAP-" + QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss-") +
           QString::number(QRandomGenerator::global()->bounded(1000, 9999));
}

void GestionCaptures::viderFormulaire()
{
    modeModification = false;
    currentCaptureId.clear();

    editCaptureId->setText(genererCaptureId());
    editShipName->clear();
    comboFishType->setCurrentIndex(0);
    editFishIcon->setText("🐟");
    spinQuantity->setValue(100);
    dateCaptureDate->setDate(QDate::currentDate());
    editTime->setText(QTime::currentTime().toString("HH:mm"));
    editAgent->clear();
    comboStatus->setCurrentIndex(0);
    spinLatitude->setValue(48.8566);
    spinLongitude->setValue(2.3522);
    editZone->clear();
    comboMethod->setCurrentIndex(0);
    textComments->clear();

    QLabel *titre = pageFormulaire->findChild<QLabel*>("titreFormulaire");
    if (titre) titre->setText("➕ Ajouter une capture");
    btnSave->setText("💾 Enregistrer");

    pages->setCurrentWidget(pageFormulaire);
}

void GestionCaptures::ajouterCapture()
{
    if (editShipName->text().trimmed().isEmpty() || editAgent->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez remplir le nom du navire et de l'agent.");
        return;
    }

    Capture c;
    c.captureId = editCaptureId->text().isEmpty() ? genererCaptureId() : editCaptureId->text();
    c.shipName = editShipName->text().trimmed();
    c.fishType = comboFishType->currentText();
    c.fishIcon = editFishIcon->text().trimmed();
    c.quantity = spinQuantity->value();
    c.captureDate = dateCaptureDate->date();
    c.time = editTime->text().trimmed();
    c.agent = editAgent->text().trimmed();
    c.status = comboStatus->currentText();
    c.latitude = spinLatitude->value();
    c.longitude = spinLongitude->value();
    c.zone = editZone->text().trimmed();
    c.method = comboMethod->currentText();
    c.comments = textComments->toPlainText().trimmed();

    if (modeModification) {
        for (int i = 0; i < listeCaptures.size(); i++) {
            if (listeCaptures[i].captureId == currentCaptureId) {
                listeCaptures[i] = c;
                break;
            }
        }
    } else {
        listeCaptures.append(c);
    }

    sauvegarderCaptures();
    QMessageBox::information(this, "Succès", modeModification ? "Capture modifiée !" : "Capture ajoutée !");
    afficherStatistiques();
    actualiserTable();
}

void GestionCaptures::loadFromDb()
{
    listeCaptures.clear();
    QSqlQuery q;
    if (!q.exec("SELECT captureId, shipName, fishType, quantity, captureDate, agent FROM captures")) {
        qDebug() << "Captures load error:" << q.lastError().text();
        return;
    }
    while (q.next()) {
        Capture c;
        c.captureId = q.value(0).toString();
        c.shipName = q.value(1).toString();
        c.fishType = q.value(2).toString();
        c.quantity = q.value(3).toInt();
        c.captureDate = q.value(4).toDate();
        c.agent = q.value(5).toString();
        listeCaptures.append(c);
    }
    actualiserTable();
    mettreAJourStatistiques();
}

void GestionCaptures::modifierCapture()
{
    int row = tableCaptures->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Attention", "Sélectionnez une capture.");
        return;
    }

    QString prefix = tableCaptures->item(row, 0)->text();
    for (const Capture &c : listeCaptures) {
        if (c.captureId.startsWith(prefix) || c.captureId.contains(prefix)) {
            modeModification = true;
            currentCaptureId = c.captureId;

            editCaptureId->setText(c.captureId);
            editShipName->setText(c.shipName);
            comboFishType->setCurrentText(c.fishType);
            editFishIcon->setText(c.fishIcon);
            spinQuantity->setValue(c.quantity);
            dateCaptureDate->setDate(c.captureDate);
            editTime->setText(c.time);
            editAgent->setText(c.agent);
            comboStatus->setCurrentText(c.status);
            spinLatitude->setValue(c.latitude);
            spinLongitude->setValue(c.longitude);
            editZone->setText(c.zone);
            comboMethod->setCurrentText(c.method);
            textComments->setText(c.comments);

            QLabel *titre = pageFormulaire->findChild<QLabel*>("titreFormulaire");
            if (titre) titre->setText("✏️ Modifier une capture");
            btnSave->setText("✔️ Modifier");

            pages->setCurrentWidget(pageFormulaire);
            return;
        }
    }
}

void GestionCaptures::supprimerCapture()
{
    int row = tableCaptures->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Attention", "Sélectionnez une capture.");
        return;
    }

    QString prefix = tableCaptures->item(row, 0)->text();
    QString shipName = tableCaptures->item(row, 1)->text();

    if (QMessageBox::question(this, "Confirmation", "Supprimer la capture du navire \"" + shipName + "\" ?") == QMessageBox::Yes) {
        for (int i = 0; i < listeCaptures.size(); i++) {
            if (listeCaptures[i].captureId.startsWith(prefix) || listeCaptures[i].captureId.contains(prefix)) {
                listeCaptures.removeAt(i);
                break;
            }
        }
        sauvegarderCaptures();
        actualiserTable();
        QMessageBox::information(this, "Succès", "Capture supprimée !");
    }
}

void GestionCaptures::rechercherCapture(const QString &texte)
{
    for (int i = 0; i < tableCaptures->rowCount(); i++) {
        bool match = false;
        if (tableCaptures->item(i, 1) && tableCaptures->item(i, 1)->text().contains(texte, Qt::CaseInsensitive))
            match = true;
        if (tableCaptures->item(i, 2) && tableCaptures->item(i, 2)->text().contains(texte, Qt::CaseInsensitive))
            match = true;
        if (tableCaptures->item(i, 7) && tableCaptures->item(i, 7)->text().contains(texte, Qt::CaseInsensitive))
            match = true;
        tableCaptures->setRowHidden(i, !match);
    }
}

void GestionCaptures::exporterCSV()
{
    if (tableCaptures->rowCount() == 0) return;

    QString fileName = QFileDialog::getSaveFileName(this, "Exporter CSV",
                                                    QDir::homePath() + "/captures_" + QDate::currentDate().toString("yyyyMMdd") + ".csv",
                                                    "CSV (*.csv)");

    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);

    for (int col = 0; col < tableCaptures->columnCount(); col++) {
        out << tableCaptures->horizontalHeaderItem(col)->text();
        if (col < tableCaptures->columnCount() - 1) out << ",";
    }
    out << "\n";

    for (int row = 0; row < tableCaptures->rowCount(); row++) {
        if (tableCaptures->isRowHidden(row)) continue;
        for (int col = 0; col < tableCaptures->columnCount(); col++) {
            QString cell = tableCaptures->item(row, col) ? tableCaptures->item(row, col)->text() : "";
            if (cell.contains(",")) cell = "\"" + cell + "\"";
            out << cell;
            if (col < tableCaptures->columnCount() - 1) out << ",";
        }
        out << "\n";
    }

    file.close();
    QMessageBox::information(this, "Succès", "Export CSV terminé !");
}

void GestionCaptures::afficherStatistiques()
{
    actualiserTable();
    pages->setCurrentWidget(pageTableauBord);
}

void GestionCaptures::appliquerStyles()
{
    setStyleSheet(R"(
        QWidget#pageFrame, QFrame#pageFrame { background-color: white; border-radius: 8px; border: 1px solid #e2e8f0; }
        QPushButton { background-color: #3b82f6; color: white; border-radius: 6px; padding: 8px; font-weight: 600; }
        QPushButton:hover { background-color: #2563eb; }
        QLineEdit, QSpinBox, QDoubleSpinBox, QComboBox, QTextEdit {
            padding: 8px;
            border: 1px solid #cbd5e1;
            border-radius: 6px;
            background-color: white;
        }
        QTableWidget {
            border: 1px solid #e2e8f0;
            border-radius: 6px;
            background-color: white;
        }
        QHeaderView::section {
            background-color: #1e293b;
            color: white;
            padding: 8px;
            border: none;
            font-weight: 600;
        }
        QTableWidget::item:selected {
            background-color: #bfdbfe;
            color: #1e3a8a;
        }
        QTableWidget::item:hover {
            background-color: #f1f5f9;
        }
    )");
}
