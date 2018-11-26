#include "ProcessFactory/ResourceMonitor.h"
#include "ProcessFactory.h"
#include "ProcessFactory/FactoryLibrary.h"


namespace ProcessFactorySpace
{

// -------------------------------------------------------
//	local functions
// -------------------------------------------------------
string AvailableBytes(string executableName)  
{
    string executable(executableName);
    executable.resize(executable.size() - 4);

    size_t pos = executable.find_last_of("\\");
    executable = executable.substr(pos+1);

	return string("\\Process(" + executable + ")\\Working Set");
}

string ProcessTime(string executableName)		
{
	string executable(executableName);
    executable.resize(executable.size() - 4);

    size_t pos = executable.find_last_of("\\");
    executable = executable.substr(pos+1);

	return string("\\Process(" + executable + ")\\% Processor Time");
}

string FileDescriptorCount(string executableName)		
{
	string executable(executableName);
    executable.resize(executable.size() - 4);

    size_t pos = executable.find_last_of("\\");
    executable = executable.substr(pos+1);

	return string("\\Process(" + executable + ")\\Handle Count");
}

string ThreadCount(string executableName)		
{
	string executable(executableName);
    executable.resize(executable.size() - 4);

    size_t pos = executable.find_last_of("\\");
    executable = executable.substr(pos+1);

	return string("\\Process(" + executable + ")\\Thread Count");
}

// -------------------------------------------------------
//		constructor/destructor ResourceMonitor
// -------------------------------------------------------
ResourceMonitor::ResourceMonitor(string hostName, int interval, bool autoStart) 
	: Thread(), 
	  nodeHostName(hostName),
	  monitorInterval(interval),
	  processId(Thread::processId()),
	  monitorResources(true),
	  notifySystemManagerThread(NULL)
{
	this->addNodeProcess();

	if(autoStart == true) start();
}

ResourceMonitor::~ResourceMonitor()
{
	guard.lock();
	if(monitorResources == false)
	{
		guard.unlock();
	}
	else
	{
		monitorResources = false;
		guard.unlock();
		//msleep(monitorInterval);
	}
}

// -------------------------------------------------------
//			run()
// -------------------------------------------------------
void ResourceMonitor::run()
{
#ifdef USE_GCC
	runOnLinux();
#else
	runOnWindows();
#endif
}

void ResourceMonitor::runOnWindows()
{
#ifndef USE_GCC
	while(monitorResources)
	{
		msleep(monitorInterval);
		
        MutexLocker lock(&guard);
		
		if(monitorResources == false) break;

		// retrieve resources used by all processes, and the node
		for(MapComponentResources::iterator it = mapComponentResources.begin(), it_end = mapComponentResources.end(); it != it_end; ++it)
		{
			HostResources &res = it->second;

			if(mapPidPdhQuery.count(res.GetProcessId()) <= 0)
			{
				cout << "ResourceMonitor::runOnWindows(): WARNING! No PdhQuery associated with process id. Res: " << res << endl;
				continue;
			}
			
			PdhQuery *query = mapPidPdhQuery[res.GetProcessId()];
			ASSERT(query);

			query->updateCounters();

			if(res.GetResourceView() == HostResources::NODE)			// -> node
			{
				setNodeResources(query, res);
			}
			else if(res.GetResourceView() == HostResources::PROCESS)	// -> component
			{
				setProcessResources(query, res);
			}
			else
				cout << "ResourceMonitor::runOnWindows(): WARNING! Not identified : " << res << endl;
			
			//printf("%d cpu %3d%% \tavail (%d kb) \n", res.GetProcessId(), res.GetCpuUsage(), res.GetMemoryUsage());
		}

		if(notifySystemManagerThread)
			notifySystemManagerThread->addAndWakeup(mapComponentResources);
	}
#endif
}

void ResourceMonitor::runOnLinux()
{
	int64 prevCPUTotal = 0, prevCPUIdle = 0;
	ProcessFactoryLibrary::Linux::MetricStateWrapper64 sentState, recvState;
	map<string, pair<int64, int64> > prevCPUStats;
	HostResources::MapStringDouble currCPUStats;

	vector<string> vectorPartitions;
	ProcessFactoryLibrary::Linux::MetricStateWrapper32 diskTransfer, diskRead, diskWrite;

	map<int, int64> mapPidTotalCPUUsage;
	int64 prevTotalCPU = 0;

	while(true)
	{
		// -- new impl --
		if(prevCPUStats.empty() == false)
			prevTotalCPU = prevCPUStats.begin()->second.second;
		// -- end new impl --

		msleep(monitorInterval);
		
		MutexLocker lock(&guard);

		int64 currentTotalCPU = 0;
		vector<int> removeDeadProcesses;
		// retrieve resources used by all processes, and the node
		for(MapComponentResources::iterator it = mapComponentResources.begin(), it_end = mapComponentResources.end(); it != it_end; ++it)
		{
			HostResources &res = it->second;
			if(res.GetResourceView() == HostResources::NODE)
			{
				double cpuUsage = ProcessFactoryLibrary::Linux::getCPUUsagePercent(prevCPUTotal, prevCPUIdle);
				ProcessFactoryLibrary::Linux::getNetworkTraffic(sentState, recvState);
				ProcessFactoryLibrary::Linux::getCPUUsagePercent(prevCPUStats, currCPUStats);
				ProcessFactoryLibrary::Linux::getDiskTraffic(monitorInterval/1000, vectorPartitions, diskTransfer, diskRead, diskWrite);
				
				ProcessFactoryLibrary::Linux::getNodeResources(res);
				res.SetMapCPUTemperature(ProcessFactoryLibrary::Linux::GetCPUTemperatures());
				res.SetCpuUsage(cpuUsage);
				res.SetMapCPUUse(currCPUStats);
				res.SetMapDiskTransfers(diskTransfer.metricState);
				res.SetMapDiskRead(diskRead.metricState);
				res.SetMapDiskWrite(diskWrite.metricState);
				res.SetMapNIAmountSent(sentState.metricState);
				res.SetMapNIAmountReceived(recvState.metricState);
				res.SetNumFileDescriptors(ProcessFactoryLibrary::Linux::GetNumberOfFileDescriptors());
				res.SetNumSockets(ProcessFactoryLibrary::Linux::GetNumberOfSockets());
			}
			else if(res.GetResourceView() == HostResources::PROCESS)
			{
				bool ret = ProcessFactoryLibrary::Linux::getProcessResources(res.GetProcessId(), res);
				if(ret == false)
				{
					//cout << "ResourceMonitor::runOnLinux(): Removed dead process with pid  " << res.GetProcessId() << endl;
					removeDeadProcesses.push_back(res.GetProcessId());
					break;
				}
				else
				{
					if(mapPidTotalCPUUsage.count(res.GetProcessId()) <= 0)
						mapPidTotalCPUUsage.insert(pair<int, int64>(res.GetProcessId(), 0));

					if(currentTotalCPU == 0)
					{
						map<string, pair<int64, int64> > currStats = prevCPUStats;
						map<string, double>	currentCPUStats;
						ProcessFactoryLibrary::Linux::getCPUUsagePercent(currStats, currentCPUStats);
						currentTotalCPU = currStats.begin()->second.second;
					}

					double cpuUsage = ProcessFactoryLibrary::Linux::getCPUUsagePercent(res.GetProcessId(), mapPidTotalCPUUsage[res.GetProcessId()], prevTotalCPU, currentTotalCPU);
					res.SetCpuUsage(cpuUsage);
				
					int numThreads = ProcessFactoryLibrary::Linux::GetNumberOfThreads(res.GetProcessId());
					res.SetNumThreads(numThreads);
					
					int fd = ProcessFactoryLibrary::Linux::GetNumberOfOpenFileDescriptors(res.GetProcessId());
					res.SetNumFileDescriptors(fd);
				}
			}
			else
				cout << "ResourceMonitor::runOnLinux(): WARNING! Not identified : " << res << endl;
			//printf("%d cpu %3d%% \tavail (%d kb) \n", res.GetProcessId(), res.GetCpuUsage(), res.GetMemoryUsage());
		}
	
		// -- remove dead processes --
		for(vector<int>::iterator it = removeDeadProcesses.begin(), it_end = removeDeadProcesses.end(); it != it_end; ++it)
			mapComponentResources.erase(*it);

		// -- update SystemManager --
		if(notifySystemManagerThread)
			notifySystemManagerThread->addAndWakeup(mapComponentResources);
	}
}

void ResourceMonitor::addNodeProcess()
{
	MutexLocker lock(&guard);

	HostResources nodeResources;
	
	nodeResources.SetResourceView(HostResources::NODE);
	nodeResources.SetHostName(nodeHostName);
	nodeResources.SetComponentName(nodeHostName);
	nodeResources.SetProcessId(1);			// == 1 which is the init process, otherwise should be this->processId
	nodeResources.SetHostDescription(HostInformation::COMPONENT_FACTORY);

	if(mapComponentResources.count(nodeResources.GetProcessId()) > 0)
		cout << "ResourceMonitor::addNodeProcess(): WARNING! node resource already exists! " << endl;

	mapComponentResources.insert(pair<int, HostResources>(nodeResources.GetProcessId(), nodeResources));

#ifdef USE_GCC
	// TODO
#else
	PdhQuery *newQuery = new PdhQuery();
	newQuery->init();
	
	newQuery->addCounter("\\Processor(_Total)\\% Processor Time");
	newQuery->addCounter("\\Memory\\Available Bytes");
	newQuery->addCounter("\\Server(_Total)\\Files Open");
	newQuery->addCounter("\\TCP\\Connections Established");
	//newQuery->addCounter("\\UDP\\Connections Established");

#if 0
    newQuery->addWildCardCounter("\\Network Interface(*//*#*)\\Bytes Received/sec");
    newQuery->addWildCardCounter("\\Network Interface(*//*#*)\\Bytes Sent/sec");
    newQuery->addWildCardCounter("\\Processor(*//*#*)\\% Processor Time");
    newQuery->addWildCardCounter("\\PhysicalDisk(*//*#*)\\Disk Transfers/sec");
    //newQuery->addWildCardCounter("\\PhysicalDisk(*/*#*)\\% Disk Read Time");
    //newQuery->addWildCardCounter("\\PhysicalDisk(*/*#*)\\% Disk Write Time");

    newQuery->addWildCardCounter("\\PhysicalDisk(*//*#*)\\Avg. Disk Bytes/Read");
    newQuery->addWildCardCounter("\\PhysicalDisk(*//*#*)\\Avg. Disk Bytes/Write");
    //newQuery->addWildCardCounter("\\PhysicalDisk(*/*#*)\\Disk Read Bytes/sec");
    //newQuery->addWildCardCounter("\\PhysicalDisk(*/*#*)\\Disk Write Bytes/sec");
#elif 1
    newQuery->addWildCardCounter("\\Network Interface(*)\\Bytes Received/sec");
    newQuery->addWildCardCounter("\\Network Interface(*)\\Bytes Sent/sec");
    newQuery->addWildCardCounter("\\Processor(*)\\% Processor Time");
    newQuery->addWildCardCounter("\\PhysicalDisk(*)\\Disk Transfers/sec");
	//newQuery->addWildCardCounter("\\PhysicalDisk(*/*#*)\\% Disk Read Time");
	//newQuery->addWildCardCounter("\\PhysicalDisk(*/*#*)\\% Disk Write Time");

    newQuery->addWildCardCounter("\\PhysicalDisk(*)\\Avg. Disk Bytes/Read");
    newQuery->addWildCardCounter("\\PhysicalDisk(*)\\Avg. Disk Bytes/Write");
	//newQuery->addWildCardCounter("\\PhysicalDisk(*/*#*)\\Disk Read Bytes/sec");
	//newQuery->addWildCardCounter("\\PhysicalDisk(*/*#*)\\Disk Write Bytes/sec");
	
    mapPidPdhQuery.insert(pair<int, PdhQuery*>(nodeResources.GetProcessId(), newQuery));
#endif

#endif
}

// -------------------------------------------------------
//	Process/Node resources
// -------------------------------------------------------
#ifndef USE_GCC
void ResourceMonitor::setNodeResources(PdhQuery *query, HostResources &res)
{
	res.SetCpuUsage(query->getCounter("\\Processor(_Total)\\% Processor Time"));
	res.SetMemoryUsage(query->getCounter64("\\Memory\\Available Bytes"));
	res.SetNumFileDescriptors(query->getCounter("\\Server(_Total)\\Files Open"));
	res.SetNumSockets(query->getCounter("\\TCP\\Connections Established"));
	
#if 0
	HostResources::MapStringInt64 mapCounterToValue;
	query->getWildCardCounters("\\Network Interface(*/*#*)\\Bytes Received/sec", mapCounterToValue);
	res.SetMapNIAmountReceived(mapCounterToValue);

	mapCounterToValue.clear();
	query->getWildCardCounters("\\Network Interface(*/*#*)\\Bytes Sent/sec", mapCounterToValue);
	res.SetMapNIAmountSent(mapCounterToValue);

	HostResources::MapStringDouble mapCPUToValue;
	query->getWildCardCounters("\\Processor(*/*#*)\\% Processor Time", mapCPUToValue);
	res.SetMapCPUUse(mapCPUToValue);

	HostResources::MapStringInt32 mapToValue;
	query->getWildCardCounters("\\PhysicalDisk(*/*#*)\\Disk Transfers/sec", mapToValue);
	res.SetMapDiskTransfers(mapToValue);

	mapToValue.clear();
	query->getWildCardCounters("\\PhysicalDisk(*/*#*)\\Avg. Disk Bytes/Read", mapToValue);
	res.SetMapDiskRead(mapToValue);

	mapToValue.clear();
	query->getWildCardCounters("\\PhysicalDisk(*/*#*)\\Avg. Disk Bytes/Write", mapToValue);
	res.SetMapDiskWrite(mapToValue);
#elif 1
    HostResources::MapStringInt64 mapCounterToValue;
    query->getWildCardCounters("\\Network Interface(*)\\Bytes Received/sec", mapCounterToValue);
    res.SetMapNIAmountReceived(mapCounterToValue);

    mapCounterToValue.clear();
    query->getWildCardCounters("\\Network Interface(*)\\Bytes Sent/sec", mapCounterToValue);
    res.SetMapNIAmountSent(mapCounterToValue);

    HostResources::MapStringDouble mapCPUToValue;
    query->getWildCardCounters("\\Processor(*)\\% Processor Time", mapCPUToValue);
    res.SetMapCPUUse(mapCPUToValue);

    HostResources::MapStringInt32 mapToValue;
    query->getWildCardCounters("\\PhysicalDisk(*)\\Disk Transfers/sec", mapToValue);
    res.SetMapDiskTransfers(mapToValue);

    mapToValue.clear();
    query->getWildCardCounters("\\PhysicalDisk(*)\\Avg. Disk Bytes/Read", mapToValue);
    res.SetMapDiskRead(mapToValue);

    mapToValue.clear();
    query->getWildCardCounters("\\PhysicalDisk(*)\\Avg. Disk Bytes/Write", mapToValue);
    res.SetMapDiskWrite(mapToValue);
#endif
}

void ResourceMonitor::setProcessResources(PdhQuery *query, HostResources &res)
{
	res.SetCpuUsage(query->getCounter((char*)ProcessTime(res.GetExecutableName()).c_str()));
	res.SetMemoryUsage(query->getCounter64((char*)AvailableBytes(res.GetExecutableName()).c_str()));
	res.SetNumThreads(query->getCounter((char*)ThreadCount(res.GetExecutableName()).c_str()));
	res.SetNumFileDescriptors(query->getCounter((char*)FileDescriptorCount(res.GetExecutableName()).c_str()));
}
#endif

// -------------------------------------------------------
//	public add/remove process
// -------------------------------------------------------
void ResourceMonitor::AddProcess(const HostInformation &info) 
{	
	MutexLocker lock(&guard);

	// -- start checks --
	if(info.GetProcessId() < 0 || info.GetComponentName().empty())
	{
		stringstream stream;
		stream << "ResourceMonitor::AddProcess(info): WARNING! Invalid HostInformation: " << info << endl;
		cout << stream.str();
		ExceptionMulticast::PostException(stream.str(), MiddlewareException::DATA_ERROR);
		return;
	}
	if(mapComponentResources.count(info.GetProcessId()))
		cout << "ResourceMonitor::AddProcess(info): WARNING! " << info.GetComponentName() << " is already being monitored! " << endl;
	// -- end checks --

	mapComponentResources.insert(pair<int, HostResources>(info.GetProcessId(), HostResources(info)));

#ifdef USE_GCC
	// TODO
#else

	// -- start check --
	if(info.GetExecutableName().empty())
	{
		stringstream stream;
		stream << "ResourceMonitor::AddProcess(info): WARNING! Missing executable name: " << info << endl;
		cout << stream.str();
		ExceptionMulticast::PostException(stream.str(), MiddlewareException::DATA_ERROR);
		return;
	}
	// -- end check --

	PdhQuery *newQuery = new PdhQuery();
	newQuery->init();
	newQuery->addCounter((char*)ProcessTime(info.GetExecutableName()).c_str());
	newQuery->addCounter((char*)AvailableBytes(info.GetExecutableName()).c_str());
	newQuery->addCounter((char*)ThreadCount(info.GetExecutableName()).c_str());
	newQuery->addCounter((char*)FileDescriptorCount(info.GetExecutableName()).c_str());	

	mapPidPdhQuery.insert(pair<int, PdhQuery*>(info.GetProcessId(), newQuery));
#endif
}

void ResourceMonitor::RemoveProcess(const HostInformation &info) 
{	
	MutexLocker lock(&guard);

	if(!mapComponentResources.count(info.GetProcessId())) 
	{
		//cerr << "Warning! " << info.GetComponentName() << " not found among monitored processes! " << endl;
		return;
	}

#ifndef USE_GCC
	if(mapPidPdhQuery.count(info.GetProcessId()) > 0)
	{
		PdhQuery *query = mapPidPdhQuery[info.GetProcessId()];
		ASSERT(query);
		delete query;
		mapPidPdhQuery.erase(info.GetProcessId());
	}
#endif

	mapComponentResources.erase(info.GetProcessId());
}

// -------------------------------------------------------
//	public Process/Node resources
//	- if id == -1 -> node resources else process resources
// -------------------------------------------------------
HostResources ResourceMonitor::GetResources(int id) 
{
	MutexLocker lock(&guard);
	
	if(id <= -1)
	{
		for(MapComponentResources::iterator it = mapComponentResources.begin(), it_end = mapComponentResources.end(); it != it_end; ++it)
			if(it->second.GetResourceView() == HostResources::NODE)
				return it->second;
	}
	else if(!mapComponentResources.count(id))
	{
		cout << "ResourceMonitor::getResources(id): WARNING! " << id << " has no statistics yet!" << endl;
		return HostResources();
	}

	return mapComponentResources[id];
}

string ResourceMonitor::GetStringAllResources()
{
	MutexLocker lock(&guard);

	stringstream stream;
	for(MapComponentResources::iterator it = mapComponentResources.begin(), it_end = mapComponentResources.end(); it != it_end; ++it)
		stream << it->second << endl;

	return stream.str();
}

map<int, HostResources> ResourceMonitor::GetMapAllResources()
{
	MutexLocker lock(&guard);

	MapComponentResources mapResources;
	for(MapComponentResources::const_iterator it = mapComponentResources.begin(), it_end = mapComponentResources.end(); it != it_end; ++it)
		mapResources.insert(pair<int, HostResources>(it->first, it->second));

	return mapResources;
}

} // namespace ProcessFactorySpace

