#include"SettingsManager.h"
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/framework/Wrapper4InputSource.hpp>

#ifdef USE_GCC
#include<sys/time.h>
#endif

// -------------------------------------------------------
//			private error handler
// -------------------------------------------------------

class ParseErrorHandler : public xercesc::ErrorHandler
{
public:
	int noOfWarnings;
	int noOfErrors;
	int noOfFatalErrors;

	string errorString;

	ParseErrorHandler() 
	{
		resetErrors();
	}

private:
	string formatMessage(const xercesc::SAXParseException& exc) 
	{
		stringstream ss;
		char* msg = xercesc::XMLString::transcode(exc.getMessage());
		char* id = xercesc::XMLString::transcode(exc.getSystemId());
		ss << " in line " << exc.getLineNumber() 
			<< ", column " << exc.getColumnNumber() 
			<< " of " << id << " : " << msg << endl << endl;
		xercesc::XMLString::release(&msg);
		xercesc::XMLString::release(&id);
		return ss.str();
	}

	virtual void warning(const xercesc::SAXParseException& exc) {
		errorString.append("Parse warning" + formatMessage(exc));
		noOfWarnings++;
	}
	virtual void error(const xercesc::SAXParseException& exc) {
		errorString.append("Non-fatal parse error" + formatMessage(exc));
		noOfErrors++;
	}
	virtual void fatalError(const xercesc::SAXParseException& exc) {
		errorString.append("Fatal parse error" + formatMessage(exc));
		noOfFatalErrors++;
	}
	virtual void resetErrors() {
		noOfWarnings = 0;
		noOfErrors = 0;
		noOfFatalErrors = 0;
		errorString = "";
	}
};

bool CheckXmlContent(const char* xmlContent) 
{
	cout << "ParseConfigString(...)  " << endl;

	xercesc::XercesDOMParser *parser = new xercesc::XercesDOMParser();

	parser->setDoNamespaces(true);
	parser->setDoSchema(true);
	parser->setValidationSchemaFullChecking(true);
	parser->setValidationScheme(xercesc::AbstractDOMParser::Val_Always);

	ParseErrorHandler* errorHandler = new ParseErrorHandler();

	parser->setErrorHandler(errorHandler);
	
	xercesc::MemBufInputSource *source = new xercesc::MemBufInputSource((const XMLByte*)xmlContent, strlen(xmlContent), "xml-content", false);
	xercesc::Wrapper4InputSource *wrapper = new xercesc::Wrapper4InputSource(source, false);

	parser->parse((const xercesc::InputSource &)*wrapper);

	bool allIsOk = true;
	if(errorHandler->noOfErrors != 0 || errorHandler->noOfFatalErrors != 0) 
	{
		ofstream outfile("settingsManager_error.log", ios::app);
		outfile << errorHandler->errorString;

		cerr << "Error: fatal=" << errorHandler->noOfFatalErrors << ", error=" << errorHandler->noOfErrors << ", warning=" << errorHandler->noOfWarnings << endl;
		allIsOk = false;
	}
	//else // ok
	//{
		//xercesc::DOMDocument* domDocument = parser->getDocument();
		//antennaConfig = new AntennaConfigDesc((DOMElement*)(domDocument->getElementsByTagName(X("Antenna"))->item(0)));
	//}

	delete source;
	delete wrapper;
	delete parser;

	return allIsOk;
}

// -------------------------------------------------------
//			initalize SettingsManagerConfig
// -------------------------------------------------------
int SettingsManagerConfig::settingsManagerPort			= MicroMiddleware::MiddlewareSettings::GetSettingsManagerPort();
string SettingsManagerConfig::networkInterfaceIP		= string("");
string SettingsManagerConfig::settingsInformationFile	= "SettingsInformation.xml";
string SettingsManagerConfig::hostInformationBackup		= "RegisteredSettingsListeners.txt";
int SettingsManagerConfig::loadConfigFileAtStartup		= 0; // 0 = no, 1 = yes (other values undefined)
#ifdef USE_GCC
string SettingsManagerConfig::settingsConfigurationFile = string("source/cpp/ComponentSettings0.xml");
#else
string SettingsManagerConfig::settingsConfigurationFile = string("C:\\Users\\knuthel\\ClearCase\\knuthel_SonarProc\\vobs\\SensorSystems\\SettingsManager\\source\\cpp\\ComponentSettings0.xml");
#endif

// -------------------------------------------------------
//	constructor/destructor SettingsManager
//		- Constructor: Initializes the SettingsManager
// -------------------------------------------------------
SettingsManager::SettingsManager(string name) 
		: Thread(name)
		, backupComponentSettings()
		, settingsManagerCommand(NULL)
		, stopSettingsManager(false)
{
#ifdef USE_GCC
	srand(Thread::currentThreadId());
#elif WIN32
	srand(::GetCurrentTime());
#endif

	stringstream ostr;
	int foundErrorsInXml = initSettingsManager(ostr
												, SettingsManagerConfig::settingsConfigurationFile
												, SettingsManagerConfig::settingsInformationFile
												, SettingsManagerConfig::hostInformationBackup);
	if(foundErrorsInXml)
	{
		cout << "SettingsManager::SettingsManager("<< name << "): Found "  << foundErrorsInXml << " errors while parsing!" << endl;
		cout << ostr.str() << endl;
	}

	settingsManagerCommand = new SettingsManagerCommand(*this);
}

SettingsManager::~SettingsManager()
{ 
	// if SettingsManager's destructor is called we assume it was shut down correctly
	// therefore, no host information is required upon restart
	this->clearBackupFile(SettingsManagerConfig::hostInformationBackup);
}

void SettingsManager::clearBackupFile(const string &backupFile)
{
	if(outComponentFile.is_open())
		outComponentFile.close();

	outComponentFile.open(backupFile.c_str(), fstream::out);
	if(outComponentFile.is_open())
	{
		outComponentFile << "" ;
		outComponentFile.flush();
		outComponentFile.close();
	}
}

// -------------------------------------------------------
//	initalize the system manager -> called from constructor
// -------------------------------------------------------
int SettingsManager::initSettingsManager(stringstream &ostr, const string &loadXmlFile, const string &backupXmlFile, const string &backupHostFile)
{
	int foundErrorsInXml = 0;
	try
	{
		stringstream xmlContent;
		std::ifstream ifile(backupXmlFile.c_str(), ios::in);
		if(ifile.is_open())
		{
			xmlContent << ifile.rdbuf();
			ifile.close();
		}

		if(xmlContent.str().empty()) 
		{
			foundErrorsInXml++;
			ostr << "initSettingsManager(): " << backupXmlFile << " is empty!" << endl;
			throw("");	
		}
		
		cout << endl;
		cout << "------------------------ Restoring Settings -----------------------------" << endl;
		cout << xmlContent.str() << endl;
		cout << "-------------------------------------------------------------------------" << endl;

		backupComponentSettings.loadFileFromString(xmlContent.str());
		foundErrorsInXml += restoreHostSettings(ostr, backupComponentSettings, "SettingsManager");
	
		if(this->mapComponentSettings.empty() == true && !loadXmlFile.empty())
		{
			ConfigurationFile configFile(loadXmlFile);
			foundErrorsInXml += parseSettingsFile(ostr, configFile, "SettingsManager/Component");
		}

		// Restore registered SettingsListeners (HostInformation)
		if(!backupHostFile.empty() || !(strcmp(backupHostFile.c_str(), "") == 0))
		{
			parseHostInformationFromDisc(backupHostFile); // -> inserts component information to mapHostComponentInformation

			outComponentFile.open(backupHostFile.c_str(), fstream::out);
			if(outComponentFile.is_open())
			{
				for(MapComponentInformation::iterator it = mapComponentHostInformation.begin(), it_end = mapComponentHostInformation.end(); it != it_end; ++it)
					HostInformationSpace::WriteHostInformationToDisc(outComponentFile, it->second);
			}
		}
	}
	catch(...)
	{
		ostr << "initSettingsManager(): ERROR! Error occured while parsing " << backupXmlFile << endl;

		backupComponentSettings = ConfigurationFile();
		backupComponentSettings.addConfigurationNode("SettingsManager");
	}

	return foundErrorsInXml;	
}

int SettingsManager::restoreHostSettings(stringstream &ostr, ConfigurationFile &configFile, string path)
{
	int foundErrorsInXml = 0;

	const IntactInfra::DOMElementList &domList = configFile.getDOMElements(path);
	for(IntactInfra::DOMElementList::const_iterator it = domList.begin(), it_end = domList.end(); it != it_end; ++it)
	{
		xercesc::DOMElement *domElement = *it;
		ASSERT(domElement);
		if(domElement == NULL) continue;
		
		xercesc::DOMNodeList* childList = domElement->getChildNodes();
		if(childList == NULL) continue;

	    for(unsigned int i = 0; i < childList->getLength(); i++) 
		{
			xercesc::DOMElement* childElement = (xercesc::DOMElement*) childList->item(i);
			ASSERT(childElement);

			ConfigurationNode node = configFile.getConfigurationNode(childElement);
			if(node.empty()) continue;
			
			//std::string tagName = configFile.fromXMLCh(childElement->getTagName());
			string componentName = node["name"];
			string xmlFile = node["file"];

			// -- debug start --
			cout << "restoreHostSettings(): Restoring settings for " << componentName << " in " << xmlFile << endl;
			//if(tagName.empty()) continue;
			ASSERT(componentName.empty() == false);
			ASSERT(xmlFile.empty() == false);
			if(componentName.empty()) continue;
			if(xmlFile.empty()) continue;
			// -- debug end --

			try
			{
				ConfigurationFile configFile(xmlFile);
				foundErrorsInXml += parseSettingsFile(ostr, configFile, "Component");
			}
			catch(std::runtime_error &err)
			{
				ostr << "restoreHostSettings(): ERROR! " << err.what() << ". Is " << xmlFile << " valid and stored on hard-drive? " << endl;
				foundErrorsInXml++;
			}
		}
	}

	return foundErrorsInXml;
}

bool SettingsManager::backupHostSettings(ConfigurationFile &configFile, const HostSettings &settings, string path)
{
	// -- tests start --
	if(settings.GetComponentName().empty()) return false;
	if(settings.GetFileName().empty()) return false;
	// -- tests end --

	bool foundComponent = false;
	const IntactInfra::DOMElementList &domList = configFile.getDOMElements(path);
	for(IntactInfra::DOMElementList::const_iterator it = domList.begin(), it_end = domList.end(); it != it_end && foundComponent == false; ++it)
	{
		if(foundComponent == true) break;

		xercesc::DOMElement *domElement = *it;
		ASSERT(domElement);
		if(domElement == NULL) continue;

		//ConfigurationNode node = configFile.getConfigurationNode(domElement);
		
		xercesc::DOMNodeList* childList = domElement->getChildNodes();
		if(childList == NULL) continue;

	    for(unsigned int i = 0; i < childList->getLength(); i++) 
		{
			xercesc::DOMElement* childElement = (xercesc::DOMElement*) childList->item(i);
			ASSERT(childElement);

			ConfigurationNode node = configFile.getConfigurationNode(childElement);
			if(node.empty()) continue;

			//std::string tagName = configFile.fromXMLCh(childElement->getTagName());
			string componentName = node["name"];
			string xmlFile = node["file"];

			// -- debug start --
			//cout << "backupHostSettings(): Settings for " << componentName << " in " << xmlFile << endl;
			//if(tagName.empty()) continue;
			if(componentName.empty()) continue;
			if(xmlFile.empty()) continue;
			// -- debug end --
	
			if(componentName == settings.GetComponentName()) 
			{
				//cout << "backupHostSettings(): found  " << componentName << " in " << xmlFile << endl;
				foundComponent = true;
				break;
			}
		}
	}
	
	if(foundComponent == false)
	{
		ConfigurationNode newNode;
		newNode["name"] = settings.GetComponentName();
		newNode["file"] = settings.GetFileName();

		stringstream componentPath;
		componentPath << "SettingsManager/" << settings.GetComponentName() ;

		configFile.addConfigurationNode(componentPath.str(), &newNode);
		configFile.saveFile(SettingsManagerConfig::settingsInformationFile);
	}
	//else
	//{
	//	cout << "backupHostSettings(): " << settings.GetComponentName() << " already present!" << endl;
	//}
	return true;
}

// -------------------------------------------------------
// Parse componentInformationFile:
//	- If SystemManager crashes, its database is
//		stored in componentInformationFile
// -------------------------------------------------------
void SettingsManager::parseHostInformationFromDisc(const string &filename)
{
	vector<HostInformation> vectorHostInformation;
	if(HostInformationSpace::ParseHostInformationFromDisc(filename, vectorHostInformation) == false)
		return;
	
	while(!vectorHostInformation.empty())
	{
		HostInformation info = vectorHostInformation.back(); // iterate from back to front to get most recent
		vectorHostInformation.pop_back();

		// -- start check information --
		try
		{
			if(info.GetComponentName().empty()) throw IOException("ComponentName is empty!");
			if(info.GetHostName().empty()) throw IOException("HostName is empty!");
		}
		catch(IOException io)
		{
			cout << "SettingsManager::parseHostInformationFromDisc(filename): ERROR in HostInformation! " << io.what() << endl;
			continue;
		}
		// -- end check information --

		// store only first HostInformation, because iterating from back to front to get most recent backup
		if(mapComponentHostInformation.count(info.GetComponentName()) > 0) continue;
		
		info.SetOnlineStatus(HostInformation::NO_ONLINE_STATUS);
		mapComponentHostInformation.insert(pair<string, HostInformation>(info.GetComponentName(), info));
	}
}


// -------------------------------------------------------
//				class SettingsManager
//	TODO: Contact SettingsListeners and update settings!
// -------------------------------------------------------
void SettingsManager::run()
{
	cout << "SettingsManager::run(): SettingsManager is running." << endl;
	
	while(true)
	{
		updateLock.lock();
		// -> wait for settings updates
		if(settingsUpdated.empty())
		{
			updateCond.wait(&updateLock);
		}

		if(stopSettingsManager == true) break;

		SetComponentName remUpdated;
		for(SetComponentName::iterator it = settingsUpdated.begin(), it_end = settingsUpdated.end(); it != it_end; ++it)
		{
			string componentName = *it;
			if(mapComponentSettings.count(componentName) <= 0)
			{
				cout << "SettingsManager::run(): " << componentName << " not in mapComponentSettings " << endl;
				continue;
			}

			HostSettings &settings = mapComponentSettings[componentName];
			SettingsListenerProxy *proxy = getOrCreateSettingsListenerProxy(settings.GetHostInformation());
			if(proxy == NULL) 
			{
				//cout << "SettingsManager::RegisterSettingsListener(info): ERROR! Can't connect SettingsListener! Info: " << settings.GetHostInformation() << endl;
				continue;
			}

			try
			{
				//cout << "SettingsManager::run(): " << componentName << " updating HostSettings" << endl;
				proxy->SetHostSettings(settings);
				remUpdated.insert(componentName);
			}
			catch(...)
			{
				continue;
			}
		}
		
		// remove updated settings;
		for(SetComponentName::iterator it = remUpdated.begin(), it_end = remUpdated.end(); it != it_end; ++it)
			settingsUpdated.erase(*it);

		// back-off if no-one is available
		if(remUpdated.empty() && !settingsUpdated.empty()) 
		{
			updateLock.unlock();
			msleep(1000);
		}
		else updateLock.unlock();
	}

	updateLock.unlock();
}

//static int debug = 0;
// -> From third-party (ex: facade)
void SettingsManager::UpdateSettings(const string &xmlCommands)
{
	MutexLocker lock(&updateLock);
	
	//cerr << "UpdateSettings(): " << debug++ << endl;
	ConfigurationFile configFile;
	configFile.loadFileFromString(xmlCommands);

	stringstream ostr;
	int foundErrorsInXml = parseSettingsFile(ostr, configFile, "SettingsManager/Component");
}

void SettingsManager::UpdateComponentSettings(const string &componentName, const string &nameSpace, const map<string, string> &mapAttributeValue)
{
	MutexLocker lock(&updateLock);

	//cerr << "UpdateComponentSettings(): " << debug++ << endl;
	// -- start xml commands --
	// Fill start
	stringstream xmlCommands;
	xmlCommands << "<SettingsManager>" << endl;
	xmlCommands << "    <Component name=\"" << componentName << "\">" << endl;
	
	// Fill <namespace attribute="value"/>
	xmlCommands << "        <" << nameSpace ;
	for(map<string, string>::const_iterator it = mapAttributeValue.begin(), it_end = mapAttributeValue.end(); it != it_end; ++it)
	{
		// attribute="value"
		xmlCommands << " " << it->first << "=\"" << it->second << "\"";
	}
	xmlCommands << "/>" << endl;

	// Fill end
	xmlCommands << "    </Component>" << endl;
	xmlCommands << "</SettingsManager>" << endl;
	// -- end xml commands --

	//cout << xmlCommands.str() << endl;

	ConfigurationFile configFile;
	configFile.loadFileFromString(xmlCommands.str());

	stringstream ostr;
	int foundErrorsInXml = parseSettingsFile(ostr, configFile, "SettingsManager/Component");
}

void SettingsManager::LoadSettings(string streamSettings)
{
	
}


// -> From components
HostSettings SettingsManager::RegisterSettingsListener(HostInformation info)
{
	MutexLocker lock(&updateLock);

	// add HostInformation
	if(mapComponentHostInformation.count(info.GetComponentName()) <= 0) // -> happens when CreateComponent is called directly from a component
	{
		mapComponentHostInformation.insert(pair<string, HostInformation>(info.GetComponentName(), info));
	}
	else // update
	{
		mapComponentHostInformation[info.GetComponentName()] = info;
	}

	// add HostSettings
	if(mapComponentSettings.count(info.GetComponentName()) <= 0)
	{
		HostSettings settings(info);
		mapComponentSettings.insert(pair<string, HostSettings>(info.GetComponentName(), settings));
	}
	else // update
	{
		HostSettings &settings = mapComponentSettings[info.GetComponentName()];
		settings.SetHostInformation(info);	
	}

	HostInformationSpace::WriteHostInformationToDisc(outComponentFile, info);
	
	cout << "SettingsManager::RegisterSettingsListener(info): Component " << info.GetComponentName() << " on host " << info.GetHostName() << " is " << info.GetOnlineStatus() << endl;

	return mapComponentSettings[info.GetComponentName()];
}

HostSettings SettingsManager::GetHostSettings(string componentName)
{
	MutexLocker lock(&updateLock);
	
	if(mapComponentSettings.count(componentName) <= 0)
	{
		cout << "SettingsManager::RegisterSettingsListener(info): WARNING! Component " << componentName << " has no Settings registered in SettingsManager!" << endl;
		return HostSettings();
	}
	
	return mapComponentSettings[componentName];
}

map<string, HostSettings> SettingsManager::GetAllComponentSettings()
{
	MutexLocker lock(&updateLock);
	return mapComponentSettings;
}

// -------------------------------------------------------
//	load settings from xmlFile
// -------------------------------------------------------
int SettingsManager::loadComponentSettings(ostream &out, string xmlFile)
{
	MutexLocker lock(&updateLock);

	stringstream ostr;
	int foundErrorsInXml = 0;
	try
	{
		ConfigurationFile configFile;
		configFile.loadFile(xmlFile);
		
		foundErrorsInXml += parseSettingsFile(ostr, configFile, "SettingsManager/Component");
	}
	catch(...)
	{
		ostr << "loadComponentSettings(): ERROR! Exception caught upon loading " << xmlFile << endl;
		foundErrorsInXml++;
	}

	out << ostr.str();

	return foundErrorsInXml;
}

// -------------------------------------------------------
//	parse an xml settings file
// -------------------------------------------------------
int SettingsManager::parseSettingsFile(stringstream &ostr, ConfigurationFile &configFile, const string &path)
{
	int foundErrorsInXml = 0;
	//SystemManagerConfig::systemConfigurationFile = xmlFile;

	try
	{
		foundErrorsInXml += parseSettingsFilePath(ostr, mapComponentSettings, configFile, path);
	}
	catch(std::runtime_error &msg)
	{
		ostr << "parseSettingsFile(configFile): ERROR! Error occured while parsing settings: " << msg.what() << endl;
		cout << ostr.str();
	}
	catch(...)
	{
		ostr << "parseSettingsFile(configFile): ERROR! Error occured while parsing settings!" << endl;
		cout << ostr.str();
	}

	return foundErrorsInXml;	
}

int SettingsManager::parseSettingsFilePath(ostream &ostr, MapComponentSettings &mapComponentSettings, ConfigurationFile &configFile, const string &path)
{
	int foundErrorsInXml = 0;
	const IntactInfra::DOMElementList &domList = configFile.getDOMElements(path);
	for(IntactInfra::DOMElementList::const_iterator it = domList.begin(), it_end = domList.end(); it != it_end; ++it)
	{
		xercesc::DOMElement *domElement = *it;
		ConfigurationNode node = configFile.getConfigurationNode(domElement);
		
		// -- begin error handling --
		if(node.empty())
		{
			cout << "SettingsManager::parseSettingsFilePath(): WARNING! empty node for " << path << endl;
			foundErrorsInXml++;
			continue;
		}
		else if(node.count("name") <= 0) 
		{
			cout << "SettingsManager::parseSettingsFilePath(): WARNING! No attribute 'name' defined for " << path << endl;
			foundErrorsInXml++;
			continue;
		}

		string componentName(node["name"]);
		if(componentName.empty() == true)
		{
			cout << "SettingsManager::parseSettingsFilePath(): ERROR! null/empty 'name' defined for " << path << endl;
			foundErrorsInXml++;
			continue;
		}
		// -- end error handling --

		string fileName("Settings." + componentName + ".xml");

		bool settingsAreUpdated = false;
		HostSettings &settings = mapComponentSettings[componentName];

		//cerr << "SettingsManager::parseSettingsFilePath(): Updating current settings : " << endl << settings.GetConfSettings() << endl;

		if(settings.IsValid() == false || settings.GetConfSettings().empty()) // first time
		{
			ConfigurationFile &currFile = settings.GetXMLFile();	
			currFile.loadFileFromString(configFile.writeToString(domElement));

			settings.SetFileName(fileName);
			settings.SetConfSettings(currFile.writeToString());	
			settings.SetHostInformation(HostInformation(componentName));	
			settings.SetConfigPath(path);		

			settingsAreUpdated = true;
		}
		else if(configFile.writeToString(domElement) != settings.GetConfSettings()) // update settings
		{
			ConfigurationFile &currFile = settings.GetXMLFile();
			
			// update currFile
			xercesc::DOMElement *thisCurrentNode = NULL;
			
			const IntactInfra::DOMElementList &domList = currFile.getDOMElements("Component");
			ASSERT(domList.size() == 1);
			for(IntactInfra::DOMElementList::const_iterator it = domList.begin(), it_end = domList.end(); it != it_end; ++it)
				thisCurrentNode = *it;

			ASSERT(thisCurrentNode);
			currFile.addOrUpdate(domElement, thisCurrentNode);
			settings.SetConfSettings(currFile.writeToString());

			// -- debug start --
			//cout << "SettingsManager::parseSettingsFilePath(): new content : " << endl << settings.GetConfSettings() << endl;
			cout << "(" << GetTimeStamp() << ") SettingsManager::parseSettingsFilePath(): Settings updated for " << componentName << endl;
			// -- debug end --

			settingsAreUpdated = true;
		}

		if(settingsAreUpdated)
		{
			// -> save settings to file
			ConfigurationFile &currFile = settings.GetXMLFile();
			ofstream ofile(fileName.c_str());
			ASSERT(ofile.is_open());
			{
				ofile << currFile.writeToString();
				ofile.close();
			}

			settingsUpdated.insert(componentName);
			updateCond.wakeAll();
			//cerr << "SettingsManager::parseSettingsFilePath(): Updated current settings : " << endl << settings.GetConfSettings() << endl;	
		}

		// -> backup host-settings
		this->backupHostSettings(this->backupComponentSettings, settings, "SettingsManager");
	}

	if(foundErrorsInXml > 0)
		cerr << "SettingsManager::parseSettingsFilePath(): Found errors? " << foundErrorsInXml << endl;

	return foundErrorsInXml;
}

// -------------------------------------------------------
//	gets or creates a proxy to a single component
// -------------------------------------------------------
SettingsListenerProxy* SettingsManager::getOrCreateSettingsListenerProxy(const HostInformation &info)
{
	if(info.GetPortNumber() < 0 || info.GetHostIP().empty()) return NULL;

	// Create component proxy
	SettingsListenerProxy *proxy = NULL;
	if(mapComponentSettingsListenerProxy.count(info.GetComponentName()) >= 1)
	{
		proxy = mapComponentSettingsListenerProxy[info.GetComponentName()];
	}
	
	bool createNewProxy = false;
	if(proxy != NULL) if(proxy->IsConnected() == false) createNewProxy = true;

	InterfaceHandle handle(info.GetPortNumber(), info.GetHostIP());
	
	if(proxy == NULL || createNewProxy)
	{
		try
		{
			if(proxy != NULL) delete proxy;
			
			proxy = SettingsListenerProxy::GetRmiClient(handle);
			mapComponentSettingsListenerProxy[info.GetComponentName()] = proxy;
			
			cout << "getOrCreateSettingsListenerProxy(): Established contact to component " << info.GetComponentName() << " at " << handle << endl;
		}
		catch(...)
		{
			//cout << "getOrCreateSettingsListenerProxy(): WARNING! Unable to contact settings listener component " << info.GetComponentName() << " at " << handle << endl;
			mapComponentSettingsListenerProxy[info.GetComponentName()] = NULL;
			mapComponentSettingsListenerProxy.erase(info.GetComponentName());
			return NULL;
		}
	}

	if(proxy == NULL)
		cout << "getOrCreateSettingsListenerProxy(): WARNING! Unable to contact settings listener component " << info.GetComponentName() << " at " << handle << endl;
	return proxy;
}


HostSettings SettingsManager::AliveChecker(HostInformation info)
{
	MutexLocker lock(&updateLock);
	IDEBUG() << "TODO! Implement logic!" ;
	return HostSettings();
}

void SettingsManager::ShutDown()
{
	MutexLocker lock(&updateLock);
	IDEBUG() << "TODO! Implement logic!" ;
}

void SettingsManager::printSettingsForComponent(std::ostream &ostr, string componentName)
{
	MutexLocker lock(&updateLock);

	try
	{
		if(mapComponentSettings.count(componentName) <= 0)
		{
			ostr << "printSettingsForComponent(): " << componentName << " was not found!" << endl;
			return;
		}

		HostSettings &settings = mapComponentSettings[componentName];
		if(settings.IsValid() == false)
		{
			ostr << "printSettingsForComponent(): WARNING! Settings for " << componentName << " are not valid! Please contact system-administrator!" << endl;
		}

		string xmlContent = settings.GetXMLFile().writeToString();
		if(xmlContent.empty())
		{
			ostr << "printSettingsForComponent(): No settings found for " << componentName << endl;
			return;
		}

		ostr << endl;
		ostr << "-------------------- Settings for " << componentName << " ---------------------------->" << endl;
		ostr << xmlContent << endl;
		ostr << "---------------------------------------------------------------------------------------" << endl;
	}
	catch(std::runtime_error err)
	{
		ostr << "printSettingsForComponent(): ERROR! Exception caught: " << err.what() << endl;
	}
	catch(...)
	{
		ostr << "printSettingsForComponent(): ERROR! Exception caught!" << endl;
	}
}

void SettingsManager::printAllComponentSettings(ostream &ostr)
{
	MutexLocker lock(&updateLock);

	try
	{
		for(MapComponentSettings::iterator it = mapComponentSettings.begin(), it_end = mapComponentSettings.end(); it != it_end; ++it)
		{
			HostSettings &settings = it->second; 
			string componentName = it->first;

			if(settings.IsValid() == false)
				ostr << "printAllComponentSettings(): WARNING! Settings for " << componentName << " are not valid! Please contact system-administrator!" << endl;

			string xmlContent = settings.GetXMLFile().writeToString();
			if(xmlContent.empty())
			{
				ostr << "No settings found for " << componentName << endl;
				continue;
			}
			
			ostr << endl;
			ostr << "-------------------- Settings for " << componentName << " ---------------------------->" << endl;
			ostr << xmlContent << endl;
			ostr << "---------------------------------------------------------------------------------------" << endl;
		}
	}
	catch(std::runtime_error err)
	{
		ostr << "printAllComponentSettings(): ERROR! Exception caught: " << err.what() << endl;
	}
	catch(...)
	{
		ostr << "printAllComponentSettings(): ERROR! Exception caught!" << endl;
	}
}

void SettingsManager::printComponents(ostream &ostr)
{
	MutexLocker lock(&updateLock);

	try
	{
		string xmlContent = backupComponentSettings.writeToString();
		if(xmlContent.empty())
		{
			ostr << "printComponents(): No components registered!" << endl;
			return;
		}
		
		ostr << endl;
		ostr << "------------------------ Components in SettingsManager --------------------------------" << endl;
		ostr << xmlContent << endl;
		ostr << "---------------------------------------------------------------------------------------" << endl;
	}
	catch(std::runtime_error err)
	{
		ostr << "printComponents(): ERROR! Exception caught: " << err.what() << endl;
	}
	catch(...)
	{
		ostr << "printComponents(): ERROR! Exception caught!" << endl;
	}
}

void SettingsManager::printHostInformation(ostream& ostr)
{
	MutexLocker lock(&updateLock);

	ostr << "---------- HostInformation of SettingsListeners in SettingsManage's Database ----------" << endl;

	for(MapComponentInformation::iterator it = mapComponentHostInformation.begin(), it_end = mapComponentHostInformation.end(); it != it_end; ++it)
		ostr << it->second << endl;

	ostr << "---------------------------------------------------------------------------------------" << endl;
}

