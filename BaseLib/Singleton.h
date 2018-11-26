#ifndef BaseLib_Singleton_h_IsIncluded
#define BaseLib_Singleton_h_IsIncluded

#include"BaseLib/CommonDefines.h"
#include"BaseLib/MutexBare.h"
#include"BaseLib/MutexLocker.h"

namespace BaseLib
{

/**
 @description Implementation of Singleton software pattern.

    class Foo : public Singleton<Foo> {};
    Foo& foo1 = Foo::GetRef();
    Foo* foo2 = Foo::GetPtr();
    Foo::Destroy();

 @url http://stackoverflow.com/questions/2346091/c-templated-class-implementation-of-the-multiton-pattern

 TODO: For thread-safe implementation use Double-checked locking pattern

 http://en.wikipedia.org/wiki/Double-checked_locking
*/
template <typename T>
class Singleton
{
public:
    Singleton()
        : instance_(NULL)
        , isInitialized_(false)
        , mutex_(MutexPolicy::No())
    { }

    virtual ~Singleton()
    {
        Destroy();
    }

    void Destroy()
    {
        MutexTypeLocker<MutexBare> mutex(&mutex_);

        if(isInitialized_)
        {
            delete instance_;
            instance_ = NULL;
            isInitialized_ = false;
        }
    }

    T* GetPtr()
    {
        if(isInitialized_ == false)
        {
            MutexTypeLocker<MutexBare> mutex(&mutex_);

            if(isInitialized_ == false)
            {
                instance_ = new T;
                isInitialized_ = true;
            }
        }
        return instance_;
    }

    T& GetRef()
    {
        if(isInitialized_ == false)
        {
            MutexTypeLocker<MutexBare> mutex(&mutex_);

            if(isInitialized_ == false)
            {
                instance_ = new T;
                isInitialized_ = true;
            }
        }
        return *instance_;;
    }

    bool IsNull()
    {
        if(isInitialized_ == true)
            return isInitialized_;

        MutexTypeLocker<MutexBare> mutex(&mutex_);
        return isInitialized_;
    }

private:
    Singleton(const Singleton&) {}
    Singleton& operator = (const Singleton&) { return *this; }

private:
    T*          instance_;
    bool        isInitialized_;
    MutexBare 	mutex_;
};

template <typename T>
class StaticSingleton
{
public:
    StaticSingleton()
    { }

    virtual ~StaticSingleton()
    {
        destroy();
    }

    static T* InstancePtr()
    {
        if(!isInitialized_)
        {
            MutexTypeLocker<MutexBare> mutex(&mutex_);

            if(!isInitialized_)
            {
                instance_ = new T;
                isInitialized_ = true;
            }
        }

        return instance_;
    }

    static T& Instance()
    {
        if(!isInitialized_)
        {
            MutexTypeLocker<MutexBare> mutex(&mutex_);

            if(!isInitialized_)
            {
                instance_ = new T;
                isInitialized_ = true;
            }
        }

        return *instance_;
    }

    static bool IsInitialized()
    {
        if(isInitialized_)
            return isInitialized_;

        MutexTypeLocker<MutexBare> mutex(&mutex_);
        return isInitialized_;
    }

private:

    static void destroy()
    {
        MutexTypeLocker<MutexBare> mutex(&mutex_);

        if(isInitialized_)
        {
            delete instance_;
            instance_ = nullptr;
            isInitialized_ = false;
        }
    }

private:
    StaticSingleton(const StaticSingleton<T>&) {}
    StaticSingleton& operator = (const StaticSingleton<T>&) { return *this; }

private:
    static T*           instance_;
    static bool         isInitialized_;
    static MutexBare 	mutex_;
};

template <typename T>
T* StaticSingleton<T>::instance_ = nullptr;

template <typename T>
bool StaticSingleton<T>::isInitialized_ = false;

template <typename T>
MutexBare StaticSingleton<T>::mutex_ = MutexBare();


template <typename T>
class StaticSingletonPtr
{
public:
    StaticSingletonPtr()
    { }

    virtual ~StaticSingletonPtr()
    {
        //destroy();
    }

    static std::shared_ptr<T> InstancePtr()
    {
        if(!isInitialized_)
        {
            MutexTypeLocker<MutexBare> mutex(&mutex_);

            if(!isInitialized_)
            {
                instance_ = std::shared_ptr<T>(new T);
                isInitialized_ = true;
            }
        }

        return instance_;
    }

    template <typename U>
    static std::shared_ptr<U> InstancePtr()
    {
        if(!isInitialized_)
        {
            MutexTypeLocker<MutexBare> mutex(&mutex_);

            if(!isInitialized_)
            {
                instance_ = std::shared_ptr<T>(new T);
                isInitialized_ = true;
            }
        }

        return std::dynamic_pointer_cast<U>(instance_);
    }


    static T& Instance()
    {
        if(!isInitialized_)
        {
            MutexTypeLocker<MutexBare> mutex(&mutex_);
            if(!isInitialized_)
            {
                instance_ = std::shared_ptr<T>(new T);
                isInitialized_ = true;
            }
        }
        return *instance_;
    }

    static bool IsInitialized()
    {
        if(isInitialized_)
            return isInitialized_;

        MutexTypeLocker<MutexBare> mutex(&mutex_);
        return isInitialized_;
    }

//private:

//    static void destroy()
//    {
//        Locker mutex(&mutex_);

//        if(isInitialized_)
//        {
//            instance_ = nullptr;
//            isInitialized_ = false;
//        }
//    }

private:
    StaticSingletonPtr(const StaticSingletonPtr<T>&) {}
    StaticSingletonPtr& operator = (const StaticSingletonPtr<T>&) { return *this; }

private:
    static std::shared_ptr<T>   instance_;
    static bool                 isInitialized_;
    static MutexBare            mutex_;
};

template <typename T>
std::shared_ptr<T> StaticSingletonPtr<T>::instance_ = nullptr;

template <typename T>
bool StaticSingletonPtr<T>::isInitialized_ = false;

template <typename T>
MutexBare StaticSingletonPtr<T>::mutex_ = MutexBare();

/*------------------------------------------------------------------------------
SingletonConstruct -
    Template class for constructing a singleton in an efficient and thread safe
    manner. This object must be a static member of the class that you want to be
    a singleton - this ensures m_p will be zero'd out during global construction
    (which is single threaded)
NOTE: Do not use your singleton in a global object constructor or destructor.
------------------------------------------------------------------------------*/
template <typename T>
class SingletonConstruct //: public no_copy
{
public:
    SingletonConstruct() : instance_(0)
    {

    }
    ~SingletonConstruct()
    {
        Unload();
    }

    // NOTE: caller must guarantee that no references are being held
    void Unload()
    {
        delete instance_;
    }

    T& Instance()
    {
        // atomic read with acquire member semantics
        // TODO: InterLockedReadAcquire
        // TODO: Use boost atomic read / write
        volatile T *p = &instance_;

        if(!p)
        {
            MutexTypeLocker<MutexBare> lock(&mutex_);
            p = instance_;
            if(!p)
            {
                p = new T;
                // atomic write with release membar semantics
                // TODO: InterlockedWriteRelease(
                &instance_ = p;
            }
        }

        return *p;
    }

private:
    SingletonConstruct(const SingletonConstruct&) {}
    SingletonConstruct& operator = (const SingletonConstruct&) { return *this; }

private:
    T           *instance_;
    MutexBare 	mutex_;
};

/*
// Example usage SingletonConstruct
class foo : public no_copy
{
    static SingletonConstruct<foo> m_foo_sc;
    friend class SingletonConstruct<foo>;

    foo() {}
    ~foo() {}

public:
    static foo& Instance() {return m_foo_sc.Instance();}
    void display() {std::cout << "I'm the foo instance!" << std::endl;}
};//foo

// m_foo_sc is global which ensures it's construction prior to threading
SingletonConstruct<foo> foo::m_foo_sc;
*/

/*------------------------------------------------------------------------------
tlsSingletonConstruct -
    Demonstrates singleton construction in an efficient and thread safe manner
    using TLS. It is assumed that TLS memory access is more efficient than
    acquiring a lock. A Posix compliant implementation is possible using this
    model.
------------------------------------------------------------------------------*/
/*template <typename T>
class tlsSingletonConstruct : public no_copy
{
public:
    tlsSingletonConstruct() : m_p(0)
    {

    }
    ~tlsSingletonConstruct()
    {
        Unload();
    }

    // NOTE - caller must guarantee that no references are being held
    void Unload()
    {
        delete m_p;
    }

    T& Instance()
    {
        // "__thread" represents TLS which is guaranteed to be initialized
        // "early". An actual implementation may use compiler extensions to
        // declare the static TLS here, or acquire TLS storage via API's within
        // tlsSingletonConstruct's constructor.
        static __thread bool g_created = false;

        if (g_created)
            return *m_p;

        Locker lock(&mutex);
        if (!m_p)
            m_p = new T;
        g_created = true;

        return *m_p;
    }

private:
    tlsSingletonConstruct(const tlsSingletonConstruct&) {}
    tlsSingletonConstruct& operator = (const tlsSingletonConstruct&) { return *this; }

private:
    T 		*m_p;
    Mutex 	mutex;
};*/

} // namespace BaseLib

#endif // BaseLib_Singleton_h_IsIncluded
