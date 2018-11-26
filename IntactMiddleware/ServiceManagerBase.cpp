#include "IntactMiddleware/ServiceManagerBase.h"

namespace IntactMiddleware 
{
	/* ----------------------------------------------------------------------
					class ServiceManagerProxy
	------------------------------------------------------------------------*/
	
	HostInformation ServiceManagerProxy::ServiceRegistration(HostInformation info)
	{
		BeginMarshal(ServiceManagerInterface::Method_ServiceRegistration);
		info.Write(GetSocketWriter());
		EndMarshal();

		BeginUnmarshal();
		info.Read(GetSocketReader());
		EndUnmarshal();

		return info;
	}

	HostInformation ServiceManagerProxy::GetServiceInformation(string componentName)
	{
		BeginMarshal(ServiceManagerInterface::Method_ServiceInformation);
		GetSocketWriter()->WriteString(componentName);
		EndMarshal();

		BeginUnmarshal();
		HostInformation wrap;
		wrap.Read(GetSocketReader());
		EndUnmarshal();

		return wrap;
	}

	void ServiceManagerProxy::ServiceRegistration(MapServiceNameHostInformation &mapService)
	{
		BeginMarshal(ServiceManagerInterface::Method_ServiceRegistrationMap);
		GetSocketWriter()->WriteInt32(mapService.size());
		for(MapServiceNameHostInformation::iterator it = mapService.begin(), it_end = mapService.end(); it != it_end; ++it)
		{
			GetSocketWriter()->WriteString(it->first);

			HostInformation &info = it->second;
			info.Write(GetSocketWriter());
		}
		EndMarshal();

		BeginUnmarshal();
		int size = GetSocketReader()->ReadInt32();
		for(int i = 0; i < size; i++)
		{
			string serviceName = GetSocketReader()->ReadString();
			HostInformation info;
			info.Read(GetSocketReader());
			mapService[serviceName] = info;
		}
		EndMarshal();
	}

	HostInformation ServiceManagerProxy::AliveChecker(HostInformation info, string componentName)
	{
		BeginMarshal(ServiceManagerInterface::Method_AliveChecker);
		info.Write(GetSocketWriter());
		GetSocketWriter()->WriteString(componentName);
		EndMarshal();

		BeginUnmarshal();
		info.Read(GetSocketReader());
		EndUnmarshal();

		return info;
	}

	std::map<string, HostInformation> ServiceManagerProxy::AliveChecker(MapServiceNameHostInformation &mapService, SetServiceName &setNames)
	{
		BeginMarshal(ServiceManagerInterface::Method_AliveCheckerMaps);
		GetSocketWriter()->WriteInt32((int)mapService.size());
		for(MapServiceNameHostInformation::iterator it = mapService.begin(), it_end = mapService.end(); it != it_end; ++it)
		{
			GetSocketWriter()->WriteString(it->first);

			HostInformation &info = it->second;
			info.Write(GetSocketWriter());
		}

		GetSocketWriter()->WriteInt32((int)setNames.size());
		for(SetServiceName::iterator it = setNames.begin(), it_end = setNames.end(); it != it_end; ++it)
		{
			GetSocketWriter()->WriteString(*it);
		}
		EndMarshal();

		BeginUnmarshal();
		MapServiceNameHostInformation subscribedServices;
		int size = GetSocketReader()->ReadInt32();
		for(int i = 0; i < size; i++)
		{
			string serviceName = GetSocketReader()->ReadString();
			HostInformation info;
			info.Read(GetSocketReader());
			subscribedServices.insert(pair<string, HostInformation>(serviceName, info));
		}
		EndMarshal();

		return subscribedServices;
	}

	void ServiceManagerProxy::ShutDown()
	{
		BeginMarshal(ServiceManagerInterface::Method_ShutDown);
		EndMarshal();
	}

	map<string, HostInformation> ServiceManagerProxy::GetAllServiceInformation()
	{
		BeginMarshal(ServiceManagerInterface::Method_AllServiceInformation);
		EndMarshal();

		BeginUnmarshal();

		map<string, HostInformation> mapComponentHostInformation;
		int sizeofmap = GetSocketReader()->ReadInt32();
		for(int i = 0; i < sizeofmap; i++)
		{
			HostInformation info;
			info.Read(GetSocketReader());

			mapComponentHostInformation.insert(pair<string, HostInformation>(info.GetComponentName(), info));
		}		

		EndUnmarshal();

		return mapComponentHostInformation;
	}

	void ServiceManagerProxy::ClearServices()
	{
		BeginMarshal(ServiceManagerInterface::Method_ClearServices);
		EndMarshal();
	}


	/* ----------------------------------------------------------------------
					class ServiceManagerStub
	------------------------------------------------------------------------*/
	void ServiceManagerStub::RMInvocation(int methodId, RmiBase* stubClient)
	{
		switch(methodId)
		{
			case ServiceManagerInterface::Method_ServiceRegistration:
			{
				HostInformation info;
				info.Read(stubClient->GetSocketReader());
				stubClient->EndUnmarshal();
				
				info = serviceManagerInterface->ServiceRegistration(info);

				stubClient->BeginMarshal(ServiceManagerInterface::Method_ServiceRegistration);
				info.Write(stubClient->GetSocketWriter());
				stubClient->EndMarshal();
				break;
			}
			case ServiceManagerInterface::Method_ServiceRegistrationMap:
			{
					MapServiceNameHostInformation mapService;
					int size = stubClient->GetSocketReader()->ReadInt32();
					for(int i = 0; i < size; i++)
					{
						string serviceName = stubClient->GetSocketReader()->ReadString();
						HostInformation info;
						info.Read(stubClient->GetSocketReader());

						mapService.insert(pair<string, HostInformation>(serviceName, info));
					}
				stubClient->EndUnmarshal();

				serviceManagerInterface->ServiceRegistration(mapService);

				stubClient->BeginMarshal(ServiceManagerInterface::Method_ServiceRegistrationMap);
					stubClient->GetSocketWriter()->WriteInt32(mapService.size());
					for(MapServiceNameHostInformation::iterator it = mapService.begin(), it_end = mapService.end(); it != it_end; ++it)
					{
						stubClient->GetSocketWriter()->WriteString(it->first);

						HostInformation &info = it->second;
						info.Write(stubClient->GetSocketWriter());
					}
				stubClient->EndMarshal();
				break;
			}
			case ServiceManagerInterface::Method_ServiceInformation:
			{	
				string name = stubClient->GetSocketReader()->ReadString();
				stubClient->EndUnmarshal();

				HostInformation info = serviceManagerInterface->GetServiceInformation(name);

				stubClient->BeginMarshal(ServiceManagerInterface::Method_ServiceInformation);
				info.Write(stubClient->GetSocketWriter());
				stubClient->EndMarshal();
				break;
			}
			case ServiceManagerInterface::Method_AliveChecker:
			{
				HostInformation info;
				string componentName;

				info.Read(stubClient->GetSocketReader());
				componentName = stubClient->GetSocketReader()->ReadString();
				stubClient->EndUnmarshal();

				info = serviceManagerInterface->AliveChecker(info, componentName);
				
				stubClient->BeginMarshal(ServiceManagerInterface::Method_AliveChecker);
				info.Write(stubClient->GetSocketWriter());
				stubClient->EndMarshal();
				break;
			}
			case ServiceManagerInterface::Method_AliveCheckerMaps:
			{
					MapServiceNameHostInformation mapService;
					
					int size = stubClient->GetSocketReader()->ReadInt32();
					for(int i = 0; i < size; i++)
					{
						string serviceName = stubClient->GetSocketReader()->ReadString();
						HostInformation info;
						info.Read(stubClient->GetSocketReader());

						mapService.insert(pair<string, HostInformation>(serviceName, info));
					}
					
					SetServiceName setNames;
					size = stubClient->GetSocketReader()->ReadInt32();
					for(int i = 0; i < size; i++)
					{
						string serviceName = stubClient->GetSocketReader()->ReadString();
						setNames.insert(serviceName);
					}
				stubClient->EndUnmarshal();

				MapServiceNameHostInformation subscribedServices = serviceManagerInterface->AliveChecker(mapService, setNames);

				stubClient->BeginMarshal(ServiceManagerInterface::Method_AliveCheckerMaps);
					stubClient->GetSocketWriter()->WriteInt32((int)subscribedServices.size());
					for(MapServiceNameHostInformation::iterator it = subscribedServices.begin(), it_end = subscribedServices.end(); it != it_end; ++it)
					{
						stubClient->GetSocketWriter()->WriteString(it->first);

						HostInformation &info = it->second;
						info.Write(stubClient->GetSocketWriter());
					}
				stubClient->EndMarshal();
				break;
			}
			case ServiceManagerInterface::Method_ShutDown:
			{
				stubClient->EndUnmarshal();
				serviceManagerInterface->ShutDown();
				break;
			}
			case ServiceManagerInterface::Method_AllServiceInformation:
			{
				stubClient->EndUnmarshal();
				
				map<string, HostInformation> mapNameHostInformation = serviceManagerInterface->GetAllServiceInformation();

				stubClient->BeginMarshal(ServiceManagerInterface::Method_AllServiceInformation);

				SocketWriter *writer = stubClient->GetSocketWriter();
				writer->WriteInt32(mapNameHostInformation.size());
				
				for(map<string, HostInformation>::iterator it = mapNameHostInformation.begin(), it_end = mapNameHostInformation.end(); it != it_end; ++it)
				{
					HostInformation &info = it->second;
					info.Write(writer);
				}				
				stubClient->EndMarshal();				
				break;
			}
			case ServiceManagerInterface::Method_ClearServices:
			{
				stubClient->EndUnmarshal();
				serviceManagerInterface->ClearServices();
				break;
			}
			default:
				IDEBUG() << "WARNING! Service Manager method Id not supported " << methodId;
				RmiServerListener::RMInvocation(methodId, stubClient);
				break;
		}
	}


} // namespace end


