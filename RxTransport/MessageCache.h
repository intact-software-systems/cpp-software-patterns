#pragma once

#include"RxTransport/CommonDefines.h"

namespace BaseLib { namespace Concurrent {

// --------------------------------------------------------------
// TODO: When data messages are added to, handle call-back to observers, strip out Message and only send T....
// TODO: Replace with cache implementation and use cache policy to clean up cache
// TODO: I need timestamps in the cache for policy checking and sorting
// Alternative to RxObserverSubject : CollectionObserver
// --------------------------------------------------------------
template <typename T>
class MessageCache : protected Templates::ContextDataShared<RxObserverSubject<T>>
{
    typedef std::shared_ptr<DataMessage<T>> DM;

public:
    MessageCache(RxData::CacheDescription description)
        : description_(description)
    { }

    virtual ~MessageCache()
    { }

    SequenceNumber Max()
    {
        auto objects = messageCache().getObjects();

        return objects.empty()
               ? SequenceNumber::NIL()
               : objects.crbegin()->first;
    }

    SequenceNumber Min()
    {
        auto objects = messageCache().getObjects();

        return objects.empty()
               ? SequenceNumber::NIL()
               : objects.cbegin()->first;
    }

    RxObserverSubject<T>& Channel()
    {
        return this->context().operator*();
    }

    bool Add(const SequenceNumber& key, const std::shared_ptr<DataMessage<T>>& message)
    {
        auto added = messageCache().Write(message, key);
        DataCache().Write(message->Value(), message->Handle());

        this->context()->OnNext(message->Value());

        message->Status().NextState(TransmissionStatusKind::RECEIVED, 1);

        return added;
    }

    bool Remove(const SequenceNumber& key)
    {
        auto message = messageCache().findObject(key);
        if(message != nullptr)
        {
            DataCache().Dispose(message->Handle());
        }
        return messageCache().Dispose(key);;
    }

    bool Update(const SequenceNumber& key, ChangeForReaderStatusKind kind)
    {
        switch(kind)
        {
            case ChangeForReaderStatusKind::ACKNOWLEDGED:
            {
                std::shared_ptr<DataMessage<T>> message = messageCache().findObject(key);
                ASSERT(message);

                message->Status().NextState(TransmissionStatusKind::ACKNOWLEDGED, 1);
                message->DeliveryPromise()->set_value(kind);
                return true;
            }
            case ChangeForReaderStatusKind::UNSENT:
            case ChangeForReaderStatusKind::UNACKNOWLEDGED:
            case ChangeForReaderStatusKind::REQUESTED:
            case ChangeForReaderStatusKind::UNDERWAY:
            case ChangeForReaderStatusKind::FILTERED:
            case ChangeForReaderStatusKind::NO:
                break;
        }
        return false;
    }

    std::shared_ptr<DataMessage<T>> Find(SequenceNumber key)
    {
        return messageCache().findObject(key);
    }

    RxData::ObjectAccessProxy<T, InstanceHandle> DataCache()
    {
        return RxData::ObjectAccessProxy<T, InstanceHandle>(cache(), BaseLib::Utility::GetTypeName<T>());
    }

private:

    RxData::ObjectAccessProxy<DM, SequenceNumber> messageCache()
    {
        return RxData::ObjectAccessProxy<DM, SequenceNumber>(cache(), BaseLib::Utility::GetTypeName<DM>());
    }

    RxData::Cache::Ptr cache()
    {
        return RxData::CacheFactory::Instance().getOrCreateCache(description_);
    }

private:
    RxData::CacheDescription description_;
};

}}

