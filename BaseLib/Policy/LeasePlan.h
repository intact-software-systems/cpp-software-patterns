#pragma once

#include "BaseLib/Collection/ISet.h"
#include"BaseLib/Export.h"

namespace BaseLib { namespace Policy {

enum class DLL_STATE LeasePlanKind : short
{
    RENEW_ON_ACCESS = 1,
    RENEW_ON_WRITE = 2,
    RENEW_ON_READ = 3,
    NO_RENEW = 4
};

/**
 * Automatic renew or terminate LeasePlan policy upon update of cached objects
 *
 * TODO: Is Kind logical in a lease? Other kinds? Periodic, indefinite, fixed, subscription?
 */
class DLL_STATE LeasePlan
    : protected Collection::ISet<LeasePlanKind>
{
public:
    LeasePlan(LeasePlanKind kind)
        : Collection::ISet<LeasePlanKind>(kind)
    { }

    LeasePlan(std::set<LeasePlanKind> kinds)
        : Collection::ISet<LeasePlanKind>(kinds)
    { }

    virtual ~LeasePlan()
    { }

    bool IsRenewOnAccess() const
    {
        return this->contains(LeasePlanKind::RENEW_ON_ACCESS);
    }

    bool IsRenewOnWrite() const
    {
        return this->contains(LeasePlanKind::RENEW_ON_WRITE);
    }

    bool IsRenewOnRead() const
    {
        return this->contains(LeasePlanKind::RENEW_ON_READ);
    }

    bool IsNoRenew() const
    {
        return this->contains(LeasePlanKind::NO_RENEW);
    }

    // --------------------------------------------
    // Static constructors
    // --------------------------------------------

    static LeasePlan NoRenew()
    {
        return LeasePlan(LeasePlanKind::NO_RENEW);
    }

    static LeasePlan RenewOnAccess()
    {
        return LeasePlan(LeasePlanKind::RENEW_ON_ACCESS);
    }

    static LeasePlan RenewOnWrite()
    {
        return LeasePlan(LeasePlanKind::RENEW_ON_WRITE);
    }

    static LeasePlan RenewOnRead()
    {
        return LeasePlan(LeasePlanKind::RENEW_ON_READ);
    }

    friend std::ostream& operator<<(std::ostream &ostr, const LeasePlanKind &kind)
    {
        ostr << (int)kind;
        return ostr;
    }

    friend std::ostream& operator<<(std::ostream &ostr, const LeasePlan &leasePlan)
    {
        ostr << "LeasePlan: ";
        for(const auto &kind : leasePlan)
        {
            ostr << (int)kind;
        }
        return ostr;
    }
};

}}
