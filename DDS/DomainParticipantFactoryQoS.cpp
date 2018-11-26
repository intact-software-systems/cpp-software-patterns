#include "DDS/DomainParticipantFactoryQoS.h"

namespace DDS
{

DomainParticipantFactoryQoS::DomainParticipantFactoryQoS()
{
}

DomainParticipantFactoryQoS::DomainParticipantFactoryQoS(const DDS::Policy::EntityFactory& entityFactory)
    :  entityFactory_(entityFactory)
{

}

template<>
const DDS::Policy::EntityFactory&
DomainParticipantFactoryQoS::policy<DDS::Policy::EntityFactory> () const
{
    return entityFactory_;
}


template<>
DDS::Policy::EntityFactory&
DomainParticipantFactoryQoS::policy<DDS::Policy::EntityFactory> ()
{
    return entityFactory_;
}

void DomainParticipantFactoryQoS::policy(const DDS::Policy::EntityFactory& efp)
{
    entityFactory_ = efp;
}

}
