#ifndef BaseLib_Status_EventCount_h_Included
#define BaseLib_Status_EventCount_h_Included

#include"BaseLib/Count.h"
#include"BaseLib/Templates/MethodInterfaces.h"
#include"BaseLib/Export.h"

namespace BaseLib { namespace Status
{

class DLL_STATE EventCount
{
public:
    EventCount();
    virtual ~EventCount();

    virtual void Accept();
    virtual void Ignore();
    virtual void Filter();
    virtual void Block();

    const Count& Total() const;
    const Count& NumAccepted() const;
    const Count& NumFiltered() const;
    const Count& NumIgnored() const;
    const Count& NumBlocked() const;

private:
    Count total_;

    Count numTimesAccepted_;
    Count numTimesFiltered_;
    Count numTimesIgnored_;
    Count numTimesBlocked_;
};

}}

#endif
