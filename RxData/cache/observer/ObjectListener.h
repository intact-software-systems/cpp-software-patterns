#ifndef RxData_Cache_ObjectListener_h_IsIncluded
#define RxData_Cache_ObjectListener_h_IsIncluded

#include"RxData/CommonDefines.h"
#include"RxData/Export.h"

namespace RxData
{

/**
 * This interface is an abstract root, from which a typed interface will be derived for each application type. 
 * This typed interface (named FooListener if the application class is named Foo) then has to be implemented 
 * by the application, so that the application will be made aware of the incoming changes on objects belonging 
 * to the FooHome.
 *
 * @author KVik
 *  
 * @param <DATA>
 */
template <typename DATA>
class ObjectObserver
{
public:
    CLASS_TRAITS(ObjectObserver)

	/**
	 * Called when a new object appears in the Cache; this operation is called with the newly created object 
	 * (the_object).
	 * 
	 * @param data
	 * 
	 * @return 
	 * 
	 * TRUE means that the event has been fully taken into account and therefore does not need to be propagated 
	 * to other ObjectListener objects (of parent classes).
	 */
	virtual bool OnObjectCreated(DATA data) = 0;
	
	
	/**
	 * Called when an object has been deleted by another participant; this operation is called with the newly 
	 * deleted object (the_object).
	 * 
	 * @param data
	 * 
	 * @return 
	 * 
	 * TRUE means that the event has been fully taken into account and therefore does not need to be propagated 
	 * to other ObjectListener objects (of parent classes).
	 */
	virtual bool OnObjectDeleted(DATA data) = 0;
	
	
	/**
	 * Called when the contents of an object changes; this operation is called with the modified object (the_object).
	 * 
	 * @param data
	 * @return 
	 * 
	 * TRUE means that the event has been fully taken into account and therefore does not need to be propagated 
	 * to other ObjectListener objects (of parent classes).
	 */
	virtual bool OnObjectModified(DATA data) = 0;
};

}

#endif
