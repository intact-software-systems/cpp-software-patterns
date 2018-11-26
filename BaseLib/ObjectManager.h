#ifndef BaseLib_ObjectManager_h_Included
#define BaseLib_ObjectManager_h_Included

#include"BaseLib/CommonDefines.h"
#include"BaseLib/Mutex.h"
#include"BaseLib/Thread.h"
#include"BaseLib/Exception.h"
#include"BaseLib/Export.h"

namespace BaseLib
{

class ObjectBase;

class DLL_STATE ObjectManager : public Thread
{
private:
    ObjectManager(std::string name = std::string("BaseLib.Manager.Object"));
	~ObjectManager();

public:
	static ObjectManager*	createObjectManager() throw();
	static ObjectManager*	getObjectManager();

public:
	bool 		AddObject(ObjectBase *base) throw();
	bool 		RemoveObject(ObjectBase *base) throw();
	ObjectBase*	GetObject(const std::string &objectName) const throw();

private:
	virtual void run();

private:
	static ObjectManager 			*objectManager_;
	typedef std::set<ObjectBase*>	SetObjectBase;
	SetObjectBase					setObjectBase_;
	mutable	Mutex					managerMutex_;
};

}; // namespace ObjectManager

#endif // BaseLib_ObjectManager_h_Included


