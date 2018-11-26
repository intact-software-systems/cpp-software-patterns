#ifndef RxData_Cache_FilterCriterion_h_IsIncluded
#define RxData_Cache_FilterCriterion_h_IsIncluded

#include"RxData/CommonDefines.h"

#include"RxData/cache/selection/MembershipState.h"
#include"RxData/cache/selection/SelectionCriterion.h"
#include"RxData/Export.h"
namespace RxData
{

/**
 * Specialization of SelectionCriterion that performs a filter based on user-defined filter algorithm.
 *
 * Extend class and implement checkObject with your algorithm.
 *
 * @author KVik
 *
 * @param <DATA>
 */
template <typename DATA>
class FilterCriterion : public SelectionCriterion
{
public:
    // ----------------------------------
    // Constructor
    // ----------------------------------

    /**
     * Empty constructor. No class attributes to initialize.
     */
    FilterCriterion()
        : SelectionCriterion(SelectionCriteria::FILTER_CRITERION)
    { }

    CLASS_TRAITS(FilterCriterion)

    // ----------------------------------
    // Operations
    // ----------------------------------

    /**
     * Check if an object passes the filter - return value is TRUE - or not - return value is FALSE (check_object).
     * This method is called with the first parameter set to the object to be checked and the second parameter set
     * to indicate whether the object previously passed the filter (membership_state). The second parameter (which
     * is actually an enumeration with three possible values - UNDEFINED MEMBERSHIP, ALREADY MEMBER, and NOT MEMBER)
     * is useful when the FilterCriterion is attached to a Selection to allow writing optimized filters.
     *
     * @param dataObject
     * @param membershipState
     * @return
     */
    virtual bool CheckObject(DATA data, MembershipState::Type membershipState) = 0;
};

}

#endif


