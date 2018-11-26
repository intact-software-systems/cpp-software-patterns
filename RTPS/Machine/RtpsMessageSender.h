/*
 * RtpsMessageSender.h
 *
 *  Created on: Apr 22, 2012
 *      Author: knuthelv
 */

#ifndef RTPS_Machine_RtpsMessageSender_h_IsIncluded
#define RTPS_Machine_RtpsMessageSender_h_IsIncluded

#include"RTPS/IncludeExtLibs.h"
#include"RTPS/Elements/IncludeLibs.h"
#include"RTPS/Stateful/ReaderProxy.h"
#include"RTPS/Stateful/WriterProxy.h"
#include"RTPS/Stateless/ReaderLocator.h"
#include"RTPS/Submessages/IncludeLibs.h"
#include"RTPS/CommonDefines.h"
#include"RTPS/Export.h"
namespace RTPS
{

class StatelessWriter;
class StatefulWriter;
class StatefulReader;
//class ReaderLocator;

/*--------------------------------------------------------------------------------------------
    sending data from reader/writer implementations

    TODO: review race conditions!
--------------------------------------------------------------------------------------------- */
class DLL_STATE RtpsMessageSender
{
public:
    typedef std::list<RtpsSubmessageBase::Ptr> 			    ListSubmessages;

public:
    RtpsMessageSender(const GuidPrefix &writerGuidPrefix, IOWriter::Ptr writer, unsigned int datagramSize);
    virtual ~RtpsMessageSender();

    CLASS_TRAITS(RtpsMessageSender)

public:
    // StatelessWriter
    DCPS::ReturnCode::Type SendTo(StatelessWriter* statelessWriter, ReaderLocator::Ptr readerLocator, const SequenceSet &sequenceSet);
    DCPS::ReturnCode::Type SendTo(StatelessWriter* statelessWriter, ReaderLocator::Ptr locator, const SequenceRange &seqNumRange);
    DCPS::ReturnCode::Type SendTo(StatelessWriter* statelessWriter, ReaderLocator::Ptr locator, RtpsHeartbeat::Ptr heartbeat);
    //DCPS::ReturnCode::Type SendTo(StatelessWriter* statelessWriter, ReaderLocator *locator, const SequenceNumberSet &seqNumSet);

public:
    // StatefulWriter
    DCPS::ReturnCode::Type SendTo(StatefulWriter* statefulWriter, ReaderProxy::Ptr readerProxy, ReaderProxy::MapChangeForReader &mapChangeForReader);
    DCPS::ReturnCode::Type SendTo(StatefulWriter* statefulWriter, ReaderProxy::Ptr readerProxy, RtpsHeartbeat::Ptr heartbeat);

public:
    // StatefulReader
    DCPS::ReturnCode::Type SendTo(StatefulReader *statefulReader, WriterProxy &writerProxy, ListSubmessages &submessages);

private:
    DCPS::ReturnCode::Type addGapMessages(ListSubmessages &submessages, SequenceSet &missingCache, const EntityId &entityId);
    DCPS::ReturnCode::Type addDataMessages(RtpsData::Ptr data, ListSubmessages &submessages, const EntityId &entityId, const Locator &locator, bool expectsInlineQos, bool isStateless);

    DCPS::ReturnCode::Type writeMessage(const ListSubmessages &submessages, const Locator &locator);
    DCPS::ReturnCode::Type writeOutDatagram(NetworkLib::SerializeWriter &msgWrapper, const Locator &locator);

private:
    // TODO:		DataEncapsulation with octet?
    const ProtocolVersion   writerVersion_;
    const VendorId		    writerVendorId_;
    const GuidPrefix		senderGuidPrefix_;
    const unsigned int      datagramSize_;
    //Timestamp		        timeStamp_;
    //GuidPrefix	destGuidPrefix_;
    //bool			haveTimestamp_;

//private:
    //bool			    verifyContent_;
    //int				nextSubmessageHeader_;
    //ListSubmessages	listSubmessages_;
    //LocatorList 	    unicastReplyLocatorList_;
    //LocatorList 	    multicastReplyLocatorList_;

private:
    IOWriter::Ptr           socketWriter_;
    mutable BaseLib::Mutex  mutex_;
};

} // namespace RTPS

#endif // RTPS_Machine_RtpsMessageSender_h_IsIncluded
