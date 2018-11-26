#pragma once

#include"DataReactor/IncludeExtLibs.h"

namespace Reactor {

template <typename T>
class DataEvents
{
public:
    /**
     * @brief Provides the Observer with new data. The Observable calls this closure 1 or more times, unless it
     * calls onError in which case this closure may never be called. The Observable will not call this closure again
     * after it calls either onCompleted or onError.
     */
    virtual void OnNext(T t) = 0;

    /**
     * @brief OnError Notifies the Observer that the Observable has experienced an error condition.
     * If the Observable calls this closure, it will not thereafter call onNext or onCompleted.
     */
    virtual void OnError(BaseLib::Exception exception) = 0;
};

template <typename T>
class DataObserver
    : public DataEvents<T>
      , public ObserverSlot<DataObserver<T> >
{
};

}
