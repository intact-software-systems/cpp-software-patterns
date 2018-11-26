#ifndef NetworkInformationClient_ActiveMeasurement_h_IsIncluded
#define NetworkInformationClient_ActiveMeasurement_h_IsIncluded

#include "NetworkInformationClient/IncludeExtLibs.h"

namespace NetworkInformationClient
{

class NetworkConnectionInformation
{
public:
	NetworkConnectionInformation(double rtt = 0.0f) : rtt_(rtt) {}
	~NetworkConnectionInformation() {}

	void SetRtt(double rtt)			{ rtt_ = rtt; }
	double GetRtt()			const 	{ return rtt_; }

private:
	double rtt_;
};

/*-----------------------------------------------------------------------
						class ActiveMeasurement
Possible active latency measurements: 
	ICMP ping (this document) 
	traceroute (this document) 
	application level ping 

Possible passive latency measurements: 
	tcp rtt (this document) 
	application level events (e.g., bittorrent handshake)
------------------------------------------------------------------------- */
class ActiveMeasurement : public Thread
{
public:
	enum MeasurementId
	{
		PingICMP = 0,
		Traceroute,
		PingApp
	};

public:
	typedef map<string, NetworkConnectionInformation> 	MapComponentConnection;
	typedef map<string, GroupInformation>				MapGroupNameGroupInformation;
	typedef map<string, HostInformation> 				MapHostInformation;

public:
	ActiveMeasurement(string name, MeasurementId id, int measureFrequencyMs);
  	~ActiveMeasurement();
	
	virtual void run();

	void SetGroupInformation(const GroupInformation &groupInfo);
	double GetRoundTripTime(const string &componentName);

private:
	double executeMeasurement(const HostInformation &hostInfo);

private:
	mutable Mutex 			mutex_;
	WaitCondition			waitCondition_;

	MapComponentConnection			mapComponentConnection_;
	MapGroupNameGroupInformation	mapGroupInformation_;
	int								measureFrequency_;
	MeasurementId					measurementId_;
	bool							runThread_;
};

}; // end namespace

#endif // ActiveMeasurement_h_IsIncluded


