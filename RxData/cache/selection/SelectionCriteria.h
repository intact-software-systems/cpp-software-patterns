#ifndef RxData_Cache_SelectionCriteria_h_IsIncluded
#define RxData_Cache_SelectionCriteria_h_IsIncluded

#include"RxData/CommonDefines.h"
#include"RxData/Export.h"

namespace RxData
{

/**
 * Describes whether a SelectionCriterion instance represents a FilterCriterion or a QueryCriterion
 * 
 * @author KVik
 */
namespace SelectionCriteria
{
    enum Type
    {
        FILTER_CRITERION,
        QUERY_CRITERION
    };
}

}

#endif

