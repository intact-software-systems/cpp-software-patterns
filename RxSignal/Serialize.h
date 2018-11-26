#pragma once

#include"RxSignal/CommonDefines.h"
#include"RxSignal/Signaller0.h"
#include"RxSignal/Export.h"

namespace BaseLib { namespace Observer
{

class DLL_STATE SerializerObserver
{
public:
    CLASS_TRAITS(SerializerObserver)

    virtual void OnSerialize() = 0;
    virtual void OnSave() = 0;
    virtual void OnLoad() = 0;
};

class DLL_STATE SerializerSubject
        : public Concurrent::Observable<SerializerObserver>
        , public SerializerObserver
{
public:
    SerializerSubject();
    virtual ~SerializerSubject();

    CLASS_TRAITS(SerializerSubject)

    virtual void OnSerialize();
    virtual void OnSave();
    virtual void OnLoad();

    virtual Concurrent::SlotHolder::Ptr Connect(SerializerObserver *observer);

    virtual bool Disconnect(SerializerObserver *observer);

private:
    Concurrent::Signaller0<void>::Ptr signaller_;
};

}}
