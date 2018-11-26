#ifndef IntactMiddleware_IntactSettings_SettingsManagerBase_h_IsIncluded
#define IntactMiddleware_IntactSettings_SettingsManagerBase_h_IsIncluded

#include<map>
#ifdef USE_GCC
#include<memory>
#endif

#include<memory>

#include"IntactMiddleware/IntactSettings/IncludeExtLibs.h"
#include"IntactMiddleware/IntactSettings/HostSettings.h"
#include"IntactMiddleware/Export.h"
namespace IntactMiddleware
{

// -------------------------------------------------------
//				SettingsManager Interfaces
// -------------------------------------------------------
/**
* RMI functions to communicate with the SettingsManager.
* The interface is used actively by each ComponentFactory.
*/
class DLL_STATE SettingsManagerInterface : public RmiInterface
{
public:
	SettingsManagerInterface() {}
	virtual ~SettingsManagerInterface() {}

	enum MethodID {
		Method_RegisterSettingsListener	= 41,
		Method_AliveChecker = 42,
		Method_HostSettings = 43,
		Method_ShutDown = 44,
		Method_AllComponentSettings = 45,
		Method_UpdateSettings = 46,
		Method_LoadSettings = 47,
		Method_UpdateComponentSettings = 48
	};
	
	enum InterfaceId
	{
		SettingsManagerInterfaceId = 101
	};
	
	virtual int GetInterfaceId() const { return SettingsManagerInterface::SettingsManagerInterfaceId; }


	/**
	* Registers the component given by HostInformation in the SettingsManager.
	*
	* @param info		HostInformation includes the component descriptions.
	* @return			Updated HostSettings.
	*/
	virtual HostSettings RegisterSettingsListener(HostInformation info) = 0;

	/**
	* Called by all Component Factories to inform the SettingsManager that it is connected/disconnected.
	*
	* @param info			The Component Factory's HostSettings.
	* @return				Updated HostInformation.
	*/
	virtual HostSettings AliveChecker(HostInformation info) = 0;
	
	/**
	* Retrieve HostInformation about a component identified by componentName.
	*
	* @param componentName	The name of the component to retrieve HostInformation.
	* @return				HostInformation about the componentName.
	*/
    virtual HostSettings GetHostSettings(std::string componentName) = 0;

	/**
	* Shuts down the System Manager.
	*/
	virtual void ShutDown() = 0;
	
	/**
    * Returns all component information in the SettingsManager in a map (string, HostInformation).
	*
	* @return				All the component information registered in the SettingsManager.
	*/
    virtual std::map<std::string, HostSettings> GetAllComponentSettings() = 0;

	/**
	* Push component settings updates to the SettingsManager.
	*
	* Format of xml-code is: 
	* <SettingsManager>  
	*
	*   <Component name="a.unique.component.name"/>
	*		// Component settings here. Any xml-code
	*	</Component>
	*
	*   <Component name="another.unique.component.name"/>
	*		// Component settings here
	*	</Component>
	*
	* </SettingsManager>
	*
	* @param settingsStream		xml-code of settings
	*/ 
    virtual void UpdateSettings(const std::string &settingsStream) = 0;

	/**
	* Push component settings updates to the SettingsManager. The input arguments represens an xml namespace.
	*
	* Example:
	*
	* <Component name="componentName"/>
	*	<namespace attribute1="value1" attribute2="value2" attribute3="value3"/>
	* </Component> 
	*
	* @param componentName		name of component to update settings for.
	* @param namespace			xml-namespace
	* @param mapAttributeValue	map<string, string> of (attribute,value) pairs, associated to namespace.
	*/
    virtual void UpdateComponentSettings(const std::string &componentName, const std::string &nameSpace, const std::map<std::string, std::string> &mapAttributeValue) = 0;

	/**
	* Currently unused. TODO: Remove!
	*/
    virtual void LoadSettings(std::string settingsStream) = 0;
};

// -------------------------------------------------------
//				Proxy Interface
// -------------------------------------------------------
/**
* TODO class-description
*/
class SettingsManagerProxy : virtual public RmiClient, virtual public SettingsManagerInterface
{
public:
	RMICLIENT_COMMON(SettingsManagerProxy);

	typedef std::shared_ptr<SettingsManagerProxy> Ptr;

	virtual HostSettings RegisterSettingsListener(HostInformation info)
	{
		BeginMarshal(SettingsManagerInterface::Method_RegisterSettingsListener);
		info.Write(GetSocketWriter());
		EndMarshal();

        BeginUnmarshal();
		HostSettings settings;
		settings.Read(GetSocketReader());
        EndUnmarshal();

		return settings;
	}
	
    virtual HostSettings GetHostSettings(std::string componentName)
	{
		BeginMarshal(SettingsManagerInterface::Method_HostSettings);
		GetSocketWriter()->WriteString(componentName);
		EndMarshal();

		BeginUnmarshal();
		HostSettings wrap;
		wrap.Read(GetSocketReader());
		EndUnmarshal();

		return wrap;
	}

	virtual HostSettings AliveChecker(HostInformation info)
	{
		BeginMarshal(SettingsManagerInterface::Method_AliveChecker);
		info.Write(GetSocketWriter());
		EndMarshal();

		BeginUnmarshal();
		HostSettings settings;
		settings.Read(GetSocketReader());
		EndUnmarshal();
		
		return settings;
	}

	virtual void ShutDown()
	{
		BeginMarshal(SettingsManagerInterface::Method_ShutDown);
		EndMarshal();
	}
	
    virtual std::map<std::string, HostSettings> GetAllComponentSettings()
	{
		BeginMarshal(SettingsManagerInterface::Method_AllComponentSettings);
		EndMarshal();

		BeginUnmarshal();

        std::map<std::string, HostSettings> mapComponentHostSettings;
		int sizeofmap = GetSocketReader()->ReadInt32();
		for(int i = 0; i < sizeofmap; i++)
		{
			HostSettings info;
			info.Read(GetSocketReader());

            mapComponentHostSettings.insert(std::pair<std::string, HostSettings>(info.GetComponentName(), info));
		}		

		EndUnmarshal();

		return mapComponentHostSettings;
	}

    virtual void UpdateSettings(const std::string &streamSettings)
	{
		BeginMarshal(SettingsManagerInterface::Method_UpdateSettings);
		GetSocketWriter()->WriteString(streamSettings);
		EndMarshal();
	}
	
    virtual void LoadSettings(std::string streamSettings)
	{
		BeginMarshal(SettingsManagerInterface::Method_LoadSettings);
		GetSocketWriter()->WriteString(streamSettings);
		EndMarshal();
	}

    virtual void UpdateComponentSettings(const std::string &componentName, const std::string &nameSpace, const std::map<std::string, std::string> &mapAttributeValue)
	{
		BeginMarshal(SettingsManagerInterface::Method_UpdateComponentSettings);
		
		GetSocketWriter()->WriteString(componentName);
		GetSocketWriter()->WriteString(nameSpace);
		
		int sz = mapAttributeValue.size();
		GetSocketWriter()->WriteInt32(sz);
		
        for(std::map<std::string, std::string>::const_iterator it = mapAttributeValue.begin(), it_end = mapAttributeValue.end(); it != it_end; ++it)
		{
			GetSocketWriter()->WriteString(it->first);		// attribute
			GetSocketWriter()->WriteString(it->second);		// value
		}
		EndMarshal();
	}
};

// -------------------------------------------------------
//				Stub Interface
// -------------------------------------------------------
/**
* TODO class-description
*/
class SettingsManagerStub : public RmiServerListener
{
private:
	SettingsManagerInterface *settingsManagerInterface;

public:
	SettingsManagerStub(const InterfaceHandle &handle, SettingsManagerInterface* serverInterface, bool autoStart)
		: RmiServerListener(handle, autoStart)
		, settingsManagerInterface(serverInterface)
	{ }
	virtual ~SettingsManagerStub() {}

	virtual void RMInvocation(int methodId, RmiBase *stubClient)
	{
		switch(methodId)
		{
			case SettingsManagerInterface::Method_RegisterSettingsListener:
			{
				HostInformation info;
				info.Read(stubClient->GetSocketReader());
				stubClient->EndUnmarshal();
				
				HostSettings settings = settingsManagerInterface->RegisterSettingsListener(info);

				stubClient->BeginMarshal(RmiInterface::ReturnValue);
				settings.Write(stubClient->GetSocketWriter());
				stubClient->EndMarshal();
				break;
			}
			case SettingsManagerInterface::Method_HostSettings:
			{	
                std::string name = stubClient->GetSocketReader()->ReadString();
				stubClient->EndUnmarshal();

				HostSettings info = settingsManagerInterface->GetHostSettings(name);

				stubClient->BeginMarshal(RmiInterface::ReturnValue);
				info.Write(stubClient->GetSocketWriter());
				stubClient->EndMarshal();
				break;
			}
			case SettingsManagerInterface::Method_AliveChecker:
			{
				HostInformation info;

				info.Read(stubClient->GetSocketReader());
				stubClient->EndUnmarshal();

				HostSettings settings = settingsManagerInterface->AliveChecker(info);
				
				stubClient->BeginMarshal(RmiInterface::ReturnValue);
				settings.Write(stubClient->GetSocketWriter());
				stubClient->EndMarshal();
				break;
			}
			case SettingsManagerInterface::Method_ShutDown:
			{
				stubClient->EndUnmarshal();
				settingsManagerInterface->ShutDown();
				break;
			}
			case SettingsManagerInterface::Method_AllComponentSettings:
			{
				stubClient->EndUnmarshal();

                std::map<std::string, HostSettings> mapNameHostSettings = settingsManagerInterface->GetAllComponentSettings();

				stubClient->BeginMarshal(RmiInterface::ReturnValue);

				SocketWriter *writer = stubClient->GetSocketWriter();
				writer->WriteInt32(mapNameHostSettings.size());
				
                for(std::map<std::string, HostSettings>::iterator it = mapNameHostSettings.begin(), it_end = mapNameHostSettings.end(); it != it_end; ++it)
				{
					HostSettings &info = it->second;
					info.Write(writer);
				}				
				stubClient->EndMarshal();				
				break;
			}
			case SettingsManagerInterface::Method_LoadSettings:
			{
                std::string streamSettings = stubClient->GetSocketReader()->ReadString();
				stubClient->EndUnmarshal();

				settingsManagerInterface->LoadSettings(streamSettings);				
				break;
			}
			case SettingsManagerInterface::Method_UpdateSettings:
			{
                std::string streamSettings = stubClient->GetSocketReader()->ReadString();
				stubClient->EndUnmarshal();

				settingsManagerInterface->UpdateSettings(streamSettings);			
				break;
			}
			case SettingsManagerInterface::Method_UpdateComponentSettings:
			{
                std::string componentName = stubClient->GetSocketReader()->ReadString();
                std::string nameSpace = stubClient->GetSocketReader()->ReadString();

				int sz = stubClient->GetSocketReader()->ReadInt32();
                std::map<std::string, std::string> mapAttributeValue;
				for(int i = 0; i < sz; i++)
				{
                    std::string attr = stubClient->GetSocketReader()->ReadString();
                    std::string val = stubClient->GetSocketReader()->ReadString();
                    mapAttributeValue.insert(std::pair<std::string, std::string>(attr, val));
				}
				stubClient->EndUnmarshal();

				settingsManagerInterface->UpdateComponentSettings(componentName, nameSpace, mapAttributeValue);
				break;
			}
			default:
				RmiServerListener::RMInvocation(methodId, stubClient);
				break;
		}
	}
};

// -------------------------------------------------------
//	SettingsManager connection used by ComponentFactory
// -------------------------------------------------------
/**
* Used by each Component to register and maintain a connection with the SettingsManager.
*/
class DLL_STATE SettingsManagerConnection : public Thread
{
private:
	HostInformation					myHostInformation;
	HostSettings					myHostSettings;
	InterfaceHandle					settingsManagerHandle;
	SettingsManagerProxy::Ptr		settingsManagerProxy;
	Mutex							mutex;
	bool							isRegistered;

	static SettingsManagerConnection *settingsManagerConnection;

	//SettingsManagerProxy::Ptr		settingsManagerProxy;

private:
	SettingsManagerConnection(HostInformation &info, InterfaceHandle managerHandle)
		: myHostInformation(info)
		, settingsManagerHandle(managerHandle)
		, isRegistered(false)
	{
		settingsManagerHandle.SetHostName(NetworkFunctions::getIPAddress(settingsManagerHandle.GetHostName()));

		if(myHostInformation.GetHostIP().empty())
			myHostInformation.SetHostIP(NetworkFunctions::getIPAddress(myHostInformation.GetHostName()));

		RegisterSettingsListener();

		start();
	}
	~SettingsManagerConnection() 
	{
		myHostInformation.SetOnlineStatus(HostInformation::OFFLINE);
		if(settingsManagerProxy.get() != NULL)
		{
			settingsManagerProxy->RegisterSettingsListener(myHostInformation);
			cerr << "Disconnected SettingsManager (" << settingsManagerHandle << ")" << endl;
		}
	}

private:
	/**
	* Main loop that maintains the connection to the SettingsManager.
	*/
	virtual void run()
	{
		while(true)
		{
			try
			{
				bool ret = RegisterSettingsListener();
				if(ret == false)
				{
					throw("");
				}
				
				while(true)
				{
					msleep(1000);

					mutex.lock();
					bool ret = getOrCreateProxy(settingsManagerHandle);
					if(settingsManagerProxy.get() == NULL)
					{
						mutex.unlock();
                        throw std::string("");
					}
					mutex.unlock();
				}
			}
			catch(IOException io)
			{
				cout << myHostInformation.GetComponentName() << ": SettingsManager connect exception: " << io.what() << ". Attempting Reconnect." << endl;
				isRegistered = false;
				msleep(1000);
			}
			catch(...)
			{
				isRegistered = false;
				msleep(1000);
			}
		}
	}

	bool getOrCreateProxy(InterfaceHandle settingsManagerHandle)
	{
		bool createNewProxy = false;
		if(settingsManagerProxy.get() != NULL) if(settingsManagerProxy->IsConnected() == false) createNewProxy = true;

		if(settingsManagerProxy.get() == NULL || createNewProxy)
		{
			try
			{
				//if(settingsManagerProxy != NULL) delete settingsManagerProxy;				
				SettingsManagerProxy *proxy = SettingsManagerProxy::GetRmiClient(settingsManagerHandle);
				settingsManagerProxy = SettingsManagerProxy::Ptr(proxy);
				cout << "getOrCreateProxy(): Established connection to SettingsManager (" << settingsManagerHandle << ")" << endl;
			}
			catch(IOException io)
			{
				settingsManagerProxy = SettingsManagerProxy::Ptr();
				//cout << "getOrCreateProxy(): WARNING! Unable to contact SettingsManager (" << settingsManagerHandle << ")" << endl;
				return false;
			}
			catch(...)
			{
				settingsManagerProxy = SettingsManagerProxy::Ptr();
				//cout << "getOrCreateProxy(): WARNING! Unable to contact SettingsManager (" << settingsManagerHandle << ")" << endl;
				return false;
			}
		}
		return true;
		//return settingsManagerProxy;
	}

public:
	static bool SetupSettingsManagerConnections(InterfaceHandle settingsManagerHandle, HostInformation info);
	static SettingsManagerConnection* GetSettingsManagerConnection();

public:
	SettingsManagerProxy::Ptr GetOrCreateProxy()
	{
		MutexLocker lock(&mutex);
		try
		{
			bool ret = getOrCreateProxy(settingsManagerHandle);
			if(settingsManagerProxy.get() == NULL)
			{
				throw("");
			}
		}
		catch(...)
		{
			//cout << "getOrCreateProxy(): WARNING! Unable to contact SettingsManager (" << settingsManagerHandle << ")" << endl;
			//throw;
		}
		return settingsManagerProxy;
	}

	bool RegisterSettingsListener()
	{
		MutexLocker lock(&mutex);

		if(isRegistered == true) return isRegistered;

		try
		{
			bool ret = getOrCreateProxy(settingsManagerHandle);
			if(settingsManagerProxy.get() == NULL)
			{
				throw "Connnection Error!";
			}

			myHostInformation.SetOnlineStatus(HostInformation::ONLINE);
			myHostSettings = settingsManagerProxy->RegisterSettingsListener(myHostInformation);
			isRegistered = true;
		}
		catch(...)
		{
			cout << "RegisterSettingsListener(): WARNING! Unable to contact SettingsManager (" << settingsManagerHandle << ")" << endl;
			isRegistered = false;
		}

		return isRegistered;
	}
};

}

#endif // SETTINGS_MANAGER_BASE_H
