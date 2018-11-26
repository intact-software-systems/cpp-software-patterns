#ifndef RxData_Cache_Contract_h_IsIncluded
#define RxData_Cache_Contract_h_IsIncluded

#include"RxData/IncludeExtLibs.h"

#include"RxData/cache/scope/ObjectScope.h"
#include"RxData/cache/object/ObjectRoot.h"

#include"RxData/Export.h"
namespace RxData
{

/**
 * Contract is the class that defines which objects will be cloned from the 
 * Cache into the CacheAccess when the latter is refreshed.
 * 
 * @author KVik
 */
class DLL_STATE Contract
{
private:
	// ----------------------------------
	// Attributes
	// ----------------------------------
	
	/**
	 * The depth of the cloning contract. This defines how many levels of relationships 
	 * will be covered by the contract (UNLIMITED_RELATED_OBJECTS when all navigable 
	 * objects must be cloned recursively). The depth only applies to a RELATED_OBJECT_SCOPE.
	 */
	int depth_;
	
	/**
	 * The scope of the cloning request (i.e., the object itself, or the object with all its 
	 * (nested) compositions, or the object with all its (nested) compositions and all the 
	 * objects that are navigable from it up till the specified depth).
	 */
	ObjectScope::Type scope_;
	
	/**
	 * The top-level object (contracted_object). This is the object that acts as 
	 * the starting point for the cloning contract.
	 */
	ObjectRoot::Ptr contractedObject_;

public:
	// ----------------------------------
	// Constructor
	// ----------------------------------
	
	/**
	 * Contract is the class that defines which objects will be cloned from the Cache into 
	 * the CacheAccess when the latter is refreshed.
	 * 
	 * @param depth
	 * @param scope
	 * @param contractedObject
	 */
	Contract(int depth, ObjectScope::Type scope, ObjectRoot::Ptr contractedObject)
        : depth_(depth)
        , scope_(scope)
        , contractedObject_(contractedObject)
	{ }

    virtual ~Contract()
    {}

	// ----------------------------------
	// Setters
	// ----------------------------------

    /**
	 * Change the depth of an existing contract. This change will only be taken into account at 
	 * the next refresh of the CacheAccess.
	 * 
	 * @param depth
	 */
	void setDepth(int depth)
	{
		this->depth_ = depth;
	}
	
	/**
	 * Change the scope of an existing contract (set_scope). This change will only be taken into 
	 * account at the next refresh of the CacheAccess.
	 * 
	 * @param scope
	 */
	void setScope(ObjectScope::Type scope)
	{
		this->scope_ = scope;
	}

	// ----------------------------------
	// Getters
	// ----------------------------------
	
	int getDepth() const
	{
		return depth_;
	}

	ObjectScope::Type getScope() const
	{
		return scope_;
	}

	ObjectRoot::Ptr getContractedObject() const
	{
		return contractedObject_;
	}

    bool isValid() const
    {
        if(contractedObject_) return true;

        return false;
    }

    // ----------------------------------
	// Comparators
	// ----------------------------------

    /**
     * @brief operator ==
     * @param contract
     * @return
     */
    bool operator==(const Contract &contract) const
    {
        return this->contractedObject_ == contract.contractedObject_ &&
                this->depth_ == contract.depth_ &&
                this->scope_ == contract.scope_;
    }

    // ----------------------------------
	// Prototypes
	// ----------------------------------

    /**
     * @brief Invalid
     * @return
     */
    static Contract Invalid()
    {
        return Contract(0, ObjectScope::SIMPLE_OBJECT_SCOPE, ObjectRoot::Ptr());
    }

};

}

#endif

