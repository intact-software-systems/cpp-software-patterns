#include "IntactMiddleware/IntactComponent.h"
#include "IntactMiddleware/ServiceConnectionManager.h"
#include "IntactMiddleware/MembershipManagerConnection.h"
#include "IntactMiddleware/GroupConnectionManager.h"
#include "IntactMiddleware/IntactSettings/IncludeLibs.h"

namespace IntactMiddleware
{
/* -------------------------------------------------------
 		class IntactComponent
 -------------------------------------------------------*/
IntactComponent::IntactComponent(std::string componentName) : MicroMiddleware::ComponentBase(componentName)
{
}

IntactComponent::~IntactComponent()
{
}
/* -------------------------------------------------------
 		callback Initialize
 -------------------------------------------------------*/
void IntactComponent::Initialize()
{
	cout << COMPONENT_FUNCTION << "Empty function"<< endl;
}

/* -------------------------------------------------------
 	Check if there are incoming settings from SettingsManager
 -------------------------------------------------------*/
bool IntactComponent::HasComponentSettings()
{
	SettingsListener *settingsListener = SettingsListener::GetOrCreateSettingsListener();
	if(settingsListener == NULL)
	{
		cerr << "SettingsListener == NULL!! " << endl;
		return false;
	}

	ConfigurationFile configFile;
	configFile.loadFileFromString(settingsListener->GetConfigurationSettings());

	string componentName;
	IntactInfra::ConfigurationNodeList lst = configFile.getConfigurationNodes("Component");
	for(IntactInfra::ConfigurationNodeList::iterator it = lst.begin(); it != lst.end(); it++)
	{
		IntactInfra::ConfigurationNode &node = *it;
		if(node.count("name") <= 0)
		{
			IWARNING()  << "WARNING! No 'name' defined for Component" ;
			continue;
		}

		componentName = string(node["name"]);
		if(componentName.empty())
		{
			IWARNING()  << "WARNING! null/empty 'name' defined for Component" ;
			continue;
		}

		cerr << "Component name : " << componentName << endl;
		if(componentName != componentName_)
		{
			IWARNING()  << componentName_ << " != " << componentName ;
			continue;
		}
		break;
	}

	if(componentName != componentName_)
	{
		IWARNING()  << componentName_ << " != " << componentName ;
		return false;
	}
	return true;
}

void IntactComponent::ParseComponentSettings()
{
	bool hasSettings = HasComponentSettings();
	if(hasSettings)
	{
		SettingsListener *settingsListener = SettingsListener::GetOrCreateSettingsListener();
		if(settingsListener == NULL)
		{
			cerr << "SettingsListener == NULL!! " << endl;
			return;
		}

		ConfigurationFile configFile;
		configFile.loadFileFromString(settingsListener->GetConfigurationSettings());

		stringstream path;
		path << "Component/" << componentName_;
		IntactInfra::ConfigurationNodeList lst = configFile.getConfigurationNodes(path.str());
		for(IntactInfra::ConfigurationNodeList::iterator it = lst.begin(); it != lst.end(); it++)
		{
			IntactInfra::ConfigurationNode &node = *it;
			if(node.count("a") > 0 && node.count("b") > 0)
				cerr << "Received new settings: a = " << node["a"] << " b=" << node["b"] << endl;
		}
	}
}

}
