#ifndef BaseLib_Concurrent_Event_h_Included
#define BaseLib_Concurrent_Event_h_Included

#include"BaseLib/Debug.h"
#include"BaseLib/Exception.h"

#include <BaseLib/Templates/EventData.h>
#include"BaseLib/Templates/AnyKeyType.h"
#include"BaseLib/Templates/AnyType.h"
#include"BaseLib/Templates/EventType.h"
#include"BaseLib/Templates/Synchronization.h"

#include"BaseLib/Status/EventStatus.h"
#include"BaseLib/Export.h"

namespace BaseLib { namespace Concurrent
{

// ----------------------------------------------------
// Event
// ----------------------------------------------------

/**
 * An event should include a timestamp and source of event, see DDS and RTPS specification.
 *
 * Function can be function pointer, i.e., &Object::OnSomeEvent
 *
 * Event is not attached to any particular observer it should be used by Observer to invoke callback and get event arguments from Event.
 *
 * AnyKey is used to be able to compare (event.EventId() == observer.EventId())
 *
 * TODO: Should event gather all the return values?
 */
class DLL_STATE Event
        : public Templates::Event
        , public Templates::Function
        , public Templates::Notifyable<Event, WaitCondition, Mutex>
        , public ENABLE_SHARED_FROM_THIS(Event)
{
public:
    template <typename EventId, typename Source>
    Event(EventData::Ptr eventData, EventId eventId, const Source *source, BaseLib::GeneralException exception = BaseLib::GeneralException(""))
        : Function(eventId)
        , source_(source)
        , eventData_(eventData)
        , stackTrace_(exception)
    {}

    Event(EventData::Ptr eventData, Templates::Any eventId);

    virtual ~Event();

    CLASS_TRAITS(Event)

    Event::Ptr GetPtr();

    // ----------------------------------------------------
    // wait for event delivery. Do I need an EventResult?
    // ----------------------------------------------------

    virtual bool WaitFor(int64 msecs = LONG_MAX) const;

    void AddResult(Templates::Any val);

    // ----------------------------------------------------
    // For filtering
    // ----------------------------------------------------

    virtual void Accept();
    virtual void Ignore();
    virtual void Filter();
    virtual void Reject();

    virtual bool IsAccepted() const;
    virtual bool IsReceived() const;
    virtual bool IsApplicationEvent() const;

    // ----------------------------------------------------
    // Status and identification
    // ----------------------------------------------------

    virtual Templates::Any EventId() const;
    virtual const Status::EventStatus& Status() const;

    EventData::Ptr Data() const;

    BaseLib::GeneralException Exception() const;

    // ----------------------------------------------------
    // Templated functions
    // ----------------------------------------------------

    template <typename T>
    T* Source() const
    {
        return Templates::AnyCast<T>(&source_);
    }

    template <typename Return>
    std::list<Return> Result(int64 msecs = LONG_MAX) const
    {
        Locker locker(this);
        waitForReceived<Status::EventStatus>(&status_, msecs);

        if(!status_.IsAccepted()) return std::list<Return>();

        std::list<Return> results;

        for(Templates::Any val : results_)
        {
            Return *valueType = Templates::AnyCast<Return>(&val);
            if(!valueType)
            {
                IWARNING() << "Could not cast value to type " << TYPE_NAME(Return);
                continue;
            }

            results.push_back(*valueType);
        }
    }

    // --------------------------------------
    // Operators
    // --------------------------------------

    friend std::ostream &operator<<(std::ostream &ostr, const Event &event)
    {
        ostr << "[Event: "
             << event.source_.type().name()
             << ", "
             << event.eventData_->Id().type().name()
             << ", "
             << event.Status()
             << "]";

        return ostr;
    }

    // ----------------------------------------------------
    // Factory functions
    // ----------------------------------------------------

    template <typename EventId, typename Source>
    static Event::Ptr Create(EventData *eventData, EventId eventId, const Source *source)
    {
        return Event::Ptr( new Event(EventData::Ptr(eventData), eventId, source) );
    }

private:
    Status::EventStatus status_;
    Templates::Any source_;
    EventData::Ptr eventData_;
    BaseLib::GeneralException stackTrace_;

    // TODO: Map to observer?
    std::list<Templates::Any> results_;
};

typedef std::list<Event::Ptr> Events;

// ----------------------------------------------------
// EventNoOp
// ----------------------------------------------------

class DLL_STATE EventNoOp : public Templates::Event
{
public:
    EventNoOp()
    { }
    virtual ~EventNoOp()
    { }

    CLASS_TRAITS(EventNoOp)

    virtual void Accept() { }
    virtual void Ignore() { }
    virtual void Filter() { }
    virtual void Reject() { }

    virtual bool IsAccepted() const
    {
        return false;
    }

    virtual bool IsReceived() const
    {
        return false;
    }

    virtual bool IsApplicationEvent() const
    {
        return false;
    }

    virtual bool WaitFor(int64 ) const
    {
        return false;
    }

    virtual Templates::Any EventId() const
    {
        return Templates::Any();
    }

    static EventNoOp::Ptr Create()
    {
        static EventNoOp::Ptr eventNoOp( new EventNoOp());
        return eventNoOp;
    }
};

}}

#endif
