/*
 * StatefulWriter.h
 *
 *  Created on: Mar 12, 2012
 *      Author: knuthelv
 */

#ifndef RTPS_Stateful_StatefulWriter_h_IsIncluded
#define RTPS_Stateful_StatefulWriter_h_IsIncluded

#include"RTPS/CommonDefines.h"
#include"RTPS/Structure/RtpsWriter.h"
#include"RTPS/Stateful/ReaderProxy.h"
#include"RTPS/Elements/IncludeLibs.h"

#include"RTPS/Export.h"
namespace RTPS
{
/*---------------------------------------------------------
Specialization of RTPS Writer used for the Stateful Reference Implementation. The RTPS StatefulWriter is configured
with the knowledge of all matched RTPS Reader endpoints and maintains state on each matched RTPS Reader endpoint.
By maintaining state on each matched RTPS Reader endpoint, the RTPS StatefulWriter can determine whether all
matched RTPS Reader endpoints have received a particular CacheChange and can be optimal in its use of network
bandwidth by avoiding to send announcements to readers that have received all the changes in the writer’s HistoryCache.
The information it maintains also simplifies QoS-based filtering on the Writer side. The attributes specific to the
StatefulWriter are described in Table 8.53.
-----------------------------------------------------------*/
class DLL_STATE StatefulWriter : public RtpsWriter, public ENABLE_SHARED_FROM_THIS(StatefulWriter)
{
public:
	typedef std::map<GUID, ReaderProxy::Ptr>	MapGUIDReaders;

public:
	StatefulWriter();
	StatefulWriter(const GUID &guid, TopicKindId::Type topicKind, ReliabilityKindId::Type reliabilityKind);
	virtual ~StatefulWriter();

    CLASS_TRAITS(StatefulWriter)

    /**
     * @brief GetPtr
     * @return
     */
    StatefulWriter::Ptr GetPtr()
    {
        return this->shared_from_this();
    }

public:
	// this automatically adds change to Cache
    virtual CacheChange::Ptr NewChange(ChangeKindId::Type kind, const RtpsData::Ptr &data, const InstanceHandle &handle);

public:
	/**
	 * Called by virtual-machine and returns after performing some work.
	 */
	DCPS::ReturnCode::Type WaitDispatch(int64 milliseconds);

	/**
	 * Callback by GuardCondition when inserting new cache change.
     */
    void operator()();

	/**
	 * Callback by ReaderLocator.GuardCondition when this->operator()() sets trigger to true.
	 */
	//void operator()(const ReaderLocator &readerLocator);
	template <typename ARG>
	void operator()(ARG &arg);

public:
	// ---------------------------------------------------
	// New virtual functions not in RTPS specification
	// ---------------------------------------------------
	virtual WriterKind::Type	GetWriterKind()	const  { return WriterKind::Stateful; }

	virtual bool ReceiveSubmessage(RtpsMessageReceiver *receiver, RtpsAckNack *ackNack);
	virtual bool ReceiveSubmessage(RtpsMessageReceiver *receiver, RtpsNackFrag *nackFrag);

public:
	// ---------------------------------------------------
	// RTPS specification
	// ---------------------------------------------------
	ReaderProxy::Ptr	NewReaderProxy(const GUID &remoteReaderGuid);

    bool IsAckedByAll(const CacheChange::Ptr &cacheChange) const;

	void MatchedReaderAdd(ReaderProxy::Ptr readerProxy);
	void MatchedReaderRemove(ReaderProxy::Ptr readerProxy);

	bool MatchedReaderRemove(const GUID &readerGuid);
	ReaderProxy::Ptr MatchedReaderLookup(const GUID &readerGuid) const;

private:
	// ---------------------------------------------------
	// New functions not in RTPS specification
	// ---------------------------------------------------
	bool sendHeartbeat();
	bool sendRequestedChanges();

private:
	bool addChangeForReader(ReaderProxy::Ptr &readerProxy, CacheChange::Ptr &cacheChange);
	void init(); // called from constructor

private:
	MapGUIDReaders		mapMatchedReaders_; 	// map of ReaderProxy pointers

private:
	WaitSet			waitSet_;

private:
    BaseLib::ElapsedTimer    heartbeatTimer_;
	BaseLib::ElapsedTimer    nackResponseTimer_;

private:
    StatefulWriter(const StatefulWriter&) {}
    StatefulWriter& operator=(const StatefulWriter&) { return *this; }
};

}

#endif
