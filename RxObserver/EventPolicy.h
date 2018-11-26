#ifndef BaseLib_Concurrent_EventPolicy_h_IsIncluded
#define BaseLib_Concurrent_EventPolicy_h_IsIncluded

#include"RxObserver/IncludeExtLibs.h"
#include"RxObserver/Export.h"

namespace BaseLib { namespace Concurrent
{

/** -------------------------------------------------------------------
TODO: Policy support in event:
- Deadline/lifetime of event. Ex: "event is only valid in a certain time-period, and all observers must receive it by then or the event is discarded."
- Notification policy - "Subject can decide that this event should notify observer via a callback upon delivery"
-
 ------------------------------------------------------------------- */
class DLL_STATE EventPolicy
{
public:
    EventPolicy(Policy::Timeout timeout);
    virtual ~EventPolicy();

    Policy::Timeout Timeout() const;

    static EventPolicy Default();

private:
    Policy::Timeout timeout_;
};

}}

#endif
