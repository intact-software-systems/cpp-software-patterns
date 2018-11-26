#include"DDS/DataWriterQoS.h"

namespace DDS
{

DataWriterQoS::DataWriterQoS()
    : lifecycle_(true)

    #ifdef  OMG_DDS_OWNERSHIP_SUPPORT
    , strength_(0)
    #endif  // OMG_DDS_OWNERSHIP_SUPPORT
{

}

DataWriterQoS::DataWriterQoS(const TopicQoS& tqos)
    : durability_(tqos.policy<DDS::Policy::Durability>())

    #ifdef  OMG_DDS_PERSISTENCE_SUPPORT
    , durability_service_(tqos.policy<DDS::Policy::DurabilityService>())
    #endif

    , deadline_(tqos.policy<DDS::Policy::Deadline>())
    , budget_(tqos.policy<DDS::Policy::LatencyBudget>())
    , liveliness_(tqos.policy<DDS::Policy::Liveliness>())
    , reliability_(tqos.policy<DDS::Policy::Reliability>())
    , order_(tqos.policy<DDS::Policy::DestinationOrder>())
    , history_(tqos.policy<DDS::Policy::History>())
    , resources_(tqos.policy<DDS::Policy::ResourceLimits>())
    , priority_(tqos.policy<DDS::Policy::TransportPriority>())
    , lifespan_(tqos.policy<DDS::Policy::Lifespan>())
    , ownership_(tqos.policy<DDS::Policy::Ownership>())
    //, lifecycle_(tqos.policy<DDS::Policy::WriterDataLifecycle>())

    #ifdef  OMG_DDS_OWNERSHIP_SUPPORT
    , strength_(0)
    #endif  // OMG_DDS_OWNERSHIP_SUPPORT
{

}

DataWriterQoS::DataWriterQoS(
                            DDS::Policy::UserData             user_data,
                            DDS::Policy::Durability           durability,

                    #ifdef  OMG_DDS_PERSISTENCE_SUPPORT
                            DDS::Policy::DurabilityService    durability_service,
                    #endif  // OMG_DDS_PERSISTENCE_SUPPORT

                            DDS::Policy::Deadline              deadline,
                            DDS::Policy::LatencyBudget         budget,
                            DDS::Policy::Liveliness            liveliness,
                            DDS::Policy::Reliability           reliability,
                            DDS::Policy::DestinationOrder      order,
                            DDS::Policy::History               history,
                            DDS::Policy::ResourceLimits        resources,
                            DDS::Policy::TransportPriority     priority,
                            DDS::Policy::Lifespan              lifespan,
                            DDS::Policy::Ownership             ownership,

                    #ifdef  OMG_DDS_OWNERSHIP_SUPPORT
                            DDS::Policy::OwnershipStrength     strength,
                    #endif  // OMG_DDS_OWNERSHIP_SUPPORT

                            DDS::Policy::WriterDataLifecycle   lifecycle)
    : user_data_(user_data)
    , durability_(durability)

    #ifdef  OMG_DDS_PERSISTENCE_SUPPORT
    , durability_service_(durability_service)
    #endif  // OMG_DDS_PERSISTENCE_SUPPORT

    , deadline_(deadline)
    , budget_(budget)
    , liveliness_(liveliness)
    , reliability_(reliability)
    , order_(order)
    , history_(history)
    , resources_(resources)
    , priority_(priority)
    , lifespan_(lifespan)
    , ownership_(ownership)
    #ifdef  OMG_DDS_OWNERSHIP_SUPPORT
    , strength_(strength)
    #endif  // OMG_DDS_OWNERSHIP_SUPPORT
    , lifecycle_(lifecycle)
{

}

DataWriterQoS::~DataWriterQoS()
{

}

void DataWriterQoS::policy(const DDS::Policy::UserData& user_data)
{
    user_data_ = user_data;
}

void DataWriterQoS::policy(const DDS::Policy::Durability& durability)
{
    durability_ = durability;
}

#ifdef  OMG_DDS_PERSISTENCE_SUPPORT
void DataWriterQoS::policy(const DDS::Policy::DurabilityService& durability_service)
{
    durability_service_ = durability_service;
}
#endif  // OMG_DDS_PERSISTENCE_SUPPORT


void DataWriterQoS::policy(const DDS::Policy::Deadline& deadline)
{
    deadline_ = deadline;
}

void DataWriterQoS::policy(const DDS::Policy::LatencyBudget&  budget)
{
    budget_ = budget;
}


void DataWriterQoS::policy(const DDS::Policy::Liveliness& liveliness)
{
    liveliness_ = liveliness;
}

void DataWriterQoS::policy(const DDS::Policy::Reliability& reliability)
{
    reliability_ = reliability;
}

void DataWriterQoS::policy(const DDS::Policy::DestinationOrder& order)
{
    order_ = order;
}

void DataWriterQoS::policy(const DDS::Policy::History& history)
{
    history_ = history;
}

void DataWriterQoS::policy(const DDS::Policy::ResourceLimits& resources)
{
    resources_ = resources;
}

void DataWriterQoS::policy(const DDS::Policy::TransportPriority& priority)
{
    priority_ = priority;
}

void DataWriterQoS::policy(const DDS::Policy::Lifespan& lifespan)
{
    lifespan_ = lifespan;
}

void DataWriterQoS::policy(const DDS::Policy::Ownership& ownership)
{
    ownership_ = ownership;
}

#ifdef  OMG_DDS_OWNERSHIP_SUPPORT
void DataWriterQoS::policy(const DDS::Policy::OwnershipStrength& strength)
{
    strength_ = strength;
}
#endif  // OMG_DDS_OWNERSHIP_SUPPORT

void DataWriterQoS::policy(const DDS::Policy::WriterDataLifecycle& lifecycle)
{
    lifecycle_ = lifecycle;
}

template<>
const DDS::Policy::UserData&
DataWriterQoS::policy<DDS::Policy::UserData>() const
{
    return user_data_;
}

template<>
const DDS::Policy::Durability&
DataWriterQoS::policy<DDS::Policy::Durability>() const
{
    return durability_;
}

#ifdef  OMG_DDS_PERSISTENCE_SUPPORT
template<> const DDS::Policy::DurabilityService&
DataWriterQoS::policy<DDS::Policy::DurabilityService>() const {
    return durability_service_;
}
#endif  // OMG_DDS_PERSISTENCE_SUPPORT

template<>
const DDS::Policy::Deadline&
DataWriterQoS::policy<DDS::Policy::Deadline>() const
{
    return deadline_;
}

template<>
const DDS::Policy::LatencyBudget&
DataWriterQoS::policy<DDS::Policy::LatencyBudget>() const
{
    return budget_;
}

template<>
const DDS::Policy::Liveliness&
DataWriterQoS::policy<DDS::Policy::Liveliness>() const
{
    return liveliness_;
}

template<>
const DDS::Policy::Reliability&
DataWriterQoS::policy<DDS::Policy::Reliability>() const
{
    return reliability_;
}

template<>
const DDS::Policy::DestinationOrder&
DataWriterQoS::policy<DDS::Policy::DestinationOrder>() const
{
    return order_;
}

template<>
const DDS::Policy::History&
DataWriterQoS::policy<DDS::Policy::History>() const
{
    return history_;
}

template<>
const DDS::Policy::ResourceLimits&
DataWriterQoS::policy<DDS::Policy::ResourceLimits>() const
{
    return resources_;
}

template<>
const DDS::Policy::TransportPriority&
DataWriterQoS::policy<DDS::Policy::TransportPriority>() const
{
    return priority_;
}

template<>
const DDS::Policy::Lifespan&
DataWriterQoS::policy<DDS::Policy::Lifespan>() const
{
    return lifespan_;
}

template<>
const DDS::Policy::Ownership&
DataWriterQoS::policy<DDS::Policy::Ownership>() const
{
    return ownership_;
}

#ifdef  OMG_DDS_OWNERSHIP_SUPPORT
template<>
const DDS::Policy::OwnershipStrength&
DataWriterQoS::policy<DDS::Policy::OwnershipStrength>() const
{
    return strength_;
}
#endif  // OMG_DDS_OWNERSHIP_SUPPORT

template<>
const DDS::Policy::WriterDataLifecycle&
DataWriterQoS::policy<DDS::Policy::WriterDataLifecycle>() const
{
    return lifecycle_;
}

}
