#ifndef BaseLib_Status_AccessStatus_h_Included
#define BaseLib_Status_AccessStatus_h_Included

#include"BaseLib/Templates/MethodInterfaces.h"

#include"BaseLib/Status/AccessCount.h"
#include"BaseLib/Status/AccessTime.h"

#include"BaseLib/Export.h"

namespace BaseLib { namespace Status
{

namespace AccessStatusKind
{
    enum Type
    {
        READ,
        WRITE,
        MODIFY,
        NOT_MODIFY,
        DELETED
    };
}

class DLL_STATE AccessStatus
        : public ::BaseLib::Templates::IsReadMethod
        , public ::BaseLib::Templates::IsModifiedMethod
        , public ::BaseLib::Templates::IsNotModifiedMethod
        , public ::BaseLib::Templates::IsDeletedMethod
        , public ::BaseLib::Templates::ModifyMethod
        , public ::BaseLib::Templates::NotModifyMethod
        , public ::BaseLib::Templates::ReadMethod
        , public ::BaseLib::Templates::DeleteMethod
{
public:
    AccessStatus();
    virtual ~AccessStatus();

    // ------------------------------------------------------
    // Simple getters
    // ------------------------------------------------------

    AccessCount &Count();
    const AccessCount& Count() const;

    AccessTime &Time();
    const AccessTime& Time() const;

    AccessStatusKind::Type State() const;

    // ------------------------------------------------------
    // Using interfaces: Called when execution starts and stops
    // ------------------------------------------------------

    virtual void Modify();
    virtual void NotModify();
    virtual void Read();
    virtual void Delete();

    // TODO: Weird function name in status, related too much to policy?
    virtual void RenewLease();

    // ------------------------------------------------------
    // Using interfaces: Check status
    // ------------------------------------------------------

    virtual bool IsRead() const;
    virtual bool IsModified() const;
    virtual bool IsNotModified() const;
    virtual bool IsDeleted() const;

private:
    AccessStatusKind::Type kind_;
    AccessTime time_;
    AccessCount count_;
};

}}

#endif
