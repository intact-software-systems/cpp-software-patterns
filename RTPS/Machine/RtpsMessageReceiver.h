/*
 * RtpsMessageReceiver.h
 *
 *  Created on: Feb 29, 2012
 *      Author: knuthelv
 */
#ifndef RTPS_Machine_RtpsMessageReceiver_h_IsIncluded
#define RTPS_Machine_RtpsMessageReceiver_h_IsIncluded

#include"RTPS/IncludeExtLibs.h"
#include"RTPS/Elements/IncludeLibs.h"
#include"RTPS/Submessages/IncludeLibs.h"
#include"RTPS/Headers/MessageHeader.h"
#include"RTPS/Export.h"
namespace RTPS
{
/*--------------------------------------------------------------------------------------------
The Entity Submessages are:
• Data: Contains information regarding the value of an application Date-object. Data Submessages are sent by Writers
(NO_KEY Writer or WITH_KEY Writer) to Readers (NO_KEY Reader or WITH_KEY Reader).
• DataFrag: Equivalent to Data, but only contains a part of the new value (one or more fragments). Allows data to be
transmitted as multiple fragments to overcome transport message size limitations.
• Heartbeat: Describes the information that is available in a Writer. Heartbeat messages are sent by a Writer
(NO_KEY Writer or WITH_KEY Writer) to one or more Readers (NO_KEY Reader or WITH_KEY Reader).
• HeartbeatFrag: For fragmented data, describes what fragments are available in a Writer. HeartbeatFrag messages
are sent by a Writer (NO_KEY Writer or WITH_KEY Writer) to one or more Readers (NO_KEY Reader or
WITH_KEY Reader).
• Gap: Describes the information that is no longer relevant to Readers. Gap messages are sent by a Writer to one or
more Readers
• AckNack: Provides information on the state of a Reader to a Writer. AckNack messages are sent by a Reader to one
or more Writers.
• NackFrag: Provides information on the state of a Reader to a Writer, more specifically what fragments the Reader is
still missing. NackFrag messages are sent by a Reader to one or more Writers.

The Interpreter Submessages are:
• InfoSource: Provides information about the source from which subsequent Entity Submessages originated. This
Submessage is primarily used for relaying RTPS Submessages. This is not discussed in the current specification.
• InfoDestination: Provides information about the final destination of subsequent Entity Submessages. This Submessage
is primarily used for relaying RTPS Submessages. This is not discussed in the current specification.
• InfoReply: Provides information about where to reply to the entities that appear in subsequent Submessages.
• InfoTimestamp: Provides a source timestamp for subsequent Entity Submessages.
• Pad: Used to add padding to a Message if needed for memory alignment.
--------------------------------------------------------------------------------------------- */

/*--------------------------------------------------------------------------------------------
The interpretation and meaning of a Submessage within a Message may depend on the previous Submessages contained
within that same Message. Therefore, the receiver of a Message must maintain state from previously deserialized
Submessages in the same Message. This state is modeled as the state of an RTPS Receiver that is reset each time a new
message is processed and provides context for the interpretation of each Submessage. The RTPS Receiver is shown in
Figure 8.11. Table 8.16 lists the attributes used to represent the state of the RTPS Receiver.
--------------------------------------------------------------------------------------------- */
class DLL_STATE RtpsMessageReceiver
{
public:
    typedef std::list<RtpsSubmessageBase::Ptr> 			ListSubmessages;

public:
    RtpsMessageReceiver(const GuidPrefix &destGuidPrefix, bool verifyContent = true);
    virtual ~RtpsMessageReceiver();

    CLASS_TRAITS(RtpsMessageReceiver)

public:
    // Parses the reader buffer, which should be exactly one RTPS MessageHeader
    bool ParseDatagram(NetworkLib::SerializeReader::Ptr &reader, const GuidPrefix &destGuidPrefix);

    // Returns the list of successfully parsed submessage objects
    ListSubmessages&	GetListSubmessages() { return listSubmessages_; }

public:
    const ProtocolVersion&      GetSourceProtocolVersion()      const { return sourceVersion_; }
    const VendorId&             GetSourceVendorId()             const { return sourceVendorId_; }
    const GuidPrefix&           GetSourceGuidPrefix()           const { return sourceGuidPrefix_; }
    const GuidPrefix&           GetDestGuidPrefix()             const { return destGuidPrefix_; }
    const LocatorList&          GetUnicastReplyLocatorList()    const { return unicastReplyLocatorList_; }
    const LocatorList&          GetMulticastReplyLocatorList()  const { return multicastReplyLocatorList_; }
    const Timestamp&            GetTimestamp()                  const { return timeStamp_; }

    bool                        HasTimeStamp()                  const { return haveTimestamp_; }

private:
    MessageHeader 		getMessageHeader(NetworkLib::SerializeReader::Ptr &reader);
    int 				getSubmessageHeader(SubmessageHeader &header, NetworkLib::SerializeReader::Ptr &reader);

    RtpsSubmessageBase* getSubmessage(SubmessageHeader &header, NetworkLib::SerializeReader::Ptr &reader);
    RtpsSubmessageBase* getInterpreterSubmessage(SubmessageHeader &header, NetworkLib::SerializeReader::Ptr &reader);
    RtpsSubmessageBase* getEntitySubmessage(SubmessageHeader &header, NetworkLib::SerializeReader::Ptr &reader);

private:
    void resetState(const GuidPrefix &destGuidPrefix);
    void initializeReceiver(const MessageHeader &messageHeader);

private:
    // TODO:		DataEncapsulation with octet?
    ProtocolVersion sourceVersion_;
    VendorId		sourceVendorId_;
    GuidPrefix		sourceGuidPrefix_;
    GuidPrefix		destGuidPrefix_;
    LocatorList 	unicastReplyLocatorList_;
    LocatorList 	multicastReplyLocatorList_;
    bool			haveTimestamp_;
    Timestamp		timeStamp_;

private:
    bool			verifyContent_;
    int				nextSubmessageHeader_;
    ListSubmessages	listSubmessages_;
};

} // namespace RTPS

#endif // RTPS_Machine_RtpsMessageReceiver_h_IsIncluded
