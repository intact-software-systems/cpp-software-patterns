#include "IntactMiddleware/ProcessFactoryBase.h"

namespace IntactMiddleware
{
std::string ProcessFactoryInterface::InterfaceID = 
            "IntactMiddleware.ProcessFactoryInterface";


    InterfaceHandle ProcessFactoryProxy::CreateComponent(const string& componentType, const string& instanceName)
    {
        BeginMarshal(ProcessFactoryInterface::Method_CreateComponent);
        GetSocketWriter()->WriteString(componentType);
        GetSocketWriter()->WriteString(instanceName);
        EndMarshal();
        BeginUnmarshal();
        InterfaceHandle handle;
		handle.Read(GetSocketReader());
        EndUnmarshal();
        return handle;
    }

	string ProcessFactoryProxy::CreateComponent(HostInformation &hostInformation, HostInformation lookupServer)
	{
		BeginMarshal(ProcessFactoryInterface::Method_CreateHostComponent);
		hostInformation.Write(GetSocketWriter());
		lookupServer.Write(GetSocketWriter());
		EndMarshal();
		BeginUnmarshal();
		hostInformation.Read(GetSocketReader());
		string msg = GetSocketReader()->ReadString();
		EndUnmarshal();
		return msg;
	}

	string ProcessFactoryProxy::CreateExtComponent(HostInformation &hostInformation, HostInformation lookupServer)
	{
		BeginMarshal(ProcessFactoryInterface::Method_CreateExtHostComponent);
		hostInformation.Write(GetSocketWriter());
		lookupServer.Write(GetSocketWriter());
		EndMarshal();
		BeginUnmarshal();
		hostInformation.Read(GetSocketReader());
		string msg = GetSocketReader()->ReadString();
		EndUnmarshal();
		return msg;
	}
	
	HostResources ProcessFactoryProxy::GetProcessResources(string componentName)
	{
		BeginMarshal(ProcessFactoryInterface::Method_GetProcessResources);
		GetSocketWriter()->WriteString(componentName);
		EndMarshal();
		
		HostResources hostResources;

		BeginUnmarshal();
		hostResources.Read(GetSocketReader());
		EndUnmarshal();

		return hostResources;
	}

	HostResources ProcessFactoryProxy::GetNodeResources()
	{
		BeginMarshal(ProcessFactoryInterface::Method_GetNodeResources);
		EndMarshal();
		
		HostResources hostResources;

		BeginUnmarshal();
		hostResources.Read(GetSocketReader());
		EndUnmarshal();

		return hostResources;	
	}

	string ProcessFactoryProxy::GetAllComponentResources()
	{
		BeginMarshal(ProcessFactoryInterface::Method_GetAllComponentResources);
		EndMarshal();

		string allInfo;

		BeginUnmarshal();
		allInfo = GetSocketReader()->ReadString();
		EndUnmarshal();

		return allInfo;
	}

	map<int, HostResources> ProcessFactoryProxy::GetMapComponentResources()
	{
		BeginMarshal(ProcessFactoryInterface::Method_GetMapComponentResources);
		EndMarshal();

		BeginUnmarshal();

		map<int, HostResources> mapPidHostResources;
		//cout << "ProcessFactoryProxy::GetmapComponentResources: " << endl;
		int sizeofmap = GetSocketReader()->ReadInt32();
		for(int i = 0; i < sizeofmap; i++)
		{
			HostResources res;
			res.Read(GetSocketReader());
			//cout << res << endl;
			mapPidHostResources.insert(pair<int, HostResources>(res.GetProcessId(), res));
		}
		
		EndUnmarshal();

		return mapPidHostResources;
	}

    InterfaceHandle ProcessFactoryProxy::GetComponent(const string& componentType, const string& instanceName)
    {
        BeginMarshal(ProcessFactoryInterface::Method_GetComponent);
        GetSocketWriter()->WriteString(componentType);
        GetSocketWriter()->WriteString(instanceName);
        EndMarshal();
        BeginUnmarshal();
        InterfaceHandle handle;
		handle.Read(GetSocketReader());
        EndUnmarshal();
        return handle;
    }

    void ProcessFactoryProxy::ComponentCreated(
        const string& hostName, 
        int mainStubPort, 
        int processId, 
        const string& instanceName, 
        const string& processFileName,
		int instanceId)
    {
        BeginMarshal(ProcessFactoryInterface::Method_ComponentCreated);
        GetSocketWriter()->WriteString(hostName);
        GetSocketWriter()->WriteInt32(mainStubPort);
        GetSocketWriter()->WriteInt32(processId);
        GetSocketWriter()->WriteString(instanceName);
        GetSocketWriter()->WriteString(processFileName);
		GetSocketWriter()->WriteInt32(instanceId);
        EndMarshal();
        BeginUnmarshal();
        EndUnmarshal();
    }
    
	int ProcessFactoryProxy::KillAll()
    {
        BeginMarshal(ProcessFactoryInterface::Method_KillAll);
        EndMarshal();
		
		BeginUnmarshal();
		int ret = GetSocketReader()->ReadInt32();
		EndUnmarshal();
		
		return ret;
    }

	int ProcessFactoryProxy::KillComponent(string componentName)
	{
		BeginMarshal(ProcessFactoryInterface::Method_KillComponent);
		GetSocketWriter()->WriteString(componentName);
		EndMarshal();

		BeginUnmarshal();
		int ret = GetSocketReader()->ReadInt32();
		EndUnmarshal();
		
		return ret;
	}

	int	ProcessFactoryProxy::IsComponentRunning(HostInformation hostInformation)
	{
		BeginMarshal(ProcessFactoryInterface::Method_IsComponentRunning);
		hostInformation.Write(GetSocketWriter());
		EndMarshal();
		
		BeginUnmarshal();
		int ret = GetSocketReader()->ReadInt32();
		EndUnmarshal();
		
		return ret;
	}


ProcessFactoryStub::ProcessFactoryStub(const InterfaceHandle &handle, ProcessFactoryInterface* componentFactoryInterface, bool autoStart)
    : RmiServerListener(handle, autoStart)
	, componentFactoryInterface(componentFactoryInterface)
{
}

void ProcessFactoryStub::RMInvocation(int methodID, RmiBase* stubClient)
{
    switch(methodID) 
	{
    case ProcessFactoryInterface::Method_CreateComponent:
        {
            string componentType = stubClient->GetSocketReader()->ReadString();
            string instanceName = stubClient->GetSocketReader()->ReadString();
            stubClient->EndUnmarshal();
            InterfaceHandle interfaceHandle = componentFactoryInterface->CreateComponent(componentType, instanceName);
            stubClient->BeginMarshal(ProcessFactoryInterface::Method_CreateComponent);
            interfaceHandle.Write(stubClient->GetSocketWriter());
            stubClient->EndMarshal();
            break;
        }
	case ProcessFactoryInterface::Method_CreateHostComponent:
		{
			HostInformation info, lookupServer;
			info.Read(stubClient->GetSocketReader());
			lookupServer.Read(stubClient->GetSocketReader());
			stubClient->EndUnmarshal();
			
			string msg = componentFactoryInterface->CreateComponent(info, lookupServer);
			
			stubClient->BeginMarshal(ProcessFactoryInterface::Method_CreateHostComponent);
			info.Write(stubClient->GetSocketWriter());
			stubClient->GetSocketWriter()->WriteString(msg);
			stubClient->EndMarshal();
			break;			
		}
	case ProcessFactoryInterface::Method_CreateExtHostComponent:
		{
			HostInformation info, lookupServer;
			info.Read(stubClient->GetSocketReader());
			lookupServer.Read(stubClient->GetSocketReader());
			stubClient->EndUnmarshal();
			
			string msg = componentFactoryInterface->CreateExtComponent(info, lookupServer);
			
			stubClient->BeginMarshal(ProcessFactoryInterface::Method_CreateExtHostComponent);
			info.Write(stubClient->GetSocketWriter());
			stubClient->GetSocketWriter()->WriteString(msg);
			stubClient->EndMarshal();
			break;			
		}
	case ProcessFactoryInterface::Method_GetProcessResources:
		{
			string componentName = stubClient->GetSocketReader()->ReadString();
			stubClient->EndUnmarshal();		
			
			HostResources hostResources = componentFactoryInterface->GetProcessResources(componentName);

			stubClient->BeginMarshal(ProcessFactoryInterface::Method_GetProcessResources);
			hostResources.Write(stubClient->GetSocketWriter());
			stubClient->EndMarshal();
			break;
		}
	case ProcessFactoryInterface::Method_GetNodeResources:
		{
			stubClient->EndUnmarshal();		
			
			HostResources hostResources = componentFactoryInterface->GetNodeResources();

			stubClient->BeginMarshal(ProcessFactoryInterface::Method_GetNodeResources);
			hostResources.Write(stubClient->GetSocketWriter());
			stubClient->EndMarshal();
			break;
		}
	case ProcessFactoryInterface::Method_GetAllComponentResources:
		{
			stubClient->EndUnmarshal();

			string allInfo = componentFactoryInterface->GetAllComponentResources();

			stubClient->BeginMarshal(ProcessFactoryInterface::Method_GetAllComponentResources);
			stubClient->GetSocketWriter()->WriteString(allInfo);
			stubClient->EndMarshal();
			break;
		}
	case ProcessFactoryInterface::Method_GetMapComponentResources:
		{
			stubClient->EndUnmarshal();

			map<int, HostResources> mapPidHostResources = componentFactoryInterface->GetMapComponentResources();

			stubClient->BeginMarshal(ProcessFactoryInterface::Method_GetMapComponentResources);
			
			SocketWriter *writer = stubClient->GetSocketWriter();
			writer->WriteInt32(mapPidHostResources.size());
			
			for(map<int, HostResources>::iterator it = mapPidHostResources.begin(), it_end = mapPidHostResources.end(); it != it_end; ++it)
			{
				HostResources &res = it->second;
				res.Write(writer);
			}
			stubClient->EndMarshal();
			break;
		}
    case ProcessFactoryInterface::Method_GetComponent:
        {
            string componentType = stubClient->GetSocketReader()->ReadString();
            string instanceName = stubClient->GetSocketReader()->ReadString();
            stubClient->EndUnmarshal();
            InterfaceHandle interfaceHandle = componentFactoryInterface->GetComponent(componentType, instanceName);
            stubClient->BeginMarshal(ProcessFactoryInterface::Method_GetComponent);
            interfaceHandle.Write(stubClient->GetSocketWriter());
            stubClient->EndMarshal();
            break;
        }
    case ProcessFactoryInterface::Method_ComponentCreated:
        {
            string hostName = stubClient->GetSocketReader()->ReadString();
            int mainStubPort = stubClient->GetSocketReader()->ReadInt32();
            int processId = stubClient->GetSocketReader()->ReadInt32();
            string instanceName = stubClient->GetSocketReader()->ReadString();
            string processFileName = stubClient->GetSocketReader()->ReadString();
			int instanceId = stubClient->GetSocketReader()->ReadInt32();
            stubClient->EndUnmarshal();
            componentFactoryInterface->ComponentCreated(hostName, mainStubPort, processId, instanceName, processFileName, instanceId);
            stubClient->BeginMarshal(ProcessFactoryInterface::Method_ComponentCreated);
            stubClient->EndMarshal();
            break;
        }
    case ProcessFactoryInterface::Method_KillAll:
        {
            stubClient->EndUnmarshal();
            int ret = componentFactoryInterface->KillAll();
            stubClient->BeginMarshal(ProcessFactoryInterface::Method_KillAll);
			stubClient->GetSocketWriter()->WriteInt32(ret);
            stubClient->EndMarshal();
            break;
        }
	case ProcessFactoryInterface::Method_KillComponent:
		{
			string componentName = stubClient->GetSocketReader()->ReadString();
			stubClient->EndUnmarshal();
			int ret = componentFactoryInterface->KillComponent(componentName);
			stubClient->BeginMarshal(ProcessFactoryInterface::Method_KillComponent);
			stubClient->GetSocketWriter()->WriteInt32(ret);
            stubClient->EndMarshal();
			break;	
		}
	case ProcessFactoryInterface::Method_IsComponentRunning:
		{
			HostInformation info;
			info.Read(stubClient->GetSocketReader());
			stubClient->EndUnmarshal();
		
			int ret = componentFactoryInterface->IsComponentRunning(info);
			
			stubClient->BeginMarshal(ProcessFactoryInterface::Method_IsComponentRunning);
			stubClient->GetSocketWriter()->WriteInt32(ret);
			stubClient->EndMarshal();
			break;
		}
    default: 
        {
            RmiServerListener::RMInvocation(methodID, stubClient);
        }
    }
}

} // namespace IntactMiddleware

