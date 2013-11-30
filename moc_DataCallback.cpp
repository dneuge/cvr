/****************************************************************************
** Meta object code from reading C++ file 'DataCallback.h'
**
** Created: Sat Nov 30 13:15:03 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "DataCallback.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DataCallback.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DataCallback[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
      29,   13,   13,   13, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_DataCallback[] = {
    "DataCallback\0\0imageUpdated()\0"
    "toggleCapture()\0"
};

void DataCallback::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        DataCallback *_t = static_cast<DataCallback *>(_o);
        switch (_id) {
        case 0: _t->imageUpdated(); break;
        case 1: _t->toggleCapture(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData DataCallback::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject DataCallback::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_DataCallback,
      qt_meta_data_DataCallback, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DataCallback::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DataCallback::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DataCallback::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DataCallback))
        return static_cast<void*>(const_cast< DataCallback*>(this));
    if (!strcmp(_clname, "IDeckLinkInputCallback"))
        return static_cast< IDeckLinkInputCallback*>(const_cast< DataCallback*>(this));
    return QObject::qt_metacast(_clname);
}

int DataCallback::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void DataCallback::imageUpdated()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
