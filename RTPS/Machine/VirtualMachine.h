#ifndef RTPS_Machine_VirtualMachine_h_IsIncluded
#define RTPS_Machine_VirtualMachine_h_IsIncluded

#include"RTPS/Machine/VirtualMachineWriter.h"
#include"RTPS/Machine/VirtualMachineReader.h"
#include"RTPS/Machine/RtpsMessageReceiver.h"
#include"RTPS/Machine/RtpsMessageSender.h"
#include"RTPS/IncludeExtLibs.h"
#include"RTPS/Export.h"
namespace RTPS
{

class RtpsFactory;

/**
 * @brief The VirtualMachine class
 */
class DLL_STATE VirtualMachine
{
public:
    VirtualMachine(const GUID &guid, RtpsMessageSender::Ptr messageSender);
    virtual ~VirtualMachine();

    CLASS_TRAITS(VirtualMachine)

    friend class RtpsFactory;

    /**
     * @brief ReceiveDatagram
     * @param reader
     * @param address
     * @return
     */
    bool ReceiveDatagram(NetworkLib::SerializeReader::Ptr reader, const NetAddress &address);

protected:

    /**
     * @brief InitializeWriter
     * @param writerType
     * @param topicKind
     * @param reliabilityKind
     * @return
     */
    bool InitializeWriter(WriterKind::Type writerType, TopicKindId::Type topicKind, ReliabilityKindId::Type reliabilityKind);

    /**
     * @brief InitializeReader
     * @param readerType
     * @param topicKind
     * @param reliabilityKind
     * @return
     */
    bool InitializeReader(ReaderKind::Type readerType, TopicKindId::Type topicKind, ReliabilityKindId::Type reliabilityKind);

private:
    GUID            endpointGuid_;

private:
    VirtualMachineWriter::Ptr   virtualMachineWriter_;
    VirtualMachineReader::Ptr   virtualMachineReader_;

    RtpsMessageReceiver::Ptr    messageReceiver_;
    RtpsMessageSender::Ptr      messageSender_;
};

} // namespace RTPS

#endif // RTPS_Machine_VirtualMachine_h_IsIncluded
