#ifndef IntactMiddleware_IntactSettings_SettingsListener_h_IsIncluded
#define IntactMiddleware_IntactSettings_SettingsListener_h_IsIncluded

#include"IntactMiddleware/IncludeExtLibs.h"
#include"IntactMiddleware/IntactSettings/HostSettings.h"
#include"IntactMiddleware/Export.h"
namespace IntactMiddleware 
{

class SettingsListener;

class DLL_STATE SettingsListenerInterface : public RmiInterface
{
public:
	SettingsListenerInterface()
	{}
	~SettingsListenerInterface()
	{}

	enum MethodID 
	{
		Method_SetHostSettings = 70
	};
	
	enum InterfaceId
	{
		SettingsListenerInterfaceId = 102
	};
	
	virtual int GetInterfaceId() const { return SettingsListenerInterface::SettingsListenerInterfaceId; }

	virtual void SetHostSettings(HostSettings &info) = 0;
};

// -------------------------------------------------------
//				Proxy Interface
// -------------------------------------------------------
/**
* TODO class-description
*/
class DLL_STATE SettingsListenerProxy : virtual public RmiClient, virtual public SettingsListenerInterface
{
public:
	RMICLIENT_COMMON(SettingsListenerProxy);

	virtual void SetHostSettings(HostSettings &info)
	{
		BeginMarshal(SettingsListenerInterface::Method_SetHostSettings);
		info.Write(GetSocketWriter());
		EndMarshal();
	}
};


class DLL_STATE SettingsListenerStub : public RmiServerListener
{
private:
	SettingsListenerInterface *settingsListenerInterface;

private:
	SettingsListenerStub(const InterfaceHandle &handle, SettingsListenerInterface* serverInterface, bool autoStart)
		: RmiServerListener(handle, autoStart)
		, settingsListenerInterface(serverInterface)
	{ }
	virtual ~SettingsListenerStub() {}

	friend class SettingsListener;

	virtual void RMInvocation(int methodId, RmiBase* stubClient)
	{
		switch(methodId)
		{
			case SettingsListenerInterface::Method_SetHostSettings:
			{
				HostSettings hostSettings;
				hostSettings.Read(stubClient->GetSocketReader());
				stubClient->EndUnmarshal();

				settingsListenerInterface->SetHostSettings(hostSettings);
				break;
			}
			default:
				RmiServerListener::RMInvocation(methodId, stubClient);
				break;
		}
	}
};

class DLL_STATE SettingsListener : public SettingsListenerInterface
{
private:
	HostSettings		hostSettings;
	WaitCondition		waitCondition;
	Mutex				mutex;
	HostInformation		myHostInformation;

	static SettingsListener* settingsListener;

private:
    SettingsListener(const std::string &name)
	{ }
	virtual ~SettingsListener() 
	{
	}
	SettingsListener(const SettingsListener&);
	SettingsListener& operator=(const SettingsListener&);

	friend class SettingsListenerStub;

	virtual void SetHostSettings(HostSettings &settings)
	{
		MutexLocker lock(&mutex);

		ASSERT(settings.IsValid());
		ASSERT(settings.GetConfSettings().empty() == false);
		if(settings.GetConfSettings() != hostSettings.GetConfSettings()) // settings changed?
		{
			//cerr << "SettingsListener::SetHostSettings(settings): " << endl;
			//cerr << settings.GetConfSettings() << endl;
			
			hostSettings = settings;
			waitCondition.wakeAll(); 
		}
	}

	inline void SetHostInformation(HostInformation hostInfo) { myHostInformation = hostInfo; }

public:
	static SettingsListener*	GetOrCreateSettingsListener();

public:
	HostSettings GetHostSettings(bool waitForSettings = true)
	{
		mutex.lock();

		if(waitForSettings == true)
		{
			while(hostSettings.IsValid() == false)
				waitCondition.wait(&mutex); 
		}

		HostSettings settings = hostSettings;

		mutex.unlock();
		return settings;
	}

    std::string GetConfigurationSettings(bool waitForSettings = true)
	{
		HostSettings settings = this->GetHostSettings(waitForSettings);	// get newest settings
		return settings.GetConfSettings();
	}

    HostSettings WaitForNewSettings(const std::string &configSettings)
	{
		mutex.lock();
		
		if(configSettings == hostSettings.GetConfSettings()) // settings changed?
			waitCondition.wait(&mutex);
		
		HostSettings newSettings = hostSettings;

		mutex.unlock();
		return newSettings;
	}

    bool HasSettingsChanged(const std::string &configSettings)
	{
		MutexLocker lock(&mutex);

		bool settingsChanged = false;
		if(configSettings != hostSettings.GetConfSettings()) // settings changed?
			settingsChanged = true;
		
		return settingsChanged;
	}

	HostInformation			GetHostInformation()	{ return myHostInformation; }
	virtual InterfaceHandle GetInterfaceHandle()	{ return InterfaceHandle(myHostInformation.GetPortNumber(), myHostInformation.GetHostIP()); }
};

} // end namespace IntactMiddleware 

#endif // 

