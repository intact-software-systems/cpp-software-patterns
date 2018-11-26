#ifndef BaseLib_Policy_FaultTolerance_h_IsIncluded
#define BaseLib_Policy_FaultTolerance_h_IsIncluded

#include"BaseLib/Export.h"

namespace BaseLib { namespace Policy
{

namespace FaultToleranceKind
{
    enum Type
    {
        FailFast,
        Tolerate,
        Isolate,
        Ignore
    };
}

// TODO: use this in a command controller to invoke failure handling strategies
// TODO: Fallback commands in separate thread pool


// Time between failures
// Time to fix failure
// Attempts (Retry)
// Report policy
// Reaction policy
// Alarm policy

// Concepts:
// - Proactive vs reactive
// - Check pointing
// - Replication
// - Redundancy
// - Job Migration
// - Isolation and Reconfiguration
// - Reliability

}}

#endif
