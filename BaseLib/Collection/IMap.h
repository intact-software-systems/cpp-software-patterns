#ifndef BaseLib_Collection_IMap_h_IsIncluded
#define BaseLib_Collection_IMap_h_IsIncluded

#include"BaseLib/CommonDefines.h"
#include"BaseLib/Templates/Synchronization.h"
#include"BaseLib/Collection/ISet.h"

namespace BaseLib { namespace Collection
{

template <typename K, typename V>
class IMap : public std::map<K,V>
           , public BaseLib::Templates::Lockable<Mutex>
{
private:
    typedef std::map<K,V>  Map;
    typedef std::pair<K,V> Pair;

    typedef typename Map::iterator         Iterator;
    typedef typename Map::reverse_iterator RIterator;

    typedef typename Map::const_iterator           ConstIterator;
    typedef typename Map::const_reverse_iterator   ConstRIterator;

public:
    IMap()
    {}

    IMap(const IMap &orig)
        : Map(orig)
    {}

    virtual ~IMap()
    {}

    CLASS_TRAITS(IMap)

public:
    bool put(const K &key, const V &value)
    {
        std::pair<Iterator, bool> pos = Map::insert(std::pair<K,V>(key, value));
        return pos.second;
    }

    bool put(const Map &values)
    {
        for(const Pair& pair : values)
        {
            this->insert(pair);
        }
        return true;
    }

    std::set<K> keySet() const
    {
        std::set<K> keyes;
        for(ConstIterator it = Map::begin(), it_end = Map::end(); it != it_end; ++it)
        {
            keyes.insert(it->first);
        }

        return keyes;
    }

    Collection::ISet<K> keyISet() const
    {
        ISet<K> keyes;
        for(ConstIterator it = Map::begin(), it_end = Map::end(); it != it_end; ++it)
        {
            keyes.insert(it->first);
        }

        return keyes;
    }

    size_t eraseKeys(const std::set<K> &keys)
    {
        size_t numErased = 0;

        for(typename std::set<K>::const_iterator it = keys.begin(), it_end = keys.end(); it != it_end; ++it)
        {
            numErased += Map::erase(*it);
        }

        return numErased;
    }

    bool containsValue(const V &value) const
    {
        for(ConstIterator it = Map::begin(), it_end = Map::end(); it != it_end; ++it)
            if(value == it->second) return true;

        return false;
    }

    bool containsKey(K key) const
    {
        return Map::find(key) != Map::end();
    }

    V remove(K key)
    {
        ConstIterator it = Map::find(key);
        if(it == Map::end())
        {
            return V();
        }

        Map::erase(it);

        return it->second;
    }

    V get(K key) const
    {
        ConstIterator it = Map::find(key);

        if(it == Map::end())
        {
            return V();
        }

        return it->second;
    }

public:
    friend std::ostream& operator<<(std::ostream &ostr, const IMap<K,V> &other)
    {
        for(ConstIterator it = other.begin(), it_end = other.end(); it != it_end; ++it)
        {
            ostr << "(" << it->first << "," << it->second << ")";
        }
        return ostr;
    }
};

template <typename K, typename V>
class IMultiMap : public std::multimap<K,V>, public BaseLib::Templates::Lockable<Mutex>
{
private:
    typedef std::multimap<K,V>  MultiMap;
    typedef std::pair<K,V> Pair;

    typedef typename MultiMap::iterator         Iterator;
    typedef typename MultiMap::reverse_iterator RIterator;

    typedef typename MultiMap::const_iterator           ConstIterator;
    typedef typename MultiMap::const_reverse_iterator   ConstRIterator;

public:
    IMultiMap()
    {}

    IMultiMap(const IMultiMap &orig)
        : MultiMap(orig)
    {}

    virtual ~IMultiMap()
    {}

    CLASS_TRAITS(IMultiMap)

public:
    Iterator put(const K &key, const V &value)
    {
        return this->insert(Pair(key, value));
    }

    std::set<K> keySet() const
    {
        std::set<K> keyes;
        for(ConstIterator it = MultiMap::begin(), it_end = MultiMap::end(); it != it_end; ++it)
        {
            keyes.insert(it->first);
        }

        return keyes;
    }

    unsigned int eraseKeys(const std::set<K> &keys)
    {
        unsigned int numErased = 0;

        for(typename std::set<K>::const_iterator it = keys.begin(), it_end = keys.end(); it != it_end; ++it)
        {
            numErased += MultiMap::erase(*it);
        }

        return numErased;
    }

    bool containsValue(const V &value) const
    {
        for(ConstIterator it = MultiMap::begin(), it_end = MultiMap::end(); it != it_end; ++it)
            if(value == it->second) return true;

        return false;
    }

    bool containsKey(K key) const
    {
        return MultiMap::find(key) != MultiMap::end();
    }

//    V remove(K key)
//    {
//        Iterator it = MultiMap::find(key);
//        if(it == MultiMap::end())
//        {
//            return V();
//        }

//        MultiMap::erase(it);

//        return it->second;
//    }

    V get(K key) const
    {
        ConstIterator it = MultiMap::find(key);

        if(it == MultiMap::end())
        {
            return V();
        }

        return it->second;
    }
};


/**
 * @brief The IMap class
 *
 * TODO: typename LOCK = NullReadWriteLock - makes this synchronized or not
 */
/*template <typename K, typename V>
class IMultiMap : public std::multimap<K,V>
                , public Templates::Lockable
{
private:
    typedef std::multimap<K,V>  MultiMap;
    typedef std::pair<K,V>      Pair;

public:
    typedef typename MultiMap::iterator         Iterator;
    typedef typename MultiMap::reverse_iterator RIterator;

    typedef typename MultiMap::const_iterator           ConstIterator;
    typedef typename MultiMap::const_reverse_iterator   ConstRIterator;

public:
    IMultiMap()
    {}

    IMultiMap(const IMultiMap &orig)
        : Map(orig)
    {}

    virtual ~IMultiMap()
    {}


    virtual Pair deleteMinimum()
    {
        if(this->empty()) return Pair();

        Iterator it = this->begin();
        Pair removedItem(it->first, it->second);

        MultiMap::erase(it);

        return removedItem;
    }

    virtual std::pair<Iterator, bool> insert(const K &key, const V &value)
    {
        return MultiMap::insert( Pair(key, value) );
    }

    virtual long double getKey(int item) = 0;
    virtual int nItems() const = 0;
    virtual long nComps() const = 0;
    virtual void dump() const = 0;

    virtual void decreaseKey(const K& newKey, const V &value)
    {

    }

};*/

}} // namespace BaseLib::Collection

#endif
