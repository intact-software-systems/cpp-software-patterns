#ifndef BaseLib_Status_ObserverStatus_h_Included
#define BaseLib_Status_ObserverStatus_h_Included

#include"BaseLib/Templates/MethodInterfaces.h"
#include"BaseLib/Status/EventStatusMethods.h"
#include"BaseLib/Export.h"

namespace BaseLib { namespace Status
{

// ---------------------------------------------
// Count number times blocked/unblocked
// Timestamp subscribe, etc
// Count policy violations?
// ---------------------------------------------

class DLL_STATE ObserverStatus
        : public Templates::SubscribeMethod<bool>
        , public Templates::UnsubscribeMethod<bool>
        , public Templates::BlockMethod
        , public Templates::UnblockMethod
        , public Templates::IsBlockedMethod
        , public Templates::IsSubscribedMethod
        , public Templates::LockableType<ObserverStatus>
{
public:
    ObserverStatus();
    virtual ~ObserverStatus();

    enum class DLL_STATE Kind : char
    {
        NO,
        SUBSCRIBED,
        BLOCKED,
        UNSUBSCRIBED
    };

    ObserverStatus::Kind State() const;

    virtual bool Subscribe();
    virtual bool Unsubscribe();
    virtual bool Block();
    virtual bool Unblock();

    virtual bool IsBlocked() const;
    virtual bool IsSubscribed() const;

    const StateStatus<Kind>& Subscribed() const;
    const StateStatus<Kind>& Blocked() const;
    const StateStatus<Kind>& Unsubscribed() const;

    // --------------------------------------
    // Operators
    // --------------------------------------

    friend std::ostream &operator<<(std::ostream &ostr, const Kind &kind)
    {
        switch(kind)
        {
    #define CASE(t) case t: ostr << #t; break;
        CASE(ObserverStatus::Kind::NO)
        CASE(ObserverStatus::Kind::SUBSCRIBED)
        CASE(ObserverStatus::Kind::BLOCKED)
        CASE(ObserverStatus::Kind::UNSUBSCRIBED)
    #undef CASE
        default:
            std::cout << "Missing operator<< case for ObserverStatus::Kind " << std::endl;
            break;
        }
        return ostr;
    }

    friend std::ostream &operator<<(std::ostream &ostr, const ObserverStatus &status)
    {
        ostr << "["
             << status.current_
             << ", "
             << status.prev_
             << "]";

        return ostr;
    }

private:
    Kind current_;
    Kind prev_;

    // TODO: Organize in a map<Kind,Templates::StateStatus<Kind>> stateStatus_;
    StateStatus<Kind> subscribed_;
    StateStatus<Kind> blocked_;
    StateStatus<Kind> unsubscribed_;
};

}}

#endif
