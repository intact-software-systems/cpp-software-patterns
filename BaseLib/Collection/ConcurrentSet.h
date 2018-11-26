#ifndef BaseLib_Collection_ConcurrentSet_h_IsIncluded
#define BaseLib_Collection_ConcurrentSet_h_IsIncluded

#include "BaseLib/CommonDefines.h"
#include "BaseLib/ReadWriteLock.h"
#include "BaseLib/WriteLocker.h"
#include "BaseLib/ReadLocker.h"
#include "BaseLib/WaitCondition.h"
#include "BaseLib/ElapsedTimer.h"
#include "BaseLib/Utility.h"

namespace BaseLib { namespace Collection
{

template <typename T>
class ConcurrentSet : private std::set<T>
{
private:
    typedef std::set<T>   Set;

public:
    typedef typename Set::iterator         Iterator;
    typedef typename Set::reverse_iterator RIterator;

    typedef typename Set::const_iterator           ConstIterator;
    typedef typename Set::const_reverse_iterator   ConstRIterator;

public:
    ConcurrentSet()
    {}

    ConcurrentSet(const ConcurrentSet &orig)
        : Set(orig)
    {}

    virtual ~ConcurrentSet()
    {}

    ConcurrentSet<T>& operator=(const ConcurrentSet<T>& orig )
    {
        WriteLocker lock(&mutex_);
        Set::operator=(orig);
        return *this;
    }

public:
    inline bool empty() const
    {
        ReadLocker lock(&mutex_);
        return Set::empty();
    }

    inline unsigned int size() const
    {
        ReadLocker lock(&mutex_);
        return Set::size();
    }

    inline void clear()
    {
        WriteLocker lock(&mutex_);
        return Set::clear();
    }

public:
    inline std::pair<Iterator, bool> insert(const T &x)
    {
        WriteLocker lock(&mutex_);
        return Set::insert(x);
    }

    inline size_t erase(const T &x)
    {
        WriteLocker lock(&mutex_);
        return Set::erase(x);
    }

    inline void erase(Iterator position)
    {
        WriteLocker lock(&mutex_);
        Set::erase(position);
    }

    inline void erase(Iterator first, Iterator last)
    {
        WriteLocker lock(&mutex_);
        Set::erase(first, last);
    }


public:
    inline Iterator unlocked_begin()
    {
        return Set::begin();
    }

    inline Iterator unlocked_end()
    {
		return Set::end();
    }

    inline ConstIterator unlocked_const_begin() const
    {
        return Set::begin();
    }

    inline ConstIterator unlocked_const_end() const
    {
		return Set::end();
    }

    inline RIterator unlocked_rbegin() const
    {
		return Set::rbegin();
    }

    inline RIterator unlocked_rend() const
    {
		return Set::rend();
    }

    inline bool unlocked_empty() const
    {
		return Set::empty();
    }

private:
    mutable ReadWriteLock mutex_;
};

}}

#endif
