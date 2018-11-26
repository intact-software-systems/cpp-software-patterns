#include "DDS/DataReaderQoS.h"

namespace DDS
{

DataReaderQoS::DataReaderQoS()
{

}

DataReaderQoS::DataReaderQoS(const DDS::TopicQoS &qos,
                             Policy::UserData userData,
                             Policy::TimeBasedFilter tfilter,
                             Policy::ReaderDataLifecycle lifecycle)
    : user_data_(userData)
    , durability_(qos.policy<DDS::Policy::Durability>())

    #ifdef OMG_DDS_PERSISTENCE_SUPPORT
    , durability_service_(qos.policy<DDS::Policy::DurabilityService>())
    #endif  // OMG_DDS_PERSISTENCE_SUPPORT

    , deadline_(qos.policy<DDS::Policy::Deadline>())
    , budget_(qos.policy<DDS::Policy::LatencyBudget>())
    , liveliness_(qos.policy<DDS::Policy::Liveliness>())
    , reliability_(qos.policy<DDS::Policy::Reliability>())
    , order_(qos.policy<DDS::Policy::DestinationOrder>())
    , history_(qos.policy<DDS::Policy::History>())
    , resources_(qos.policy<DDS::Policy::ResourceLimits>())
    , priority_(qos.policy<DDS::Policy::TransportPriority>())
    , lifespan_(qos.policy<DDS::Policy::Lifespan>())
    , ownership_(qos.policy<DDS::Policy::Ownership>())
    , tfilter_(tfilter)
    , lifecycle_(lifecycle)
{ }

DataReaderQoS::DataReaderQoS(
                            DDS::Policy::UserData               user_data,
                            DDS::Policy::Durability              durability,

                    #ifdef  OMG_DDS_PERSISTENCE_SUPPORT
                            DDS::Policy::DurabilityService       durability_service,
                    #endif  // OMG_DDS_PERSISTENCE_SUPPORT

                            DDS::Policy::Deadline       deadline,
                            DDS::Policy::LatencyBudget  budget,
                            DDS::Policy::Liveliness     liveliness,
                            DDS::Policy::Reliability             reliability,
                            DDS::Policy::DestinationOrder        order,
                            DDS::Policy::History                 history,
                            DDS::Policy::ResourceLimits          resources,
                            DDS::Policy::TransportPriority       priority,
                            DDS::Policy::Lifespan                lifespan,
                            DDS::Policy::Ownership               ownership,
                            DDS::Policy::TimeBasedFilter         tfilter,
                            DDS::Policy::ReaderDataLifecycle     lifecycle)
    : user_data_(user_data)
    , durability_(durability)

    #ifdef OMG_DDS_PERSISTENCE_SUPPORT
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
    , tfilter_(tfilter)
    , lifecycle_(lifecycle)
{

}

DataReaderQoS::~DataReaderQoS()
{

}

void DataReaderQoS::policy(const DDS::Policy::UserData& user_data)
{
    user_data_ = user_data;
}

void DataReaderQoS::policy(const DDS::Policy::Durability& durability)
{
    durability_ = durability;
}

#ifdef  OMG_DDS_PERSISTENCE_SUPPORT
void DataReaderQoS::policy(const DDS::Policy::DurabilityService& durability_service)
{
    durability_service_ = durability_service;
}
#endif  // OMG_DDS_PERSISTENCE_SUPPORT


void DataReaderQoS::policy(const DDS::Policy::Deadline& deadline)
{
    deadline_ = deadline;
}

void DataReaderQoS::policy(const DDS::Policy::LatencyBudget&  budget)
{
    budget_ = budget;
}

void DataReaderQoS::policy(const DDS::Policy::Liveliness& liveliness)
{
    liveliness_ = liveliness;
}

void DataReaderQoS::policy(const DDS::Policy::Reliability& reliability)
{
    reliability_ = reliability;
}

void DataReaderQoS::policy(const DDS::Policy::DestinationOrder& order)
{
    order_ = order;
}

void DataReaderQoS::policy(const DDS::Policy::History& history)
{
    history_ = history;
}

void DataReaderQoS::policy(const DDS::Policy::ResourceLimits& resources)
{
    resources_ = resources;
}

void DataReaderQoS::policy(const DDS::Policy::TransportPriority& priority)
{
    priority_ = priority;
}

void DataReaderQoS::policy(const DDS::Policy::Lifespan& lifespan)
{
    lifespan_ = lifespan;
}

void DataReaderQoS::policy(const DDS::Policy::Ownership& ownership)
{
    ownership_ = ownership;
}

void DataReaderQoS::policy(const DDS::Policy::TimeBasedFilter& tfilter)
{
    tfilter_ = tfilter;
}
void DataReaderQoS::policy(const DDS::Policy::ReaderDataLifecycle& lifecycle)
{
    lifecycle_ = lifecycle;
}

//
//template<>
//const DDS::Policy::UserData&
//DataReaderQoS::policy<DDS::Policy::UserData>() const {
//    return user_data_;
//}

//template <>
//const int&
//DataReaderQoS::policy<int>() const {
//    return 0;
//}

template<>
const DDS::Policy::Durability&
DataReaderQoS::policy<DDS::Policy::Durability>() const
{
    return durability_;
}

template<>
DDS::Policy::Durability&
DataReaderQoS::policy<DDS::Policy::Durability>()
{
    return durability_;
}

template<>
const DDS::Policy::UserData&
DataReaderQoS::policy<DDS::Policy::UserData>() const
{
    return user_data_;
}

template<>
DDS::Policy::UserData&
DataReaderQoS::policy<DDS::Policy::UserData>()
{
    return user_data_;
}

#ifdef  OMG_DDS_PERSISTENCE_SUPPORT
template<>
const DDS::Policy::DurabilityService&
DataReaderQoS::policy<DDS::Policy::DurabilityService>() const
{
    return durability_service_;
}

template<>
DDS::Policy::DurabilityService&
DataReaderQoS::policy<DDS::Policy::DurabilityService>()
{
    return durability_service_;
}
#endif  // OMG_DDS_PERSISTENCE_SUPPORT

template<>
const DDS::Policy::Deadline&
DataReaderQoS::policy<DDS::Policy::Deadline>() const
{
    return deadline_;
}

template<>
DDS::Policy::Deadline&
DataReaderQoS::policy<DDS::Policy::Deadline>()
{
    return deadline_;
}


template<>
const DDS::Policy::LatencyBudget&
DataReaderQoS::policy<DDS::Policy::LatencyBudget>() const
{
    return budget_;
}

template<>
DDS::Policy::LatencyBudget&
DataReaderQoS::policy<DDS::Policy::LatencyBudget>()
{
    return budget_;
}


template<>
const DDS::Policy::Liveliness&
DataReaderQoS::policy<DDS::Policy::Liveliness>() const
{
    return liveliness_;
}

template<>
DDS::Policy::Liveliness&
DataReaderQoS::policy<DDS::Policy::Liveliness>()
{
    return liveliness_;
}


template<>
const DDS::Policy::Reliability&
DataReaderQoS::policy<DDS::Policy::Reliability>() const
{
    return reliability_;
}

template<>
DDS::Policy::Reliability&
DataReaderQoS::policy<DDS::Policy::Reliability>()
{
    return reliability_;
}


template<>
const DDS::Policy::DestinationOrder&
DataReaderQoS::policy<DDS::Policy::DestinationOrder>() const
{
    return order_;
}

template<>
DDS::Policy::DestinationOrder&
DataReaderQoS::policy<DDS::Policy::DestinationOrder>()
{
    return order_;
}


template<>
const DDS::Policy::History&
DataReaderQoS::policy<DDS::Policy::History>() const
{
    return history_;
}

template<>
DDS::Policy::History&
DataReaderQoS::policy<DDS::Policy::History>()
{
    return history_;
}


template<>
const DDS::Policy::ResourceLimits&
DataReaderQoS::policy<DDS::Policy::ResourceLimits>() const
{
    return resources_;
}

template<>
DDS::Policy::ResourceLimits&
DataReaderQoS::policy<DDS::Policy::ResourceLimits>()
{
    return resources_;
}

template<>
const DDS::Policy::TransportPriority&
DataReaderQoS::policy<DDS::Policy::TransportPriority>() const
{
    return priority_;
}

template<>
DDS::Policy::TransportPriority&
DataReaderQoS::policy<DDS::Policy::TransportPriority>()
{
    return priority_;
}

template<>
const DDS::Policy::Lifespan&
DataReaderQoS::policy<DDS::Policy::Lifespan>() const
{
    return lifespan_;
}

template<>
DDS::Policy::Lifespan&
DataReaderQoS::policy<DDS::Policy::Lifespan>()
{
    return lifespan_;
}


template<> const  DDS::Policy::Ownership&
DataReaderQoS::policy<DDS::Policy::Ownership>() const
{
    return ownership_;
}

template<>
DDS::Policy::Ownership&
DataReaderQoS::policy<DDS::Policy::Ownership>()
{
    return ownership_;
}

template<>
const DDS::Policy::TimeBasedFilter&
DataReaderQoS::policy<DDS::Policy::TimeBasedFilter>() const
{
    return tfilter_;
}

template<>
DDS::Policy::TimeBasedFilter&
DataReaderQoS::policy<DDS::Policy::TimeBasedFilter>()
{
    return tfilter_;
}

template<> const  DDS::Policy::ReaderDataLifecycle&
DataReaderQoS::policy<DDS::Policy::ReaderDataLifecycle>() const
{
    return lifecycle_;
}

template<>
DDS::Policy::ReaderDataLifecycle&
DataReaderQoS::policy<DDS::Policy::ReaderDataLifecycle>()
{
    return lifecycle_;
}

}
