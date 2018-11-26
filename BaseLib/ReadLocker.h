#ifndef BaseLib_ReadLocker_h_IsIncluded
#define BaseLib_ReadLocker_h_IsIncluded

#include"BaseLib/ReadWriteLock.h"
#include"BaseLib/CommonDefines.h"
#include"BaseLib/Export.h"

namespace BaseLib
{
class DLL_STATE ReadLocker
{
public:
    inline explicit ReadLocker(ReadWriteLock *mutex)
        : mutex_(mutex)
    {
        if(mutex_ == NULL)
			throw std::runtime_error("ReadLocker::ReadLocker(ReadWriteLocker*): Argument error! ReadWriteLocker* == NULL!");

        mutex_->lockForRead();
    }

    inline ~ReadLocker()
    {
        mutex_->unlock();
    }

private:
    ReadWriteLock *mutex_;

private:
    ReadLocker(const ReadLocker &) { }
    ReadLocker& operator=(const ReadLocker&) { return *this; }
};

}

#endif
