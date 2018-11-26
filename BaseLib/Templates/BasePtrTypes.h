#ifndef BaseLib_Templates_BasePtrTypes_h_Included
#define BaseLib_Templates_BasePtrTypes_h_Included

#include"BaseLib/CommonDefines.h"
#include"BaseLib/MutexLocker.h"
#include"BaseLib/Templates/MethodInterfaces.h"

namespace BaseLib { namespace Templates
{

template <typename T, typename Lock = NullMutex>
class ImmutableTypePtr
{
protected:
    ImmutableTypePtr() : t_(nullptr) { }
    ImmutableTypePtr(const ImmutableTypePtr& p) : t_(p.t_) { }

public:
    ImmutableTypePtr(T *t)
        : t_(t)
    {}

//    template <typename ARG>
//    ImmutableTypePtr(const ARG& arg)
//        : t_(new T(arg))
//    { }

//    template <typename ARG1, typename ARG2>
//    ImmutableTypePtr(const ARG1& arg1, const ARG2& arg2)
//        : t_(new T(arg1, arg2))
//    { }

//    template <typename ARG1, typename ARG2, typename ARG3>
//    ImmutableTypePtr(const ARG1& arg1, const ARG2& arg2, const ARG3& arg3)
//        : t_(new T(arg1, arg2, arg3))
//    { }

//    template <typename ARG1, typename ARG2, typename ARG3, typename ARG4>
//    ImmutableTypePtr(const ARG1& arg1, const ARG2& arg2, const ARG3& arg3, const ARG4& arg4)
//        : t_(new T(arg1, arg2, arg3, arg4))
//    { }

//    template <typename ARG1, typename ARG2, typename ARG3, typename ARG4, typename ARG5>
//    ImmutableTypePtr(const ARG1& arg1, const ARG2& arg2, const ARG3& arg3, const ARG4& arg4, const ARG5& arg5)
//        : t_(new T(arg1, arg2, arg3, arg4, arg5))
//    { }

//    template <typename ARG1, typename ARG2, typename ARG3, typename ARG4, typename ARG5, typename ARG6>
//    ImmutableTypePtr(const ARG1& arg1, const ARG2& arg2, const ARG3& arg3, const ARG4& arg4, const ARG5& arg5, const ARG6& arg6)
//        : t_(new T(arg1, arg2, arg3, arg4, arg5, arg6))
//    { }

    virtual ~ImmutableTypePtr()
    {}

public:
    T* Clone() const
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        return new T(*t_);
    }

    bool IsSet() const
    {
        return this->t_ != nullptr;
    }

    const T* const operator->() const
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        return this->t_;
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

//public:
//    friend std::ostream& operator<<(std::ostream &ostr, const ImmutableTypePtr<T> &handle)
//    {
//        ostr << handle.delegate();
//        return ostr;
//    }

protected:
    T *t_;
    mutable Lock lock_;
};

/**
 * @brief The MutableType class
 */
template <typename T, typename Lock = NullMutex>
class MutableTypePtr
{
protected:
    MutableTypePtr() : t_(nullptr) { }
    MutableTypePtr(const MutableTypePtr& p) : t_(p.t_) { }

public:
    MutableTypePtr(T *t)
        : t_(t)
    {}

//    template <typename ARG>
//    MutableTypePtr(const ARG& arg)
//        : t_(new T(arg))
//    { }

//    template <typename ARG1, typename ARG2>
//    MutableTypePtr(const ARG1& arg1, const ARG2& arg2)
//        : t_(new T(arg1, arg2))
//    { }

//    template <typename ARG1, typename ARG2, typename ARG3>
//    MutableTypePtr(const ARG1& arg1, const ARG2& arg2, const ARG3& arg3)
//        : t_(new T(arg1, arg2, arg3))
//    { }

//    template <typename ARG1, typename ARG2, typename ARG3, typename ARG4>
//    MutableTypePtr(const ARG1& arg1, const ARG2& arg2, const ARG3& arg3, const ARG4& arg4)
//        : t_(new T(arg1, arg2, arg3, arg4))
//    { }

//    template <typename ARG1, typename ARG2, typename ARG3, typename ARG4, typename ARG5>
//    MutableTypePtr(const ARG1& arg1, const ARG2& arg2, const ARG3& arg3, const ARG4& arg4, const ARG5& arg5)
//        : t_(new T(arg1, arg2, arg3, arg4, arg5))
//    { }

//    template <typename ARG1, typename ARG2, typename ARG3, typename ARG4, typename ARG5, typename ARG6>
//    MutableTypePtr(const ARG1& arg1, const ARG2& arg2, const ARG3& arg3, const ARG4& arg4, const ARG5& arg5, const ARG6& arg6)
//        : t_(new T(arg1, arg2, arg3, arg4, arg5, arg6))
//    { }

    virtual ~MutableTypePtr()
    { }

public:
    // ------------------------------------------------
    //  const operations
    // ------------------------------------------------

    T* Clone() const
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        return new T(*t_);
    }

    bool IsSet() const
    {
        return this->t_ != nullptr;
    }

    const T* const operator->() const
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        return this->t_;
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

    // ------------------------------------------------
    //  non-const operations
    // ------------------------------------------------

    void Set(T *t)
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        this->t_ = t;
    }

    T* operator->()
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        return this->t_;
    }

    T& operator*()
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        return *this->t_;
    }

    T& delegate()
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        return *this->t_;
    }

    // ------------------------------------------------
    //  operator manipulations
    // ------------------------------------------------

    MutableTypePtr<T>& operator+=(const MutableTypePtr<T> &other)
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        this->t_ = this->t_ + other.t_;
        return *this;
    }

    MutableTypePtr<T> operator+(const MutableTypePtr<T> &other) const
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        return MutableTypePtr<T>(*this) += other;
    }

    MutableTypePtr<T>& operator+=(const T &other)
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        this->t_ = this->t_ + other;
        return *this;
    }

    MutableTypePtr<T> operator+(const T &other) const
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        return MutableTypePtr<T>(*this) += other;
    }

public:
    friend std::ostream& operator<<(std::ostream &ostr, const ImmutableTypePtr<T> &handle)
    {
        ostr << handle.delegate();
        return ostr;
    }

protected:
    T *t_;
    mutable Lock lock_;
};

}}


#endif
