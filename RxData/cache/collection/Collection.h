#ifndef RxData_Cache_Collection_h_IsIncluded
#define RxData_Cache_Collection_h_IsIncluded

#include"RxData/CommonDefines.h"
#include"RxData/Export.h"

namespace RxData
{
/**
 * 
 * @author KVik
 *
 * @param <DATA>
 */
//template <typename DATA>
class Collection
{
public:
	/**
	 * to get a list that contains the indexes of the added elements.	 *  
	 * @return 
	 */
	//public abstract int addedElements();
	//public abstract int removedElements();
	//public abstract int modifiedElements();

	/**
	 * Get 
	 */
	virtual int getLength() = 0;
};

}

#endif
