#pragma once

#include"BaseLib/MutexLocker.h"
#include"BaseLib/Policy/Flyweight.h"

namespace BaseLib { namespace Templates
{

template <typename T, typename Lock = BaseLib::NullMutex>
class SharedTypePtr
{
public:
    SharedTypePtr(T *t, BaseLib::Policy::Flyweight flyweight = BaseLib::Policy::Flyweight::No())
        : t_(t)
        , flyweight_(flyweight)
    {}
    SharedTypePtr(std::shared_ptr<T> t, BaseLib::Policy::Flyweight flyweight = BaseLib::Policy::Flyweight::No())
            : t_(t)
            , flyweight_(flyweight)
    {}
    virtual ~SharedTypePtr()
    { }

    // ------------------------------------------------
    //  const operations
    // ------------------------------------------------

    T* ClonePtr() const
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        return new T(*t_);
    }

    T Clone() const
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        return T(*t_);
    }

    bool IsSet() const
    {
        return this->t_ != nullptr;
    }

    T* operator->() const
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        return this->t_.get();
    }

    const T& operator*() const
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        return *this->t_;
    }

    const T& delegate() const
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        return *this->t_;
    }

    std::shared_ptr<T> SharedPtr() const
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        return this->t_;
    }

    // ------------------------------------------------
    //  non-const operations
    // ------------------------------------------------

    bool Set(T *t)
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        this->t_ = std::shared_ptr<T>(t);

        return true;
    }

    T* operator->()
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        if(isShared()) detach();

        return this->t_.get();
    }

    T& operator*()
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        if(isShared()) detach();

        return *this->t_;
    }

    T& delegate()
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        if(isShared()) detach();

        return *this->t_;
    }

private:
    virtual void detach()
    {
        if(flyweight_.IsCopyOnWrite())
        {
            IINFO() << "Detached the pointer because use count " << t_.use_count();
            ASSERT(t_.use_count() > 1);

            T *local = t_.get();
            t_ = std::shared_ptr<T>(new T(*local));
        }
    }

    bool isShared() {
        return t_.use_count() > 1;
    }

protected:
    std::shared_ptr<T> t_;
    mutable Lock lock_;

    BaseLib::Policy::Flyweight flyweight_;
};

}}
