#pragma once

#include"RxObserver/KeyValueRxObserver.h"

namespace BaseLib {

template <typename K, typename V>
class KeyValueRxEventsSubject
    : public Concurrent::SubjectEventsType<KeyValueRxEvents<K, V>, KeyValueRxEventsSource<K, V>>
{
public:
    KeyValueRxEventsSubject(
        Concurrent::SubjectDescription description = KeyValueRxEventsSource<K, V>::DefaultDescription(),
        Concurrent::SubjectPolicy policy = Concurrent::SubjectPolicy::Default())
        : Concurrent::SubjectEventsType<KeyValueRxEvents<K, V>, KeyValueRxEventsSource<K, V>>(KeyValueRxEventsSource<K, V>::CreatePtr(), description, policy)
    { }

    virtual ~KeyValueRxEventsSubject()
    { }
};

}
