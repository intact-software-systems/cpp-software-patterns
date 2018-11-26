#ifndef BaseLib_Templates_BaseTypes_h_Included
#define BaseLib_Templates_BaseTypes_h_Included

#include"BaseLib/CommonDefines.h"
#include"BaseLib/MutexLocker.h"
#include"BaseLib/Singleton.h"
#include"BaseLib/Templates/MethodInterfaces.h"

namespace BaseLib { namespace Templates
{

/**
 * @brief The ImmutableType class
 *
 * TODO: Rename to ConstType, NonConstType?
 */
template <typename T, typename Lock = NullMutex>
class ImmutableType
{
protected:
    ImmutableType() : t_() { }
    ImmutableType(const ImmutableType& p) : t_(p.t_) { }

public:
    ImmutableType(const T &t)
        : t_(t)
    {}

    template <typename ARG>
    ImmutableType(const ARG& arg)
        : t_(arg)
    { }

    template <typename ARG1, typename ARG2>
    ImmutableType(const ARG1& arg1, const ARG2& arg2)
        : t_(arg1, arg2)
    { }

    template <typename ARG1, typename ARG2, typename ARG3>
    ImmutableType(const ARG1& arg1, const ARG2& arg2, const ARG3& arg3)
        : t_(arg1, arg2, arg3)
    { }

    template <typename ARG1, typename ARG2, typename ARG3, typename ARG4>
    ImmutableType(const ARG1& arg1, const ARG2& arg2, const ARG3& arg3, const ARG4& arg4)
        : t_(arg1, arg2, arg3, arg4)
    { }

    template <typename ARG1, typename ARG2, typename ARG3, typename ARG4, typename ARG5>
    ImmutableType(const ARG1& arg1, const ARG2& arg2, const ARG3& arg3, const ARG4& arg4, const ARG5& arg5)
        : t_(arg1, arg2, arg3, arg4, arg5)
    { }

    template <typename ARG1, typename ARG2, typename ARG3, typename ARG4, typename ARG5, typename ARG6>
    ImmutableType(const ARG1& arg1, const ARG2& arg2, const ARG3& arg3, const ARG4& arg4, const ARG5& arg5, const ARG6& arg6)
        : t_(arg1, arg2, arg3, arg4, arg5, arg6)
    { }

    virtual ~ImmutableType()
    {}

public:
    T Clone() const
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        return this->t_;
    }

    const T* const operator->() const
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        return &this->t_;
    }

    const T& operator*() const
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        return this->t_;
    }

    const T& delegate() const
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        return this->t_;
    }

//public:
//    friend std::ostream& operator<<(std::ostream &ostr, const ImmutableType<T> &handle)
//    {
//        ostr << handle.delegate();
//        return ostr;
//    }

protected:
    T t_;
    mutable Lock lock_;
};

/**
 * @brief The MutableType class
 */
template <typename T, typename Lock = NullMutex>
class MutableType
{
protected:
    MutableType() : t_() { }
    MutableType(const MutableType& p) : t_(p.t_) { }

public:
    MutableType(const T &t)
        : t_(t)
    {}

    template <typename ARG>
    MutableType(const ARG& arg)
        : t_(arg)
    { }

    template <typename ARG1, typename ARG2>
    MutableType(const ARG1& arg1, const ARG2& arg2)
        : t_(arg1, arg2)
    { }

    template <typename ARG1, typename ARG2, typename ARG3>
    MutableType(const ARG1& arg1, const ARG2& arg2, const ARG3& arg3)
        : t_(arg1, arg2, arg3)
    { }

    template <typename ARG1, typename ARG2, typename ARG3, typename ARG4>
    MutableType(const ARG1& arg1, const ARG2& arg2, const ARG3& arg3, const ARG4& arg4)
        : t_(arg1, arg2, arg3, arg4)
    { }

    template <typename ARG1, typename ARG2, typename ARG3, typename ARG4, typename ARG5>
    MutableType(const ARG1& arg1, const ARG2& arg2, const ARG3& arg3, const ARG4& arg4, const ARG5& arg5)
        : t_(arg1, arg2, arg3, arg4, arg5)
    { }

    template <typename ARG1, typename ARG2, typename ARG3, typename ARG4, typename ARG5, typename ARG6>
    MutableType(const ARG1& arg1, const ARG2& arg2, const ARG3& arg3, const ARG4& arg4, const ARG5& arg5, const ARG6& arg6)
        : t_(arg1, arg2, arg3, arg4, arg5, arg6)
    { }

    virtual ~MutableType()
    {}

public:
    // ------------------------------------------------
    //  const operations
    // ------------------------------------------------

    T Clone() const
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        return this->t_;
    }

    const T* const operator->() const
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        return &this->t_;
    }

    const T& operator*() const
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        return this->t_;
    }

    const T& delegate() const
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        return this->t_;
    }

    // ------------------------------------------------
    //  non-const operations
    // ------------------------------------------------

    void Set(T t)
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        this->t_ = t;
    }

    void SetByRef(const T &t)
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        this->t_ = t;
    }

    T* operator->()
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        return &this->t_;
    }

    T& operator*()
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        return this->t_;
    }

    T& delegate()
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        return this->t_;
    }

    // ------------------------------------------------
    //  operator manipulations
    // ------------------------------------------------

    // Pre-increment
    MutableType<T>& operator++()
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        this->t_++;
        return *this;
    }

    // Post-increment
    MutableType<T> operator++(int)
    {
        MutexTypeLocker<Lock> lock(&this->lock_);

        MutableType<T> result(*this);
        ++(*this);
        return result;
    }

    // Pre-decrement
    MutableType<T>& operator--()
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        this->t_--;
        return *this;
    }

    // Post-decrement
    MutableType<T> operator--(int)
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        MutableType<T> result(*this);
        --(*this);
        return result;
    }

    // ------------------------------------------------
    //  plus - operator manipulations
    // ------------------------------------------------

    MutableType<T>& operator+=(const MutableType<T> &other)
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        this->t_ = this->t_ + other.t_;
        return *this;
    }

    MutableType<T> operator+(const MutableType<T> &other) const
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        return MutableType<T>(*this) += other;
    }

    MutableType<T>& operator+=(const T &other)
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        this->t_ = this->t_ + other;
        return *this;
    }

    MutableType<T> operator+(const T &other) const
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        return MutableType<T>(*this) += other;
    }

    // ------------------------------------------------
    //  minus - operator manipulations
    // ------------------------------------------------

    MutableType<T>& operator-=(const MutableType<T> &other)
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        this->t_ = this->t_ - other.t_;
        return *this;
    }

//    MutableType<T> operator-(const MutableType<T> &other) const
//    {
//        MutexTypeLocker<Lock> lock(&this->lock_);
//        return MutableType<T>(*this) -= other;
//    }

    T operator-(const MutableType<T> &other) const
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        return this->t_ - other.t_;
    }

    MutableType<T>& operator-=(const T &other)
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        this->t_ = this->t_ - other;
        return *this;
    }

    T operator-(const T &other) const
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        return this->t_ - other;
    }

    // ------------------------------------------------
    //  equals - operators
    // ------------------------------------------------

    MutableType<T>& operator=(const T &other)
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        this->t_ = other;
        return *this;
    }

    MutableType<T>& operator=(const MutableType<T> &other)
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        this->t_ = other.t_;
        return *this;
    }


public:
    friend std::ostream& operator<<(std::ostream &ostr, const MutableType<T> &handle)
    {
        ostr << handle.delegate();
        return ostr;
    }

protected:
    T t_;
    mutable Lock lock_;
};

/**
 * @brief The ComparableImmutableType class
 */
template <typename T, typename Lock = NullMutex>
class ComparableImmutableType : public ImmutableType<T, Lock>
{
public:
    ComparableImmutableType(T t) : ImmutableType<T,Lock>(t)
    {}
    ComparableImmutableType() : ImmutableType<T,Lock>()
    {}
    virtual ~ComparableImmutableType()
    {}

    // ------------------------------------------------
    //  operator comparisons
    // ------------------------------------------------
    bool operator==(const T &rvalue) const
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        return this->t_ == rvalue;
    }

    bool operator!=(const T &rvalue) const
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        return this->t_ != rvalue;
    }

    bool operator<(const T &rvalue) const
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        return this->t_ < rvalue;
    }

    bool operator<=(const T &rvalue) const
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        return this->t_ <= rvalue;
    }

    bool operator>(const T &rvalue) const
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        return this->t_ > rvalue;
    }

    bool operator>=(const T &rvalue) const
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        return this->t_ >= rvalue;
    }

    // ------------------------------------------------
    //  operator comparisons
    // ------------------------------------------------
    bool operator==(const ComparableImmutableType<T> &rvalue) const
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        return this->t_ == rvalue.t_;
    }

    bool operator!=(const ComparableImmutableType<T> &rvalue) const
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        return this->t_ != rvalue.t_;
    }

    bool operator<(const ComparableImmutableType<T> &rvalue) const
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        return this->t_ < rvalue.t_;
    }

    bool operator<=(const ComparableImmutableType<T> &rvalue) const
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        return this->t_ <= rvalue.t_;
    }

    bool operator>(const ComparableImmutableType<T> &rvalue) const
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        return this->t_ > rvalue.t_;
    }

    bool operator>=(const ComparableImmutableType<T> &rvalue) const
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        return this->t_ >= rvalue.t_;
    }
};

/**
 * @brief The ComparableMutableType class
 */
template <typename T, typename Lock = NullMutex>
class ComparableMutableType : public MutableType<T, Lock>
{
public:
    ComparableMutableType(T t) : MutableType<T,Lock>(t)
    {}
    ComparableMutableType() : MutableType<T,Lock>()
    {}
    virtual ~ComparableMutableType()
    {}
    // ------------------------------------------------
    //  operator comparisons
    // ------------------------------------------------
    bool operator==(const T &rvalue) const
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        return this->t_ == rvalue;
    }

    bool operator!=(const T &rvalue) const
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        return this->t_ != rvalue;
    }

    bool operator<(const T &rvalue) const
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        return this->t_ < rvalue;
    }

    bool operator<=(const T &rvalue) const
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        return this->t_ <= rvalue;
    }

    bool operator>(const T &rvalue) const
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        return this->t_ > rvalue;
    }

    bool operator>=(const T &rvalue) const
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        return this->t_ >= rvalue;
    }

    // ------------------------------------------------
    //  operator comparisons
    // ------------------------------------------------
    bool operator==(const ComparableMutableType<T> &rvalue) const
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        return this->t_ == rvalue.t_;
    }

    bool operator!=(const ComparableMutableType<T> &rvalue) const
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        return this->t_ != rvalue.t_;
    }

    bool operator<(const ComparableMutableType<T> &rvalue) const
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        return this->t_ < rvalue.t_;
    }

    bool operator<=(const ComparableMutableType<T> &rvalue) const
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        return this->t_ <= rvalue.t_;
    }

    bool operator>(const ComparableMutableType<T> &rvalue) const
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        return this->t_ > rvalue.t_;
    }

    bool operator>=(const ComparableMutableType<T> &rvalue) const
    {
        MutexTypeLocker<Lock> lock(&this->lock_);
        return this->t_ >= rvalue.t_;
    }
};

/**
 * @brief The NullObjectType class
 *
 * TODO: Is this really a good implementation of null object pattern?
 */
template <typename T>
class NullObjectType
{
public:
    virtual ~NullObjectType() {}

    static bool IsNullObject(const T &obj)
    {
        return obj == nullObject_.GetRef();
    }

private:
    static Singleton<T> nullObject_;
};

/**
 * @brief The NoCopy class
 */
template <typename T>
class NoCopy
{
public:
    virtual ~NoCopy() { }

protected:
    NoCopy() {}

private:
    NoCopy(const NoCopy<T> &) {}
    NoCopy& operator=(const NoCopy<T> &) { return *this; }
};

//template <typename T>
//class DisposeNoCopy : public NoCopy<T> {};

}}

#endif
