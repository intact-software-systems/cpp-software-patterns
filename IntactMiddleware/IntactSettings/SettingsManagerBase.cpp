#include "IntactMiddleware/IntactSettings/SettingsManagerBase.h"
#include "IntactMiddleware/IntactSettings/SettingsListener.h"

namespace IntactMiddleware
{
SettingsManagerConnection* SettingsManagerConnection::settingsManagerConnection = NULL;

SettingsManagerConnection* SettingsManagerConnection::GetSettingsManagerConnection()
{
	if(SettingsManagerConnection::settingsManagerConnection == NULL)
		cerr << "GetOrCreateSettingsManagerConnection(info, manager): WARNING! SettingsManagerConnect::SetupSettingsManagerConnections must be called first!" << endl;

	return SettingsManagerConnection::settingsManagerConnection;
}

bool SettingsManagerConnection::SetupSettingsManagerConnections(InterfaceHandle settingsManagerHandle, HostInformation info)
{
	try
	{
		SettingsListener *settingsListener = SettingsListener::GetOrCreateSettingsListener();
		if(settingsListener == NULL)
		{
			throw string("Could not start SettingsListener");
		}

		if(SettingsManagerConnection::settingsManagerConnection == NULL)
		{
			InterfaceHandle settingsListenerHandle = settingsListener->GetInterfaceHandle();
			info.SetHostName(settingsListenerHandle.GetHostName());
			info.SetHostIP(settingsListenerHandle.GetHostName());
			info.SetPortNumber(settingsListenerHandle.GetPortNumber());
			info.SetOnlineStatus(HostInformation::ONLINE);

			SettingsManagerConnection::settingsManagerConnection = new SettingsManagerConnection(info, settingsManagerHandle);
			bool ret = SettingsManagerConnection::settingsManagerConnection->RegisterSettingsListener();
			if(ret == false)
			{
				throw("Could not connect to SettingsManager");
			}
		}
		else
			cerr << "CreateSettingsManagerConnection(info, manager): WARNING! Already started!" << endl;
	}
	catch(string msg)
	{
		cerr << "SetupSettingsManagerConnection(handle): ERROR! Exception caught! " << msg << endl;
		return false;
	}

	return true;
}

}

