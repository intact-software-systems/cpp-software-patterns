#ifndef IntactMiddleware_IntactComponent_h_IsIncluded
#define IntactMiddleware_IntactComponent_h_IsIncluded

#include"IntactMiddleware/IncludeExtLibs.h"
#include"IntactMiddleware/Export.h"

namespace IntactMiddleware
{

/* -------------------------------------------------------
 		class IntactComponent
TODO:
	- Make IntactComponent inherit from BaseLib::Thread, 
	and have a dummy implementation of a run()
	- Then, each component can reimplement the run() function
	if necessary, and no silly run function with only a 
	while(true) sleep(1) is necessary.
 -------------------------------------------------------*/
class DLL_STATE IntactComponent : public MicroMiddleware::ComponentBase
{
public:
	IntactComponent(std::string componentName = std::string(""));
	virtual ~IntactComponent();

	virtual void Initialize();

	// SettingsManager
	virtual bool HasComponentSettings();
	virtual void ParseComponentSettings();
};

} // namespace IntactMiddleware

#endif // IntactMiddleware_IntactComponent_h_IsIncluded
