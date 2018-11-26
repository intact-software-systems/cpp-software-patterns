#ifndef BaseLib_Status_SubjectStatus_h_Included
#define BaseLib_Status_SubjectStatus_h_Included

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

class DLL_STATE SubjectStatus
        : public Templates::BlockMethod
        , public Templates::UnblockMethod
        , public Templates::IsBlockedMethod
        , public Templates::LockableType<SubjectStatus>
{
public:
    SubjectStatus();
    virtual ~SubjectStatus();

    enum class DLL_STATE Kind : char
    {
        NO,
        ACTIVE,
        BLOCKED,
        INACTIVE
    };

    SubjectStatus::Kind State() const;

    virtual bool Activate();
    virtual bool Deactivate();

    virtual bool Block();
    virtual bool Unblock();

    virtual bool IsBlocked() const;
    virtual bool IsActive() const;

    const StateStatus<Kind>& Active() const;
    const StateStatus<Kind>& Blocked() const;
    const StateStatus<Kind>& Inactive() const;


    // --------------------------------------
    // Operators
    // --------------------------------------

    friend std::ostream &operator<<(std::ostream &ostr, const Kind &kind)
    {
        switch(kind)
        {
    #define CASE(t) case t: ostr << #t; break;
        CASE(Kind::NO)
        CASE(Kind::ACTIVE)
        CASE(Kind::BLOCKED)
        CASE(Kind::INACTIVE)
    #undef CASE
        default:
            std::cout << "Missing operator<< case for ObserverStatus::Kind " << std::endl;
            break;
        }
        return ostr;
    }

    friend std::ostream &operator<<(std::ostream &ostr, const SubjectStatus &status)
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

    StateStatus<Kind> active_;
    StateStatus<Kind> blocked_;
    StateStatus<Kind> inactive_;
};

}}

#endif
