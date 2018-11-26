#ifndef BaseLib_Templates_IteratorBase_h_Included
#define BaseLib_Templates_IteratorBase_h_Included

#include"BaseLib/Templates/BaseTypes.h"

namespace BaseLib { namespace Templates
{

/**
 * Iterators can be used to iterate over aggregates, for example, sets, lists, maps, etc.
 * This means it can be combined with many other software patterns:
 *  - Interceptors, Memento, ObjectManager/Cache, etc.
 */
template <typename T>
class Iterator
{
public:
    /**
     * @brief HasNext
     * @return
     */
    virtual bool HasNext() const = 0;

    /**
     * @brief Next
     * @return
     */
    virtual T Next() = 0;

    /**
     * @brief Remove
     */
    virtual void Remove() = 0;

    /**
     * @brief Rewind
     */
    virtual void Rewind() = 0;
};

/**
 * Iterable constructs an interator.
 *
 * Implemented by object that wishes to support "iterate internal data externally".
 */
template <typename T>
class Iterable
{
public:
    /**
     * @brief Iterator
     * @return
     */
    virtual Templates::Iterator<T> Iterator() = 0;
};



namespace details
{

template <typename Collection, typename T>
class IteratorBase : public Templates::Iterator<T>
{
public:
    IteratorBase() : Templates::Iterator<T>()
    {}
    virtual ~IteratorBase()
    {}

protected:

    static typename Collection::iterator nextIterator(const typename Collection::iterator &curr)
    {
        typename Collection::iterator next = curr;
        ++next;

        return next;
    }

    static bool hasNext(const Collection &collection, const typename Collection::iterator &curr)
    {
        return nextIterator(curr) != collection.end();
    }

    static bool isAtEnd(const Collection &collection, const typename Collection::iterator &curr)
    {
        return curr == collection.end();
    }
};

template <typename Collection, typename T>
class IteratorStdList : public details::IteratorBase<Collection, T>
{
public:
    IteratorStdList(Collection &collection)
        : details::IteratorBase<Collection, T>()
        , collection_(collection)
        , curr_(collection.begin())
    {}
    virtual ~IteratorStdList()
    {}

    virtual bool HasNext() const
    {
        return !IteratorBase<Collection,T>::isAtEnd(collection_, curr_) && IteratorBase<Collection,T>::hasNext(collection_, curr_);
    }

    virtual T Next()
    {
        if(curr_ != collection_.begin())
            ++curr_;

        return *curr_;
    }

    virtual void Remove()
    {
        if(IteratorBase<Collection,T>::isAtEnd(collection_, curr_)) return;

        typename Collection::iterator removeIterator = curr_;

        if(curr_ != collection_.begin())
            --curr_;
        else
            ++curr_;

        collection_.erase(removeIterator);
    }

    virtual void Rewind()
    {
        curr_ = collection_.begin();
    }

private:
    Collection &collection_;
    typename Collection::iterator curr_;
};

/**
 *
 */
template <typename MapCollection, typename T>
class IteratorStdMap : public details::IteratorBase<MapCollection, T>
{
public:
    IteratorStdMap(MapCollection &collection)
        : details::IteratorBase<MapCollection, T>()
        , collection_(collection)
        , curr_(collection.begin())
    {}
    virtual ~IteratorStdMap()
    {}

    virtual bool HasNext() const
    {
        return !IteratorBase<MapCollection,T>::isAtEnd(collection_, curr_) && IteratorBase<MapCollection,T>::hasNext(collection_, curr_);
    }

//    virtual MapCollection::iterator Next()
//    {
//        if(curr_ != collection.begin())
//            ++curr_;

//        return curr_;
//    }

    virtual T Next()
    {
        return T();
    }

    virtual void Remove()
    {
        if(IteratorBase<MapCollection,T>::isAtEnd(collection_, curr_)) return;

        typename MapCollection::iterator removeIterator = curr_;

        if(curr_ != collection_.begin())
            --curr_;
        else
            ++curr_;

        collection_.erase(removeIterator);
    }

    virtual void Rewind()
    {
        curr_ = collection_.begin();
    }

private:
    MapCollection &collection_;
    typename MapCollection::iterator curr_;
};

}


}}

#endif
