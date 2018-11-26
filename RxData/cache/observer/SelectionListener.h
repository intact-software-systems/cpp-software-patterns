#ifndef RxData_Cache_SelectionListener_h_IsIncluded
#define RxData_Cache_SelectionListener_h_IsIncluded

#include"RxData/CommonDefines.h"
#include"RxData/Export.h"

namespace RxData
{

/**
 * Interface to be implemented by the application to be made aware on updates made on objects 
 * belonging to that selection.
 * 
 * This interface is an abstract template interface, from which a typed interface will be derived for 
 * each application type. 
 * 
 * This typed interface (named FooSelectionListener, if the application class is named Foo) has to be 
 * implemented by the application in order to be made aware of the incoming changes on objects belonging to 
 * a FooSelection.
 *  
 * @author KVik
 *  
 * @param <DATA>
 */
template <typename DATA>
class SelectionObserver
{
public:
    CLASS_TRAITS(SelectionObserver)

	/**
	 * Called when an object enters the Selection. 
	 * 
	 * @param data
	 */
	virtual void OnObjectIn(DATA data) = 0;
	
	/**
	 * Called when an object exits the Selection. 
	 * 
	 * @param data
	 */
	virtual void OnObjectOut(DATA data) = 0;
	
	/**
	 * Called when the contents of an object belonging to the Selection changes
	 * 
	 * @param data
	 */
	virtual void OnObjectModified(DATA data) = 0;
};

}

#endif
