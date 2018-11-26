#ifndef RxData_Cache_Ejection_h_IsIncluded
#define RxData_Cache_Ejection_h_IsIncluded

#include"RxData/CommonDefines.h"
#include"RxData/Export.h"

namespace RxData
{

/**
 * Enumerated value
 */
namespace EjectionKind
{
    enum Type
    {
        LEAST_RECENTLY_USED,
        LEAST_FREQUENTLY_USED
    };
}

/**
 * Policy to decide which objects to evict from the cache once ResourceLimits 
 * or Lifespan policies trigger.
 * 
 * Also known as Replacement policy in cache algorithms. 
 * 
 * @author KVik
 */
class DLL_STATE Ejection
{
private:
	
	// ----------------------------------
	// Attributes
	// ----------------------------------

	EjectionKind::Type ejectionKind_;

public:
	// ----------------------------------
	// Constructor
	// ----------------------------------
	
	/**
	 * Constructor sets the ejection strategy to default.
	 */
	Ejection()
        : ejectionKind_(EjectionKind::LEAST_RECENTLY_USED)
	{ }

	/**
	 * User defined ejection strategy
	 * 
	 * @param ejectionKind
	 */
	Ejection(EjectionKind::Type kind)
        : ejectionKind_(kind)
	{ }

    ~Ejection()
    { }

	// ----------------------------------
	// Getters
	// ----------------------------------
	
	EjectionKind::Type getEjectionKind() const
	{
		return ejectionKind_;
	}

	// ----------------------------------
	// Static convenience functions
	// ----------------------------------
	
	/**
	 * @return Ejection with policy to evict objects based on least recently used
	 */
	static Ejection LeastRecentlyUsed()
	{
		return Ejection(EjectionKind::LEAST_RECENTLY_USED);
	}
	
	/**
	 * @return Ejection with policy to evict objects based on least frequently used
	 */
	static Ejection LeastFrequentlyAccessed()
	{
		return Ejection(EjectionKind::LEAST_FREQUENTLY_USED);
	}
};

}

#endif

