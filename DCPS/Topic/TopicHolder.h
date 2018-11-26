#ifndef DCPS_Topic_TopicHolder_h_IsIncluded
#define DCPS_Topic_TopicHolder_h_IsIncluded

#include"DCPS/Domain/DomainFwd.h"
#include"DCPS/Export.h"

namespace DCPS { namespace Topic
{

class DLL_STATE TopicHolder
{
public:
    CLASS_TRAITS(TopicHolder)

    virtual Domain::DomainParticipantPtr        GetParticipant() const = 0;
    virtual const DDS::Elements::TopicName&     GetTopicName() const = 0;
    virtual const DDS::Elements::TypeName&      GetTypeName() const = 0;
};

}}

#endif // DCPS_Topic_TopicHolder_h_IsIncluded
