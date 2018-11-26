#ifndef BaseLib_Templates_LeasingType_h_Included
#define BaseLib_Templates_LeasingType_h_Included

#include"BaseLib/Templates/BaseTypes.h"

namespace BaseLib { namespace Templates
{

/**
 * TODO: This is similar in behavior to a WeakReference<T>, however this would be a LeaseReference<T>.
 * The difference is that a lease reference can be timed out, or be made invalid/expired/deleted by other controller entity.
 */
template <typename T>
class Leasing
{
public:
    /**
     * @brief AcquireResource is called by Resource User
     */
    virtual void AcquireResource() = 0;

    /**
     * @brief ReleaseResource is called by Resource provider
     */
    virtual void ReleaseResource() = 0;

    /**
     * @brief ReleaseReference is called by Resource User
     */
    virtual void ReleaseReference() = 0;

    /**
     * @brief IsLeaseExpired check if lease has expired
     * @return
     */
    virtual bool IsLeaseExpired() const = 0;
};

template <typename T, typename V>
class LeaseReference
        : public Leasing<T>
        , protected MutableType<T>
{
public:
    LeaseReference(T t, V policy)
        : MutableType<T>(t)
        , policy_(policy)
    {}
    virtual ~LeaseReference()
    {}

    virtual void AcquireResource()
    {
        // TODO: From data source like a cache?
    }

    virtual void ReleaseResource()
    {
        this->Set(T());
    }

    virtual void ReleaseReference()
    {
        this->Set(T());
    }

    virtual bool IsLeaseExpired() const
    {
        return policy_.IsExpired();
    }

//    std::shared_ptr<T> Lock() const
//    {
//        return this->delegate();
//    }

    T Clone() const
    {
        return this->Clone();
    }

    const T* const operator->() const
    {
        return this->operator ->();
    }

    const T& operator*() const
    {
        return this->operator *();
    }

    const T& delegate() const
    {
        return this->delegate();
    }

private:
    V policy_;
};


}}

#endif
