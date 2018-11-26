#ifndef BaseLib_Templates_StateMethods_h_Included
#define BaseLib_Templates_StateMethods_h_Included

#include <limits.h>

namespace BaseLib { namespace Templates
{

/**
 * State Machine implementation must provide a "state machine" which generates the next state based on input.
 *
 * Ex: Define a set of allowed states "ON" "OFF" "PAUSE". Define state machine code of the form: Next(current-state, current-status, policy) = next-state
 *
 * next-state is associated with a command which executes, and upon finish, next state is associated with a command, etc, until final state is reached.
 *
 * To allow parallel execution use keys?
 *
 * TODO: This is similar to an "iterator pattern".
 *
 * It is possible for a controller to iterate through the state machine. A scheduler can use controller and iterate in a parallell/sequential, etc, way without knowing the arguments.
 */
template <typename State, typename Status, typename Policy>
class NextStateMethod
{
public:
    virtual ~NextStateMethod() {}

    virtual State Next(State currentState, Status currentStatus, Policy policy) = 0;
};


// Reactor must be scheduled to run periodically based on interval policy
// Reactor must be triggered by subject to notify observer
// One Reactor for each observer because the interval policies may differ between observers
// Use action interface
template <typename Return, typename Selector, typename Event>
class RxReactor
{
public:
    RxReactor(Selector selector)
        : selector_(selector)
    {}
    virtual ~RxReactor()
    {}
    Return EventLoop(int64 msecs = LONG_MAX)
    {
        // Block waiting for events to occur.
        Event event = selector_->Select(msecs);
        // Dispatch the event.
        // handler = identify_handler (event);
        // handler.handle_event (event);
        event.Dispatch();
    }
private:
    Selector selector_;
};

}}

#endif
