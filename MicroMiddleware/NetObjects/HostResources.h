#ifndef MicroMiddleware_HostResources_h_IsIncluded
#define MicroMiddleware_HostResources_h_IsIncluded

#include <stdlib.h>
#include <map>
#include <sstream>
#include <iostream>

#include "NetworkLib/IncludeLibs.h"
#include "MicroMiddleware/RmiObjectBase.h"
#include "MicroMiddleware/NetObjects/HostInformation.h"

using namespace std;

#include "MicroMiddleware/Export.h"

namespace MicroMiddleware
{

class DLL_STATE HostResources : public RmiObjectBase
{
public:
	/**
	* Enumerated ResourceView.
	*/
	enum ResourceView	{	NO_RESOURCE_VIEW = 0, 
							NODE, 
							PROCESS 
						};

	typedef std::map<std::string, double>		MapStringDouble;
	typedef std::map<std::string, int>			MapStringInt32;
	typedef std::map<std::string, long long>	MapStringInt64;

private:
	std::string		componentName;
	std::string		executableName;
	std::string		hostName;
	int				processId;
	double			cpu;
	long long		memory;
	int				executionTime;
	int				fileDescriptors;
	int				numThreads;
	int				numSockets;

	MapStringInt32	mapCPUTemperature;
	MapStringDouble	mapCPUUse;
	MapStringInt32	mapDiskTransfers;
	MapStringInt32	mapDiskRead;
	MapStringInt32  mapDiskWrite;
	MapStringInt64	mapNIAmountSent;
	MapStringInt64	mapNIAmountReceived;

	ResourceView	resourceView;
	
	HostInformation::HostDescription	hostDescription;

private:
	inline void init()
	{
		cpu					= 0; 
		memory				= 0; 
		executionTime		= 0;
		fileDescriptors		= 0;
		numThreads			= 0;
		numSockets			= 0;
	}

	inline void writeMap32(NetworkLib::SerializeWriter *writer, const MapStringInt32 &mapStringInt)
	{
		writer->WriteInt32(mapStringInt.size());
		for(MapStringInt32::const_iterator it = mapStringInt.begin(), it_end = mapStringInt.end(); it != it_end; ++it)
		{
			writer->WriteString(it->first);
			writer->WriteInt32(it->second);
		}
	}
	
	inline void writeMap64(NetworkLib::SerializeWriter *writer, const MapStringInt64 &mapStringInt)
	{
		writer->WriteInt32(mapStringInt.size());
		for(MapStringInt64::const_iterator it = mapStringInt.begin(), it_end = mapStringInt.end(); it != it_end; ++it)
		{
			writer->WriteString(it->first);
			writer->WriteInt64(it->second);
		}
	}
	
	inline void writeMap(NetworkLib::SerializeWriter *writer, const MapStringDouble &mapStringDouble)
	{
		writer->WriteInt32(mapStringDouble.size());
		for(MapStringDouble::const_iterator it = mapStringDouble.begin(), it_end = mapStringDouble.end(); it != it_end; ++it)
		{
			writer->WriteString(it->first);
			writer->WriteDouble(it->second);
		}
	}

	inline void readMap32(NetworkLib::SerializeReader *reader, MapStringInt32 &mapStringInt)
	{
		int size = reader->ReadInt32();
		for(int i = 0; i < size; i++)
		{
			std::string str = reader->ReadString();
			int val = reader->ReadInt32();

			mapStringInt.insert(pair<std::string, int>(str, val));
		}
	}

	inline void readMap64(NetworkLib::SerializeReader *reader, MapStringInt64 &mapStringInt)
	{
		int size = reader->ReadInt32();
		for(int i = 0; i < size; i++)
		{
			std::string str = reader->ReadString();
			long long val = reader->ReadInt64();
			mapStringInt.insert(pair<std::string, long long>(str, val));
		}
	}
	
	inline void readMap(NetworkLib::SerializeReader *reader, MapStringDouble &mapStringDouble)
	{
		int size = reader->ReadInt32();
		for(int i = 0; i < size; i++)
		{
			std::string str = reader->ReadString();
			double val = reader->ReadDouble();
			mapStringDouble.insert(pair<std::string, double>(str, val));
		}
	}

public:
	/**
	* Constructor to copy HostInformation into HostResources.
	*
	* @param	info is the HostInformation to be copied into HostResources.
	*/
	HostResources(const HostInformation &info) : 
			componentName(info.GetComponentName()),		
			executableName(info.GetExecutableName()),
			hostName(info.GetHostName()),
			processId(info.GetProcessId()),
		resourceView(HostResources::PROCESS),
			hostDescription(info.GetHostDescription())
	{
		init();
	}
	
	/**
	* Empty constructor, initializes class variables.
	*/
	HostResources() : 
			processId(-1), 
			resourceView(HostResources::NO_RESOURCE_VIEW), 
			hostDescription(HostInformation::NO_HOST_DESCRIPTION)
	{ 
		init();
	}

	/**
	* HostResources destructor.
	*/
    virtual ~HostResources()
	{ }

	/**
	* Writes HostResources values to a SerializeWriter.
	*/
	virtual void Write(NetworkLib::SerializeWriter *writer)
	{
		writer->WriteString(componentName);
		writer->WriteString(executableName);
		writer->WriteString(hostName);
		writer->WriteInt32(processId);
		writer->WriteDouble(cpu);
		writer->WriteInt64(memory);
		writer->WriteInt32(executionTime);
		writer->WriteInt32(fileDescriptors);
		writer->WriteInt32(numThreads);
		writer->WriteInt32(numSockets);
		writer->WriteInt32((int)resourceView);
		writer->WriteInt32((int)hostDescription);

		writeMap32(writer, mapCPUTemperature);
		writeMap(writer, mapCPUUse);
		writeMap32(writer, mapDiskTransfers);
		writeMap32(writer, mapDiskRead);
		writeMap32(writer, mapDiskWrite);
		writeMap64(writer, mapNIAmountReceived);
		writeMap64(writer, mapNIAmountSent);
	}

	/**
	* Reads HostResources values from a SerializeReader.
	*/
	virtual void Read(NetworkLib::SerializeReader *reader)
	{
		componentName		= reader->ReadString();
		executableName		= reader->ReadString();
		hostName			= reader->ReadString();
		processId			= reader->ReadInt32();
		cpu					= reader->ReadDouble();
		memory				= reader->ReadInt64();
		executionTime		= reader->ReadInt32();
		fileDescriptors		= reader->ReadInt32();
		numThreads			= reader->ReadInt32();
		numSockets			= reader->ReadInt32();
		resourceView		= (ResourceView)reader->ReadInt32();
		hostDescription		= (HostInformation::HostDescription)reader->ReadInt32();
		
		readMap32(reader, mapCPUTemperature);
		readMap(reader, mapCPUUse);
		readMap32(reader, mapDiskTransfers);
		readMap32(reader, mapDiskRead);
		readMap32(reader, mapDiskWrite);
		readMap64(reader, mapNIAmountReceived);
		readMap64(reader, mapNIAmountSent);
	}

	/**
	* Convenience function to update the cpu, memory, etc of the object.
	*/
	/*inline void UpdateResources(const HostResources &update)
	{
		SetCpuUsage(update.GetCpuUsage());
		SetMemoryUsage(update.GetMemoryUsage());
		SetExecutionTime(update.GetExecutionTime());
		SetResourceView(update.GetResourceView());
	
		SetMapCPUUse(update.GetMapCPUUse());
		SetMapNIAmountReceived(update.GetMapNIAmountReceived());
		SetMapNIAmountSent(update.GetMapNIAmountSent());
	}*/
public:
	inline void SetComponentName(std::string name)			{ componentName = name; }
	inline void SetExecutableName(std::string name)			{ executableName = name; }
	inline void SetHostName(std::string name)				{ hostName = name; }
	inline void SetProcessId(int id)						{ processId = id; }
	inline void SetCpuUsage(double c)						{ cpu = c; }
	inline void SetMemoryUsage(long long m)					{ memory = m; }
	inline void SetExecutionTime(int et)					{ executionTime = et; }
	inline void SetResourceView(ResourceView rv)			{ resourceView = rv; }
	inline void SetNumFileDescriptors(int fd)				{ fileDescriptors = fd; }
	inline void SetNumThreads(int th)						{ numThreads = th; }
	inline void SetNumSockets(int s)						{ numSockets = s; }
	
	inline void SetMapCPUTemperature(const MapStringInt32 &mapInt)		{ mapCPUTemperature = mapInt; }
	inline void SetMapCPUUse(const MapStringDouble &mapInt)				{ mapCPUUse = mapInt; }
	inline void SetMapDiskTransfers(const MapStringInt32 &mapInt)		{ mapDiskTransfers = mapInt; }
	inline void SetMapDiskRead(const MapStringInt32 &mapInt)			{ mapDiskRead = mapInt; }
	inline void SetMapDiskWrite(const MapStringInt32 &mapInt)			{ mapDiskWrite = mapInt; }
	inline void SetMapNIAmountSent(const MapStringInt64 &mapInt)		{ mapNIAmountSent = mapInt; }
	inline void	SetMapNIAmountReceived(const MapStringInt64 &mapInt)	{ mapNIAmountReceived = mapInt; }

	inline void SetHostDescription(HostInformation::HostDescription h)	{ hostDescription = h; }

	inline std::string GetComponentName()	const			{ return componentName; }
	inline std::string GetExecutableName()	const			{ return executableName; }
	inline std::string GetHostName()		const			{ return hostName; }
	inline int GetProcessId()				const			{ return processId; }
	inline double GetCpuUsage()				const			{ return cpu; }
	inline long long GetMemoryUsage()		const			{ return memory; }
	inline int GetExecutionTime()			const			{ return executionTime; }
	inline ResourceView GetResourceView()	const			{ return resourceView; }
	inline int GetNumFileDescriptors()		const			{ return fileDescriptors; }
	inline int GetNumThreads()				const			{ return numThreads; }
	inline int GetNumSockets()				const			{ return numSockets; }

	inline MapStringInt32 GetMapCPUTemperature()	const	{ return mapCPUTemperature; }
	inline MapStringDouble GetMapCPUUse()			const	{ return mapCPUUse; }
	inline MapStringInt32 GetMapDiskTransfers()		const	{ return mapDiskTransfers; }
	inline MapStringInt32 GetMapDiskRead()			const	{ return mapDiskRead; }
	inline MapStringInt32 GetMapDiskWrite()			const	{ return mapDiskWrite; }
	inline MapStringInt64 GetMapNIAmountSent()		const	{ return mapNIAmountSent; }
	inline MapStringInt64 GetMapNIAmountReceived()	const	{ return mapNIAmountReceived; }
	
	inline HostInformation::HostDescription GetHostDescription()	const	{ return hostDescription; }
};

// --------------------------------------------------------
//			HostResourcesSpace
// --------------------------------------------------------
/**
* namespace HostResourcesSpace contains convenience functions for class HostResources.
*/
namespace HostResourcesSpace
{
	/**
	* Retrieves enumerated ResourceView.
	*
	* @param resource								char* that identifies ResourceView.
	* @return HostResources::ResourceView		ResourceView retrieved from resource.
	*/
	HostResources::ResourceView GetEnumResourceView(const char *resource);

	/**
	* Parse HostResources from file.
	*
	* @param infile					File to read HostResources from.
	* @param vectorHostResources	Vector to fill with HostResources from infile.
	* @return bool					Success or failure.
	*/
	//bool ParseHostResourcesFromString(istream &infile, vector<HostResources> &vectorHostResources);

} // end namespace HostResourcesSpace

// --------------------------------------------------------
//			operators
// --------------------------------------------------------
DLL_STATE ostream& operator<<(ostream &str, const HostResources &pub);
DLL_STATE ostream& operator<<(ostream &str, const HostResources::ResourceView &stat);
DLL_STATE ostream& operator<<(ostream &str, const HostResources::MapStringInt32 &mapStringInt32);
DLL_STATE ostream& operator<<(ostream &str, const HostResources::MapStringInt64 &mapStringInt64);
DLL_STATE ostream& operator<<(ostream &str, const HostResources::MapStringDouble &mapStringDouble);

} // namespace MicroMiddleware

#endif // HOST_RESOURCES_H


