#ifndef RxData_Cache_DataFlyweightWeakReference_h_IsIncluded
#define RxData_Cache_DataFlyweightWeakReference_h_IsIncluded

#include"RxData/CommonDefines.h"
#include"RxData/Export.h"

namespace RxData
{

/**
 * Implements a flyweight approach, where the object maintains a map of weak-references to generic type DATA.
 */
template <typename DATA>
class DataFlyweightWeakReference
{
private:
    CLASS_TRAITS(DATA)

public:
	DataFlyweightWeakReference() {}
	
	/**
	 * Return existing flyweight instance, or creates a new if no such exists.
	 */
	typename DATA::Ptr getUnique(typename DATA::Ptr state)
    {
        if(!state) return DATA::Ptr();

		// ------------------------------------------------------
		// Double-checked locking pattern
		// ------------------------------------------------------
        typename DATA::WeakPtr stateReference = uniqueData_.get(state);

        typename DATA::Ptr existingState = !stateReference ? DATA::Ptr() : stateReference.lock();
		
		if(!existingState)
		{
			// ---------------------------------------------------
			// A static synchronized method uses its class as lock
			// ---------------------------------------------------
            MutexLocker lock(&lock_);

            // -----------------------------------------------------------------------
            // Repeat this for thread safety as the previous lookup was unsynchronized
            // -----------------------------------------------------------------------
            stateReference = uniqueData_.get(state);

            existingState = !stateReference ? DATA::Ptr() : stateReference.lock();

            if(!existingState)
            {
                bool isInserted = uniqueData_.put(state, state);

                // -- debug --
                ASSERT(isInserted);
                // -- debug --

                return state;
            }
		}
		return existingState;
	}

private:
    IMap<typename DATA::WeakPtr, typename DATA::WeakPtr> uniqueData_;

private:
    mutable Mutex   lock_;
};

}

#endif
