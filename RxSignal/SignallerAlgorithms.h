#pragma once

#include"RxSignal/CommonDefines.h"
#include"RxSignal/SignalSlotBase.h"
#include"RxSignal/Templates/SignalMethods.h"
#include"RxSignal/Export.h"

namespace BaseLib { namespace Concurrent {

/**
 * TODO:
 * - Add support for any number of arguments by using NoArgFunctionPointerInvoker, OneArgFunctionPointerInvoker, TwoArgFunctionPointerInvoker, etc ...
 * - Add support for replay of events
 * - Add support for scalable asynchronous dispatch of events.
 * - Add a baseclass "class Event" or similar with support for invoke or dispatch of event to handler/observer.
 * - Add event meta data for monitoring: timestamp, etc
 */
class DLL_STATE SignallerAlgorithms
{
public:
    virtual ~SignallerAlgorithms();

    // --------------------------------------------------
    // Connect functions
    // --------------------------------------------------

    template <typename Return, typename SIGNALLER, typename Collection, typename Class>
    static typename SlotHolder::Ptr connect(SIGNALLER parent, Collection& slotMap, Class* observer, Return (Class::*member)())
    {
        // ----------------------------------------
        // Initialize the slot
        // ----------------------------------------
        Templates::InvokeMethod0<Return>* funcPointer = new Templates::FunctionPointer0<Class, Return>(observer, member);
        typename SlotHolder::Ptr slotHolder(new details::SlotHolderImpl(parent->Handle(), parent->GetPtr()));

        typename Callback<Templates::InvokeMethod0<Return> >::Ptr callback(
            new Callback<Templates::InvokeMethod0<Return> >(funcPointer, slotHolder->GetPtr()));

        // ----------------------------------------
        // insert the slot
        // ----------------------------------------
        {
            MutexTypeLocker<Collection> lock(&slotMap);
            std::pair<typename Collection::iterator, bool>
                                        inserted =
                                            slotMap.insert(
                                                std::pair<InstanceHandle, typename Callback<Templates::InvokeMethod0<Return> >::Ptr>(
                                                    parent->Handle(),
                                                    callback->GetPtr()));

            ASSERT(inserted.second);

            // ----------------------------------------
            // Increment handle for next slot
            // ----------------------------------------
            parent->incHandle();
        }

        return callback->GetSlotHolder()->GetPtr();
    }

    template <typename Return, typename SIGNALLER, typename Collection, typename Class, typename Arg1>
    static typename SlotHolder::Ptr connect(SIGNALLER parent, Collection& slotMap, Class* observer, Return (Class::*member)(Arg1))
    {
        // ----------------------------------------
        // Initialize the slot
        // ----------------------------------------
        Templates::InvokeMethod1<Return, Arg1>* funcPointer = new Templates::FunctionPointer1<Class, Return, Arg1>(observer, member);
        typename SlotHolder::Ptr slotHolder(new details::SlotHolderImpl(parent->Handle(), parent->GetPtr()));

        typename Callback<Templates::InvokeMethod1<Return, Arg1> >::Ptr callback(
            new Callback<Templates::InvokeMethod1<Return, Arg1> >(funcPointer, slotHolder->GetPtr()));

        // ----------------------------------------
        // insert the slot
        // ----------------------------------------
        {
            MutexTypeLocker<Collection> lock(&slotMap);
            std::pair<typename Collection::iterator, bool>
                                        inserted =
                                            slotMap.insert(
                                                std::pair<InstanceHandle, typename Callback<Templates::InvokeMethod1<Return, Arg1> >::Ptr>(
                                                    parent->Handle(),
                                                    callback->GetPtr()));

            ASSERT(inserted.second);

            // ----------------------------------------
            // Increment handle for next slot
            // ----------------------------------------
            parent->incHandle();
        }

        return callback->GetSlotHolder()->GetPtr();
    }

    template <typename Return, typename SIGNALLER, typename Collection, typename Class, typename Arg1, typename Arg2>
    static typename SlotHolder::Ptr connect(SIGNALLER parent, Collection& slotMap, Class* observer, Return (Class::*member)(Arg1, Arg2))
    {
        // ----------------------------------------
        // Initialize the slot
        // ----------------------------------------
        Templates::InvokeMethod2<Return, Arg1, Arg2>* funcPointer = new Templates::FunctionPointer2<Class, Return, Arg1, Arg2>(observer, member);
        typename SlotHolder::Ptr slotHolder(new details::SlotHolderImpl(parent->Handle(), parent->GetPtr()));

        typename Callback<Templates::InvokeMethod2<Return, Arg1, Arg2> >::Ptr callback(
            new Callback<Templates::InvokeMethod2<Return, Arg1, Arg2> >(funcPointer, slotHolder->GetPtr()));

        // ----------------------------------------
        // insert the slot
        // ----------------------------------------
        {
            MutexTypeLocker<Collection> lock(&slotMap);
            std::pair<typename Collection::iterator, bool>
                                        inserted =
                                            slotMap.insert(
                                                std::pair<InstanceHandle, typename Callback<Templates::InvokeMethod2<Return, Arg1, Arg2> >::Ptr>(
                                                    parent->Handle(),
                                                    callback->GetPtr()));

            ASSERT(inserted.second);

            // ----------------------------------------
            // Increment handle for next slot
            // ----------------------------------------
            parent->incHandle();
        }

        return callback->GetSlotHolder()->GetPtr();
    }

    template <typename Return, typename SIGNALLER, typename Collection, typename Class, typename Arg1, typename Arg2, typename Arg3>
    static typename SlotHolder::Ptr connect(SIGNALLER parent, Collection& slotMap, Class* observer, Return (Class::*member)(Arg1, Arg2, Arg3))
    {
        // ----------------------------------------
        // Initialize the slot
        // ----------------------------------------
        Templates::InvokeMethod3<Return, Arg1, Arg2, Arg3>* funcPointer = new Templates::FunctionPointer3<Class, Return, Arg1, Arg2, Arg3>(observer, member);
        typename SlotHolder::Ptr slotHolder(new details::SlotHolderImpl(parent->Handle(), parent->GetPtr()));

        typename Callback<Templates::InvokeMethod3<Return, Arg1, Arg2, Arg3> >::Ptr callback(
            new Callback<Templates::InvokeMethod3<Return, Arg1, Arg2, Arg3> >(funcPointer, slotHolder->GetPtr()));

        // ----------------------------------------
        // insert the slot
        // ----------------------------------------
        {
            MutexTypeLocker<Collection> lock(&slotMap);
            std::pair<typename Collection::iterator, bool>
                                        inserted =
                                            slotMap.insert(
                                                std::pair<InstanceHandle, typename Callback<Templates::InvokeMethod3<Return, Arg1, Arg2, Arg3> >::Ptr>(
                                                    parent->Handle(),
                                                    callback->GetPtr()));

            ASSERT(inserted.second);

            // ----------------------------------------
            // Increment handle for next slot
            // ----------------------------------------
            parent->incHandle();
        }

        return callback->GetSlotHolder()->GetPtr();
    }

    // --------------------------------------------------
    // Disconnect functions
    // --------------------------------------------------

    template <typename Collection>
    static bool disconnectHandle(Collection& slotMap, const InstanceHandle& id)
    {
        MutexTypeLocker<Collection> lock(&slotMap);

        typename Collection::iterator it = slotMap.find(id);

        if(it == slotMap.end())
        {
            return false;
        }

        slotMap.erase(it);

        return true;
    }

    template <typename Collection>
    static bool disconnectAll(Collection& slotMap)
    {
        InstanceHandleSet handleSet;

        {
            MutexTypeLocker<Collection>       lock(&slotMap);
            for(typename Collection::iterator it = slotMap.begin(), it_end = slotMap.end(); it != it_end; ++it)
            {
                handleSet.insert(it->first);
            }
        }

        for(InstanceHandleSet::iterator it = handleSet.begin(), it_end = handleSet.end(); it != it_end; ++it)
        {
            bool isDisconnected = SignallerAlgorithms::disconnectHandle<Collection>(slotMap, *it);

            ASSERT(isDisconnected);
        }

        return slotMap.empty();
    }

    // --------------------------------------------------
    // Disconnect based on observer instance
    // --------------------------------------------------

    template <typename Return, typename Collection, typename Class>
    static bool disconnect(Collection& slotMap, Class* observer)
    {
        std::vector<typename Collection::iterator> deleteVector;

        {
            MutexTypeLocker<Collection>       lock(&slotMap);
            for(typename Collection::iterator it = slotMap.begin(), it_end = slotMap.end(); it != it_end; ++it)
            {
                typename Callback<Templates::InvokeMethod0<Return> >::Ptr slot = it->second;

                typename Templates::FunctionPointer0<Class, Return>* funcPointer = dynamic_cast<Templates::FunctionPointer0<Class, Return>* >(slot->GetArgBase());

                if(funcPointer != NULL && funcPointer->Delegate() == observer)
                {
                    deleteVector.push_back(it);
                }
            }
        }

        SignallerAlgorithms::emptyCollection<Collection>(slotMap, deleteVector);

        return !deleteVector.empty();
    }

    template <typename Return, typename Collection, typename Class, typename Arg1>
    static bool disconnect(Collection& slotMap, Class* observer)
    {
        std::vector<typename Collection::iterator> deleteVector;

        {
            MutexTypeLocker<Collection>       lock(&slotMap);
            for(typename Collection::iterator it = slotMap.begin(), it_end = slotMap.end(); it != it_end; ++it)
            {
                typename Callback<Templates::InvokeMethod1<Return, Arg1> >::Ptr slot = it->second;

                typename Templates::FunctionPointer1<Class, Return, Arg1>* funcPointer = dynamic_cast<Templates::FunctionPointer1<Class, Return, Arg1>* >(slot->GetArgBase());

                if(funcPointer != NULL && funcPointer->Delegate() == observer)
                {
                    deleteVector.push_back(it);
                }
            }
        }

        SignallerAlgorithms::emptyCollection<Collection>(slotMap, deleteVector);

        return !deleteVector.empty();
    }

    template <typename Return, typename Collection, typename Class, typename Arg1, typename Arg2>
    static bool disconnect(Collection& slotMap, Class* observer)
    {
        std::vector<typename Collection::iterator> deleteVector;

        {
            MutexTypeLocker<Collection>       lock(&slotMap);
            for(typename Collection::iterator it = slotMap.begin(), it_end = slotMap.end(); it != it_end; ++it)
            {
                typename Callback<Templates::InvokeMethod2<Return, Arg1, Arg2> >::Ptr slot = it->second;

                typename Templates::FunctionPointer2<Class, Return, Arg1, Arg2>
                    * funcPointer = dynamic_cast<Templates::FunctionPointer2<Class, Return, Arg1, Arg2>* >(slot->GetArgBase());

                if(funcPointer != NULL && funcPointer->Delegate() == observer)
                {
                    deleteVector.push_back(it);
                }
            }
        }

        SignallerAlgorithms::emptyCollection<Collection>(slotMap, deleteVector);

        return !deleteVector.empty();
    }

    template <typename Return, typename Collection, typename Class, typename Arg1, typename Arg2, typename Arg3>
    static bool disconnect(Collection& slotMap, Class* observer)
    {
        std::vector<typename Collection::iterator> deleteVector;

        {
            MutexTypeLocker<Collection>       lock(&slotMap);
            for(typename Collection::iterator it = slotMap.begin(), it_end = slotMap.end(); it != it_end; ++it)
            {
                typename Callback<Templates::InvokeMethod3<Return, Arg1, Arg2, Arg3> >::Ptr slot = it->second;

                typename Templates::FunctionPointer3<Class, Return, Arg1, Arg2, Arg3>
                    * funcPointer = dynamic_cast<Templates::FunctionPointer3<Class, Return, Arg1, Arg2, Arg3>* >(slot->GetArgBase());

                if(funcPointer != NULL && funcPointer->Delegate() == observer)
                {
                    deleteVector.push_back(it);
                }
            }
        }

        SignallerAlgorithms::emptyCollection<Collection>(slotMap, deleteVector);

        return !deleteVector.empty();
    }
    // --------------------------------------------------
    // Disconnect based on observer instance and function
    // --------------------------------------------------

    template <typename Return, typename Collection, typename Class>
    static bool disconnect(Collection& slotMap, Class* observer, Return (Class::*member)())
    {
        std::vector<typename Collection::iterator> deleteVector;

        {
            MutexTypeLocker<Collection>       lock(&slotMap);
            for(typename Collection::iterator it = slotMap.begin(), it_end = slotMap.end(); it != it_end; ++it)
            {
                typename Callback<Templates::InvokeMethod0<Return> >::Ptr slot = it->second;

                typename Templates::FunctionPointer0<Class, Return>* funcPointer = dynamic_cast<Templates::FunctionPointer0<Class, Return>* > (slot->GetArgBase());

                if(funcPointer != NULL && funcPointer->Delegate() == observer && funcPointer->Function() == member)
                {
                    deleteVector.push_back(it);
                }
            }
        }

        SignallerAlgorithms::emptyCollection<Collection>(slotMap, deleteVector);

        return !deleteVector.empty();
    }

    template <typename Return, typename Collection, typename Class, typename Arg1>
    static bool disconnect(Collection& slotMap, Class* observer, Return (Class::*member)(Arg1))
    {
        std::vector<typename Collection::iterator> deleteVector;

        {
            MutexTypeLocker<Collection>       lock(&slotMap);
            for(typename Collection::iterator it = slotMap.begin(), it_end = slotMap.end(); it != it_end; ++it)
            {
                typename Callback<Templates::InvokeMethod1<Return, Arg1> >::Ptr slot = it->second;

                typename Templates::FunctionPointer1<Class, Return, Arg1>* funcPointer = dynamic_cast<Templates::FunctionPointer1<Class, Return, Arg1>* > (slot->GetArgBase());

                if(funcPointer != NULL && funcPointer->Delegate() == observer && funcPointer->Function() == member)
                {
                    deleteVector.push_back(it);
                }
            }
        }

        SignallerAlgorithms::emptyCollection<Collection>(slotMap, deleteVector);

        return !deleteVector.empty();
    }

    template <typename Return, typename Collection, typename Class, typename Arg1, typename Arg2>
    static bool disconnect(Collection& slotMap, Class* observer, Return (Class::*member)(Arg1, Arg2))
    {
        std::vector<typename Collection::iterator> deleteVector;

        {
            MutexTypeLocker<Collection>       lock(&slotMap);
            for(typename Collection::iterator it = slotMap.begin(), it_end = slotMap.end(); it != it_end; ++it)
            {
                typename Callback<Templates::InvokeMethod2<Return, Arg1, Arg2> >::Ptr slot = it->second;

                typename Templates::FunctionPointer2<Class, Return, Arg1, Arg2>
                    * funcPointer = dynamic_cast<Templates::FunctionPointer2<Class, Return, Arg1, Arg2>* > (slot->GetArgBase());

                if(funcPointer != NULL && funcPointer->Delegate() == observer && funcPointer->Function() == member)
                {
                    deleteVector.push_back(it);
                }
            }
        }

        SignallerAlgorithms::emptyCollection<Collection>(slotMap, deleteVector);

        return !deleteVector.empty();
    }

    template <typename Return, typename Collection, typename Class, typename Arg1, typename Arg2, typename Arg3>
    static bool disconnect(Collection& slotMap, Class* observer, Return (Class::*member)(Arg1, Arg2, Arg3))
    {
        std::vector<typename Collection::iterator> deleteVector;

        {
            MutexTypeLocker<Collection>       lock(&slotMap);
            for(typename Collection::iterator it = slotMap.begin(), it_end = slotMap.end(); it != it_end; ++it)
            {
                typename Callback<Templates::InvokeMethod3<Return, Arg1, Arg2, Arg3> >::Ptr slot = it->second;

                typename Templates::FunctionPointer3<Class, Return, Arg1, Arg2, Arg3>
                    * funcPointer = dynamic_cast<Templates::FunctionPointer3<Class, Return, Arg1, Arg2, Arg3>* > (slot->GetArgBase());

                if(funcPointer != NULL && funcPointer->Delegate() == observer && funcPointer->Function() == member)
                {
                    deleteVector.push_back(it);
                }
            }
        }

        SignallerAlgorithms::emptyCollection<Collection>(slotMap, deleteVector);

        return !deleteVector.empty();
    }

    // --------------------------------------------------
    // Empty collection based on vector
    // --------------------------------------------------
    template <typename Collection>
    static void emptyCollection(Collection& slotMap, std::vector<typename Collection::iterator>& deleteVector)
    {
        MutexTypeLocker<Collection> lock(&slotMap);

        for(size_t i = 0; i < deleteVector.size(); i++)
        {
            typename Collection::iterator it = deleteVector[i];

            slotMap.erase(it);
        }
    }

    // --------------------------------------------------
    // Signalling functions without specifying function
    // --------------------------------------------------

    template <typename Return, typename Collection>
    static void signal(Collection& slotMap)
    {
        typedef typename Callback<Templates::InvokeMethod0<Return> >::Ptr CallbackPtr;
        typedef typename std::pair<InstanceHandle, CallbackPtr>           CollectionPair;

        MutexTypeLocker<Collection> lock(&slotMap);

        for(CollectionPair entry : slotMap)
        {
            CallbackPtr slot = entry.second;

            if(slot->GetSlotHolder()->IsBlocked()) continue;

            slot->GetArgBase()->Invoke();
        }
    }

    template <typename Return, typename Collection, typename Arg1>
    static void signal(Collection& slotMap, Arg1 arg1)
    {
        typedef typename Callback<Templates::InvokeMethod1<Return, Arg1> >::Ptr CallbackPtr;
        typedef std::pair<InstanceHandle, CallbackPtr>                          CollectionPair;

        MutexTypeLocker<Collection> lock(&slotMap);

        for(CollectionPair entry : slotMap)
        {
            CallbackPtr slot = entry.second;

            if(slot->GetSlotHolder()->IsBlocked()) continue;

            slot->GetArgBase()->Invoke(arg1);
        }
    }

    template <typename Return, typename Collection, typename Arg1, typename Arg2>
    static void signal(Collection& slotMap, Arg1 arg1, Arg2 arg2)
    {
        typedef typename Callback<Templates::InvokeMethod2<Return, Arg1, Arg2> >::Ptr CallbackPtr;
        typedef std::pair<InstanceHandle, CallbackPtr>                                CollectionPair;

        MutexTypeLocker<Collection> lock(&slotMap);

        for(CollectionPair entry : slotMap)
        {
            CallbackPtr slot = entry.second;

            if(slot->GetSlotHolder()->IsBlocked()) continue;

            slot->GetArgBase()->Invoke(arg1, arg2);
        }
    }

    template <typename Return, typename Collection, typename Arg1, typename Arg2, typename Arg3>
    static void signal(Collection& slotMap, Arg1 arg1, Arg2 arg2, Arg3 arg3)
    {
        typedef typename Callback<Templates::InvokeMethod3<Return, Arg1, Arg2, Arg3> >::Ptr CallbackPtr;
        typedef std::pair<InstanceHandle, CallbackPtr>                                      CollectionPair;

        MutexTypeLocker<Collection> lock(&slotMap);

        for(CollectionPair entry : slotMap)
        {
            CallbackPtr slot = entry.second;

            if(slot->GetSlotHolder()->IsBlocked()) continue;

            slot->GetArgBase()->Invoke(arg1, arg2, arg3);
        }
    }

    // --------------------------------------------------
    // Signalling functions specifying function
    // --------------------------------------------------

    template <typename Return, typename Collection, typename Class>
    static int signal(Collection& slotMap, Return (Class::*member)())
    {
        typedef typename Templates::FunctionPointer0<Class, Return>       FunctionPointer;
        typedef typename Callback<Templates::InvokeMethod0<Return> >::Ptr CallbackPtr;
        typedef std::pair<InstanceHandle, CallbackPtr>                    CollectionPair;

        int callbackCounter = 0;

        MutexTypeLocker<Collection> lock(&slotMap);

        for(CollectionPair entry : slotMap)
        {
            CallbackPtr slot = entry.second;

            if(slot->GetSlotHolder()->IsBlocked()) continue;

            FunctionPointer* funcPointer = dynamic_cast<FunctionPointer*>(slot->GetArgBase());

            if(funcPointer != NULL && funcPointer->Function() == member)
            {
                slot->GetArgBase()->Invoke();
                ++callbackCounter;
            }
        }

        return callbackCounter;
    }

    template <typename Return, typename Collection, typename Class, typename Arg1>
    static int signal(Collection& slotMap, Arg1 arg1, Return (Class::*member)(Arg1))
    {
        typedef typename Templates::FunctionPointer1<Class, Return, Arg1>       FunctionPointer;
        typedef typename Callback<Templates::InvokeMethod1<Return, Arg1> >::Ptr CallbackPtr;
        typedef std::pair<InstanceHandle, CallbackPtr>                          CollectionPair;

        int callbackCounter = 0;

        MutexTypeLocker<Collection> lock(&slotMap);

        for(CollectionPair entry : slotMap)
        {
            CallbackPtr slot = entry.second;

            if(slot->GetSlotHolder()->IsBlocked()) continue;

            FunctionPointer* funcPointer = dynamic_cast<FunctionPointer*>(slot->GetArgBase());

            if(funcPointer != NULL && funcPointer->Function() == member)
            {
                slot->GetArgBase()->Invoke(arg1);
                ++callbackCounter;
            }
        }

        return callbackCounter;
    }

    template <typename Return, typename Collection, typename Class, typename Arg1, typename Arg2>
    static int signal(Collection& slotMap, Arg1 arg1, Arg2 arg2, Return (Class::*member)(Arg1, Arg2))
    {
        typedef typename Templates::FunctionPointer2<Class, Return, Arg1, Arg2>       FunctionPointer;
        typedef typename Callback<Templates::InvokeMethod2<Return, Arg1, Arg2> >::Ptr CallbackPtr;
        typedef std::pair<InstanceHandle, CallbackPtr>                                CollectionPair;

        int callbackCounter = 0;

        MutexTypeLocker<Collection> lock(&slotMap);

        for(CollectionPair entry : slotMap)
        {
            CallbackPtr slot = entry.second;

            if(slot->GetSlotHolder()->IsBlocked()) continue;

            FunctionPointer* funcPointer = dynamic_cast<FunctionPointer*>(slot->GetArgBase());

            if(funcPointer != NULL && funcPointer->Function() == member)
            {
                slot->GetArgBase()->Invoke(arg1, arg2);
                ++callbackCounter;
            }
        }

        return callbackCounter;
    }

    template <typename Return, typename Collection, typename Class, typename Arg1, typename Arg2, typename Arg3>
    static int signal(Collection& slotMap, Arg1 arg1, Arg2 arg2, Arg3 arg3, Return (Class::*member)(Arg1, Arg2, Arg3))
    {
        typedef typename Templates::FunctionPointer3<Class, Return, Arg1, Arg2, Arg3>       FunctionPointer;
        typedef typename Callback<Templates::InvokeMethod3<Return, Arg1, Arg2, Arg3> >::Ptr CallbackPtr;
        typedef std::pair<InstanceHandle, CallbackPtr>                                      CollectionPair;

        int callbackCounter = 0;

        MutexTypeLocker<Collection> lock(&slotMap);

        for(CollectionPair entry : slotMap)
        {
            CallbackPtr slot = entry.second;

            if(slot->GetSlotHolder()->IsBlocked()) continue;

            FunctionPointer* funcPointer = dynamic_cast<FunctionPointer*>(slot->GetArgBase());

            if(funcPointer != NULL && funcPointer->Function() == member)
            {
                slot->GetArgBase()->Invoke(arg1, arg2, arg3);
                ++callbackCounter;
            }
        }

        return callbackCounter;
    }

    // --------------------------------------------------
    // Asynch signalling functions
    // --------------------------------------------------

    template <typename SIGNALLER>
    static void asynchSignal(SIGNALLER parent)
    {
        if(parent->IsEmpty()) return;

        SignallerAlgorithms::scheduleSignal(new Templates::Signal0<SIGNALLER>(parent));
    }

    template <typename SIGNALLER, typename Arg1>
    static void asynchSignal(SIGNALLER parent, Arg1 arg1)
    {
        if(parent->IsEmpty()) return;

        SignallerAlgorithms::scheduleSignal(new Templates::Signal1<SIGNALLER, Arg1>(parent, arg1));
    }

    template <typename SIGNALLER, typename Arg1, typename Arg2>
    static void asynchSignal(SIGNALLER parent, Arg1 arg1, Arg2 arg2)
    {
        if(parent->IsEmpty()) return;

        SignallerAlgorithms::scheduleSignal(new Templates::Signal2<SIGNALLER, Arg1, Arg2>(parent, arg1, arg2));
    }

    template <typename SIGNALLER, typename Arg1, typename Arg2, typename Arg3>
    static void asynchSignal(SIGNALLER parent, Arg1 arg1, Arg2 arg2, Arg3 arg3)
    {
        if(parent->IsEmpty()) return;

        SignallerAlgorithms::scheduleSignal(new Templates::Signal3<SIGNALLER, Arg1, Arg2, Arg3>(parent, arg1, arg2, arg3));
    }

    // --------------------------------------------------
    // Asynch signalling functions specifying function
    // --------------------------------------------------

    template <typename Return, typename SIGNALLER, typename Class>
    static void asynchSignal(SIGNALLER parent, Return (Class::*member)())
    {
        if(parent->IsEmpty()) return;

        SignallerAlgorithms::scheduleSignal(new Templates::FunctionSignal0<Return, Class, SIGNALLER>(parent, member));
    }

    template <typename Return, typename SIGNALLER, typename Class, typename Arg1>
    static void asynchSignal(SIGNALLER parent, Arg1 arg1, Return (Class::*member)(Arg1))
    {
        if(parent->IsEmpty()) return;

        SignallerAlgorithms::scheduleSignal(new Templates::FunctionSignal1<Return, Class, SIGNALLER, Arg1>(parent, member, arg1));
    }

    template <typename Return, typename SIGNALLER, typename Class, typename Arg1, typename Arg2>
    static void asynchSignal(SIGNALLER parent, Arg1 arg1, Arg2 arg2, Return (Class::*member)(Arg1, Arg2))
    {
        if(parent->IsEmpty()) return;

        SignallerAlgorithms::scheduleSignal(new Templates::FunctionSignal2<Return, Class, SIGNALLER, Arg1, Arg2>(parent, member, arg1, arg2));
    }

    template <typename Return, typename SIGNALLER, typename Class, typename Arg1, typename Arg2, typename Arg3>
    static void asynchSignal(SIGNALLER parent, Arg1 arg1, Arg2 arg2, Arg3 arg3, Return (Class::*member)(Arg1, Arg2, Arg3))
    {
        if(parent->IsEmpty()) return;

        SignallerAlgorithms::scheduleSignal(new Templates::FunctionSignal3<Return, Class, SIGNALLER, Arg1, Arg2, Arg3>(parent, member, arg1, arg2, arg3));
    }

    // --------------------------------------------------
    // Signal to threadPool
    // --------------------------------------------------

    static void scheduleSignal(Templates::Signal* signal);


    //template <typename CollectorT>
    //typename CollectorT::return_type Signal(CollectorT& collect)
    //{
    //    MutexLocker lock(&mutex_);
    //    return this->signal<CollectorT>(collect);
    //}

    /*template <typename CollectorT>
    typename CollectorT::return_type signal(CollectorT& collect)
    {
        std::vector<Return> results;
        for(typename MapIdHandler::iterator it = slotMap_.begin(), end = slotMap_.end(); it != end; ++it)
        {
            ASSERT( (it->second)->GetArgBase() );
            ASSERT( (it->second)->GetSlotHolder() );

            if((it->second)->GetSlotHolder()->IsBlocked() == true)
                continue;

            results.push_back( (it->second)->GetArgBase()->Invoke() );
        }

        return collect(results.begin(),results.end());
    }*/


    //template <typename CollectorT>
    //typename CollectorT::return_type Signal(Arg1 arg1, CollectorT& collect)
    //{
    //    MutexLocker lock(&mutex_);
    //    return this->signal<CollectorT>(arg1, collect);
    //}
    //    template <typename CollectorT>
    //    typename CollectorT::return_type signal(Arg1 arg1, CollectorT& collect)
    //    {
    //        std::vector<Return> results;
    //        for(typename MapIdHandler::iterator it = slotMap_.begin(), end = slotMap_.end(); it != end; ++it)
    //        {
    //            ASSERT( (it->second)->GetArgBase() );
    //            ASSERT( (it->second)->GetSlotHolder() );
    //            if((it->second)->GetSlotHolder()->IsBlocked() == true)
    //                continue;
    //            results.push_back( (it->second)->GetArgBase()->Invoke(arg1) );
    //        }
    //        return collect(results.begin(),results.end());
    //    }

};

}}
