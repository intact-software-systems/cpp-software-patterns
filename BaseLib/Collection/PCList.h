#ifndef BaseLib_PCList_h_IsIncluded
#define BaseLib_PCList_h_IsIncluded

#include <list>

#include "BaseLib/CommonDefines.h"
#include "BaseLib/ProtectedVar.h"
#include "BaseLib/ReadWriteLock.h"
#include "BaseLib/ReadLocker.h"
#include "BaseLib/WriteLocker.h"
#include "BaseLib/WaitCondition.h"

namespace BaseLib
{

// TODO: Move VertexSet from GraphLib to here
// TODO: Create a Container class? Look at how Java does it and Qt
//
// TODO:
// - Add producer/consumer functions with timeouts
// - Add bounded queue functionality
template <class T>
class PCList : private std::list<T>
{
public:
    typedef typename std::list<T>::iterator         Iterator;
    typedef typename std::list<T>::reverse_iterator RIterator;

    typedef typename std::list<T>::const_iterator           ConstIterator;
    typedef typename std::list<T>::const_reverse_iterator   ConstRIterator;

private:
    mutable ReadWriteLock   mutex_;
    mutable WaitCondition   waitCondition_;

public:
    PCList(const PCList<T>& orig)
        : std::list<T>(orig)
    {
    }

    PCList()
    {
    }

    virtual ~PCList()
    {
    }

    PCList<T>& operator=( const PCList<T>& orig )
    {
        WriteLocker lock(&mutex_);
        std::list<T>::operator=(orig);
        return *this;
    }

    inline bool empty() const
    {
        ReadLocker lock(&mutex_);
        return std::list<T>::empty();
    }

    inline unsigned int length() const
    {
        ReadLocker lock(&mutex_);
        return std::list<T>::size();
    }

    inline T front() const
    {
        ReadLocker lock(&mutex_);
        return std::list<T>::front();
    }

    inline T back() const
    {
        ReadLocker lock(&mutex_);
        return std::list<T>::back();
    }

#if 0
    /*
     * T& inf() can not be
     * implemented with producer-consumer lists
     */
    inline T inf(Iterator r)
    {
        lock();
        T ret = *r;
        unlock();
        return ret;
    }
#endif

    inline void pop_front()
    {
        WriteLocker lock(&mutex_);
        std::list<T>::pop_front();
    }

    inline void pop_back()
    {
        WriteLocker lock(&mutex_);
        std::list<T>::pop_back();
    }

    inline void clear()
    {
        WriteLocker lock(&mutex_);
        std::list<T>::clear();
    }

    inline T pop()
    {
        WriteLocker lock(&mutex_);
        return unlocked_pop_front();
    }

    inline void push_front( T x )
    {
        WriteLocker lock(&mutex_);
        std::list<T>::push_front(x);
    }

    inline void push_back( T x )
    {
        WriteLocker lock(&mutex_);
        std::list<T>::push_back(x);
    }

public:
    inline T consumer_pop_front(int64 msecs = LONG_MAX)
    {
        WriteLocker lock(&mutex_);

        //while(unlocked_empty())
        if(unlocked_empty())
        {
            waitCondition_.wait(&mutex_, msecs);
        }

        return unlocked_pop_front();
    }

    inline T consumer_pop_back(int64 msecs = LONG_MAX)
    {
        WriteLocker lock(&mutex_);

        //while(unlocked_empty())
        if(unlocked_empty())
        {
            waitCondition_.wait(&mutex_, msecs);
        }

        return unlocked_pop_back();
    }

    inline void producer_push_front( T x )
    {
        WriteLocker lock(&mutex_);
        std::list<T>::push_front(x);
        waitCondition_.wakeOne();
    }

    inline void producer_push_back( T x )
    {
        WriteLocker lock(&mutex_);
        std::list<T>::push_back(x);
        waitCondition_.wakeOne();
    }

public:
    inline Iterator unlocked_begin()
    {
        return std::list<T>::begin();
    }

    inline Iterator unlocked_end()
    {
        return std::list<T>::end();
    }

    inline ConstIterator unlocked_const_begin() const
    {
        return std::list<T>::begin();
    }

    inline ConstIterator unlocked_const_end() const
    {
        return std::list<T>::end();
    }

    inline RIterator unlocked_rbegin() const
    {
        return std::list<T>::rbegin();
    }

    inline RIterator unlocked_rend() const
    {
        return std::list<T>::rend();
    }

    inline bool unlocked_empty() const
    {
        return std::list<T>::empty();
    }

    inline T unlocked_pop_front()
    {
        T ret = std::list<T>::front();
        std::list<T>::pop_front();
        return ret;
    }

    inline T unlocked_pop_back()
    {
        T ret = std::list<T>::back();
        std::list<T>::pop_back();
        return ret;
    }

    inline void unlocked_append( T x )
    {
        std::list<T>::push_back(x);
    }

    inline T unlocked_inf(Iterator r)
    {
        return *r;
    }

    inline void unlocked_erase(Iterator item)
    {
        std::list<T>::erase(item);
    }

    inline void unlocked_ins_before( Iterator x, T y )
    {
        if( x == std::list<T>::end() )
            std::list<T>::push_back(y);
        else
            std::list<T>::insert(x,y);
    }

    inline void unlocked_ins_after( Iterator x, T y )
    {
        if( x != std::list<T>::end() ) x++;
        if( x == std::list<T>::end() )
            std::list<T>::push_back(y);
        else
            std::list<T>::insert(x,y);
    }
};


}

#endif //

