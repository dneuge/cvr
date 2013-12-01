/****************************************************************************
** Meta object code from reading C++ file 'PulseAudioSink.h'
**
** Created: Sun Dec 1 16:45:45 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "PulseAudioSink.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PulseAudioSink.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_PulseAudioSink[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      36,   16,   15,   15, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_PulseAudioSink[] = {
    "PulseAudioSink\0\0rawAudio,byteLength\0"
    "playAudio(char*,ulong)\0"
};

void PulseAudioSink::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        PulseAudioSink *_t = static_cast<PulseAudioSink *>(_o);
        switch (_id) {
        case 0: _t->playAudio((*reinterpret_cast< char*(*)>(_a[1])),(*reinterpret_cast< ulong(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData PulseAudioSink::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject PulseAudioSink::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_PulseAudioSink,
      qt_meta_data_PulseAudioSink, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PulseAudioSink::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PulseAudioSink::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PulseAudioSink::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PulseAudioSink))
        return static_cast<void*>(const_cast< PulseAudioSink*>(this));
    return QObject::qt_metacast(_clname);
}

int PulseAudioSink::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
