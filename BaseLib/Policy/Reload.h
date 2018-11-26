#ifndef BaseLib_Policy_Reload_h_IsIncluded
#define BaseLib_Policy_Reload_h_IsIncluded

#include"BaseLib/Templates/BaseTypes.h"
#include"BaseLib/Export.h"

namespace BaseLib { namespace Policy
{

namespace ReloadKind
{
    enum Type
    {
        NO,
        ON_CREATE,
        ON_DELETE,
        ON_MODIFY,

        /**
          * Enables refreshing the cache without cleanup or access to cache.
          * Rather, it is based on timer
          */
        ON_REFRESH
    };
}

/**
 * @brief The Reload class
 *
 * TODO: Rename to Load (Load on expiry)?
 * TODO: store kind as a list internally to support multiple
 */
class DLL_STATE Reload
        : public Templates::ComparableImmutableType<ReloadKind::Type>
{
public:
    Reload(ReloadKind::Type kind)
        : Templates::ComparableImmutableType<ReloadKind::Type>(kind)
    {}
    virtual ~Reload()
    {}

    ReloadKind::Type Kind() const
    {
        return this->Clone();
    }

    bool IsNo() const
    {
        return this->Clone() == ReloadKind::NO;
    }

    bool IsOnCreate() const
    {
        return this->Clone() == ReloadKind::ON_CREATE;
    }

    bool IsOnDelete() const
    {
        return this->Clone() == ReloadKind::ON_DELETE;
    }

    bool IsOnModify() const
    {
        return this->Clone() == ReloadKind::ON_MODIFY;
    }

    bool IsOnRefresh() const
    {
        return this->Clone() == ReloadKind::ON_REFRESH;
    }

    //---------------------------------------------------
    // factory functions
    //---------------------------------------------------

    static Reload No()
    {
        static Reload reload(ReloadKind::NO);
        return reload;
    }

    static Reload OnCreate()
    {
        static Reload reload(ReloadKind::ON_CREATE);
        return reload;
    }

    static Reload OnDelete()
    {
        static Reload reload(ReloadKind::ON_DELETE);
        return reload;
    }

    static Reload OnModify()
    {
        static Reload reload(ReloadKind::ON_MODIFY);
        return reload;
    }

    static Reload OnRefresh()
    {
        static Reload reload(ReloadKind::ON_REFRESH);
        return reload;
    }
};

}}

#endif
