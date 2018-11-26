#ifndef RxData_Cache_CacheSubject_h_IsIncluded
#define RxData_Cache_CacheSubject_h_IsIncluded

#include"RxData/CommonDefines.h"
#include"RxData/cache/observer/CacheListener.h"

namespace RxData
{

/**
 *
 * CacheObserver is an interface that must be implemented by the application in order to be made aware
 * of the arrival of updates on the cache.
 *
 * In between, the updates are reported on home or selection listeners. Section 8.1.6.4, "Listeners Activation,"
 * on page 208 describes which notifications are performed and in what order.
 *
 * @author KVik
 */
class CacheSubject
        : public CacheObserver
        , public BaseLib::Concurrent::Observable<CacheObserver>
{
public:
    CacheSubject()
        : cacheObserverSignaller_( new Signaller1<void, CacheDescription>() )
    {}
    virtual ~CacheSubject()
    {}

    /**
     * Indicates that updates are following. Actual modifications in the cache will be performed only
     * when exiting this method (assuming that updates_enabled is true).
     */
    virtual void OnBeginUpdates(CacheDescription description)
    {
        cacheObserverSignaller_->Signal<CacheObserver>(description, &CacheObserver::OnBeginUpdates);
    }

    /**
     * Indicates that no more update is foreseen.
     */
    virtual void OnEndUpdates(CacheDescription description)
    {
        cacheObserverSignaller_->Signal<CacheObserver>(description, &CacheObserver::OnEndUpdates);
    }

    /**
     * Indicates that the Cache has switched to automatic update mode. Incoming data will now trigger
     * the corresponding Listeners.
     */
    virtual void OnUpdatesEnabled(CacheDescription description)
    {
        cacheObserverSignaller_->Signal<CacheObserver>(description, &CacheObserver::OnUpdatesEnabled);
    }

    /**
     * Indicates that the Cache has switched to manual update mode. Incoming data will no longer trigger
     * the corresponding Listeners, and will only be taken into account during the next refresh operation.
     */
    virtual void OnUpdatesDisabled(CacheDescription description)
    {
        cacheObserverSignaller_->Signal<CacheObserver>(description, &CacheObserver::OnUpdatesDisabled);
    }

    /**
     * Attach the CacheObserver to the Cache.
     *
     * @param observer
     */
    SlotHolder::Ptr Connect(CacheObserver *observer)
    {
        cacheObserverSignaller_->Connect<CacheObserver>(observer, &CacheObserver::OnBeginUpdates);
        cacheObserverSignaller_->Connect<CacheObserver>(observer, &CacheObserver::OnEndUpdates);
        cacheObserverSignaller_->Connect<CacheObserver>(observer, &CacheObserver::OnUpdatesDisabled);
        return cacheObserverSignaller_->Connect<CacheObserver>(observer, &CacheObserver::OnUpdatesEnabled);
    }

    /**
     * Detach the CacheObserver from the Cache.
     *
     * @param observer
     */
    bool Disconnect(RxData::CacheObserver *observer)
    {
        return cacheObserverSignaller_->Disconnect<CacheObserver>(observer);
    }


    void DisconnectAll()
    {
        cacheObserverSignaller_->DisconnectAll();
    }

private:
    /**
     * The attached CacheObserver objects.
     */
    Signaller1<void, CacheDescription>::Ptr cacheObserverSignaller_;
};

}

#endif
