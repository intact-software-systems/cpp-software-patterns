#ifndef RxData_Cache_SelectionBase_h_IsIncluded
#define RxData_Cache_SelectionBase_h_IsIncluded

#include"RxData/CommonDefines.h"

#include"RxData/cache/object/ObjectRoot.h"
#include"RxData/cache/object/interfaces/ObjectCacheReader.h"
#include"RxData/cache/selection/SelectionCriterion.h"

#include"RxData/Export.h"
namespace RxData
{

/**
 * @brief The SelectionBase class
 */
class DLL_STATE SelectionBase
{
public:
    CLASS_TRAITS(SelectionBase)

    /**
     * @brief refresh
     */
    virtual void refresh(ObjectCacheReader::Ptr) = 0;

    /**
     * @brief filter
     * @return
     */
    virtual bool filter(ObjectRoot::Ptr) = 0;

    /**
     * @brief getSelectionCriterion
     * @return
     */
    virtual SelectionCriterion::Ptr getSelectionCriterion() const = 0;

    /**
     * @brief isAutoRefresh
     * @return
     */
	virtual bool isAutoRefresh() const = 0;

    /**
     * @brief isConcernsContained
     * @return
     */
	virtual bool isConcernsContained() const = 0;
};

}

#endif // RxData_Cache_SelectionBase_h_IsIncluded
