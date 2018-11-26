/*
 * RtpsFactory.cpp
 *
 *  Created on: Mar 25, 2012
 *      Author: knuthelv
 */

#include"RTPS/Factory/RtpsFactory.h"
#include"RTPS/Stateless/IncludeLibs.h"
#include"RTPS/Stateful/IncludeLibs.h"
#include"RTPS/Machine/IncludeLibs.h"

namespace RTPS
{

/*-------------------------------------------------------------------------
            class RtpsFactory
-------------------------------------------------------------------------*/
RtpsFactory::RtpsFactory()
{

}

RtpsFactory::~RtpsFactory()
{
}

/*-------------------------------------------------------------------------
            create RTPS virtual machine
-------------------------------------------------------------------------*/
VirtualMachine* RtpsFactory::NewVirtualMachine(const GUID &guid
                                                , IOReaderWriter::Ptr socketStream
                                                , unsigned int datagramSize
                                                , TopicKindId::Type topicKind
                                                , ReliabilityKindId::Type reliabilityKind
                                                , WriterKind::Type writerType
                                                , ReaderKind::Type readerType)
{
    RtpsMessageSender::Ptr messageSender(new RtpsMessageSender(guid.GetGuidPrefix(), socketStream, datagramSize ));

    VirtualMachine *virtualMachine = new VirtualMachine(guid, messageSender);

    //RtpsReader* rtpsReader = RtpsFactory::NewDefaultRtpsReader(readerType, guid, topicKind, reliabilityKind);
    //ASSERT(rtpsReader);

    bool success = virtualMachine->InitializeReader(readerType, topicKind, reliabilityKind);
    ASSERT(success);

    success = virtualMachine->InitializeWriter(writerType, topicKind, reliabilityKind);
    ASSERT(success);

    return virtualMachine;
}

/*-------------------------------------------------------------------------
            create default RTPS writer
-------------------------------------------------------------------------*/
RtpsWriter* RtpsFactory::NewDefaultRtpsWriter(WriterKind::Type writerKind,
                                                const GUID &guid,
                                                TopicKindId::Type topicKind,
                                                ReliabilityKindId::Type reliabilityKind)
{
    RtpsWriter *rtpsWriter = NULL;

    switch(writerKind)
    {
        case WriterKind::Stateless:
        {
            Duration resendDataPeriod(1);
            rtpsWriter = new StatelessWriter(guid, topicKind, reliabilityKind, resendDataPeriod);
            break;
        }
        case WriterKind::Stateful:
        {
            rtpsWriter = new StatefulWriter(guid, topicKind, reliabilityKind);
            break;
        }
        case WriterKind::Undefined:
        default:
            throw CriticalException("WriterKind was undefined!");
            break;
    }

    return rtpsWriter;
}

/*-------------------------------------------------------------------------
            create default RTPS reader
-------------------------------------------------------------------------*/
RtpsReader* RtpsFactory::NewDefaultRtpsReader(ReaderKind::Type readerKind,
                                                const GUID &guid,
                                                TopicKindId::Type topicKind,
                                                ReliabilityKindId::Type reliabilityKind)
{
    RtpsReader *rtpsReader = NULL;

    switch(readerKind)
    {
        case ReaderKind::Stateless:
        {
            rtpsReader = new StatelessReader(guid, topicKind, reliabilityKind);
            break;
        }
        case ReaderKind::Stateful:
        {
            rtpsReader = new StatefulReader(guid, topicKind, reliabilityKind);
            break;
        }
        case ReaderKind::Undefined:
        default:
            throw CriticalException("ReaderKind was undefined!");
            break;
    }

    return rtpsReader;
}

/*
The HistoryCache of the SPDPbuiltinParticipantReader contains information on all active discovered participants;
the key used to identify each data-object corresponds to the Participant GUID.

Each time information on a participant is received by the SPDPbuiltinParticipantReader, the SPDP examines the
HistoryCache looking for an entry with a key that matches the Participant GUID. If an entry with a matching key
is not there, a new entry is added keyed by the GUID of the Participant.

Periodically, the SPDP examines the SPDPbuiltinParticipantReader HistoryCache looking for stale entries defined
as those that have not been refreshed for a period longer than their specified leaseDuration. Stale entries are removed.
*/

/**
 * @brief RtpsFactory::CreateSimpleParticipantDiscovery
 * @param guid
 * @return
 */
RtpsParticipant::Ptr RtpsFactory::CreateSimpleParticipantDiscovery(const GUID &guid)
{
    /*
    SPDPbuiltinParticipantWriter
    attribute                   type                    value
    unicastLocatorList          Locator_t[*]            <auto-detected>
                                                        Transport-kinds and addresses are either auto-detected or configured by the application.
                                                        Ports are a parameter to the SPDP initialization or else are set to a PSM-specified value that depends on the domainId.
    multicastLocatorList        Locator_t[*]            <parameter to the SPDP initialization> Defaults to a PSM-specified value.
    reliabilityLevel            ReliabilityKind_t       BEST_EFFORT
    topicKind                   TopicKind_t             WITH_KEY
    resendPeriod                Duration_t              <parameter to the SPDP initialization> Defaults to a PSM-specified value.
    readerLocators              ReaderLocator[*]        <parameter to the SPDP initialization>
    */

    GUID writerGuid(guid.GetGuidPrefix(), EntityId::EntityId_SPDP_BUILTIN_PARTICIPANT_WRITER());

    StatelessWriter::Ptr writer(new StatelessWriter(writerGuid, TopicKindId::WithKey, ReliabilityKindId::BestEffort, Duration::FromMilliseconds(2000)));


    /*
    SPDPbuiltinParticipantReader
    attribute                   type                    value
    unicastLocatorList          Locator_t[*]            <auto-detected>
                                                        Transport-kinds and addresses are either auto-detected or configured by the application.
                                                        Ports are a parameter to the SPDP initialization or else are set to a PSM-specified value that depends on the domainId.
    multicastLocatorList        Locator_t[*]            <parameter to the SPDP initialization>. Defaults to a PSM-specified value.
    reliabilityLevel            ReliabilityKind_t       BEST_EFFORT
    topicKind                   TopicKind_t             WITH_KEY
    */
    GUID readerGuid(guid.GetGuidPrefix(), EntityId::EntityId_ENTITYID_SPDP_BUILTIN_PARTICIPANT_READER());

    StatelessReader::Ptr reader(new StatelessReader(readerGuid, TopicKindId::WithKey, ReliabilityKindId::BestEffort));

    RtpsParticipant::Ptr participant(new RtpsParticipant(guid));

    return participant;
}

} // namespace RTPS
