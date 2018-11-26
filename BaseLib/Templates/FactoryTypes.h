#ifndef BaseLib_Templates_FactoryTypes_h_Included
#define BaseLib_Templates_FactoryTypes_h_Included

#include"BaseLib/CommonDefines.h"

#include"BaseLib/Templates/BaseTypes.h"
#include"BaseLib/Templates/MethodInterfaces.h"
#include"BaseLib/Templates/Synchronization.h"
#include"BaseLib/Templates/Lifecycle.h"
#include"BaseLib/Templates/DoubleCheckedLocking.h"

namespace BaseLib { namespace Templates
{

/**
 * @brief The FactoryDisposeType class
 */
template <typename T, typename Lock = Mutex>
class FactoryDisposeType : public ComparableMutableType<T, Lock>
{
public:
    FactoryDisposeType(T &t) : ComparableMutableType<T,Lock>(t)
    {}
    virtual ~FactoryDisposeType()
    {}

    bool Dispose()  { return this->t_.Dispose(); }
    T Create()      { return this->t_.Create(); }
};

/** TODO: Introduce Factory/Builder for construction as a static
 *  - Factory factory();
 *  - Builder builder();
 */
template <typename T, typename Lock = Mutex>
class FactorySingleton
        : public StaticSingleton<T>
        , public Templates::NoCopy<T>
        , public Templates::LockableType<T, Mutex>
{
public:
    virtual ~FactorySingleton() {}

    static T& Factory()
    {
        return StaticSingleton<T>::Instance();
    }
};

template <typename T, typename Lock = Mutex>
class BuilderTypeSingleton
        : public StaticSingleton<T>
        , public Templates::NoCopy<T>
        , public Templates::LockableType<T, Mutex>
{
public:
    virtual ~BuilderTypeSingleton() {}

    static T& Builder()
    {
        return StaticSingleton<T>::Instance();
    }
};

template <typename T, typename Lock = Mutex>
class SupervisorSingleton
        : public StaticSingletonPtr<T>
        , public Templates::NoCopy<T>
        , public Templates::LockableType<T, Mutex>
        , public Templates::InitializeMethods
{
public:
    virtual ~SupervisorSingleton() {}

    static T& Supervisor()
    {
        DoubleCheckedLocking::Initialize<T>(StaticSingletonPtr<T>::InstancePtr().get());
        return StaticSingletonPtr<T>::Instance();
    }

    static std::shared_ptr<T> SupervisorPtr()
    {
        DoubleCheckedLocking::Initialize<T>(StaticSingletonPtr<T>::InstancePtr().get());
        return StaticSingletonPtr<T>::InstancePtr();
    }
};

template <typename T>
class FactoryFunction0
{
public:
    virtual ~FactoryFunction0() {}

    static T* CreatePtr()
    {
        return new T;
    }

    static std::shared_ptr<T> CreateSharedPtr()
    {
        return std::shared_ptr<T>(new T);
    }
};

template <typename T, typename Arg1>
class FactoryFunction1
{
public:
    virtual ~FactoryFunction1() {}

    static T* CreatePtr(Arg1 arg1)
    {
        return new T(arg1);
    }

    static std::shared_ptr<T> CreateSharedPtr(Arg1 arg1)
    {
        return std::shared_ptr<T>(new T(arg1));
    }
};

template <typename T, typename Arg1, typename Arg2>
class FactoryFunction2
{
public:
    virtual ~FactoryFunction2() {}

    static T* CreatePtr(Arg1 arg1, Arg2 arg2)
    {
        return new T(arg1, arg2);
    }

    static std::shared_ptr<T> CreateSharedPtr(Arg1 arg1, Arg2 arg2)
    {
        return std::shared_ptr<T>(new T(arg1, arg2));
    }
};

}}

#endif
