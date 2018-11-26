#ifndef BaseLib_ReadWriteLock_h_IsIncluded
#define BaseLib_ReadWriteLock_h_IsIncluded

#include"BaseLib/CommonDefines.h"
#include"BaseLib/ProtectedVar.h"
#include"BaseLib/WaitCondition.h"

#include"BaseLib/Export.h"

namespace BaseLib
{

/**
 * @brief The ReadWriteVariableHolder class
 */
class DLL_STATE ReadWriteVariableHolder
{
public:
    ReadWriteVariableHolder();
    ~ReadWriteVariableHolder();

    CLASS_TRAITS(ReadWriteVariableHolder)

#ifdef USE_GCC
    pthread_rwlock_t&    GetMutex() { return mutex_; }
#else
    SRWLOCK&   GetMutex() { return criticalSection_; }
#endif

private:

#ifdef USE_GCC
    pthread_rwlock_t 	mutex_;
#else
    SRWLOCK	criticalSection_;
#endif

private:
    ReadWriteVariableHolder(const ReadWriteVariableHolder &) { }
    ReadWriteVariableHolder& operator=(const ReadWriteVariableHolder&) { return *this; }
};

/**
 * @brief The ReadWriteLock class
 */
class DLL_STATE ReadWriteLock
{
public:
    /**
     * @brief ReadWriteLock
     */
    explicit ReadWriteLock(MutexPolicy policy = MutexPolicy::Debug());

    ~ReadWriteLock();

    /**
     * WaitCondition access this class
     */
    friend class WaitCondition;

    /**
     * @brief lockForRead
     */
    void lockForRead();

    /**
     * @brief lockForWrite
     */
    void lockForWrite();

    /**
     * Convenience to lock for write in templates MutexTypeLocker
     */
    void lock();

    /**
     * @brief tryLockForRead
     * @return
     */
    bool tryLockForRead();

    /**
     * @brief tryLockForRead
     * @param timeout
     * @return
     */
    //bool tryLockForRead(int timeout);

    /**
     * @brief tryLockForWrite
     * @return
     */
    bool tryLockForWrite();

    /**
     * @brief tryLockForWrite
     * @param timeout
     * @return
     */
    //bool tryLockForWrite(int timeout);

    /**
     * @brief unlock
     */
    void unlock();

    /**
     * @brief unlockRead
     */
    //void unlockRead();

    /**
     * @brief unlockWrite
     */
    //void unlockWrite();

    /**
     * @brief isDebug
     * @return
     */
    inline bool isDebug()	{ return policy_.IsDebug(); }

private:
    MutexPolicy	policy_;
    int64 mutexOwnerThreadId_;

private:
    ProtectedVar<int>   accessCount_;

private:
    ReadWriteVariableHolder::Ptr mutexHolder_;
};


/**
 * @brief The NullReadWriteLock class is useful when used as template argument
 * into an object with varying needs of synchronization.
 *
 * This can be used to create an unsynchronized object and maintain portability.
 */
class DLL_STATE NullReadWriteLock
{
public:
    /**
     * @brief ReadWriteLock
     */
    NullReadWriteLock(bool = true)
    { }

    ~NullReadWriteLock()
    {}

    /**
     * WaitCondition access this class
     */
    friend class WaitCondition;

    /**
     * @brief lockForRead
     */
    void lockForRead() { }

    /**
     * @brief lockForWrite
     */
    void lockForWrite() { }

    /**
     * @brief tryLockForRead
     * @return
     */
    bool tryLockForRead()  { return true; }

    /**
     * @brief tryLockForRead
     * @param timeout
     * @return
     */
    //bool tryLockForRead(int timeout);

    /**
     * @brief tryLockForWrite
     * @return
     */
    bool tryLockForWrite() { return true; }

    /**
     * @brief tryLockForWrite
     * @param timeout
     * @return
     */
    //bool tryLockForWrite(int timeout);

    /**
     * @brief unlock
     */
    void unlock() { }
};

}

#endif
