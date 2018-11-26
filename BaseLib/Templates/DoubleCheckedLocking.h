#ifndef BaseLib_Templates_DoubleCheckedLocking_h_Included
#define BaseLib_Templates_DoubleCheckedLocking_h_Included

#include"BaseLib/CommonDefines.h"
#include"BaseLib/Templates/BaseTypes.h"

namespace BaseLib { namespace Templates
{

/**
 * @brief The DoubleCheckedLocking class implements versions of the "Double checked locking pattern".
 */
class DoubleCheckedLocking
{
public:
    virtual ~DoubleCheckedLocking() {}

    // -------------------------------------------
    // Initialize
    // -------------------------------------------

    template <typename T>
    static bool Initialize(T *service)
    {
        if(service->IsInitialized())
            return true;

        MutexTypeLocker<T> lock(service);

        if(service->IsInitialized())
            return true;

        service->Initialize();

        return service->IsInitialized();
    }

    // -------------------------------------------
    // Validate
    // -------------------------------------------

    template <typename T>
    static bool Validate(T *service)
    {
        if(service->IsValidated())
            return true;

        MutexTypeLocker<T> lock(service);

        if(service->IsValidated())
            return true;

        service->Validate();

        return service->IsValidated();
    }


    // -------------------------------------------
    // Resume and Suspend
    // -------------------------------------------

    template <typename T>
    static bool ConditionalResume(T *service)
    {
        if(!service->IsResumable())
            return false;

        MutexTypeLocker<T> lock(service);

        if(!service->IsResumable())
            return false;

        service->Resume();

        return service->IsResumed();
    }

    template <typename T>
    static bool ConditionalSuspend(T *service)
    {
        if(!service->IsSuspendable())
            return false;

        MutexTypeLocker<T> lock(service);

        if(!service->IsSuspendable())
            return false;

        service->Suspend();

        return service->IsSuspended();
    }


    // -------------------------------------------
    // Login
    // -------------------------------------------

    template <typename T>
    static bool Login(T *service)
    {
        if(service->IsLoggedIn())
            return true;

        MutexTypeLocker<T> lock(service);

        if(service->IsLoggedIn())
            return true;

        service->Login();

        return service->IsLoggedIn();
    }

    // -------------------------------------------
    // Unique
    // -------------------------------------------

    /**
     * Return existing instance, or creates a new if no such exists.
     */
    template <typename T, typename DATAPtr, typename DataWeakPtr, typename MapCollection>
    static DATAPtr UniqueWeakPtr(
            T *sync,
            MapCollection &uniqueData,
            const DATAPtr state)
    {
        if(!state || !sync) return state;

        // ------------------------------------------------------
        // Double-checked locking pattern
        // ------------------------------------------------------
        DATAPtr existingState = uniqueData.get(state).lock();

        if(!existingState)
        {
            MutexTypeLocker<T> lock(sync);

            existingState = uniqueData.get(state).lock();

            if(!existingState)
            {
                uniqueData.put(state, state);
                return state;
            }
        }
        return existingState;
    }

    /**
     * Return existing instance, or creates a new if no such exists.
     */
//    template <typename T, typename DATA, typename MapCollection>
//    static typename DATA::Ptr UniquePtr(T *sync, MapCollection<typename DATA::Ptr, typename DATA::Ptr> &uniqueData, const typename DATA::Ptr state)
//    {
//        if(!state) return state;

//        // ------------------------------------------------------
//        // Double-checked locking pattern
//        // ------------------------------------------------------
//        typename DATA::Ptr existingState = uniqueData.get(state);

//        if(!existingState)
//        {
//            // ---------------------------------------------------
//            // A static synchronized method uses its class as lock
//            // ---------------------------------------------------
//            MutexTypeLocker<T> lock(sync);

//            // -----------------------------------------------------------------------
//            // Repeat this for thread safety as the previous lookup was unsynchronized
//            // -----------------------------------------------------------------------
//            existingState = uniqueData.get(state);

//            if(!existingState)
//            {
//                bool isInserted = uniqueData.put(state, state);

//                // -- debug --
//                ASSERT(isInserted);
//                // -- debug --

//                return state;
//            }
//        }
//        return existingState;
//    }

    /**
     * Return existing instance, or creates a new if no such exists.
     */
    template <typename T, typename DATA, typename MapCollection>
    static DATA Unique(
            T *sync,
            MapCollection &uniqueData,
            const DATA &state)
    {
        if(!state || !sync) return state;

        // ------------------------------------------------------
        // Double-checked locking pattern
        // ------------------------------------------------------
        DATA existingState = uniqueData.get(state);

        if(!existingState)
        {
            MutexTypeLocker<T> lock(sync);

            existingState = uniqueData.get(state);

            if(!existingState)
            {
                uniqueData.put(state, state);
                return state;
            }
        }
        return existingState;
    }


    /**
     * Return existing instance, or creates a new if no such exists.
     */
    template <typename T, typename DATA, typename MapCollection>
    static DATA* UniqueRaw(
            T *sync,
            MapCollection &uniqueData,
            const DATA *const state)
    {
        if(!state || !sync) return state;

        // ------------------------------------------------------
        // Double-checked locking pattern
        // ------------------------------------------------------
        DATA *existingState = uniqueData.get(state);

        if(!existingState)
        {
            // ---------------------------------------------------
            // A static synchronized method uses its class as lock
            // ---------------------------------------------------
            MutexTypeLocker<T> lock(sync);

            // -----------------------------------------------------------------------
            // Repeat this for thread safety as the previous lookup was unsynchronized
            // -----------------------------------------------------------------------
            existingState = uniqueData.get(state);

            if(!existingState)
            {
                uniqueData.put(state, state);
                return state;
            }
        }
        return existingState;
    }

//    template <typename Factory, typename K, typename V, typename Arg1>
//    static V GetOrCreate(Factory factory, K key, Arg1 arg1)
//    {
//        V existingInstance;
//        V newInstance;

//        // -------------------------------------------------
//        // NB! This critical section is mutex with remove
//        // -------------------------------------------------
//        {
//            MutexTypeLocker<Factory> lock(factory);

//            existingInstance = factory.findValue(cacheMasterId);

//            if (existingInstance == null || existingInstance.isExpired()) {
//                newInstance = new V(cacheMasterId, cacheMasterPolicy);
//                state().getCacheMasters().put(newInstance.getMasterCacheId(), newInstance);
//            }
//        }

//        // -----------------------------------------------
//        // Expire existing (old) master if new is created
//        // -----------------------------------------------
//        if (newInstance != null && existingInstance != null) {
//            // Implies: existingMaster isExpired == true;
//            existingInstance.setExpired();
//            existingInstance.clearAll();
//        }

//        // -----------------------------------------------
//        // Observer callbacks
//        // -----------------------------------------------
//        if (existingInstance != null && existingInstance.isExpired()) {
//            state().getMasterCacheSubject().onExpiredMasterCache(existingInstance);
//        }
//        if (newInstance != null) {
//            state().getMasterCacheSubject().onCreatedMasterCache(newInstance);
//        }

//        // --------------------------------------
//        // always return new instance if created
//        // --------------------------------------
//        return newInstance != null
//                ? newInstance
//                : existingInstance;
//    }

};

}}

#endif
