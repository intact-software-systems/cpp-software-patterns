#ifndef MicroMiddleware_ComponentManager_h_IsIncluded
#define MicroMiddleware_ComponentManager_h_IsIncluded

#include"MicroMiddleware/IncludeExtLibs.h"
#include"MicroMiddleware/Export.h"
namespace MicroMiddleware
{

class ComponentBase;

class DLL_STATE ComponentManager  : public BaseLib::Thread
						, public BaseLib::ObjectBase
{
public:
	ComponentManager(std::string componentName = std::string("MicroMiddleware.Manager.Component"));
	~ComponentManager();

	typedef int ComponentId;
	typedef std::map<ComponentId, ComponentBase*>		MapIdComponent;

	static ComponentManager* 	getOrCreate();

	bool			AddComponent(ComponentBase *);
	bool 			RemoveComponent(ComponentBase *);
	ComponentBase*	GetComponent(ComponentId id);

protected:
	virtual void run();

private:
	static ComponentManager* 	componentManager_;
	MapIdComponent				mapIdComponent_;

private:
	mutable Mutex 	mutex_;
};

}

#endif // MicroMiddleware_ComponentManager_h_IsIncluded


