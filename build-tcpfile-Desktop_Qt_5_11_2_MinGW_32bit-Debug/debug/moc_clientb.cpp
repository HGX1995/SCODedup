/****************************************************************************
** Meta object code from reading C++ file 'clientb.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../tcpfile/clientb.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'clientb.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ClientB_t {
    QByteArrayData data[10];
    char stringdata0[132];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ClientB_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ClientB_t qt_meta_stringdata_ClientB = {
    {
QT_MOC_LITERAL(0, 0, 7), // "ClientB"
QT_MOC_LITERAL(1, 8, 22), // "on_Btn_getfile_clicked"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 16), // "acceptConnection"
QT_MOC_LITERAL(4, 49, 10), // "acceptData"
QT_MOC_LITERAL(5, 60, 9), // "sendToken"
QT_MOC_LITERAL(6, 70, 6), // "getPOW"
QT_MOC_LITERAL(7, 77, 8), // "download"
QT_MOC_LITERAL(8, 86, 21), // "on_Btn_upload_clicked"
QT_MOC_LITERAL(9, 108, 23) // "on_Btn_download_clicked"

    },
    "ClientB\0on_Btn_getfile_clicked\0\0"
    "acceptConnection\0acceptData\0sendToken\0"
    "getPOW\0download\0on_Btn_upload_clicked\0"
    "on_Btn_download_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ClientB[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   54,    2, 0x08 /* Private */,
       3,    0,   55,    2, 0x08 /* Private */,
       4,    0,   56,    2, 0x08 /* Private */,
       5,    0,   57,    2, 0x08 /* Private */,
       6,    0,   58,    2, 0x08 /* Private */,
       7,    0,   59,    2, 0x08 /* Private */,
       8,    0,   60,    2, 0x08 /* Private */,
       9,    0,   61,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ClientB::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ClientB *_t = static_cast<ClientB *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_Btn_getfile_clicked(); break;
        case 1: _t->acceptConnection(); break;
        case 2: _t->acceptData(); break;
        case 3: _t->sendToken(); break;
        case 4: _t->getPOW(); break;
        case 5: _t->download(); break;
        case 6: _t->on_Btn_upload_clicked(); break;
        case 7: _t->on_Btn_download_clicked(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject ClientB::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ClientB.data,
      qt_meta_data_ClientB,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *ClientB::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ClientB::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ClientB.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int ClientB::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
