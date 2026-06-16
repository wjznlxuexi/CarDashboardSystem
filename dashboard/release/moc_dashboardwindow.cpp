/****************************************************************************
** Meta object code from reading C++ file 'dashboardwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../dashboardwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'dashboardwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_DashboardWindow_t {
    QByteArrayData data[19];
    char stringdata[273];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DashboardWindow_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DashboardWindow_t qt_meta_stringdata_DashboardWindow = {
    {
QT_MOC_LITERAL(0, 0, 15),
QT_MOC_LITERAL(1, 16, 20),
QT_MOC_LITERAL(2, 37, 0),
QT_MOC_LITERAL(3, 38, 11),
QT_MOC_LITERAL(4, 50, 4),
QT_MOC_LITERAL(5, 55, 24),
QT_MOC_LITERAL(6, 80, 9),
QT_MOC_LITERAL(7, 90, 14),
QT_MOC_LITERAL(8, 105, 7),
QT_MOC_LITERAL(9, 113, 19),
QT_MOC_LITERAL(10, 133, 20),
QT_MOC_LITERAL(11, 154, 18),
QT_MOC_LITERAL(12, 173, 15),
QT_MOC_LITERAL(13, 189, 7),
QT_MOC_LITERAL(14, 197, 16),
QT_MOC_LITERAL(15, 214, 13),
QT_MOC_LITERAL(16, 228, 15),
QT_MOC_LITERAL(17, 244, 5),
QT_MOC_LITERAL(18, 250, 22)
    },
    "DashboardWindow\0onVehicleDataChanged\0"
    "\0VehicleData\0data\0onConnectionStateChanged\0"
    "connected\0onNetworkError\0message\0"
    "onLeftTurnTriggered\0onRightTurnTriggered\0"
    "onTurnOffTriggered\0onHazardToggled\0"
    "checked\0onResetTriggered\0onMuteToggled\0"
    "onVolumeChanged\0value\0onWarningTestTriggered"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DashboardWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   69,    2, 0x08 /* Private */,
       5,    1,   72,    2, 0x08 /* Private */,
       7,    1,   75,    2, 0x08 /* Private */,
       9,    0,   78,    2, 0x08 /* Private */,
      10,    0,   79,    2, 0x08 /* Private */,
      11,    0,   80,    2, 0x08 /* Private */,
      12,    1,   81,    2, 0x08 /* Private */,
      14,    0,   84,    2, 0x08 /* Private */,
      15,    1,   85,    2, 0x08 /* Private */,
      16,    1,   88,    2, 0x08 /* Private */,
      18,    0,   91,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::Bool,    6,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   13,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   13,
    QMetaType::Void, QMetaType::Int,   17,
    QMetaType::Void,

       0        // eod
};

void DashboardWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DashboardWindow *_t = static_cast<DashboardWindow *>(_o);
        switch (_id) {
        case 0: _t->onVehicleDataChanged((*reinterpret_cast< const VehicleData(*)>(_a[1]))); break;
        case 1: _t->onConnectionStateChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->onNetworkError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->onLeftTurnTriggered(); break;
        case 4: _t->onRightTurnTriggered(); break;
        case 5: _t->onTurnOffTriggered(); break;
        case 6: _t->onHazardToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: _t->onResetTriggered(); break;
        case 8: _t->onMuteToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: _t->onVolumeChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->onWarningTestTriggered(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< VehicleData >(); break;
            }
            break;
        }
    }
}

const QMetaObject DashboardWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_DashboardWindow.data,
      qt_meta_data_DashboardWindow,  qt_static_metacall, 0, 0}
};


const QMetaObject *DashboardWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DashboardWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DashboardWindow.stringdata))
        return static_cast<void*>(const_cast< DashboardWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int DashboardWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
