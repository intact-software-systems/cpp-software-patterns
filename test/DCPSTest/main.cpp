#include "BaseLib/IncludeLibs.h"
#include "RxData/IncludeLibs.h"
#include "NetworkLib/IncludeLibs.h"
#include "RTPS/IncludeLibs.h"
#include "DCPS/IncludeLibs.h"

using namespace BaseLib;
using namespace BaseLib::Collection;
using namespace BaseLib::Concurrent;
using namespace NetworkLib;
using namespace RTPS;
using namespace DCPS;
using namespace DCPS::Domain;
using namespace DCPS::Subscription;
using namespace DCPS::Publication;
using namespace DDS;
using namespace DDS::Elements;
using namespace DCPS::Topic;

namespace DCPSTest
{

class DataListener : public DCPS::Publication::DataWriterListener<DomainId>
{
public:

    CLASS_TRAITS(DataListener)

    void OnOfferedIncompatibleQos(DataWriter<DomainId> *writer, const DCPS::Status::OfferedIncompatibleQosStatus &status)
    {}

    void OnOfferedDeadlineMissed(DataWriter<DomainId> *writer, const DCPS::Status::OfferedDeadlineMissedStatus &status)
    {}

    void OnLivelinessLost(DataWriter<DomainId> *writer, const DCPS::Status::LivelinessLostStatus &status)
    {}

    void OnPublicationMatch(DataWriter<DomainId> *writer, const DCPS::Status::PublicationMatchedStatus &status)
    {}

};

void DCPSBasicTest()
{
    DataListener::Ptr dataListener(new DataListener());


    DomainParticipant::Ptr participant = DomainParticipantFactory::Instance().CreateParticipant(DomainId::NIL());

    ASSERT(participant);

    Publisher::Ptr publisher = participant->CreatePublisher();

    ASSERT(publisher);

    DCPS::Topic::Topic<DomainId>::Ptr topic = participant->CreateTopic<DomainId>(TopicName("hallo"), TypeName("DomainId"));

    ASSERT(topic);

    DataWriter<DomainId>::Ptr dataWriter = publisher->CreateDataWriter<DomainId>(topic, topic->GetQoS(), dataListener, DCPS::Status::StatusMask::All());


    dataWriter->Write(DomainId(1), InstanceHandle(1));


}


}


int main(int argc, char* argv[])
{
    DCPSRuntime::Initialize();

    DCPSTest::DCPSBasicTest();

    return 0;
}
