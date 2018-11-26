#ifndef RxData_Cache_DataMap_h_IsIncluded
#define RxData_Cache_DataMap_h_IsIncluded

#include"RxData/CommonDefines.h"
#include"RxData/Export.h"

namespace RxData
{

/**
 * 
 * @author KVik
 *
 * @param <KEY>
 * @param <DATA>
 */
template <typename KEY, typename DATA>
class DataMap : public Collection
{
private:
	//std::map<KEY, DATA> mapIdData;

public:
	DataMap()
	{ 
	}
	
	/**
	 * Retrieve an item in the collection (based on its key).
	 * 
	 * @param key
	 * @return
	 */
//	DATA getData(KEY key)
//	{
//		return mapIdData.get(key);
//	}
	
	/**
	 * Put an item in the collection.
	 * 
	 * @param key
	 * @param data
	 */
//	void putData(KEY key, DATA data)
//	{
//		mapIdData.put(key, data);
//	}

	/**
	 * Remove an element from the Map, identified by the KEY.
	 * 
	 * @param key
	 */
//	void removeData(KEY key)
//	{
//		mapIdData.remove(key);
//	}
	
//	void clearAll()
//	{
//		mapIdData.clear();
//	}

//	/**
//	 * Get a list that contains the keys of the added elements.
//	 */
//	std::set<KEY> addedElements()
//	{
		
//		return null;
//	}
	
//	/**
//	 * Get a list that contains the keys of the removed elements.
//	 */
//	std::set<KEY> removedElements()
//	{
//		return null;
//	}
	
//	/**
//	 * Get a list that contains the keys of the modified elements.
//	 */
//	std::set<KEY> modifiedElements()
//	{
//		return null;
//	}
};

}

#endif
