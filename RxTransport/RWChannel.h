#pragma once

#include"RxTransport/CommonDefines.h"
#include"RxTransport/MessageChannel.h"
#include "SubscriberChannel.h"
#include "PublisherChannel.h"

namespace BaseLib { namespace Concurrent {

template <typename T>
class RWChannel
{
public:
    RWChannel(const std::shared_ptr<SubscriberChannel<T>>& subscriber, std::shared_ptr<PublisherChannel<T>> publisher)
        : subscriber_(subscriber)
        , publisher_(publisher)
    { }

    virtual ~RWChannel()
    { }

    std::shared_ptr<SubscriberChannel<T>> subscriber() const
    {
        return subscriber_;
    }

    std::shared_ptr<PublisherChannel<T>> publisher() const
    {
        return publisher_;
    }

private:
    std::shared_ptr<SubscriberChannel<T>> subscriber_;
    std::shared_ptr<PublisherChannel<T>>  publisher_;
};

}}
