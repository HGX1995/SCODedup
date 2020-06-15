/****************************************************************************
** Meta object code from reading C++ file 'clientwidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../tcpfile/clientwidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'clientwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ClientWidget_t {
    QByteArrayData data[19];
    char stringdata0[243];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ClientWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ClientWidget_t qt_meta_stringdata_ClientWidget = {
    {
QT_MOC_LITERAL(0, 0, 12), // "ClientWidget"
QT_MOC_LITERAL(1, 13, 20), // "acceptFileConnection"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 18), // "acceptKsConnection"
QT_MOC_LITERAL(4, 54, 8), // "acceptKs"
QT_MOC_LITERAL(5, 63, 6), // "sendKS"
QT_MOC_LITERAL(6, 70, 10), // "selectFile"
QT_MOC_LITERAL(7, 81, 9), // "sendToken"
QT_MOC_LITERAL(8, 91, 18), // "updateFileProgress"
QT_MOC_LITERAL(9, 110, 21), // "on_pushButton_clicked"
QT_MOC_LITERAL(10, 132, 8), // "sendAttr"
QT_MOC_LITERAL(11, 141, 7), // "sendCSP"
QT_MOC_LITERAL(12, 149, 8), // "sendFile"
QT_MOC_LITERAL(13, 158, 8), // "filename"
QT_MOC_LITERAL(14, 167, 8), // "numBytes"
QT_MOC_LITERAL(15, 176, 11), // "generateSCO"
QT_MOC_LITERAL(16, 188, 6), // "getPOW"
QT_MOC_LITERAL(17, 195, 23), // "on_pushButton_2_clicked"
QT_MOC_LITERAL(18, 219, 23) // "on_pushButton_3_clicked"

    },
    "ClientWidget\0acceptFileConnection\0\0"
    "acceptKsConnection\0acceptKs\0sendKS\0"
    "selectFile\0sendToken\0updateFileProgress\0"
    "on_pushButton_clicked\0sendAttr\0sendCSP\0"
    "sendFile\0filename\0numBytes\0generateSCO\0"
    "getPOW\0on_pushButton_2_clicked\0"
    "on_pushButton_3_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ClientWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   94,    2, 0x08 /* Private */,
       3,    0,   95,    2, 0x08 /* Private */,
       4,    0,   96,    2, 0x08 /* Private */,
       5,    0,   97,    2, 0x08 /* Private */,
       6,    0,   98,    2, 0x08 /* Private */,
       7,    0,   99,    2, 0x08 /* Private */,
       8,    0,  100,    2, 0x08 /* Private */,
       9,    0,  101,    2, 0x08 /* Private */,
      10,    0,  102,    2, 0x08 /* Private */,
      11,    0,  103,    2, 0x08 /* Private */,
      12,    1,  104,    2, 0x08 /* Private */,
       8,    1,  107,    2, 0x08 /* Private */,
      15,    0,  110,    2, 0x08 /* Private */,
      16,    0,  111,    2, 0x08 /* Private */,
      17,    0,  112,    2, 0x08 /* Private */,
      18,    0,  113,    2, 0x08 /* Private */,

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
    QMetaType::Void, QMetaType::QString,   13,
    QMetaType::Void, QMetaType::LongLong,   14,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ClientWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ClientWidget *_t = static_cast<ClientWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->acceptFileConnection(); break;
        case 1: _t->acceptKsConnection(); break;
        case 2: _t->acceptKs(); break;
        case 3: _t->sendKS(); break;
        case 4: _t->selectFile(); break;
        case 5: _t->sendToken(); break;
        case 6: _t->updateFileProgress(); break;
        case 7: _t->on_pushButton_clicked(); break;
        case 8: _t->sendAttr(); break;
        case 9: _t->sendCSP(); break;
        case 10: _t->sendFile((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 11: _t->updateFileProgress((*reinterpret_cast< qint64(*)>(_a[1]))); break;
        case 12: _t->generateSCO(); break;
        case 13: _t->getPOW(); break;
        case 14: _t->on_pushButton_2_clicked(); break;
        case 15: _t->on_pushButton_3_clicked(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ClientWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ClientWidget.data,
      qt_meta_data_ClientWidget,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *ClientWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ClientWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ClientWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int ClientWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 16)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 16;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
