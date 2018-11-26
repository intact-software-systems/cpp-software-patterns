#include "NetworkInformationClient/ActiveMeasurement.h"

namespace NetworkInformationClient
{

/*-----------------------------------------------------------------------
						class ActiveMeasurement
------------------------------------------------------------------------- */
ActiveMeasurement::ActiveMeasurement(string name, MeasurementId id, int measureFrequency) 
	: Thread(name)
	, measureFrequency_(measureFrequency)
	, measurementId_(id)
	, runThread_(true)
{
	start();
}

ActiveMeasurement::~ActiveMeasurement()
{
	mutex_.lock();
		runThread_ = false;
		waitCondition_.wakeAll();
	mutex_.unlock();

	wait();
}

/*-----------------------------------------------------------------------
					main thread	
------------------------------------------------------------------------- */
void ActiveMeasurement::run()
{
	IDEBUG() << "running";

	while(runThread_)
	{
		mutex_.lock();
		
		bool ret = waitCondition_.wait(&mutex_, measureFrequency_);
		if(runThread_ == false) break;
		
		MapGroupNameGroupInformation tempGroupInformation = mapGroupInformation_;
		mutex_.unlock();
		
		for(MapGroupNameGroupInformation::iterator it = tempGroupInformation.begin(), it_end = tempGroupInformation.end(); it != it_end; ++it)
		{
			GroupInformation &groupInformation = it->second;
			const MapHostInformation &mapHostInformation = groupInformation.GetGroupMembers();
		
			for(MapHostInformation::const_iterator it = mapHostInformation.begin(), it_end = mapHostInformation.end(); it != it_end; ++it)
			{
				const HostInformation &hostInfo = it->second;
				double minimumRtt = executeMeasurement(hostInfo);
				mapComponentConnection_[hostInfo.GetComponentName()] = NetworkConnectionInformation(minimumRtt);
			}
		}
	}

	mutex_.unlock();
}

double ActiveMeasurement::executeMeasurement(const HostInformation &hostInfo)
{
	// call pinger ?
	// call traceroute ?
	// call icmp pinger ?
	//IDEBUG() << " Pinging " << hostInfo.GetHostIP() ;
	double minimumRtt = 0.0f;

	switch(measurementId_)
	{
		case ActiveMeasurement::PingICMP:
		{
			minimumRtt = NetworkFunctions::my_ping(hostInfo.GetHostIP().c_str(), 1);
			IDEBUG() << " Pinging " << hostInfo.GetHostIP() << " rtt " << minimumRtt ;
			break;
		}
		case ActiveMeasurement::Traceroute:
		{
			break;
		}
		case ActiveMeasurement::PingApp:
		{
			break;
		}
		default:
			IDEBUG() << "Could not recognize MeasurementId " ;
			break;
	}
	return minimumRtt;
}

// TODO: Merge is probably better, since I am not sending any data but only probing IP addresses
void ActiveMeasurement::SetGroupInformation(const GroupInformation &groupInfo)
{
	MutexLocker lock(&mutex_);
	
	ASSERT(groupInfo.IsValid());
	mapGroupInformation_[groupInfo.GetGroupName()] = groupInfo;
}

double ActiveMeasurement::GetRoundTripTime(const string &componentName) 
{	
	MutexLocker lock(&mutex_);

	ASSERT(componentName.empty() == false);
	if(mapComponentConnection_.count(componentName) <= 0) 
	{
		IDEBUG() << "WARNING! " << componentName << " not found!" ;
		return 0.0f;
	}

	const NetworkConnectionInformation &conn = mapComponentConnection_[componentName];
	return conn.GetRtt();
}


} // end namespace NetworkInformationClient

