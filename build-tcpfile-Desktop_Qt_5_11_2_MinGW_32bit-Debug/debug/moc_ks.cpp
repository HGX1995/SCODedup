/****************************************************************************
** Meta object code from reading C++ file 'ks.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../tcpfile/ks.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ks.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ks_t {
    QByteArrayData data[9];
    char stringdata0[89];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ks_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ks_t qt_meta_stringdata_ks = {
    {
QT_MOC_LITERAL(0, 0, 2), // "ks"
QT_MOC_LITERAL(1, 3, 20), // "acceptDataConnection"
QT_MOC_LITERAL(2, 24, 0), // ""
QT_MOC_LITERAL(3, 25, 12), // "receviceData"
QT_MOC_LITERAL(4, 38, 18), // "updateFileProgress"
QT_MOC_LITERAL(5, 57, 8), // "numBytes"
QT_MOC_LITERAL(6, 66, 8), // "sendFile"
QT_MOC_LITERAL(7, 75, 8), // "filename"
QT_MOC_LITERAL(8, 84, 4) // "port"

    },
    "ks\0acceptDataConnection\0\0receviceData\0"
    "updateFileProgress\0numBytes\0sendFile\0"
    "filename\0port"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ks[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   34,    2, 0x0a /* Public */,
       3,    0,   35,    2, 0x0a /* Public */,
       4,    1,   36,    2, 0x0a /* Public */,
       6,    2,   39,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::LongLong,    5,
    QMetaType::Void, QMetaType::QString, QMetaType::UShort,    7,    8,

       0        // eod
};

void ks::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ks *_t = static_cast<ks *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->acceptDataConnection(); break;
        case 1: _t->receviceData(); break;
        case 2: _t->updateFileProgress((*reinterpret_cast< qint64(*)>(_a[1]))); break;
        case 3: _t->sendFile((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< quint16(*)>(_a[2]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ks::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_ks.data,
      qt_meta_data_ks,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *ks::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ks::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ks.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ks::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
