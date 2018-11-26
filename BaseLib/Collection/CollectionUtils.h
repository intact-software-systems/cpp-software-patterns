#ifndef BaseLib_Collection_CollectionUtils_h_IsIncluded
#define BaseLib_Collection_CollectionUtils_h_IsIncluded

#include"BaseLib/CommonDefines.h"
#include"BaseLib/Collection/IList.h"

namespace BaseLib { namespace Collection
{

// ------------------------------------------------
// TODO: Add select(collection, filter) functions
// others?
// ------------------------------------------------
class CollectionUtils
{
public:
    virtual ~CollectionUtils() {}

    template <typename T>
    static std::list<T> EmptyList()
    {
        return std::list<T>();
    }

    template <typename T>
    static IList<T> EmptyIList()
    {
        return IList<T>();
    }

    template <typename T>
    static std::vector<T> EmptyVector()
    {
        return std::vector<T>();
    }

    template <typename T>
    static std::set<T> EmptySet()
    {
        return std::set<T>();
    }

    template <typename K, typename V>
    static std::map<K,V> EmptyMap()
    {
        return std::map<K,V>();
    }
};

}}

#endif
