#include"RxSignal/Serialize.h"

namespace BaseLib { namespace Observer
{

SerializerSubject::SerializerSubject()
    : signaller_(new Concurrent::Signaller0<void>())
{}

SerializerSubject::~SerializerSubject()
{}

void SerializerSubject::OnSerialize()
{
    signaller_->Signal<SerializerObserver>(&SerializerObserver::OnSerialize);
}

void SerializerSubject::OnSave()
{
    signaller_->Signal<SerializerObserver>(&SerializerObserver::OnSave);
}

void SerializerSubject::OnLoad()
{
    signaller_->Signal<SerializerObserver>(&SerializerObserver::OnLoad);
}

SlotHolder::Ptr SerializerSubject::Connect(SerializerObserver *observer)
{
    signaller_->Connect<SerializerObserver>(observer, &SerializerObserver::OnSerialize);
    signaller_->Connect<SerializerObserver>(observer, &SerializerObserver::OnSave);
    return signaller_->Connect<SerializerObserver>(observer, &SerializerObserver::OnLoad);
}

bool SerializerSubject::Disconnect(SerializerObserver *observer)
{
    return signaller_->Disconnect<SerializerObserver>(observer);
}

}}

