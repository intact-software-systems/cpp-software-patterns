#ifndef RxData_Cache_ObjectSubject_h_IsIncluded
#define RxData_Cache_ObjectSubject_h_IsIncluded

#include"RxData/CommonDefines.h"
#include"RxData/cache/observer/ObjectListener.h"

namespace RxData
{

template <typename DATA>
class ObjectSubject
        : public ObjectObserver<DATA>
        , public BaseLib::Concurrent::Observable<ObjectObserver<DATA> >
{
public:
    ObjectSubject()
            : objectObserverSignaller_(new Signaller1<bool, DATA>())
    {}
    virtual ~ObjectSubject()
    {}

    /**
     * Attach an ObjectListener. When a listener is attached, a bool parameter specifies,
     * when set to TRUE, that the listener should listen also for the modification of the
     * contained objects (concerns_contained_objects).
     *
     * @param listener
     */
    virtual SlotHolder::Ptr Connect(ObjectObserver<DATA> *observer)
    {
        objectObserverSignaller_->template Connect<ObjectObserver<DATA> >(observer, &ObjectObserver<DATA>::OnObjectCreated);
        objectObserverSignaller_->template Connect<ObjectObserver<DATA> >(observer, &ObjectObserver<DATA>::OnObjectDeleted);
        return objectObserverSignaller_->template Connect<ObjectObserver<DATA> >(observer, &ObjectObserver<DATA>::OnObjectModified);
    }

    /**
     * Detach the ObjectListener from the ObjectHome.
     *
     * @param listener
     */
    virtual bool Disconnect(ObjectObserver<DATA> *observer)
    {
        return objectObserverSignaller_->template Disconnect< ObjectObserver<DATA> >(observer);
    }

    /**
     * @brief DisconnectAll
     */
    virtual void DisconnectAll()
    {
        objectObserverSignaller_->DisconnectAll();
    }


    /**
     * Called when a new object appears in the Cache; this operation is called with the newly created object
     * (the_object).
     *
     * @param data
     *
     * @return
     *
     * TRUE means that the event has been fully taken into account and therefore does not need to be propagated
     * to other ObjectListener objects (of parent classes).
     */
    virtual bool OnObjectCreated(DATA data)
    {
        objectObserverSignaller_->template Signal<ObjectObserver<DATA> >(data, &ObjectObserver<DATA>::OnObjectCreated);
        return false;
    }


    /**
     * Called when an object has been deleted by another participant; this operation is called with the newly
     * deleted object (the_object).
     *
     * @param data
     *
     * @return
     *
     * TRUE means that the event has been fully taken into account and therefore does not need to be propagated
     * to other ObjectListener objects (of parent classes).
     */
    virtual bool OnObjectDeleted(DATA data)
    {
        objectObserverSignaller_->template Signal<ObjectObserver<DATA> >(data, &ObjectObserver<DATA>::OnObjectDeleted);
        return true;
    }


    /**
     * Called when the contents of an object changes; this operation is called with the modified object (the_object).
     *
     * @param data
     * @return
     *
     * TRUE means that the event has been fully taken into account and therefore does not need to be propagated
     * to other ObjectListener objects (of parent classes).
     */
    virtual bool OnObjectModified(DATA data)
    {
        objectObserverSignaller_->template Signal<ObjectObserver<DATA> >(data, &ObjectObserver<DATA>::OnObjectModified);
        return true;
    }

private:
    /**
     * The signaller to the attached ObjectObserver
     */
    typename Signaller1<bool, DATA>::Ptr objectObserverSignaller_;
};

}

#endif
