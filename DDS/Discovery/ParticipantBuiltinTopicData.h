#ifndef DDS_Discovery_ParticipantBuiltinTopicData_h_IsIncluded
#define DDS_Discovery_ParticipantBuiltinTopicData_h_IsIncluded

#include"DDS/Elements/TopicKey.h"
#include"DDS/DomainParticipantQoS.h"

#include"DDS/Export.h"

namespace DDS { namespace Discovery
{

/*
- DCPSParticipant (entry created when a DomainParticipant object is created).
key         BuiltinTopicKey_t           DDS key to distinguish entries.
user_data   UserDataQosPolicy           Policy of the corresponding DomainParticipant.
*/

/**
 * @brief The ParticipantBuiltinTopicData class
 */
class DLL_STATE ParticipantBuiltinTopicData : public BaseLib::Templates::ComparableImmutableType<Elements::TopicKey>
{
public:
    ParticipantBuiltinTopicData();
    ParticipantBuiltinTopicData(Elements::TopicKey key, const DomainParticipantQoS &qos);

    virtual ~ParticipantBuiltinTopicData();

    Elements::TopicKey Key() const;

    DomainParticipantQoS Qos() const;
    void SetQos(const DomainParticipantQoS &qos);

private:
    DomainParticipantQoS qos_;
};

}}

#endif
