#include "BaseLib/MutexPolicy.h"

namespace BaseLib
{

MutexPolicy::MutexPolicy(bool debug, bool shared)
    : debug_(debug)
    , shared_(shared)
{}

MutexPolicy::~MutexPolicy()
{}

bool MutexPolicy::IsDebug() const { return debug_; }

bool MutexPolicy::IsShared() const { return shared_; }

MutexPolicy MutexPolicy::No()
{
    return MutexPolicy(false, false);
}

MutexPolicy MutexPolicy::Debug()
{
    return MutexPolicy(true, false);
}

MutexPolicy MutexPolicy::Share()
{
    return MutexPolicy(false, true);
}

MutexPolicy MutexPolicy::DebugAndShare()
{
    return MutexPolicy(true, true);
}

}
