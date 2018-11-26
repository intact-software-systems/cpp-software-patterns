#ifndef RxData_Cache_DataSet_h_IsIncluded
#define RxData_Cache_DataSet_h_IsIncluded

#include"RxData/CommonDefines.h"
#include"RxData/Export.h"

namespace RxData
{

/**
 * @author KVik
 *
 * @param <DATA>
 */
template <typename DATA>
class DataSet : public Collection
{
private:
	std::set<DATA> setData;

public:
	DataSet()
	{ 
	}
	
	/**
	 * @param  data
	 * 
	 * @return whether the specified value is already contained in the Set (true) or not (false).
	 */
//	bool contains(DATA data)
//	{
//		return setData.contains(data);
//	}
//
	/**
	 * Add an element to the Set. If the specified element was already contained in the Set, 
	 * the operation is ignored.
	 * 
	 * @param data
	 */
//	void addData(DATA data)
//	{
//		setData.insert(data);
//	}

	/**
	 * Remove an element from the Set. If the specified element is not contained in the Set, 
	 * the operation is ignored.
	 * 
	 * @param data
	 */
//	void removeData(DATA data)
//	{
//		setData.remove(data);
//	}
	
//	void clearAll()
//	{
//		setData.clear();
//	}
	
	/**
	 * to return the elements added in the last update round.
	 */
//	std::set<DATA> addedElements()
//	{
//        return std::set<DATA>();
//	}
	
	/**
	 * to return the elements removed in the last update round.
	 */
//	std::set<DATA> removedElements()
//	{
//        return std::set<DATA>();
//	}
	
//	virtual int getLength()
//	{
//		return setData.size();
//	}
};

}

#endif
