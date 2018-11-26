#ifndef IntactMiddleware_MembershipManagerConnection_h_IsIncluded
#define IntactMiddleware_MembershipManagerConnection_h_IsIncluded

#include"IntactMiddleware/IncludeExtLibs.h"
#include"IntactMiddleware/MembershipManagerBase.h"
#include"IntactMiddleware/Export.h"
namespace IntactMiddleware
{
// --------------------------------------------------------
//			struct MembershipManagerConnectionConfig
// --------------------------------------------------------
struct MembershipManagerConnectionConfig
{
	static int membershipManagerPort;
};

// --------------------------------------------------------
//			class MembershipManagerConnection
// --------------------------------------------------------
/**
* Class MembershipManagerConnection logs in the component.
*/
class DLL_STATE MembershipManagerConnection : protected Thread
{
private:
	MembershipManagerConnection(const HostInformation &info, InterfaceHandle handle, bool autorStart = true);
	virtual ~MembershipManagerConnection();
	
public:
	static MembershipManagerConnection *GetOrCreate(InterfaceHandle *membershipHandle = NULL);

public:
	bool Login();
	void Logout();
	
	HostInformation	GetHostInformation() const;

	bool IsLoggedIn();
	bool StopConnection(bool wait = true);
	bool WaitConnected(bool wait = true);
	void WaitForTermination() 	{ Thread::wait(); }

private:
	virtual void run();
	
	bool login();
	void logout();

private:
	inline bool runThread()			const { MutexLocker lock(&classMutex_); return runThread_; }
	inline bool automaticLogin()	const { MutexLocker lock(&classMutex_); return automaticLogin_; }

	inline void lock()			{ classMutex_.lock(); }
	inline void unlock()		{ classMutex_.unlock(); }
	inline void wakeAll()		{ classCondition_.wakeAll(); }

	inline bool lockedAndUnlockedWait(int64 ms)
	{
		lock();
		bool wokenUp = classCondition_.wait(&classMutex_, ms);
		unlock();
		return wokenUp;
	}

private:
	MembershipManagerClient *membershipManagerProxy_;
	HostInformation			myHostInformation_;
	InterfaceHandle			membershipManagerHandle_;

	bool					isLoggedIn_;
	bool 					automaticLogin_;
	bool 					runThread_;

private:
	mutable Mutex			classMutex_;
	WaitCondition			classCondition_;

private:
	static MembershipManagerConnection *membershipManagerConnection_;
};

} // namespace IntactMiddleware

#endif


