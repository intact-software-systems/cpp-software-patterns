#ifndef RxData_Cache_ObjectHomeBase_h_IsIncluded
#define RxData_Cache_ObjectHomeBase_h_IsIncluded

#include"RxData/IncludeExtLibs.h"

#include"RxData/cache/CacheInterface.h"
#include"RxData/cache/object/policy/ObjectHomePolicy.h"

#include"RxData/Export.h"
namespace RxData
{

/**
 * @brief The ObjectHomeBase class
 */
class DLL_STATE ObjectHomeBase
{
public:
    CLASS_TRAITS(ObjectHomeBase)

    /**
     * @brief refresh
     */
    virtual void refresh(CacheInterface*) = 0;

    /**
     * @brief getTypeName
     * @return
     */
	virtual std::string getTypeName() const = 0;

    /**
     * @brief getContentFilter
     * @return
     */
	virtual std::string getContentFilter() const = 0;

    /**
     * @brief isAutoDeref
     * @return
     */
	virtual bool isAutoDeref() const = 0;

    /**
     * @brief getPolicy
     * @return
     */
	virtual ObjectHomePolicy getPolicy() const = 0;
};

}

#endif // OBJECTHOMBASE_H
