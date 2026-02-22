/****************************************************************************
** Meta object code from reading C++ file 'gestionquai.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.7.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../gestionquai.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'gestionquai.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.7.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSMonCamembertENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSMonCamembertENDCLASS = QtMocHelpers::stringData(
    "MonCamembert"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSMonCamembertENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

Q_CONSTINIT const QMetaObject MonCamembert::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CLASSMonCamembertENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSMonCamembertENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSMonCamembertENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MonCamembert, std::true_type>
    >,
    nullptr
} };

void MonCamembert::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *MonCamembert::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MonCamembert::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSMonCamembertENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int MonCamembert::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSMonHistogrammeENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSMonHistogrammeENDCLASS = QtMocHelpers::stringData(
    "MonHistogramme"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSMonHistogrammeENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

Q_CONSTINIT const QMetaObject MonHistogramme::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CLASSMonHistogrammeENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSMonHistogrammeENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSMonHistogrammeENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MonHistogramme, std::true_type>
    >,
    nullptr
} };

void MonHistogramme::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *MonHistogramme::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MonHistogramme::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSMonHistogrammeENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int MonHistogramme::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSGestionQuaiENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSGestionQuaiENDCLASS = QtMocHelpers::stringData(
    "GestionQuai",
    "showAjouter",
    "",
    "showModifier",
    "showSupprimer",
    "showConsulter",
    "showStatistiques",
    "showProposition",
    "showSimulation",
    "showExportCSV",
    "ajouterQuai",
    "supprimerQuai",
    "remplirFormulaireDepuisTable",
    "row",
    "col",
    "modifierQuai",
    "rechercherQuai",
    "texte",
    "trierQuais",
    "index",
    "trouverMeilleurQuai",
    "lancerSimulation",
    "exporterCSV",
    "genererScenarioAleatoire",
    "onPlacesChanged",
    "onNaviresChanged",
    "onTravauxChanged"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSGestionQuaiENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      21,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  140,    2, 0x08,    1 /* Private */,
       3,    0,  141,    2, 0x08,    2 /* Private */,
       4,    0,  142,    2, 0x08,    3 /* Private */,
       5,    0,  143,    2, 0x08,    4 /* Private */,
       6,    0,  144,    2, 0x08,    5 /* Private */,
       7,    0,  145,    2, 0x08,    6 /* Private */,
       8,    0,  146,    2, 0x08,    7 /* Private */,
       9,    0,  147,    2, 0x08,    8 /* Private */,
      10,    0,  148,    2, 0x08,    9 /* Private */,
      11,    0,  149,    2, 0x08,   10 /* Private */,
      12,    2,  150,    2, 0x08,   11 /* Private */,
      15,    0,  155,    2, 0x08,   14 /* Private */,
      16,    1,  156,    2, 0x08,   15 /* Private */,
      18,    1,  159,    2, 0x08,   17 /* Private */,
      20,    0,  162,    2, 0x08,   19 /* Private */,
      21,    0,  163,    2, 0x08,   20 /* Private */,
      22,    0,  164,    2, 0x08,   21 /* Private */,
      23,    0,  165,    2, 0x08,   22 /* Private */,
      24,    1,  166,    2, 0x08,   23 /* Private */,
      25,    1,  169,    2, 0x08,   25 /* Private */,
      26,    1,  172,    2, 0x08,   27 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   13,   14,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   17,
    QMetaType::Void, QMetaType::Int,   19,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::Int,    2,

       0        // eod
};

Q_CONSTINIT const QMetaObject GestionQuai::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CLASSGestionQuaiENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSGestionQuaiENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSGestionQuaiENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<GestionQuai, std::true_type>,
        // method 'showAjouter'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'showModifier'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'showSupprimer'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'showConsulter'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'showStatistiques'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'showProposition'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'showSimulation'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'showExportCSV'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'ajouterQuai'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'supprimerQuai'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'remplirFormulaireDepuisTable'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'modifierQuai'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'rechercherQuai'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'trierQuais'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'trouverMeilleurQuai'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'lancerSimulation'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'exporterCSV'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'genererScenarioAleatoire'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onPlacesChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onNaviresChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onTravauxChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>
    >,
    nullptr
} };

void GestionQuai::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<GestionQuai *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->showAjouter(); break;
        case 1: _t->showModifier(); break;
        case 2: _t->showSupprimer(); break;
        case 3: _t->showConsulter(); break;
        case 4: _t->showStatistiques(); break;
        case 5: _t->showProposition(); break;
        case 6: _t->showSimulation(); break;
        case 7: _t->showExportCSV(); break;
        case 8: _t->ajouterQuai(); break;
        case 9: _t->supprimerQuai(); break;
        case 10: _t->remplirFormulaireDepuisTable((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 11: _t->modifierQuai(); break;
        case 12: _t->rechercherQuai((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 13: _t->trierQuais((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 14: _t->trouverMeilleurQuai(); break;
        case 15: _t->lancerSimulation(); break;
        case 16: _t->exporterCSV(); break;
        case 17: _t->genererScenarioAleatoire(); break;
        case 18: _t->onPlacesChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 19: _t->onNaviresChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 20: _t->onTravauxChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject *GestionQuai::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GestionQuai::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSGestionQuaiENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int GestionQuai::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 21)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 21;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 21)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 21;
    }
    return _id;
}
QT_WARNING_POP
