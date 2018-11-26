/*
 * StatelessWriter.h
 *
 *  Created on: Mar 12, 2012
 *      Author: knuthelv
 */

#ifndef RTPS_Stateless_StatelessWriter_h_IsIncluded
#define RTPS_Stateless_StatelessWriter_h_IsIncluded

#include"RTPS/Structure/RtpsWriter.h"
#include"RTPS/Stateless/ReaderLocator.h"
#include"RTPS/Machine/RtpsMessageSender.h"
#include"RTPS/IncludeExtLibs.h"
#include"RTPS/Export.h"
namespace RTPS
{

/*---------------------------------------------------------
                class StatelessWriter
-----------------------------------------------------------*/
class DLL_STATE StatelessWriter : public RtpsWriter
                                , public ENABLE_SHARED_FROM_THIS(StatelessWriter)
{
public:
    typedef std::map<Locator, ReaderLocator::Ptr>	MapLocatorReaders;

public:
    StatelessWriter(const Duration &resendDataPeriod);
    StatelessWriter(const GUID &guid, TopicKindId::Type topicKind, ReliabilityKindId::Type reliabilityKind, const Duration &resendDataPeriod);
    virtual ~StatelessWriter();

    CLASS_TRAITS(StatelessWriter)

    /**
     * @brief GetPtr
     * @return
     */
    StatelessWriter::Ptr GetPtr()
    {
        return this->shared_from_this();
    }

    /*
     * Called by virtual-machine and returns after performing some work.
     */
    DCPS::ReturnCode::Type WaitDispatch(int64 milliseconds);

    /*
     * Callback by GuardCondition when inserting new cache change.
     */
    void operator()();

    /*
     * Callback by ReaderLocator.GuardCondition when this->operator()() sets trigger to true.
     */
    //void operator()(const ReaderLocator &readerLocator);
    template <typename ARG>
    void operator()(ARG &arg);

public:
    // ---------------------------------------------------
    // New virtual functions not in RTPS specification
    // ---------------------------------------------------
    virtual WriterKind::Type	GetWriterKind()	const  { return WriterKind::Stateless; }

    virtual bool ReceiveSubmessage(RtpsMessageReceiver *receiver, RtpsAckNack *ackNack);
    virtual bool ReceiveSubmessage(RtpsMessageReceiver *receiver, RtpsNackFrag *nackFrag);

public:
    // ---------------------------------------------------
    //  RTPS specification
    // ---------------------------------------------------
    void ReaderLocatorAdd(const Locator &locator);
    void ReaderLocatorRemove(const Locator &locator);
    void UnsentChangesReset();

public:
    Duration			GetResendDataPeriod()		const 	{ return resendDataPeriod_; }
    MapLocatorReaders&	GetReaderLocators()					{ return mapReaderLocators_; }

    void 				SetResendDataPeriod(Duration d)		{ resendDataPeriod_ = d; }

private:
    // ---------------------------------------------------
    // New functions not in RTPS specification
    // ---------------------------------------------------
    bool sendHeartbeat();
    bool sendRequestedChanges();

private:
    // ---------------------------------------------------
    //  helper functions
    // ---------------------------------------------------
    ReaderLocator::Ptr  addReaderLocator(const Locator &locator);
    ReaderLocator::Ptr  getOrCreateReaderLocator(const Locator &locator);

private:
    void                init(); // called from constructor

private:
    Duration 			    resendDataPeriod_;
    MapLocatorReaders	    mapReaderLocators_;

private:
    RtpsMessageSender::Ptr messageSender_;

private:
    WaitSet			waitSet_;

private:
    BaseLib::ElapsedTimer    heartbeatTimer_;
    BaseLib::ElapsedTimer    nackResponseTimer_;

private:
    StatelessWriter(const StatelessWriter&) {}
    StatelessWriter& operator=(const StatelessWriter&) { return *this; }
};

} // namespace RTPS

#endif // RTPS_Stateless_StatelessWriter_h_IsIncluded
