#pragma once

#include "BaseLib/CommonDefines.h"
#include "BaseLib/Collection/IMap.h"
#include "BaseLib/Collection/IList.h"

namespace BaseLib { namespace Collection {

template <typename T>
class QueueCollection
{
public:
    virtual ~QueueCollection()
    { }

    CLASS_TRAITS(QueueCollection)

    // --------------------------------------------
    // Enqueue, Dequeue and Peek
    // --------------------------------------------

    virtual bool TryEnqueue(const T& t) = 0;

    virtual bool Enqueue(const T& t, int64 msecs = LONG_MAX) = 0;

    virtual T TryDequeue() = 0;

    virtual T Dequeue(int64 msecs = LONG_MAX) = 0;

    virtual T Peek(int64 msecs = LONG_MAX) const = 0;

    virtual bool Pop(int numElements) = 0;

    // --------------------------------------------
    // Check status of ring buffer, etc
    // --------------------------------------------

    virtual int Length() const   = 0;
    virtual int Capacity() const = 0;

    virtual bool IsEmpty() const = 0;
    virtual bool IsFull() const  = 0;

    virtual void Clear() = 0;
    //virtual void ClearAndResize(int newCapacity) = 0;

    virtual void WakeAll() const = 0;
    virtual void WakeOne() const = 0;

    virtual bool WaitForData(int numElements, int64 msecs) const = 0;

    virtual bool WaitForCapacity(int numElements, int64 msecs) const = 0;
};

template <typename T>
class CollectionType
{
public:
    virtual ~CollectionType()
    { }

    CLASS_TRAITS(CollectionType)

    virtual bool Add(const T& t)    = 0;
    virtual bool Remove(const T& t) = 0;

    virtual int Length() const   = 0;
    virtual int Capacity() const = 0;

    virtual bool IsEmpty() const = 0;
    virtual bool IsFull() const  = 0;

    virtual void Clear() = 0;
};

template <typename K, typename V>
class MapCollectionType
{
public:
    virtual ~MapCollectionType()
    { }

    virtual bool Add(const K& key, const V& value) = 0;

    virtual V Remove(const K& key) = 0;

    virtual bool Clear() = 0;

    virtual size_t Size() const = 0;

    virtual bool IsEmpty() const = 0;

    virtual bool Contains(const K& key) const = 0;

    virtual V Find(const K& key) const = 0;

    //virtual int Capacity() const = 0;
    //virtual bool IsFull() const = 0;
};


namespace details {

// TODO: Add a FlowStatus or MapStatus?

template <typename K, typename V>
class StdMapCollectionType
    : public MapCollectionType<K, V>
      , public Collection::IMap<K, V>
{
    typedef MutexTypeLocker<StdMapCollectionType<K, V>> Locker;

public:
    StdMapCollectionType() : Collection::IMap<K, V>()
    { }

    virtual ~StdMapCollectionType()
    { }

    virtual bool Add(const K& key, const V& value)
    {
        Locker locker(this);

        return this->insert(std::pair<K, V>(key, value)).second;
    }

    virtual V Remove(const K& key)
    {
        Locker locker(this);

        auto entry = this->find(key);
        if(entry != this->end())
        {
            this->erase(entry);
        }

        return entry->second;
    }

    virtual bool Clear()
    {
        Locker locker(this);

        bool isEmpty = this->empty();
        this->clear();
        return isEmpty;
    }

    virtual size_t Size() const
    {
        Locker locker(this);
        return this->size();
    }

    virtual bool IsEmpty() const
    {
        Locker locker(this);
        return this->empty();
    }

    virtual bool Contains(const K& key) const
    {
        Locker locker(this);
        return this->find(key) != this->end();
    }

    virtual V Find(const K& key) const
    {
        Locker locker(this);

        typename Collection::IMap<K, V>::const_iterator entry;

        entry = this->find(key);

        return entry != this->end() ? entry->second : V();
    }

    V GetOrCreate(const K& key, std::function<V()> factoryFunction)
    {
        Locker locker(this);

        auto entry = this->find(key);

        if(entry == this->end())
        {
            V value = factoryFunction();
            this->insert(std::pair<K, V>(key, value));
            return value;
        }
        else
        {
            return entry->second;
        }
    }

    // ------------------------------------------------------------------
    // Apply functions on elements
    // ------------------------------------------------------------------

    void ForEach(std::function<void(V)> action)
    {
        Locker locker(this);

        for(auto entry : *this)
        {
            action(entry.second);
        }
    }

    void ForEachReverse(std::function<void(V)> action)
    {
        Locker locker(this);

        for(auto it = this->rbegin(), it_end = this->rend(); it != it_end; ++it)
        {
            action(it->second);
        }
    }

    void ForEach(std::function<void(V)> action) const
    {
        Locker locker(this);

        for(auto entry : *this)
        {
            action(entry.second);
        }
    }

    void ForEachReverse(std::function<void(V)> action) const
    {
        Locker locker(this);

        for(auto it = this->crbegin(), it_end = this->crend(); it != it_end; ++it)
        {
            action(it->second);
        }
    }

    template <typename Return>
    void ForEachMemberFunction(std::function<Return(const V&)> action) const
    {
        Locker locker(this);

        for(const auto& entry : *this)
        {
            action(entry.second);
        }
    }

    template <typename Return>
    void ForEachMemberFunction(std::function<Return(V&)> action)
    {
        Locker locker(this);

        for(auto& entry : *this)
        {
            action(entry.second);
        }
    }

    void ForEachEntry(std::function<void(K, V)> action) const
    {
        Locker locker(this);

        for(auto entry : *this)
        {
            action(entry.first, entry.second);
        }
    }

    void ForEachEntryReverse(std::function<void(K, V)> action) const
    {
        Locker locker(this);

        for(auto it = this->crbegin(), it_end = this->crend(); it != it_end; ++it)
        {
            action(it->first, it->second);
        }
    }

    void ForEachEntry(std::function<void(K, V)> action)
    {
        Locker locker(this);

        for(auto entry : *this)
        {
            action(entry.first, entry.second);
        }
    }

    void ForEachEntryReverse(std::function<void(K, V)> action)
    {
        Locker locker(this);

        for(auto it = this->rbegin(), it_end = this->rend(); it != it_end; ++it)
        {
            action(it->first, it->second);
        }
    }

    template <typename Return>
    void ForEachEntry(std::function<Return(K, V)> action) const
    {
        Locker locker(this);

        for(auto entry : *this)
        {
            action(entry.first, entry.second);
        }
    }

    template <typename Return>
    void ForEachEntry(std::function<Return(K, V)> action)
    {
        Locker locker(this);

        for(auto entry : *this)
        {
            action(entry.first, entry.second);
        }
    }

    bool Apply(const K& key, std::function<bool(V)> action) const
    {
        typename Collection::IMap<K, V>::const_iterator entry;
        {
            Locker locker(this);
            entry = this->find(key);
        }

        return entry == this->cend()
               ? false
               : action(entry->second);
    }

    bool Apply(const K& key, std::function<bool(V)> action)
    {
        typename Collection::IMap<K, V>::iterator entry;
        {
            Locker locker(this);
            entry = this->find(key);
        }

        return entry == this->cend()
               ? false
               : action(entry->second);
    }

    template <typename Return>
    Return FindAndApply(const K& key, std::function<Return(V)> applyFunc) const
    {
        typename Collection::IMap<K, V>::const_iterator entry;
        {
            Locker locker(this);
            entry = this->find(key);
        }

        if(entry == this->end())
        {
            return Return();
        }
        else
        {
            return applyFunc(entry->second);
        }
    }

    template <typename Return>
    Return FindAndApply(const K& key, std::function<Return(V)> applyFunc)
    {
        typename Collection::IMap<K, V>::iterator entry;
        {
            Locker locker(this);
            entry = this->find(key);
        }

        if(entry == this->end())
        {
            return Return();
        }
        else
        {
            return applyFunc(entry->second);
        }
    }

    template <typename Return>
    Return RemoveAndApply(const K& key, std::function<Return(V)> applyFunc)
    {
        bool found = false;

        V value;

        typename Collection::IMap<K, V>::iterator entry;
        {
            Locker locker(this);
            entry = this->find(key);
            if(entry != this->end())
            {
                found = true;
                value = entry->second;
                this->erase(entry);
            }
        }

        if(!found)
        {
            return Return();
        }
        else
        {
            return applyFunc(value);
        }
    }

    static bool Apply(const V& value, std::function<bool(V)> action)
    {
        return action(value);
    }

    const Collection::IMap<K, V>& collection() const
    {
        return *this;
    }

    Collection::IMap<K, V>& collection()
    {
        return *this;
    }

    const StdMapCollectionType<K, V>& self() const
    {
        return *this;
    };

    StdMapCollectionType<K, V>& self()
    {
        return *this;
    };

    //virtual int Capacity() const = 0;
    //virtual bool IsFull() const = 0;
};

template <typename T>
class StdListCollectionType
    : public CollectionType<T>
      , private Collection::IList<T>
{
public:

    virtual bool Add(const T& t)
    {
        this->push_back(t);
        return true;
    }

    virtual bool Remove(const T& t)
    {
        this->remove(t);
        return this->contains(t);
    }

    virtual int Length() const
    {
        return this->size();
    }

    virtual int Capacity() const
    {
        return INT_MAX;
    }

    virtual bool IsEmpty() const
    {
        return this->empty();
    }

    virtual bool IsFull() const
    {
        return false;
    }

    virtual void Clear()
    {
        this->clear();
    }

    void ForEachValue(std::function<void(T)> action) const
    {
        for(const T& value : *this)
        {
            action(value);
        }
    }

    const Collection::IList<T>& collection() const
    {
        return *this;
    }

    Collection::IList<T>& collection()
    {
        return *this;
    }
};

}

}}

