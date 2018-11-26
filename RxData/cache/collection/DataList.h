#ifndef RxData_Cache_DataList_h_IsIncluded
#define RxData_Cache_DataList_h_IsIncluded

#include"RxData/CommonDefines.h"
#include"RxData/Export.h"

#include"RxData/cache/object/ObjectReference.h"

namespace RxData
{

/**
 * @author KVik
 *
 * @param <DATA>
 */
template<typename DATA>
class DataList : public Collection
{
private:
	// ----------------------------------
	// Attributes
	// ----------------------------------
	//std::list< ObjectReference<DATA> > listData;

public:
	// ----------------------------------
	// Constructor
	// ----------------------------------
	DataList()
	{ 
	}
	
	// ----------------------------------
	// Operations
	// ----------------------------------
	
	/**
	 * Add an item to the end of the list. 
	 * 
	 * @param data
	 */
	void addData(DATA )
	{
		//ObjectReference<DATA> objRef = new ObjectReference<DATA>(data,)
		//listData.add(data);			
	}
	
	/**
	 * Put an item in the collection at a specified index.
	 * 
	 * @param data
	 */
	void putData(int , DATA )
	{
		//listData.add(index, data);			
	}
	
	/**
	 * Retrieve an item in the collection (based on its index).
	 * 
	 * @param data
	 */
//	DATA getData(int index)
//	{
//		//return listData.get(index).get();
//	}

	/**
	 * Remove an element from the Set. If the specified element is not contained in the Set, 
	 * the operation is ignored.
	 * 
	 * @param data
	 */
	void removeData(DATA )
	{
		//listData.remove(data);
	}
	
    /**
     * @brief clearAll
     */
	void clearAll()
	{
		//listData.clear();
	}
	
	/**
	 * Get a list that contains the index of the added elements.
	 */
//	std::set<int> addedElements()
//	{
//		return null;
//	}
	
	/**
	 * Get a list that contains the index of the removed elements.
	 */
//	std::set<int> removedElements()
//	{
//		return null;
//	}
	
	/**
	 * Get a list that contains the index of the modified elements.
	 */
//	std::set<int> modifiedElements()
//	{
//		return null;
//	}

    /**
     * @brief getLength
     * @return
     */
//	virtual int getLength()
//	{
//		return listData.size();
//	}
};

}

#endif
