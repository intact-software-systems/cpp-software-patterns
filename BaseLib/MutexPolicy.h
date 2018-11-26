#pragma once

#include"BaseLib/CommonDefines.h"
#include"BaseLib/Export.h"

namespace BaseLib
{

class DLL_STATE MutexPolicy
{
public:
    MutexPolicy(bool debug, bool shared);
    virtual ~MutexPolicy();

    bool IsDebug() const;
    bool IsShared() const;

    static MutexPolicy No();
    static MutexPolicy Debug();
    static MutexPolicy Share();
    static MutexPolicy DebugAndShare();

private:
    bool debug_;
    bool shared_;
};

}

