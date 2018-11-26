#ifndef RxData_Cache_SelectionCriterion_h_IsIncluded
#define RxData_Cache_SelectionCriterion_h_IsIncluded

#include"RxData/CommonDefines.h"
#include"RxData/cache/selection/SelectionCriteria.h"

#include"RxData/Export.h"
namespace RxData
{
/**
 * Class whose instances act as filter for Selection objects. When a Selection is created, it must be 
 * given a SelectionCriterion.
 * 
 * @author KVik
 */
class DLL_STATE SelectionCriterion : public ENABLE_SHARED_FROM_THIS(SelectionCriterion)
{
private:
	// ----------------------------------
	// Attributes
	// ----------------------------------
	
	SelectionCriteria::Type kind_;

public:
	// ----------------------------------
	// Constructor
	// ----------------------------------

	SelectionCriterion(SelectionCriteria::Type kind)
        : kind_(kind)
	{ }

    virtual ~SelectionCriterion()
    { }

    CLASS_TRAITS(SelectionCriterion)
	
    /**
     * @brief GetPtr
     * @return
     */
    SelectionCriterion::Ptr GetPtr()
    {
        return this->shared_from_this();
    }

	// ----------------------------------
	// Getters
	// ----------------------------------
	
	SelectionCriteria::Type getKind() const
    {
		return kind_;
	}
};

}

#endif

