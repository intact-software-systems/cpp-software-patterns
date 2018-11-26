#include "IntactMiddleware/IntactSettings/SettingsListener.h"
#include "IntactMiddleware/ServiceConnectionManager.h"

namespace IntactMiddleware
{
SettingsListener* SettingsListener::settingsListener = NULL;

SettingsListener* SettingsListener::GetOrCreateSettingsListener()
{
	if(SettingsListener::settingsListener == NULL)
	{
		SettingsListenerStub *stub = NULL;

		try
		{
			HostInformation myHostInformation = Runtime::GetHostInformation();
			if(myHostInformation.GetComponentName().empty())
			{
				cerr << "SettingsListener::GetOrCreateSettingsListener(): Runtime::GetHostInformation() returned invalid HostInformation!" << endl;
				throw("");
			}

			stringstream stubName;
			stubName << myHostInformation.GetComponentName() << ".SettingsListener";
			
			SettingsListener::settingsListener = new SettingsListener(stubName.str()); 
			stub = new SettingsListenerStub(InterfaceHandle(), SettingsListener::settingsListener, true);
			
			myHostInformation.SetComponentName(stubName.str());
			myHostInformation.SetHostDescription(HostInformation::STUB);
			myHostInformation.SetPortNumber(stub->GetInterfaceHandle().GetPortNumber());
			//myHostInformation.SetHostIP(MicroMiddleware::S);
			//myHostInformation.SetHostName(CURRENT_HOST_NAME);

			SettingsListener::settingsListener->SetHostInformation(myHostInformation);

			// register service in connection-manager
			ServiceConnectionManager *serviceConnectionManager = ServiceConnectionManager::GetOrCreate();
			ASSERT(serviceConnectionManager);
			serviceConnectionManager->RegisterService(myHostInformation);
		}
		catch(...)
		{
			cerr << "SettingsListener::GetOrCreateSettingsListener(): ERROR! Exception caught! " << endl;
			if(SettingsListener::settingsListener)
			{
				delete SettingsListener::settingsListener;
				SettingsListener::settingsListener = NULL;
			}
			if(stub) delete stub;
		}
	}

	return SettingsListener::settingsListener;
}


} // end namespace IntactMiddleware

