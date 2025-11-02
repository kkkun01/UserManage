/****************************************************************************
** Meta object code from reading C++ file 'customserver.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../customserver.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'customserver.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CustomServer_t {
    QByteArrayData data[21];
    char stringdata0[254];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CustomServer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CustomServer_t qt_meta_stringdata_CustomServer = {
    {
QT_MOC_LITERAL(0, 0, 12), // "CustomServer"
QT_MOC_LITERAL(1, 13, 18), // "serverStateChanged"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 12), // "SERVER_STATE"
QT_MOC_LITERAL(4, 46, 5), // "state"
QT_MOC_LITERAL(5, 52, 10), // "statusInfo"
QT_MOC_LITERAL(6, 63, 15), // "clientConnected"
QT_MOC_LITERAL(7, 79, 10), // "clientInfo"
QT_MOC_LITERAL(8, 90, 18), // "clientDisconnected"
QT_MOC_LITERAL(9, 109, 12), // "dataReceived"
QT_MOC_LITERAL(10, 122, 4), // "data"
QT_MOC_LITERAL(11, 127, 19), // "sglRequestClassList"
QT_MOC_LITERAL(12, 147, 21), // "sglHttpChangeUserData"
QT_MOC_LITERAL(13, 169, 9), // "className"
QT_MOC_LITERAL(14, 179, 10), // "memberName"
QT_MOC_LITERAL(15, 190, 2), // "id"
QT_MOC_LITERAL(16, 193, 7), // "newInfo"
QT_MOC_LITERAL(17, 201, 9), // "newStatus"
QT_MOC_LITERAL(18, 211, 15), // "onNewConnection"
QT_MOC_LITERAL(19, 227, 11), // "onReadyRead"
QT_MOC_LITERAL(20, 239, 14) // "onDisconnected"

    },
    "CustomServer\0serverStateChanged\0\0"
    "SERVER_STATE\0state\0statusInfo\0"
    "clientConnected\0clientInfo\0"
    "clientDisconnected\0dataReceived\0data\0"
    "sglRequestClassList\0sglHttpChangeUserData\0"
    "className\0memberName\0id\0newInfo\0"
    "newStatus\0onNewConnection\0onReadyRead\0"
    "onDisconnected"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CustomServer[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   59,    2, 0x06 /* Public */,
       6,    1,   64,    2, 0x06 /* Public */,
       8,    1,   67,    2, 0x06 /* Public */,
       9,    2,   70,    2, 0x06 /* Public */,
      11,    0,   75,    2, 0x06 /* Public */,
      12,    5,   76,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      18,    0,   87,    2, 0x0a /* Public */,
      19,    0,   88,    2, 0x0a /* Public */,
      20,    0,   89,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::QString,    4,    5,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    7,   10,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::Int, QMetaType::QString, QMetaType::QString,   13,   14,   15,   16,   17,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void CustomServer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CustomServer *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->serverStateChanged((*reinterpret_cast< SERVER_STATE(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 1: _t->clientConnected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->clientDisconnected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->dataReceived((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 4: _t->sglRequestClassList(); break;
        case 5: _t->sglHttpChangeUserData((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< QString(*)>(_a[4])),(*reinterpret_cast< QString(*)>(_a[5]))); break;
        case 6: _t->onNewConnection(); break;
        case 7: _t->onReadyRead(); break;
        case 8: _t->onDisconnected(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CustomServer::*)(SERVER_STATE , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CustomServer::serverStateChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (CustomServer::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CustomServer::clientConnected)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (CustomServer::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CustomServer::clientDisconnected)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (CustomServer::*)(const QString & , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CustomServer::dataReceived)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (CustomServer::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CustomServer::sglRequestClassList)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (CustomServer::*)(QString , QString , int , QString , QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CustomServer::sglHttpChangeUserData)) {
                *result = 5;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CustomServer::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_CustomServer.data,
    qt_meta_data_CustomServer,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CustomServer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CustomServer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CustomServer.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int CustomServer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void CustomServer::serverStateChanged(SERVER_STATE _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CustomServer::clientConnected(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void CustomServer::clientDisconnected(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void CustomServer::dataReceived(const QString & _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void CustomServer::sglRequestClassList()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void CustomServer::sglHttpChangeUserData(QString _t1, QString _t2, int _t3, QString _t4, QString _t5)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
