#ifndef MicroMiddleware_HostInformation_h_IsIncluded
#define MicroMiddleware_HostInformation_h_IsIncluded

#include <stdlib.h>
#include <sstream>
#include <iostream>

#include "MicroMiddleware/IncludeExtLibs.h"
#include "MicroMiddleware/RmiObjectBase.h"
#include "MicroMiddleware/NetObjects/InterfaceHandle.h"

#include "MicroMiddleware/Export.h"

namespace MicroMiddleware
{
/**
* HostInformation contains information to identify components.
*/
class DLL_STATE HostInformation : public RmiObjectBase
{
public:
	// NB! If these are updated, then remember to update HostInformation.cpp: 
	//		- HostInformationSpace::OnlineStatusChar, HostInformationSpace::HostDescriptionChar and HostInformation::StartDescription
	/**
	* Enumerated OnlineStatus.
	*/
	enum OnlineStatus	 {	NO_ONLINE_STATUS = 0, 
							OFFLINE, 
							ONLINE 
	};

	/**
	* Enumerated HostDescription.
	*/
	enum HostDescription {  NO_HOST_DESCRIPTION = 0,
							COMPONENT_FACTORY,
							LOOKUP_SERVER,
							SYSTEM_MANAGER,
							SETTINGS_MANAGER,
							COMPONENT,
							PUBLISHER,
							SUBSCRIBER,
							PROXY,
							STUB
	};

	/**
	* Enumerated StartDescription.
	*/
	enum StartDescription { NO_START_DESCRIPTION = 0,
							STOP,
							START,
							RESTART
	};
public:
	/**
	* Constructor generally used to inform that a component is OFFLINE.
	*
	* @param component		The components Name.
	*/
	HostInformation(const std::string &component)
		: componentName(component), componentId(-1), portNumber(-1), processId(-1), onlineStatus(HostInformation::OFFLINE), hostDescription(HostInformation::NO_HOST_DESCRIPTION), startDescription(HostInformation::NO_START_DESCRIPTION), timeStamp(0)
	{ }

	/**
	* Constructor for creating HostInformation for proxies/stubs/factories that are using TCP.
	*
	* @param component		The components Name.
	* @param host			The host Name on which the component is intended to run.
	* @param port			Port number that the component is using.
	* @param description	HostDescription, default = PROXY.
	* @param online			OnlineStatus, default = ONLINE.
	*/
	HostInformation(const std::string &component, const std::string &host, int port, HostDescription description = HostInformation::PROXY, OnlineStatus online = HostInformation::ONLINE)
		: componentName(component),  componentId(-1), hostName(host), portNumber(port), processId(-1), onlineStatus(online), hostDescription(description), startDescription(HostInformation::NO_START_DESCRIPTION), timeStamp(0)
	{ }

	/**
	* Constructor for creating HostInformation for publishers/subscribers that are using UDP Multicast.
	*
	* @param component		The components Name.
	* @param host			The host Name on which the component is intended to run.
	* @param multicast		The multicast-address the component is using.
	* @param port			Port number that the component is using.
	* @param description	HostDescription, default = PUBLISHER.
	*/
	HostInformation(const std::string &component, const std::string &host, const std::string &multicast = std::string(""), int port = -1, HostDescription description = HostInformation::PUBLISHER)
		: componentName(component), componentId(-1), hostName(host), multicastAddress(multicast), portNumber(port), processId(-1), onlineStatus(HostInformation::ONLINE), hostDescription(description), startDescription(HostInformation::NO_START_DESCRIPTION), timeStamp(0)
	{ }
	
	/**
	* Empty constructor generally used a return value to inform that something went wrong.
	*/
	HostInformation() 
		: componentId(-1), portNumber(-1), processId(-1), onlineStatus(HostInformation::OFFLINE), hostDescription(HostInformation::NO_HOST_DESCRIPTION), startDescription(HostInformation::NO_START_DESCRIPTION), timeStamp(0)
	{ }

	/**
	* Destructor for HostInformation.
	*/
    virtual ~HostInformation()
	{ }

	virtual void Write(NetworkLib::SerializeWriter *writer)
	{
		writer->WriteString(componentName);
		writer->WriteInt32(componentId);
		writer->WriteString(connectedToName);
		writer->WriteString(interfaceName);
		writer->WriteString(executableName);
		writer->WriteString(hostName);
		writer->WriteString(ipAddress);
		writer->WriteString(multicastAddress);
		writer->WriteInt32(portNumber);
		writer->WriteInt32(processId);
		writer->WriteInt32((int)onlineStatus);
		writer->WriteInt32((int)hostDescription);
		writer->WriteString(argumentList);
		writer->WriteInt32(startDescription);
		writer->WriteInt32(timeStamp);

		hostHandle.Write(writer);
		remoteHandle.Write(writer);
	}

	virtual void Read(NetworkLib::SerializeReader *reader)
	{
		componentName		= reader->ReadString();
		componentId			= reader->ReadInt32();
		connectedToName		= reader->ReadString();
		interfaceName		= reader->ReadString();
		executableName		= reader->ReadString();
		hostName			= reader->ReadString();
		ipAddress			= reader->ReadString(); 
		multicastAddress	= reader->ReadString();
		portNumber			= reader->ReadInt32();
		processId			= reader->ReadInt32();
		onlineStatus		= (OnlineStatus)reader->ReadInt32();
		hostDescription		= (HostDescription)reader->ReadInt32();
		argumentList		= reader->ReadString();
		startDescription	= (StartDescription)reader->ReadInt32();
		timeStamp			= reader->ReadInt32();

		hostHandle.Read(reader);
		remoteHandle.Read(reader);
	}

	bool CompareTo(const HostInformation &info) const
	{
		if(IsValid() == false) return false;

		if(GetComponentName() != info.GetComponentName()) return false;
		if(GetComponentId() != info.GetComponentId()) return false;
		if(GetConnectedToName() != info.GetConnectedToName()) return false;
		if(GetInterfaceName() != info.GetInterfaceName()) 	return false;
		if(GetExecutableName() != info.GetExecutableName()) return false;
		if(GetHostName() != info.GetHostName()) return false;
		if(GetHostIP() != info.GetHostIP()) return false;
		if(GetMulticastAddress() != info.GetMulticastAddress()) return false;
		if(GetPortNumber() != info.GetPortNumber()) return false;
		if(GetProcessId() != info.GetProcessId()) return false;
		if(GetOnlineStatus() != info.GetOnlineStatus()) return false;
		if(GetHostDescription() != info.GetHostDescription()) return false;
		if(GetArgumentList() != info.GetArgumentList()) return false;
		if(GetStartDescription() != info.GetStartDescription()) return false;
		//if(GetTimeStamp() != info.GetTimeStamp()) return false;
	
		return true;
	}

	bool IsValid() const
	{ 	
		if(GetComponentName().empty() == true) 
			return false; 
		return true; 
	}

public:
	inline void SetComponentName(std::string Name)			{ componentName = Name; }
	inline void SetComponentId(int id)						{ componentId = id; }
	inline void SetConnectedToName(std::string Name)		{ connectedToName = Name; }
	inline void SetInterfaceName(std::string Name)			{ interfaceName = Name; }
	inline void SetExecutableName(std::string Name)			{ executableName = Name; }
	inline void SetHostName(std::string Name)				{ hostName = Name; }
	inline void SetHostIP(std::string ip)					{ ipAddress = ip; }
	inline void SetMulticastAddress(std::string a)			{ multicastAddress = a; }
	inline void SetPortNumber(int number)					{ portNumber = number; }
	inline void SetProcessId(int id)						{ processId = id; }
	inline void SetOnlineStatus(OnlineStatus st)			{ onlineStatus = st; }
	inline void SetHostDescription(HostDescription h)		{ hostDescription = h; }
	inline void SetArgumentList(std::string arg)					{ argumentList = arg; }
	inline void SetStartDescription(StartDescription s)		{ startDescription = s; }
	inline void SetTimeStamp(int t)							{ timeStamp = t; }
	inline void SetHostHandle(const InterfaceHandle &h) 	{ hostHandle = h; }
	inline void SetRemoteHandle(const InterfaceHandle &h) 	{ remoteHandle = h; }
	
	inline std::string GetComponentName()			const	{ return componentName; }
	inline int GetComponentId()						const	{ return componentId; }
	inline std::string GetConnectedToName()			const 	{ return connectedToName; }
	inline std::string GetInterfaceName()			const	{ return interfaceName; }
	inline std::string GetExecutableName()			const	{ return executableName; }
	inline std::string GetHostName()				const	{ return hostName; }
	inline std::string GetHostIP()					const	{ return ipAddress; }
	inline std::string GetMulticastAddress()		const	{ return multicastAddress; }
	inline int GetPortNumber()						const	{ return portNumber; }
	inline int GetProcessId()						const	{ return processId; }
	inline OnlineStatus GetOnlineStatus()			const	{ return onlineStatus; }
	inline HostDescription GetHostDescription()		const	{ return hostDescription; }
	inline std::string GetArgumentList()			const	{ return argumentList; }
	inline StartDescription GetStartDescription() 	const	{ return startDescription; }
	inline int GetTimeStamp()						const	{ return timeStamp; }

	inline InterfaceHandle GetHostHandle()			const  	{ return hostHandle; }
	inline InterfaceHandle GetRemoteHandle()		const 	{ return remoteHandle; }

private:
	std::string			componentName;
	int					componentId;
	std::string			connectedToName;
	std::string			interfaceName;
	std::string			executableName;
	std::string			hostName;
	std::string			ipAddress;
	std::string			multicastAddress;
	int					portNumber;
	int					processId;
	OnlineStatus		onlineStatus;
	HostDescription		hostDescription;
	std::string			argumentList;
	StartDescription	startDescription;
	int					timeStamp;

	InterfaceHandle		hostHandle;
	InterfaceHandle		remoteHandle;
};

// --------------------------------------------------------
//			HostInformationSpace
// --------------------------------------------------------
/**
* namespace HostInformationSpace contains convenience functions for class HostInformation, etc.
*/
namespace HostInformationSpace
{
	/**
	* Retrieves enumerated HostDescription.
	*
	* @param description						char* that identifies HostDescription.
	* @return HostInformation::HostDescription	HostDescription retrieved from description.
	*/
    DLL_STATE HostInformation::HostDescription GetEnumHostDescription(const char *description);

	/**
	* Retrieves enumerated OnlineStatus.
	*
	* @param status								char* that identifies OnlineStatus.
	* @return HostInformation::OnlineStatus		OnlineStatus retrieved from status.
	*/
    DLL_STATE HostInformation::OnlineStatus GetEnumOnlineStatus(const char *status);
	
	/**
	* Retrieves enumerated StartDescription.
	*
	* @param start								char* that identifies StartDescription.
	* @return HostInformation::StartDescription	StartDescription retrieved from status.
	*/
    DLL_STATE HostInformation::StartDescription GetEnumStartDescription(const char *start);

	/**
	* Writes HostInformation to outfile.
	*
	* @param outfile		File to write HostInformation.
	* @param info			HostInformation to write to outfile.
	* @return bool			Success or failure.
	*/
    DLL_STATE bool WriteHostInformationToDisc(ofstream &outfile, HostInformation info);

	/**
	* Writes HostInformation to filename.
	*
	* @param filename		File to write HostInformation.
	* @param info			HostInformation to write to filename.
	* @return bool			Success or failure.
	*/
    DLL_STATE bool WriteHostInformationToDisc(std::string filename, HostInformation info);

	/**
	* Parse HostInformation from file.
	*
	* @param filename				File to read HostInformation from.
	* @param vectorHostInformation	Vector to fill with HostInformation from filename.
	* @return bool					Success or failure.
	*/
    DLL_STATE bool ParseHostInformationFromDisc(std::string filename, vector<HostInformation> &vectorHostInformation);

	/**
	* Parse HostInformation from file.
	*
	* @param infile					File to read HostInformation from.
	* @param vectorHostInformation	Vector to fill with HostInformation from infile.
	* @return bool					Success or failure.
	*/
    DLL_STATE bool ParseHostInformationFromDisc(istream &infile, vector<HostInformation> &vectorHostInformation);
	
	/**
	* Verify that ipAddress is on hostName.
	*
	* @param hostName				HostName to retrieve IP from.
	* @param ipAddress				Ip address to verify is on hostName.
	* @return bool					True, if ipAddress found on hostName.
	*/
	//bool IsIPAddressOnHost(std::string hostName, std::string ipAddress);

	/**
	* Retrieve IP address from hostName.
	*
	* @param hostName				HostName to retrieve IP from.
	* @return std::string				IP address that matches hostName.
	*/
	//std::string GetIPAddress(std::string hostName);

	/**
	* Retrieve executable name from std::string command.
	*
	* @param cmd						Command line used to start component in ComponentFactory.
	* @param executableFileNameSize 	Size of executable file name std::string.
	* @return std::string				Executable name.
	*/
    DLL_STATE std::string GetExecutableFileName(std::string cmd, int executableFileNameSize);
	
	/**
	* Generates a timestamp based on current time.
	*/
	//int GetTimeStamp();

	/**
	* Used to tokenize (split) std::string according to the delimiters, result is added to the vector<std::string> tokens.
	*
	* @param str			String to tokenize (split).
	* @param tokens			Vector to add results of tokenizing.
	* @param delimiters		Delimiters used to determine tokenizing (splits).
	*/
    DLL_STATE void TokenizeString(const std::string& str, vector<std::string>& tokens, const std::string& delimiters = " ");

	/**
	* Used internally by ParsehostInformationFromDisc, to retrieve argument from vectorString.
	*
	* @param vectorString		Vector with arguments.
	* @param i					Current position in vectorString.
	* @param nextSubstr			Stop when nextSubstr is found
	* @return					String found in the range <i, nestSubstr> in vectorString.
	*/
	//std::string getArgument(const vector<std::string> &vectorString, size_t &i, std::string nextSubstr);
} 
// end namespace HostInformationSpace

// --------------------------------------------------------
//			operators
// --------------------------------------------------------
DLL_STATE ostream& operator<<(ostream &str, const HostInformation &pub);
DLL_STATE ostream& operator<<(ostream &str, const HostInformation::HostDescription &stat);
DLL_STATE ostream& operator<<(ostream &str, const HostInformation::OnlineStatus &stat);
DLL_STATE ostream& operator<<(ostream &str, const HostInformation::StartDescription &start);

}

#endif // HOST_INFORMATION_H



