#ifndef BaseLib_WriteLocker_h_IsIncluded
#define BaseLib_WriteLocker_h_IsIncluded

#include"BaseLib/ReadWriteLock.h"
#include"BaseLib/CommonDefines.h"
#include"BaseLib/Export.h"

namespace BaseLib
{
class DLL_STATE WriteLocker
{
public:
    inline explicit WriteLocker(ReadWriteLock *mutex)
        : mutex_(mutex)
    {
        if(mutex_ == NULL)
			throw std::runtime_error("WriteLocker::WriteLocker(ReadWriteLocker*): Argument error! ReadWriteLocker* == NULL!");

        mutex_->lockForWrite();
    }

    inline ~WriteLocker()
    {
        mutex_->unlock();
    }

private:
    ReadWriteLock *mutex_;

private:
    WriteLocker(const WriteLocker &) { }
    WriteLocker& operator=(const WriteLocker&) { return *this; }
};

}

#endif
