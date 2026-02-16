/****************************************************************************
** Meta object code from reading C++ file 'gestioncaptures.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.7.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../gestioncaptures.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'gestioncaptures.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSGestionCapturesENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSGestionCapturesENDCLASS = QtMocHelpers::stringData(
    "GestionCaptures",
    "ajouterCapture",
    "",
    "modifierCapture",
    "supprimerCapture",
    "rechercherCapture",
    "texte",
    "actualiserTable",
    "exporterCSV",
    "afficherStatistiques",
    "viderFormulaire"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSGestionCapturesENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   62,    2, 0x08,    1 /* Private */,
       3,    0,   63,    2, 0x08,    2 /* Private */,
       4,    0,   64,    2, 0x08,    3 /* Private */,
       5,    1,   65,    2, 0x08,    4 /* Private */,
       7,    0,   68,    2, 0x08,    6 /* Private */,
       8,    0,   69,    2, 0x08,    7 /* Private */,
       9,    0,   70,    2, 0x08,    8 /* Private */,
      10,    0,   71,    2, 0x08,    9 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject GestionCaptures::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CLASSGestionCapturesENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSGestionCapturesENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSGestionCapturesENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<GestionCaptures, std::true_type>,
        // method 'ajouterCapture'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'modifierCapture'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'supprimerCapture'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'rechercherCapture'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'actualiserTable'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'exporterCSV'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'afficherStatistiques'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'viderFormulaire'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void GestionCaptures::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<GestionCaptures *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->ajouterCapture(); break;
        case 1: _t->modifierCapture(); break;
        case 2: _t->supprimerCapture(); break;
        case 3: _t->rechercherCapture((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: _t->actualiserTable(); break;
        case 5: _t->exporterCSV(); break;
        case 6: _t->afficherStatistiques(); break;
        case 7: _t->viderFormulaire(); break;
        default: ;
        }
    }
}

const QMetaObject *GestionCaptures::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GestionCaptures::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSGestionCapturesENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int GestionCaptures::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 8;
    }
    return _id;
}
QT_WARNING_POP
