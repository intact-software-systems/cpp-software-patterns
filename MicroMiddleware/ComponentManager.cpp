#include "MicroMiddleware/ComponentManager.h"
#include "MicroMiddleware/ComponentBase.h"

namespace MicroMiddleware
{

ComponentManager* ComponentManager::componentManager_ = NULL;

ComponentManager::ComponentManager(string componentName) 
		: BaseLib::Thread(componentName)
		, BaseLib::ObjectBase(componentName)
{

}

ComponentManager::~ComponentManager()
{
	
}

ComponentManager* ComponentManager::getOrCreate()
{
	static Mutex staticMutex;
	MutexLocker lock(&staticMutex);

	if(ComponentManager::componentManager_ == NULL)
	{
		ComponentManager::componentManager_ = new ComponentManager();
		ComponentManager::componentManager_->start();
	}

	return componentManager_;
}

void ComponentManager::run()
{
	//IDEBUG() << "Component manager is up!" ;

	while(true)
	{
		// do something!
		sleep(1);
	}
}

bool ComponentManager::AddComponent(ComponentBase *component)
{
	MutexLocker lock(&mutex_);

	if(mapIdComponent_.count(component->componentId() > 0))
	{
		IDEBUG() << "WARNING! Component " << component->componentId() << " already registered!";
		return false;
	}
	
	mapIdComponent_[component->componentId()] = component;
	//IDEBUG() << "Component " << component->componentId() << " " << component->componentName() << " started!" ;
	
	return true;
}

bool ComponentManager::RemoveComponent(ComponentBase *component)
{
	MutexLocker lock(&mutex_);
	
	if(mapIdComponent_.count(component->componentId() <= 0))
	{
		IDEBUG() << "WARNING! Component " << component->componentId() << " already stopped!" ;
		return false;
	}
	
	mapIdComponent_.erase(component->componentId());
	IDEBUG() << "Component " << component->componentId() << " " << component->GetComponentName() << " stopped!" ;
	
	return true;
}

ComponentBase* ComponentManager::GetComponent(ComponentId id)
{
	MutexLocker lock(&mutex_);
	
	if(mapIdComponent_.count((id) <= 0))
	{
		IDEBUG() << "WARNING! Component " << id << " not found!" ;
		return NULL;
	}

	return mapIdComponent_[id];
}


}; // namespace MicroMiddleware


