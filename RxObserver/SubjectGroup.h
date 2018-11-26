#pragma once

#include"RxObserver/Subjects.h"
#include"RxObserver/SubjectDescription.h"

namespace BaseLib {
namespace Concurrent {

template <typename EventType>
class SubjectGroup
    : public SubjectsType<EventType>
      , public Templates::Action0<bool>
      , public Templates::ReactorMethods<Event::Ptr, Event::Ptr, BaseLib::Exception>
      , public BaseLib::Subject::FilterOutSubject1<Event::Ptr>
{
public:
    SubjectGroup(SubjectDescription description = SubjectDescription::Default(), SubjectPolicy policy = SubjectPolicy::Default())
        : SubjectsType<EventType>(description, policy)
        , flowController_(new Concurrent::details::FlowControllerAction<Event::Ptr>())
    { }

    virtual ~SubjectGroup()
    { }

    CLASS_TRAITS(SubjectGroup)

    // ---------------------------------------------------------
    // Factory functions
    // ---------------------------------------------------------

    template <typename Source, typename Return>
    Event::Ptr NextEvent(Return (EventType::*member)(), Source* source)
    {
        typedef Return (EventType::*Member)();

        Event::Ptr event = Concurrent::Event::Create<Member, Source>(new Event0(member), member, source);

        return Next(event);
    }

    template <typename Source, typename Return, typename Arg1>
    Event::Ptr NextEvent(Arg1 arg1, Return (EventType::*member)(Arg1), Source* source)
    {
        typedef Return (EventType::*Member)(Arg1);

        Event::Ptr event = Concurrent::Event::Create<Member, Source>(new Event1<Arg1>(member, arg1), member, source);

        return Next(event);
    }

    template <typename Source, typename Return, typename Arg1, typename Arg2>
    Event::Ptr NextEvent(Arg1 arg1, Arg2 arg2, Return (EventType::*member)(Arg1, Arg2), Source* source)
    {
        typedef Return (EventType::*Member)(Arg1, Arg2);

        Event::Ptr event = Concurrent::Event::Create<Member, Source>(new Event2<Arg1, Arg2>(member, arg1, arg2), member, source);

        return Next(event);
    }

    // ---------------------------------------------------------
    // Reactor methods
    // ---------------------------------------------------------

    virtual Event::Ptr Next(Event::Ptr event)
    {
        bool initialize = Templates::DoubleCheckedLocking::Initialize<SubjectGroup<EventType>>(this);
        if(!initialize) return Event::Ptr();

        bool isAdded = flowController_->Next(event);

        if(!isAdded)
        {
            event->Reject();
            OnFilteredOut(event);
        }
        else if(this->config().DefaultSubjectPolicy().Invocation().IsSync())
        {
            bool received = event->WaitFor(this->config().DefaultSubjectPolicy().Timeout().AsDuration().InMillis());

            ASSERT(received);
        }

        return event;
    }

    virtual bool Complete()
    {
        return flowController_->Complete();
    }

    virtual bool Error(Exception exception)
    {
        this->Cancel();

        return flowController_->Error(exception);
    }

    // ---------------------------------------------------------
    // Action0 interface
    // ---------------------------------------------------------

    virtual bool Invoke()
    {
        { // -- pre conditions --
            bool initialize = Templates::DoubleCheckedLocking::Initialize<SubjectGroup<EventType>>(this);
            if(!initialize)
            {
                IFATAL() << "Failed to initialize subject group!";
                return false;
            }
            if(this->IsBlocked()) return false;
            if(this->numObservers() <= 0)
            {
                IWARNING() << "No observers!";
                return false;
            }
            ASSERT(!this->IsEmpty());
        } // -- pre conditions --


        Events events = flowController_->Pull();

        for(Event::Ptr event : events)
        {
            SubjectId subjectId(event->EventId());

            Subject::Ptr subject = Subjects::Find(subjectId);

            ASSERT(subject != nullptr);

            // What about content filtering and feedback?
            if(subject->Filter(event))
            {
                IINFO() << "Event is filtered out " << event.operator*();

                event->Filter();
                OnFilteredOut(event);
            }
            else
            {
                // Submit directly, the throughput is based on the group.
                bool submitted = subject->Submit(event);

                ASSERT(submitted);

                // If ordering is FIFO then i have to wait for delivery.
                if(this->config().DefaultSubjectPolicy().Ordering().IsFIFO())
                {
                    bool isReceived;

//                    try
                    {
                        ScopedTimer timer(IINFO());
                        isReceived = event->WaitFor(this->config().DefaultSubjectPolicy().Timeout().AsDuration().InMillis());
                    }
//                    catch(InterruptedException ex)
//                    {
//                        IINFO() << "Interrupted sending event: " << event.operator*();
//                    }

                    if(!isReceived)
                    {
                        IINFO() << subject.operator*() << " : " << event.operator*();
                        ASSERT(!event->IsReceived());
                    }

                    ASSERT(isReceived);
                }
            }
        }

        return true;
    }

    virtual bool Cancel()
    {
        return flowController_->Cancel();
    }

    virtual bool IsDone() const
    {
        return flowController_->IsDone();
    }

    virtual bool IsSuccess() const
    {
        return flowController_->IsSuccess();
    }

    // ---------------------------------------------------------
    // TODO: Feedback interface - Attach as a listener
    // ---------------------------------------------------------

    virtual bool OnNext(Event::Ptr event)
    {
        if(event->IsAccepted())
        {
            flowController_->Trigger();
        }
        else
        {
            IWARNING() << "Event not accepted: " << event.operator*();
        }

        return true;
    }

    virtual bool OnError(GeneralException )
    {
        // Cancel?
        return true;
    }

    virtual bool OnComplete()
    {
        // What to do?
        return true;
    }

    // ---------------------------------------------------------
    // Initialize methods
    // ---------------------------------------------------------

    bool Initialize()
    {
        Templates::Action0<bool>::Ptr action = std::dynamic_pointer_cast<Templates::Action0<bool> >(this->GetPtr());

        ASSERT(action != nullptr);

        bool isAttached = flowController_->Set(action);
        status_.Activate();

        ASSERT(isAttached);
        return isAttached;
    }

    bool IsInitialized() const
    {
        return status_.IsActive();
    }

private:

    int numObservers() const
    {
        for(Subject::Ptr subject :this->state().subjects())
        {
            return subject->Size();
        }
        return 0;
    }

private:
    Concurrent::FlowController<Event::Ptr>::Ptr flowController_;
    Status::SubjectStatus                       status_;
};

}
}
