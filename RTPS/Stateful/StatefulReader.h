/*
 * StatefulReader.h
 *
 *  Created on: Mar 12, 2012
 *      Author: knuthelv
 */

#pragma once

#include"RTPS/Structure/RtpsReader.h"
#include"RTPS/Stateful/WriterProxy.h"
#include"RTPS/Elements/IncludeLibs.h"
#include"RTPS/Machine/RtpsMessageSender.h"
#include"RTPS/Export.h"

namespace RTPS {

class RtpsData;
class RtpsGap;
class RtpsHeartbeat;
class RtpsMessageReceiver;

/*---------------------------------------------------------
Specialization of RTPS Reader. The RTPS StatefulReader keeps
state on each matched RTPS Writer. The state kept on
each writer is encapsulated in the RTPS WriterProxy class.
-----------------------------------------------------------*/
class DLL_STATE StatefulReader : public RtpsReader
{
public:
    typedef std::map<GUID, WriterProxy::Ptr> MapGUIDWriters;

public:
    StatefulReader();
    StatefulReader(const GUID& guid, TopicKindId::Type topicKind, ReliabilityKindId::Type reliabilityKind);
    virtual ~StatefulReader();

    /*
     * Called by virtual-machine and returns after performing some work.
     */
    DCPS::ReturnCode::Type WaitDispatch(int64 milliseconds);

    // ---------------------------------------------------
    // New virtual functions not in RTPS specification
    // ---------------------------------------------------
    virtual ReaderKind::Type GetReaderKind() const    { return ReaderKind::Stateful; }

    virtual bool ReceiveSubmessage(RtpsMessageReceiver* messageReceiver, RtpsData* data);
    virtual bool ReceiveSubmessage(RtpsMessageReceiver* messageReceiver, RtpsGap* gap);
    virtual bool ReceiveSubmessage(RtpsMessageReceiver* messageReceiver, RtpsHeartbeat* heartbeat);

    // ---------------------------------------------------
    //  RTPS specification
    // ---------------------------------------------------
    void             MatchedWriterAdd(WriterProxy::Ptr& writerProxy);
    void             MatchedWriterRemove(WriterProxy::Ptr& writerProxy);
    bool             MatchedWriterRemove(const GUID& writerGuid);
    WriterProxy::Ptr MatchedWriterLookup(const GUID& writerGuid);

private:
    // ---------------------------------------------------
    // New functions not in RTPS specification
    // ---------------------------------------------------
    bool sendAckNack();

private:
    MapGUIDWriters mapMatchedWriters_;    // map of ReaderProxy pointers

    RtpsMessageSender::Ptr messageSender_;

    WaitSet               waitSet_;
    BaseLib::ElapsedTimer ackNackResponseTimer_;
};

}

