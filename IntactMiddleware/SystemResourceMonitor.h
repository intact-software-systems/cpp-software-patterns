#ifndef IntactMiddleware_SystemResourceMonitor_h_IsIncluded
#define IntactMiddleware_SystemResourceMonitor_h_IsIncluded

#include"IntactMiddleware/IncludeExtLibs.h"
namespace IntactMiddleware
{
/* -------------------------------------------------------
	class SystemResourceMonitor
-------------------------------------------------------*/
class SystemResourceMonitor : public Thread
{
private:
	SystemResourceMonitor(InterfaceHandle handle, bool autoStart = true);
	virtual ~SystemResourceMonitor(void) {}

private:
	virtual void run();

public:
	static SystemResourceMonitor* GetOrCreate(InterfaceHandle *handle = NULL);

public:
	inline void Shutdown()					{ MutexLocker lock(&mutex_); runThread_ = false; }

protected:
	void init(InterfaceHandle managerHandle);

private:
	mutable Mutex			mutex_;
	WaitCondition			waitCondition_;
	bool					runThread_;

private:
	static SystemResourceMonitor *systemResourceMonitor_;
};

} // namespace IntactMiddleware

#endif // IntactMiddleware_SystemResourceMonitor_h_IsIncluded



