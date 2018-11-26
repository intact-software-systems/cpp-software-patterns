#pragma once

#include"RxSignal/CommonDefines.h"
#include"RxSignal/SignalSlotBase.h"
#include"RxSignal/Signaller1.h"
#include"RxSignal/Signaller2.h"

namespace BaseLib { namespace Templates
{

//template <typename Return, typename T>
//class Observable
//{
//public:
//    // Return = Subscription
//    virtual Return Subscribe(T t) = 0;
//};

template <typename T>
class ObjectObserver
{
public:
    CLASS_TRAITS(ObjectObserver)

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
    virtual void OnObjectCreated(T data) = 0;


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
    virtual void OnObjectDeleted(T data) = 0;


    /**
     * Called when the contents of an object changes; this operation is called with the modified object (the_object).
     *
     * @param data
     * @return
     *
     * TRUE means that the event has been fully taken into account and therefore does not need to be propagated
     * to other ObjectListener objects (of parent classes).
     */
    virtual void OnObjectModified(T data) = 0;
};


// TODO: Make Observable base type that automatically initializes signal/slot?
template <typename T>
class ObjectSubject
        : public ObjectObserver<T>
        , public Concurrent::Observable< ObjectObserver<T> >
{
public:
    ObjectSubject()
        : signaller_( new Concurrent::Signaller1<void, T>())
    {}
    virtual ~ObjectSubject()
    {}

    /**
     * @brief Connect
     * @param observer
     */
    virtual typename Concurrent::SlotHolder::Ptr Connect(ObjectObserver<T> *observer)
    {
        signaller_->Connect(observer, &ObjectObserver<T>::OnObjectCreated);
        signaller_->Connect(observer, &ObjectObserver<T>::OnObjectDeleted);
        return signaller_->Connect(observer, &ObjectObserver<T>::OnObjectModified);
    }

    /**
     * @brief RemoveObserver
     * @param observer
     */
    virtual bool Disconnect(ObjectObserver<T> *observer)
    {
        return signaller_-> template Disconnect<ObjectObserver<T>>(observer);
    }

    /**
     * @brief SignalObjectCreated
     * @param t
     */
    virtual void OnObjectCreated(T t)
    {
        signaller_->Signal(t, &ObjectObserver<T>::OnObjectCreated);
    }

    /**
     * @brief SignalObjectDeleted
     * @param t
     */
    virtual void OnObjectDeleted(T t)
    {
        signaller_->Signal(t, &ObjectObserver<T>::OnObjectDeleted);
    }

    /**
     * @brief SignalObjectModified
     * @param t
     */
    virtual void OnObjectModified(T t)
    {
        signaller_->Signal(t, &ObjectObserver<T>::OnObjectModified);
    }

private:
    typename Concurrent::Signaller1<void, T>::Ptr signaller_;
};


template <typename K, typename V>
class KeyValueObjectObserver
{
public:
    CLASS_TRAITS(KeyValueObjectObserver)

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
    virtual void OnObjectCreated(K key, V value) = 0;


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
    virtual void OnObjectDeleted(K key) = 0;


    /**
     * Called when the contents of an object changes; this operation is called with the modified object (the_object).
     *
     * @param data
     * @return
     *
     * TRUE means that the event has been fully taken into account and therefore does not need to be propagated
     * to other ObjectListener objects (of parent classes).
     */
    virtual void OnObjectModified(K key, V value) = 0;
};

template <typename K, typename V>
class KeyValueSubject
        : public KeyValueObjectObserver<K, V>
        , public Concurrent::Observable< KeyValueObjectObserver<K,V> >
{
public:
    KeyValueSubject()
        : twoArgSignaller_( new Concurrent::Signaller2<void, K, V>())
        , oneArgSignaller_( new Concurrent::Signaller1<void, K>())
    {}
    virtual ~KeyValueSubject()
    {}

    /**
     * @brief Connect
     * @param observer
     */
    virtual typename Concurrent::SlotHolder::Ptr Connect(KeyValueObjectObserver<K,V> *observer)
    {
        oneArgSignaller_->Connect(observer, &KeyValueObjectObserver<K,V>::OnObjectDeleted);
        twoArgSignaller_->Connect(observer, &KeyValueObjectObserver<K,V>::OnObjectCreated);
        return twoArgSignaller_->Connect(observer, &KeyValueObjectObserver<K,V>::OnObjectModified);
    }

    /**
     * @brief RemoveObserver
     * @param observer
     */
    virtual bool Disconnect(KeyValueObjectObserver<K,V> *observer)
    {
        oneArgSignaller_-> template Disconnect<KeyValueObjectObserver<K,V>>(observer);
        return twoArgSignaller_->template Disconnect<KeyValueObjectObserver<K,V>>(observer);
    }

    /**
     * @brief DisconnectAll
     */
    virtual void DisconnectAll()
    {
        twoArgSignaller_->DisconnectAll();
        oneArgSignaller_->DisconnectAll();
    }

    /**
     * @brief SignalObjectCreated
     * @param t
     */
    virtual void OnObjectCreated(K key, V value)
    {
        twoArgSignaller_->Signal(key, value, &KeyValueObjectObserver<K,V>::OnObjectCreated);
    }

    /**
     * @brief SignalObjectDeleted
     * @param t
     */
    virtual void OnObjectDeleted(K key)
    {
        oneArgSignaller_->Signal(key, &KeyValueObjectObserver<K,V>::OnObjectDeleted);
    }

    /**
     * @brief SignalObjectModified
     * @param t
     */
    virtual void OnObjectModified(K key, V value)
    {
        twoArgSignaller_->Signal(key, value, &KeyValueObjectObserver<K,V>::OnObjectModified);
    }

private:
    typename Concurrent::Signaller2<void, K, V>::Ptr twoArgSignaller_;
    typename Concurrent::Signaller1<void, K>::Ptr oneArgSignaller_;
};

}}
