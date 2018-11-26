#ifndef RxData_Cache_MembershipState_h_IsIncluded
#define RxData_Cache_MembershipState_h_IsIncluded

#include"RxData/CommonDefines.h"
#include"RxData/Export.h"

namespace RxData
{

/**
 * Useful when a FilterCriterion is attached to a Selection to allow writing optimized filters.
 * 
 * @author KVik
 */
namespace MembershipState
{
    enum Type
    {
        UNDEFINED_MEMBERSHIP,
        ALREADY_MEMBER,
        NOT_MEMBER
    };
}

}

#endif

