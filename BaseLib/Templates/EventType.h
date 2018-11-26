#ifndef BaseLib_Templates_EventType_h_Included
#define BaseLib_Templates_EventType_h_Included

#include"BaseLib/Templates/BaseTypes.h"
#include"BaseLib/Templates/ActionHolder.h"
#include"BaseLib/Templates/AnyType.h"

namespace BaseLib { namespace Templates
{

/**
 * An event should include a timestamp and source of event, see DDS and RTPS specification.
 *
 * EventType can be function pointer, i.e., &Object::OnSomeEvent
 *
 * TODO: Move to Concurrent::Observer?
 */
class Event
{
public:
    virtual ~Event() {}

    CLASS_TRAITS(Event)

    // --------------------------
    // For filtering
    // --------------------------

    virtual void Accept() = 0;
    virtual void Ignore() = 0;
    virtual void Filter() = 0;
    virtual void Reject() = 0;

    virtual bool IsAccepted() const = 0;
    virtual bool IsReceived() const = 0;

    /**
     * true if event originated from within the application, otherwise the event
     * originated from outside the application.
     */
    virtual bool IsApplicationEvent() const = 0;

    /**
     * Wait for event deilivery.
     * @return true if accepted.
     */
    virtual bool WaitFor(int64 msecs = LONG_MAX) const = 0;

    //virtual EventType Type() const = 0;
    //virtual const Status::EventStatus& Status() const = 0;

    virtual Templates::Any EventId() const = 0;
};

//template <typename T>
//class EventSource
//{
//public:
//    virtual T Source() const = 0;
//};


// Can I use Action?

class Event0
        : public Event
        , public Templates::Action
{
public:
    virtual ~Event0() {}

    CLASS_TRAITS(Event0)
};

template <typename ARG1>
class Event1
        : public Event
        , public Templates::Action
{
public:
    virtual ~Event1() {}

    CLASS_TRAITS(Event1)

    virtual ARG1 Arg1() const = 0;
};

template <typename ARG1, typename ARG2>
class Event2
        : public Event
        , public Templates::Action
{
public:
    virtual ~Event2() {}

    CLASS_TRAITS(Event2)

    virtual ARG1 Arg1() const = 0;
    virtual ARG2 Arg2() const = 0;
};


template <typename ARG1, typename ARG2, typename ARG3>
class Event3
        : public Event
        , public Templates::Action
{
public:
    virtual ~Event3() {}

    CLASS_TRAITS(Event3)

    virtual ARG1 Arg1() const = 0;
    virtual ARG2 Arg2() const = 0;
    virtual ARG3 Arg3() const = 0;
};


template <typename ARG1, typename ARG2, typename ARG3, typename ARG4>
class Event4
        : public Event
        , public Templates::Action
{
public:
    virtual ~Event4() {}

    CLASS_TRAITS(Event4)

    virtual ARG1 Arg1() const = 0;
    virtual ARG2 Arg2() const = 0;
    virtual ARG3 Arg3() const = 0;
    virtual ARG4 Arg4() const = 0;
};

template <typename ARG1, typename ARG2, typename ARG3, typename ARG4, typename ARG5>
class Event5
        : public Event
        , public Templates::Action
{
public:
    virtual ~Event5() {}

    CLASS_TRAITS(Event5)

    virtual ARG1 Arg1() const = 0;
    virtual ARG2 Arg2() const = 0;
    virtual ARG3 Arg3() const = 0;
    virtual ARG4 Arg4() const = 0;
    virtual ARG5 Arg5() const = 0;
};

}}

#endif
