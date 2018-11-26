#ifndef BaseLib_Collection_IList_h_IsIncluded
#define BaseLib_Collection_IList_h_IsIncluded

#include"BaseLib/CommonDefines.h"
#include"BaseLib/Templates/Synchronization.h"

namespace BaseLib { namespace Collection
{

template <typename T>
class IList : public std::list<T>
           , public BaseLib::Templates::Lockable<Mutex>
{
protected:
    typedef std::list<T>  List;

    typedef typename List::iterator         Iterator;
    typedef typename List::reverse_iterator RIterator;

    typedef typename List::const_iterator           ConstIterator;
    typedef typename List::const_reverse_iterator   ConstRIterator;

public:
    IList()
    {}

    IList(const IList &orig)
        : List(orig)
    {}

    virtual ~IList()
    {}

    CLASS_TRAITS(IList)

    bool contains(const T &t) const
    {
        for(ConstIterator it = List::begin(), it_end = List::end(); it != it_end; ++it)
        {
            if(*it == t) return true;
        }

        return false;
    }

    void put(const T &t)
    {
        List::push_back(t);
    }

//public:
//    bool put(const K &key, const V &value)
//    {
//        std::pair<Iterator, bool> pos = this->insert(Pair(key, value));
//        return pos.second;
//    }

//    std::set<KEY> keySet() const
//    {
//        std::set<KEY> keyes;
//        for(ConstIterator it = begin(), it_end = end(); it != it_end; ++it)
//        {
//            keyes.insert(it->first);
//        }

//        return keyes;
//    }

//    bool containsKey(K key) const
//    {
//        for(ConstIterator it = begin(), it_end = end(); it != it_end; ++it)
//            if(key == it->first) return true;

//        return false;
//    }

//    bool containsValue(const V &value) const
//    {
//        return objectHomes_.find(value) != objectHomes_.end();
//    }
};

}}

#endif
