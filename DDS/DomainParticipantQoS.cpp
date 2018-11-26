#include"DDS/DomainParticipantQoS.h"

namespace DDS
{

DomainParticipantQoS::DomainParticipantQoS()
{

}

DomainParticipantQoS::DomainParticipantQoS(
                        const DDS::Policy::UserData& user_data,
                        const DDS::Policy::EntityFactory& entity_factory)
    : user_data_(user_data)
    , entity_factory_(entity_factory)
{

}

template<>
const DDS::Policy::UserData&
DomainParticipantQoS::policy<DDS::Policy::UserData> () const
{
    return user_data_;
}

template<>
DDS::Policy::UserData&
DomainParticipantQoS::policy<DDS::Policy::UserData> ()
{
    return user_data_;
}


template<>
const DDS::Policy::EntityFactory&
DomainParticipantQoS::policy<DDS::Policy::EntityFactory> () const
{
    return entity_factory_;
}


template<>
DDS::Policy::EntityFactory&
DomainParticipantQoS::policy<DDS::Policy::EntityFactory> ()
{
    return entity_factory_;
}

void DomainParticipantQoS::policy(const DDS::Policy::UserData& ud)
{
    user_data_ = ud;
}

void DomainParticipantQoS::policy(const DDS::Policy::EntityFactory& efp)
{
    entity_factory_ = efp;
}

}

