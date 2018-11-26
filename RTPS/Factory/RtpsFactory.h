/*
 * RtpsFactory.h
 *
 *  Created on: Mar 25, 2012
 *      Author: knuthelv
 */

#ifndef RTPS_Factory_RtpsFactory_h_IsIncluded
#define RTPS_Factory_RtpsFactory_h_IsIncluded

#include"RTPS/Elements/IncludeLibs.h"
#include"RTPS/Structure/IncludeLibs.h"
#include"RTPS/Export.h"

namespace NetworkLib
{
    class SocketStream;
}

namespace RTPS
{

class VirtualMachine;
class RtpsWriter;
class RtpsReader;

/*-------------------------------------------------------------------------
            class RtpsFactory

- RTPS factory functions to create RTPS reader/writers and VirtualMachine
-------------------------------------------------------------------------*/
class DLL_STATE RtpsFactory
{
public:
    RtpsFactory();
    ~RtpsFactory();

    // TODO: Move socketStream and datagramSize, should not be here!
    static VirtualMachine* NewVirtualMachine(const GUID &guid
                                                , IOReaderWriter::Ptr socketStream
                                                , unsigned int datagramSize
                                                , TopicKindId::Type topicKind
                                                , ReliabilityKindId::Type reliabilityKind
                                                , WriterKind::Type writerType
                                                , ReaderKind::Type readerType);

    static RtpsWriter*	NewDefaultRtpsWriter(WriterKind::Type writerKind, const GUID &guid, TopicKindId::Type topicKind, ReliabilityKindId::Type reliabilityKind);
    static RtpsReader*	NewDefaultRtpsReader(ReaderKind::Type readerKind, const GUID &guid, TopicKindId::Type topicKind, ReliabilityKindId::Type reliabilityKind);



    static RtpsParticipant::Ptr  CreateSimpleParticipantDiscovery(const GUID &guid);

};

} // namespace RTPS

#endif // RTPS_Factory_RtpsFactory_h_IsIncluded
