#ifndef BaseLib_Collection_ConcurrentMap_h_IsIncluded
#define BaseLib_Collection_ConcurrentMap_h_IsIncluded

#include "BaseLib/CommonDefines.h"
#include "BaseLib/ReadWriteLock.h"
#include "BaseLib/WriteLocker.h"
#include "BaseLib/ReadLocker.h"
#include "BaseLib/WaitCondition.h"
#include "BaseLib/ElapsedTimer.h"
#include "BaseLib/Utility.h"

namespace BaseLib { namespace Collection
{

template <typename K, typename V>
class ConcurrentMap : private std::map<K,V>
{
private:
    typedef std::map<K,V>   Map;
    typedef std::pair<K,V>  Pair;

public:
    typedef typename Map::iterator         Iterator;
    typedef typename Map::reverse_iterator RIterator;

    typedef typename Map::const_iterator           ConstIterator;
    typedef typename Map::const_reverse_iterator   ConstRIterator;

public:
    ConcurrentMap()
    {}

    ConcurrentMap(const ConcurrentMap &orig)
        : Map(orig)
    {}

    virtual ~ConcurrentMap()
    {}

    ConcurrentMap<K,V>& operator=(const ConcurrentMap<K,V>& orig )
    {
        WriteLocker lock(&mutex_);
        Map::operator=(orig);
        return *this;
    }

    CLASS_TRAITS(ConcurrentMap)

public:
    inline bool empty() const
    {
        ReadLocker lock(&mutex_);
        return Map::empty();
    }

    inline unsigned int length() const
    {
        ReadLocker lock(&mutex_);
        return Map::size();
    }

    inline std::pair<Iterator, bool> insert(const Pair &pair)
    {
        WriteLocker lock(&mutex_);
        return Map::insert(pair);
    }

    inline size_t erase(const K &x)
    {
        WriteLocker lock(&mutex_);
        return Map::erase(x);
    }

    inline void erase(Iterator position)
    {
        WriteLocker lock(&mutex_);
        Map::erase(position);
    }

    inline void erase(Iterator first, Iterator last)
    {
        WriteLocker lock(&mutex_);
        Map::erase(first, last);
    }

    V& operator[](const K &key)
    {
        ReadLocker lock(&mutex_);
        return Map::operator [](key);
    }

//    Iterator find(const K &key)
//    {
//        ReadLocker lock(&mutex_);
//        return Map::find(key);
//    }

    ConstIterator find(const K &key) const
    {
        ReadLocker lock(&mutex_);
        return Map::find(key);
    }

//    Iterator end()
//    {
//        ReadLocker lock(&mutex_);
//		return Map::end();
//    }

    ConstIterator end() const
    {
        ReadLocker lock(&mutex_);
		return Map::end();
    }

    ConstIterator begin() const
    {
        ReadLocker lock(&mutex_);
		return Map::begin();
    }

public:
    inline Iterator unlocked_begin()
    {
        return Map::begin();
    }

    inline Iterator unlocked_end()
    {
		return Map::end();
    }

    inline ConstIterator unlocked_const_begin() const
    {
        return Map::begin();
    }

    inline ConstIterator unlocked_const_end() const
    {
		return Map::end();
    }

    inline RIterator unlocked_rbegin() const
    {
		return Map::rbegin();
    }

    inline RIterator unlocked_rend() const
    {
		return Map::rend();
    }

    inline bool unlocked_empty() const
    {
		return Map::empty();
    }

private:
    mutable ReadWriteLock mutex_;
};

}}

#endif // BaseLib_Collection_ConcurrentMap_h_IsIncluded
