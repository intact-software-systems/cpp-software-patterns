#ifndef MicroMiddleware_ComponentBase_h_IsIncluded
#define MicroMiddleware_ComponentBase_h_IsIncluded

#include"MicroMiddleware/IncludeExtLibs.h"
#include"MicroMiddleware/Export.h"
namespace MicroMiddleware
{

class DLL_STATE ComponentBase : public BaseLib::ObjectBase
{
public:
	ComponentBase(string componentName = string(""));
	virtual ~ComponentBase();

	string GetComponentName() const;
	int componentId() const;

	// use these function for Component-specific initializations
    // Callbacks from ServiceConnectionManager and GroupConnectionManager
	virtual void Initialize();
	virtual void InitializeServices();
	virtual void InitializeGroups();

protected:
	string 			componentName_;
	int				componentId_;
	mutable Mutex 	mutex_;
};

}

#endif // MicroMiddleware_ComponentBase_h_IsIncluded

