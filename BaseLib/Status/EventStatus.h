#ifndef BaseLib_Status_EventStatus_h_Included
#define BaseLib_Status_EventStatus_h_Included

#include"BaseLib/Status/EventStatusMethods.h"

#include"BaseLib/Export.h"

namespace BaseLib { namespace Status
{

// ---------------------------------------------
// Count number of events (produced/consumed)
// Timestamp event (first, last, etc)
// Count event policy violations?
// ---------------------------------------------

// TODO: Organize in a map<Kind,Templates::StateStatus<Kind>> stateStatus_;
class DLL_STATE EventStatus
        : public Templates::LockableType<EventStatus>
{
public:
    EventStatus();
    virtual ~EventStatus();

    enum class DLL_STATE Kind : char
    {
        NO,
        ACCEPTED,
        IGNORED,
        FILTERED,
        REJECTED,
        BLOCKED,
        QUEUED
    };

    EventStatus::Kind State() const;

    virtual void Accept();
    virtual void Ignore();
    virtual void Filter();
    virtual void Reject();
    virtual void Block();
    virtual void Unblock();
    virtual void Queue();

    virtual bool IsAccepted() const;
    virtual bool IsIgnored() const;
    virtual bool IsFiltered() const;
    virtual bool IsRejected() const;
    virtual bool IsReceived() const;
    virtual bool IsBlocked() const;

    const StateStatus<Kind>& Accepted() const;
    const StateStatus<Kind>& Ignored() const;
    const StateStatus<Kind>& Filtered() const;
    const StateStatus<Kind>& Rejected() const;
    const StateStatus<Kind>& Blocked() const;
    const StateStatus<Kind>& Queued() const;

    // ------------------------------------------------------
    // Operators
    // ------------------------------------------------------

    friend std::ostream& operator<<(std::ostream& ostr, const EventStatus::Kind& kind)
    {
        switch(kind)
        {
    #define CASE(t) case t: ostr << #t; break;
        CASE(EventStatus::Kind::NO)
        CASE(EventStatus::Kind::ACCEPTED)
        CASE(EventStatus::Kind::IGNORED)
        CASE(EventStatus::Kind::FILTERED)
        CASE(EventStatus::Kind::REJECTED)
        CASE(EventStatus::Kind::BLOCKED)
        CASE(EventStatus::Kind::QUEUED)
    #undef CASE
        default:
            std::cerr << "Missing operator<< case for EventStatus::Kind" << std::endl;
            break;
        }
        return ostr;
    }

    // --------------------------------------
    // Operators
    // --------------------------------------

    friend std::ostream &operator<<(std::ostream &ostr, const EventStatus &status)
    {
        ostr << "["
             << status.kind_
             << ", "
             << status.prev_
             << "]";

        return ostr;
    }

private:
    Kind kind_;
    Kind prev_;

    // TODO: Organize in a map<Kind,Templates::StateStatus<Kind>> stateStatus_;
    StateStatus<Kind> accepted_;
    StateStatus<Kind> ignored_;
    StateStatus<Kind> filtered_;
    StateStatus<Kind> rejected_;
    StateStatus<Kind> blocked_;
    StateStatus<Kind> queued_;
};

}}

#endif
