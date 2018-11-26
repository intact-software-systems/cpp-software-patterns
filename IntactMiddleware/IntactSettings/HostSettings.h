#ifndef IntactMiddleware_IntactSettings_HostSettings_h_IsIncluded
#define IntactMiddleware_IntactSettings_HostSettings_h_IsIncluded

#include <map>
#include <stdlib.h>
#include <sstream>
#include <iostream>

#include"IntactMiddleware/IncludeExtLibs.h"
#include"IntactMiddleware/Export.h"
namespace IntactMiddleware
{

/**
* HostSettings contains information to identify components.
*/
class DLL_STATE HostSettings
{
private:
	HostInformation		hostInformation;
    std::string			configSettings;
	ConfigurationFile	xmlFile;
    std::string			xsdFile;
    std::string			fileName;
    std::string			configPath;

public:
	HostSettings(HostInformation hostInfo = HostInformation()) : hostInformation(hostInfo)
	{ }
    HostSettings(std::string componentName) : hostInformation(componentName)
	{ }
	virtual ~HostSettings() 
	{ }
	HostSettings(const HostSettings &settings) 
		: hostInformation(settings.GetHostInformation())
		, configSettings(settings.GetConfSettings())
		, xsdFile(settings.GetXSDFile())
		, fileName(settings.GetFileName())
		, configPath(settings.GetConfigPath())
	{
	
	}

	HostSettings& operator = (const HostSettings &other)
	{
		if(this != &other)
		{
			hostInformation = other.GetHostInformation();
			configSettings = other.GetConfSettings();
			xsdFile = other.GetXSDFile();
			fileName = other.GetFileName();
			configPath = other.GetConfigPath();
		}		
		return *this;
	}

	inline void Write(SocketWriter *writer)
	{
		hostInformation.Write(writer);
		writer->WriteString(configSettings);
		writer->WriteString(xsdFile);
		writer->WriteString(fileName);
		writer->WriteString(configPath);
	}

	inline void Read(SocketReader *reader)
	{
		hostInformation.Read(reader);
		configSettings = reader->ReadString();
		xsdFile = reader->ReadString();
		fileName = reader->ReadString();
		configPath = reader->ReadString();
	}

	bool IsValid()
	{
		return !(hostInformation.GetComponentName().empty());
	}

public:
	inline void SetHostInformation(const HostInformation &in)	{ hostInformation = in; }
    inline void SetConfSettings(const std::string &st)			{ configSettings = st; }
	inline void SetXMLFile(const ConfigurationFile &file)		{ xmlFile = file; }
    inline void SetFileName(std::string name)					{ fileName = name; }
    inline void SetConfigPath(std::string path)					{ configPath = path; }
    inline void SetXSDFile(std::string file)					{ xsdFile = file; }
	
    inline std::string GetComponentName()					const		{ return hostInformation.GetComponentName(); }
    inline HostInformation GetHostInformation()				const		{ return hostInformation; }
    inline std::string GetConfSettings()					const		{ return configSettings; }
    inline ConfigurationFile& GetXMLFile()								{ return xmlFile; }
    inline std::string GetFileName()						const		{ return fileName; }
    inline std::string GetConfigPath()						const		{ return configPath; }
    inline std::string GetXSDFile()							const		{ return xsdFile; }
};

// --------------------------------------------------------
//			operators
// --------------------------------------------------------
DLL_STATE std::ostream& operator<<(std::ostream &str, const HostSettings &pub);

} // namespace IntactMiddleware

#endif // IntactMiddleware_IntactSettings_HostSettings_h_IsIncluded

