#ifndef SYSTEM_MANAGER_BASE_H
#define SYSTEM_MANAGER_BASE_H

#include<map>

#include"IntactMiddleware/IncludeExtLibs.h"
#include"IntactMiddleware/Export.h"

namespace IntactMiddleware
{

// -------------------------------------------------------
//				SystemManager Interfaces
// -------------------------------------------------------
/**
* RMI functions to communicate with the SystemManager.
* The interface is used actively by each ComponentFactory.
*/
class DLL_STATE SystemManagerInterface : public RmiInterface
{
public:
	SystemManagerInterface() {}
    ~SystemManagerInterface() {}

	enum MethodID {
		Method_HostRegistration	= 11,
		Method_AliveChecker = 12,

		Method_HostInformation = 13,
		Method_KillAll = 14,
		Method_KillComponent = 15,
		Method_StartComponents = 16,
		Method_ShutDown = 17,
		Method_AllComponentInformation = 18,
		Method_ParseXmlFile = 20,
		Method_ClearAndKillAll = 21,
		Method_MapResourceInformation = 22,
		Method_GetMiddlewareExceptions = 23,
		Method_LookupServerHandle = 24,
		Method_SetResourceInformation = 25,
		Method_RestartDefault = 26
	};

	enum InterfaceId
	{
		SystemManagerInterfaceId = 8
	};

	virtual int GetInterfaceId() const { return SystemManagerInterface::SystemManagerInterfaceId; }
	
	/**
	* Returns the InterfaceHandle for the LookupServer.
	*/
	virtual InterfaceHandle	GetLookupServerHandle() = 0;

	/**
	* Registers the component given by HostInformation in the SystemManager.
	*
	* @param info		HostInformation includes the component descriptions.
	* @return			Updated HostInformation.
	*/
	virtual HostInformation HostRegistration(HostInformation info) = 0;

	/**
	* Called by all Component Factories to inform the SystemManager that it is connected/disconnected.
	*
	* @param info			The Component Factory's HostInformation.
	* @return				Updated HostInformation.
	*/
	virtual HostInformation	AliveChecker(HostInformation info) = 0;
	
	/**
	* Retrieve HostInformation about a component identified by componentName.
	*
	* @param componentName	The name of the component to retrieve HostInformation.
	* @return				HostInformation about the componentName.
	*/
    virtual HostInformation GetHostInformation(std::string componentName) = 0;
	
	/**
	* Kills all components registered in the SystemManager.
	* @param returnMsg		Error messages are stored.
	*/
    virtual void KillAll(std::string &returnMsg) = 0;
	
	/**
	* Kills a component identified by componentName.
	*
	* @param componentName	The component's name.
	* @param returnMsg		Error messages are stored.
	*
	* @return				1 if a component was killed, 0 if not.
	*/
    virtual int KillComponent(std::string componentName, std::string &returnMsg) = 0;
	
	/**
	* Starts the components that are given in the string xmlCommands. xmlCommands contains xml-code.
	*
	* @param xmlCommands			Contains the xml-code.
	* @param mapErrorMessages		Error messages from system manager and factories.
	*
	* @return				0 if success, > 0 if errors were found.
	*/	
    virtual int StartComponents(std::string xmlCommands, std::map<std::string, std::string> &mapErrorMessages) = 0;

	/**
	* Shuts down the System Manager.
	*/
	virtual void ShutDown() = 0;
	
	/**
	* Returns all component information in the SystemManager in a map (string, HostInformation).
	*
	* @return				All the component information registered in the SystemManager.
	*/
    virtual std::map<std::string, HostInformation> GetAllComponentInformation() = 0;
	
	/**
	* Returns middleware exceptions that have been multicast by components and received by SystemManager.
	*
	* @param lastTimeStamp		Timestamp of last received exception message, can be empty if no previously received exception message.
	* @return					map containing middleware exceptions received by SystemManager, organized by timestamp.
	*/
    virtual std::map<long long, ExceptionMessage> GetMiddlewareExceptions(long long &lastTimeStamp) = 0;

	/**
	* Returns component information retrieved from xmlFile. The returned string can be "decoded"
	* through HostInformationSpace::ParseHostInformationFromDisc(string, vector<HostInformation>&).
	*
	* @param xmlCommands	Xml-code that is to be parsed.		
	* @param returnMsg		Error messages are stored.
	*
	* @return				map (string, HostInformation> containing all component information.
	*/
    virtual std::map<std::string, HostInformation> ParseXmlFile(std::string xmlCommands, std::string &returnMsg) = 0;
	
	/**
	* Clears database and kills all components registered in the SystemManager.
	*
	* @param returnMsg		Error messages are stored.
	* @return				Returns 1 on success, 0 if otherwise.
	*/
    virtual int ClearAndKillAll(std::string &returnMsg) = 0;

	/**
	* Returns a map (processId, HostResources) of all component's resource information in the SystemManager.
	*
	* @return		Returns all the component resources registered in the SystemManager.
	*/
    virtual std::map<std::string, HostResources> GetMapResourceInformation() = 0;

	/**
	* Sends the current HostResources image for that ComponentFactory to the SystemManager.
	*
	* @param HostInformation		Factory's HostInformation
	* @param mapHostResources		The current HostResources images for that ComponentFactory.
	*/
    virtual void SetResourceInformation(HostInformation &factoryInfo, std::map<int, HostResources> &mapHostResources) = 0;

	/**
	* Kills, clears and starts all components from the initial configuration file in SystemManager.
	*
	* @param returnMsg		Error messages are stored
	* @return				Returns 0 on success, otherwise, number of errors detected.
	*/
    virtual int RestartDefault(std::string &returnMsg) = 0;
};

// -------------------------------------------------------
//				Proxy Interface
// -------------------------------------------------------
/**
* TODO class-description
*/
class DLL_STATE SystemManagerProxy : virtual public RmiClient, virtual public SystemManagerInterface
{
public:
	RMICLIENT_COMMON(SystemManagerProxy);
	
	virtual InterfaceHandle	GetLookupServerHandle()
	{
		BeginMarshal(SystemManagerInterface::Method_LookupServerHandle);
		EndMarshal();

        BeginUnmarshal();
        InterfaceHandle handle;
		handle.Read(GetSocketReader());
        EndUnmarshal();
        return handle;
	}

	virtual HostInformation HostRegistration(HostInformation info)
	{
		BeginMarshal(SystemManagerInterface::Method_HostRegistration);
		info.Write(GetSocketWriter());
		EndMarshal();

        BeginUnmarshal();
		info.Read(GetSocketReader());
        EndUnmarshal();

		return info;
	}
	
    virtual HostInformation GetHostInformation(std::string componentName)
	{
		BeginMarshal(SystemManagerInterface::Method_HostInformation);
        GetSocketWriter()->WriteString(componentName);
		EndMarshal();

		BeginUnmarshal();
		HostInformation wrap;
		wrap.Read(GetSocketReader());
		EndUnmarshal();

		return wrap;
	}

	virtual HostInformation AliveChecker(HostInformation info)
	{
		BeginMarshal(SystemManagerInterface::Method_AliveChecker);
		info.Write(GetSocketWriter());
		EndMarshal();

		BeginUnmarshal();
		info.Read(GetSocketReader());
		EndUnmarshal();
		
		return info;
	}

    virtual void KillAll(std::string &returnMsg)
	{
		BeginMarshal(SystemManagerInterface::Method_KillAll);
		GetSocketWriter()->WriteString(returnMsg);
		EndMarshal();
		
		BeginUnmarshal();
		returnMsg = GetSocketReader()->ReadString();
		EndUnmarshal();
	}

    virtual int KillComponent(std::string componentName, std::string &returnMsg)
	{
		BeginMarshal(SystemManagerInterface::Method_KillComponent);
		GetSocketWriter()->WriteString(componentName);
		GetSocketWriter()->WriteString(returnMsg);
		EndMarshal();

		BeginUnmarshal();
		returnMsg = GetSocketReader()->ReadString();
		int ret = GetSocketReader()->ReadInt32();
		EndUnmarshal();

		return ret;
	}

    virtual int StartComponents(std::string xmlFile, std::map<std::string, std::string> &mapErrorMessages)
	{
		BeginMarshal(SystemManagerInterface::Method_StartComponents);
		GetSocketWriter()->WriteString(xmlFile);
		EndMarshal();

		mapErrorMessages.clear();

		BeginUnmarshal();
		int sz = GetSocketReader()->ReadInt32();
		for(int i = 0; i < sz; i++)
		{
            std::string id = GetSocketReader()->ReadString();
            std::string msg = GetSocketReader()->ReadString();
            mapErrorMessages.insert(std::pair<std::string, std::string>(id, msg));
		}
		int ret = GetSocketReader()->ReadInt32();
		EndUnmarshal();

		return ret;
	}

	virtual void ShutDown()
	{
		BeginMarshal(SystemManagerInterface::Method_ShutDown);
		EndMarshal();
	}
	
    virtual std::map<std::string, HostInformation> GetAllComponentInformation()
	{
		BeginMarshal(SystemManagerInterface::Method_AllComponentInformation);
		EndMarshal();

		BeginUnmarshal();

        std::map<std::string, HostInformation> mapComponentHostInformation;
		int sizeofmap = GetSocketReader()->ReadInt32();
		for(int i = 0; i < sizeofmap; i++)
		{
			HostInformation info;
			info.Read(GetSocketReader());

            mapComponentHostInformation.insert(std::pair<std::string, HostInformation>(info.GetComponentName(), info));
		}		

		EndUnmarshal();

		return mapComponentHostInformation;
	}

    virtual std::map<long long, ExceptionMessage> GetMiddlewareExceptions(long long &lastTimeStamp)
	{
		BeginMarshal(SystemManagerInterface::Method_GetMiddlewareExceptions);
		GetSocketWriter()->WriteInt64(lastTimeStamp);
		EndMarshal();

		BeginUnmarshal();
		
        std::map<long long, ExceptionMessage> mapTimestampExceptionMessage;
		lastTimeStamp = GetSocketReader()->ReadInt64();
		int sizeofmap = GetSocketReader()->ReadInt32();
		for(int i = 0; i < sizeofmap; i++)
		{
			long long timeStamp = GetSocketReader()->ReadInt64();
			ExceptionMessage ex;
			ex.Read(GetSocketReader());

            mapTimestampExceptionMessage.insert(std::pair<long long, ExceptionMessage>(timeStamp, ex));
		}		
		EndUnmarshal();

		return mapTimestampExceptionMessage;
	}

    virtual	std::map<std::string, HostResources> GetMapResourceInformation()
	{
		BeginMarshal(SystemManagerInterface::Method_MapResourceInformation);
		EndMarshal();

		BeginUnmarshal();
		
        std::map<std::string, HostResources> mapComponentHostResources;
		//cout << "GetMapComponentResources: " << endl;
		int sizeofmap = GetSocketReader()->ReadInt32();
		for(int i = 0; i < sizeofmap; i++)
		{
			HostResources res;
			res.Read(GetSocketReader());
			//cout << res << endl;
            mapComponentHostResources.insert(std::pair<std::string, HostResources>(res.GetComponentName(), res));
		}		
		EndUnmarshal();

		return mapComponentHostResources;
	}

    virtual void SetResourceInformation(HostInformation &factoryInfo, std::map<int, HostResources> &mapHostResources)
	{
		if(mapHostResources.empty() == true) return;

		BeginMarshal(SystemManagerInterface::Method_SetResourceInformation);
		SocketWriter *writer = GetSocketWriter();

		factoryInfo.Write(writer);		
		writer->WriteInt32(mapHostResources.size());
        for(std::map<int, HostResources>::iterator it = mapHostResources.begin(), it_end = mapHostResources.end(); it != it_end; ++it)
		{
			HostResources &res = it->second;
			res.Write(writer);
		}
		EndMarshal();
	}

    virtual std::map<string, HostInformation> ParseXmlFile(std::string xmlFile, std::string &returnMsg)
	{
		BeginMarshal(SystemManagerInterface::Method_ParseXmlFile);
		GetSocketWriter()->WriteString(xmlFile);
		GetSocketWriter()->WriteString(returnMsg);
		EndMarshal();

		BeginUnmarshal();

		returnMsg = GetSocketReader()->ReadString();

        std::map<std::string, HostInformation> mapComponentHostInformation;
		int sizeofmap = GetSocketReader()->ReadInt32();
		for(int i = 0; i < sizeofmap; i++)
		{
			HostInformation info;
			info.Read(GetSocketReader());

            mapComponentHostInformation.insert(std::pair<std::string, HostInformation>(info.GetComponentName(), info));
		}		

		EndUnmarshal();

		return mapComponentHostInformation;
	}

    virtual int ClearAndKillAll(std::string &returnMsg)
	{
		BeginMarshal(SystemManagerInterface::Method_ClearAndKillAll);
		GetSocketWriter()->WriteString(returnMsg);
		EndMarshal();

		BeginUnmarshal();
		returnMsg = GetSocketReader()->ReadString();
		int ret = GetSocketReader()->ReadInt32();
		EndUnmarshal();

		return ret;
	}

    virtual int RestartDefault(std::string &errorMsg)
	{
		BeginMarshal(SystemManagerInterface::Method_RestartDefault);
		GetSocketWriter()->WriteString(errorMsg);
		EndMarshal();

		BeginUnmarshal();
		errorMsg = GetSocketReader()->ReadString();
		int ret = GetSocketReader()->ReadInt32();
		EndUnmarshal();
		
		return ret;
	}
};

// -------------------------------------------------------
//				Stub Interface
// -------------------------------------------------------
/**
* TODO class-description
*/
class DLL_STATE SystemManagerStub : public RmiServerListener
{
public:
	SystemManagerStub(const InterfaceHandle &handle, SystemManagerInterface* serverInterface, bool autoStart)
		: RmiServerListener(handle, autoStart)
		, systemManagerInterface(serverInterface)
	{ }
	virtual ~SystemManagerStub() {}

	//static SystemManagerStub* GetStub(SystemManagerInterface* systemManagerInterface, bool autoStart) 
    //{
    //    Stub* existingStub = FindStub(systemManagerInterface);
    //    return existingStub ? dynamic_cast<SystemManagerStub*>(existingStub) : new SystemManagerStub(AllocatePort(), systemManagerInterface, autoStart);
    //}

	//virtual string& GetInterfaceID() const
	//{
//		return SystemManagerInterface::InterfaceID;
//	}

	virtual void RMInvocation(int methodId, RmiBase* stubClient)
	{
		switch(methodId)
		{
			case SystemManagerInterface::Method_LookupServerHandle:
			{
				stubClient->EndUnmarshal();
				InterfaceHandle handle = systemManagerInterface->GetLookupServerHandle();

				stubClient->BeginMarshal(SystemManagerInterface::Method_LookupServerHandle);
				handle.Write(stubClient->GetSocketWriter());				
				stubClient->EndMarshal();
				break;
			}
			case SystemManagerInterface::Method_HostRegistration:
			{
				HostInformation info;
				info.Read(stubClient->GetSocketReader());
				stubClient->EndUnmarshal();
				
				info = systemManagerInterface->HostRegistration(info);

				stubClient->BeginMarshal(SystemManagerInterface::Method_HostRegistration);
				info.Write(stubClient->GetSocketWriter());
				stubClient->EndMarshal();
				break;
			}
			case SystemManagerInterface::Method_HostInformation:
			{	
                std::string name = stubClient->GetSocketReader()->ReadString();
				stubClient->EndUnmarshal();

				HostInformation info = systemManagerInterface->GetHostInformation(name);

				stubClient->BeginMarshal(SystemManagerInterface::Method_HostInformation);
				info.Write(stubClient->GetSocketWriter());
				stubClient->EndMarshal();
				break;
			}
			case SystemManagerInterface::Method_AliveChecker:
			{
				HostInformation info;

				info.Read(stubClient->GetSocketReader());
				stubClient->EndUnmarshal();

				info = systemManagerInterface->AliveChecker(info);
				
				stubClient->BeginMarshal(SystemManagerInterface::Method_AliveChecker);
				info.Write(stubClient->GetSocketWriter());
				stubClient->EndMarshal();
				break;
			}
			case SystemManagerInterface::Method_KillAll:
			{
                std::string returnMsg = stubClient->GetSocketReader()->ReadString();
				stubClient->EndUnmarshal();

				systemManagerInterface->KillAll(returnMsg);
				
				stubClient->BeginMarshal(SystemManagerInterface::Method_KillAll);
				stubClient->GetSocketWriter()->WriteString(returnMsg);
				stubClient->EndMarshal();
				break;
			}
			case SystemManagerInterface::Method_KillComponent:
			{
                std::string componentName = stubClient->GetSocketReader()->ReadString();
                std::string returnMsg = stubClient->GetSocketReader()->ReadString();
				stubClient->EndUnmarshal();

				int ret = systemManagerInterface->KillComponent(componentName, returnMsg);

				stubClient->BeginMarshal(SystemManagerInterface::Method_KillComponent);
				stubClient->GetSocketWriter()->WriteString(returnMsg);
				stubClient->GetSocketWriter()->WriteInt32(ret);
				stubClient->EndMarshal();
				break;
			}
			case SystemManagerInterface::Method_StartComponents:
			{
                std::string xmlFile = stubClient->GetSocketReader()->ReadString();
				stubClient->EndUnmarshal();

                std::map<std::string, std::string> mapErrorMessages;
				int ret = systemManagerInterface->StartComponents(xmlFile, mapErrorMessages);

				stubClient->BeginMarshal(SystemManagerInterface::Method_StartComponents);
				SocketWriter *writer = stubClient->GetSocketWriter();
				
				writer->WriteInt32(mapErrorMessages.size());
                for(std::map<std::string, std::string>::iterator it = mapErrorMessages.begin(), it_end = mapErrorMessages.end(); it != it_end; ++it)
				{
					writer->WriteString(it->first);
					writer->WriteString(it->second);
				}				
				writer->WriteInt32(ret);

				stubClient->EndMarshal();
				break;
			}
			case SystemManagerInterface::Method_ShutDown:
			{
				stubClient->EndUnmarshal();
				systemManagerInterface->ShutDown();
				break;
			}
			case SystemManagerInterface::Method_AllComponentInformation:
			{
				stubClient->EndUnmarshal();

                std::map<std::string, HostInformation> mapNameHostInformation = systemManagerInterface->GetAllComponentInformation();

				stubClient->BeginMarshal(SystemManagerInterface::Method_AllComponentInformation);

				SocketWriter *writer = stubClient->GetSocketWriter();
				writer->WriteInt32(mapNameHostInformation.size());
				
                for(std::map<std::string, HostInformation>::iterator it = mapNameHostInformation.begin(), it_end = mapNameHostInformation.end(); it != it_end; ++it)
				{
					HostInformation &info = it->second;
					info.Write(writer);
				}				
				stubClient->EndMarshal();				
				break;
			}
			case SystemManagerInterface::Method_GetMiddlewareExceptions:
			{
				long long lastTimestamp = stubClient->GetSocketReader()->ReadInt64();
				stubClient->EndUnmarshal();

                std::map<long long, ExceptionMessage> mapTimestampExceptionMessage = systemManagerInterface->GetMiddlewareExceptions(lastTimestamp);
				
				stubClient->BeginMarshal(SystemManagerInterface::Method_GetMiddlewareExceptions);
				
				SocketWriter *writer = stubClient->GetSocketWriter();
				writer->WriteInt64(lastTimestamp);
				writer->WriteInt32(mapTimestampExceptionMessage.size());
				
                for(std::map<long long, ExceptionMessage>::iterator it = mapTimestampExceptionMessage.begin(), it_end = mapTimestampExceptionMessage.end(); it != it_end; ++it)
				{
					long long timeStamp = it->first;
					writer->WriteInt64(timeStamp);

					ExceptionMessage &ex = it->second;
					ex.Write(writer);
				}				
				stubClient->EndMarshal();				
				break;
			}
			case SystemManagerInterface::Method_MapResourceInformation:
			{
				stubClient->EndUnmarshal();

                std::map<std::string, HostResources> mapComponentHostResources = systemManagerInterface->GetMapResourceInformation();

				stubClient->BeginMarshal(SystemManagerInterface::Method_MapResourceInformation);
				
				SocketWriter *writer = stubClient->GetSocketWriter();
				writer->WriteInt32(mapComponentHostResources.size());
				
                for(std::map<std::string, HostResources>::iterator it = mapComponentHostResources.begin(), it_end = mapComponentHostResources.end(); it != it_end; ++it)
				{
					HostResources &res = it->second;
					res.Write(writer);
				}
				stubClient->EndMarshal();
				break;
			}			
			case SystemManagerInterface::Method_SetResourceInformation:
			{
				HostInformation factoryInfo;
				factoryInfo.Read(stubClient->GetSocketReader());

                std::map<int, HostResources> mapHostResources;
				int sizeofmap = stubClient->GetSocketReader()->ReadInt32();
				for(int i = 0; i < sizeofmap; i++)
				{
					HostResources res;
					res.Read(stubClient->GetSocketReader());

                    mapHostResources.insert(std::pair<int, HostResources>(res.GetProcessId(), res));
				}				
				stubClient->EndMarshal();

				systemManagerInterface->SetResourceInformation(factoryInfo, mapHostResources);			
				break;
			}
			case SystemManagerInterface::Method_ParseXmlFile:
			{
                std::string xmlCommands = stubClient->GetSocketReader()->ReadString();
                std::string returnMsg = stubClient->GetSocketReader()->ReadString();
				stubClient->EndUnmarshal();

                std::map<std::string, HostInformation> mapNameHostInformation = systemManagerInterface->ParseXmlFile(xmlCommands, returnMsg);

				stubClient->BeginMarshal(SystemManagerInterface::Method_ParseXmlFile);
				stubClient->GetSocketWriter()->WriteString(returnMsg);
				SocketWriter *writer = stubClient->GetSocketWriter();
				writer->WriteInt32(mapNameHostInformation.size());
				
                for(std::map<std::string, HostInformation>::iterator it = mapNameHostInformation.begin(), it_end = mapNameHostInformation.end(); it != it_end; ++it)
				{
					HostInformation &info = it->second;
					info.Write(writer);
				}				
				stubClient->EndMarshal();	
				break;
			}
			case SystemManagerInterface::Method_ClearAndKillAll:
			{
                std::string returnMsg = stubClient->GetSocketReader()->ReadString();
				stubClient->EndUnmarshal();

				int ret = systemManagerInterface->ClearAndKillAll(returnMsg);
				
				stubClient->BeginMarshal(SystemManagerInterface::Method_ClearAndKillAll);
				stubClient->GetSocketWriter()->WriteString(returnMsg);
				stubClient->GetSocketWriter()->WriteInt32(ret);
				stubClient->EndMarshal();
				break;
			}
			case SystemManagerInterface::Method_RestartDefault:
			{
                std::string errorMsg = stubClient->GetSocketReader()->ReadString();
				stubClient->EndUnmarshal();

				int ret = systemManagerInterface->RestartDefault(errorMsg);
				
				stubClient->BeginMarshal(SystemManagerInterface::Method_RestartDefault);
				stubClient->GetSocketWriter()->WriteString(errorMsg);
				stubClient->GetSocketWriter()->WriteInt32(ret);
				stubClient->EndMarshal();
				break;
			}
			default:
				RmiServerListener::RMInvocation(methodId, stubClient);
				break;
		}
	}
private:
	SystemManagerInterface *systemManagerInterface;
};

// -------------------------------------------------------
//	SystemManager connection used by ComponentFactory
// -------------------------------------------------------
/**
* Used by each ComponentFactory to register and maintain a connection with the SystemManager.
*/
class DLL_STATE SystemManagerFactoryConnection : public Thread
{
public:
	/**
	* Connects to the SystemManager using the InterfaceHandle managerHandle.
	*
	* @param info				The HostInformation of the host connecting to the SystemManager.
	* @param managerHandle		The InterfaceHandle (port, host) to the SystemManager.
	* @param autoStart			Starts the Thread that maintains the connection to the SystemManager, default = true.
	*/
	SystemManagerFactoryConnection(HostInformation &info, InterfaceHandle managerHandle, bool autoStart = true) 
		: myHostInformation(info)
		, systemManagerHandle(managerHandle)
		, systemManagerProxy(NULL)
	{
		systemManagerHandle.SetHostName(NetworkFunctions::getIPAddress(systemManagerHandle.GetHostName()));
		myHostInformation.SetHostIP(NetworkFunctions::getIPAddress(myHostInformation.GetHostName()));

		if(autoStart == true) start();
	}

	/**
	* Disconnects the host from the SystemManager.
	*/
    virtual ~SystemManagerFactoryConnection();
    /*{
		myHostInformation.SetOnlineStatus(HostInformation::OFFLINE);
		systemManagerProxy->HostRegistration(myHostInformation);
		cerr << "Disconnected SystemManager (" << systemManagerHandle.GetHostName() << "," << systemManagerHandle.GetPortNumber() << ")" << endl;
	
		if(systemManagerProxy) delete systemManagerProxy;
		systemManagerProxy = NULL;
    }*/

	/**
	* Main loop that maintains the connection to the SystemManager.
	*/
	virtual void run()
	{
		systemManagerProxy = new SystemManagerProxy(systemManagerHandle, true); 
		
		while(true)
		{
			try
			{
				if(systemManagerProxy->IsConnected() == false)
				{
					systemManagerProxy->WaitConnected();
					//throw Exception("SystemManagerFactoryConnection::run(): systemManagerProxy not connected to SystemManager");
				}


				myHostInformation.SetOnlineStatus(HostInformation::ONLINE);
				myHostInformation = systemManagerProxy->HostRegistration(myHostInformation);
				if(myHostInformation.GetOnlineStatus() == HostInformation::OFFLINE)
				{
					cerr << "SystemManager going OFFLINE!" << endl;
					msleep(10000);
					continue;
				}

				cout << "Connected to SystemManager (" << systemManagerHandle.GetHostName() << "," << systemManagerHandle.GetPortNumber() << ")" << endl;

				while(true)
				{
					msleep(ALIVE_CHECK_INTERVAL_MS);
					myHostInformation = systemManagerProxy->AliveChecker(myHostInformation);
					if(myHostInformation.GetOnlineStatus() == HostInformation::OFFLINE)
					{
						cerr << "SystemManager going OFFLINE!" << endl;
						msleep(10000);
						break;
					}
				}
			}
			catch(Exception io)
			{
				//IDEBUG() << myHostInformation.GetComponentName() << ": SystemManager connect exception: " << io.msg() << ". Attempting Reconnect." << endl;
				msleep(1000);
			}
			catch(...)
			{
				//IDEBUG() << myHostInformation.GetComponentName() << ": SystemManager registration exception!"<< endl;
				msleep(1000);
			}
		}
	}

private:
	HostInformation				myHostInformation;
	InterfaceHandle				systemManagerHandle;
	SystemManagerProxy			*systemManagerProxy;
};

} // namespace ends

#endif // SYSTEM_MANAGER_BASE_H


