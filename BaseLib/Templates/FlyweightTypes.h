#ifndef BaseLib_Templates_FlyweightTypes_h_Included
#define BaseLib_Templates_FlyweightTypes_h_Included

#include"BaseLib/CommonDefines.h"

#include"BaseLib/Templates/Synchronization.h"
#include"BaseLib/Templates/DoubleCheckedLocking.h"

namespace BaseLib { namespace Templates
{

/**
 * Implements a flyweight approach, where the object maintains a map of weak-references to generic type DATA.
 */
template <typename DATA>
class FlyweightWeakReference
        : public LockableType< FlyweightWeakReference<DATA> >
{
private:
    CLASS_TRAITS(DATA)

public:
    virtual ~FlyweightWeakReference() {}

    /**
     * Return existing flyweight instance, or creates a new if no such exists.
     */
    template <typename MapCollection>
    typename DATA::Ptr Unique(MapCollection uniqueData, typename DATA::Ptr state)
    {
        return DoubleCheckedLocking::UniqueWeakPtr<FlyweightWeakReference<DATA>, typename DATA::Ptr, typename DATA::WeakPtr, MapCollection>(this, uniqueData, state);
    }
};

}}

#endif
