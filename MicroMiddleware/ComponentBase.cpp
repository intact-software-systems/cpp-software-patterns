#include "MicroMiddleware/ComponentBase.h"
#include "MicroMiddleware/ComponentManager.h"
#include "MicroMiddleware/CommonDefines.h"

namespace MicroMiddleware
{

ComponentBase::ComponentBase(string componentName)
        : BaseLib::ObjectBase(componentName)
        , componentName_(componentName)
        , componentId_(-1)
{
    static int id = 1;
    componentId_ = id++;

    ComponentManager::getOrCreate()->AddComponent(this);
}

ComponentBase::~ComponentBase()
{

}

std::string ComponentBase::GetComponentName() const
{
    MutexLocker lock(&mutex_);
    return ObjectBase::Name();
}

int ComponentBase::componentId() const
{
    MutexLocker lock(&mutex_);
    return componentId_;
}

void ComponentBase::Initialize()
{
    cout << COMPONENT_FUNCTION << endl;
}

void ComponentBase::InitializeServices()
{
    cout << COMPONENT_FUNCTION << endl;
}

void ComponentBase::InitializeGroups()
{
    cout << COMPONENT_FUNCTION << endl;
}

}; // namespace MicroMiddleware

