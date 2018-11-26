#include"DDS/TopicQoS.h"

namespace DDS
{

TopicQoS::TopicQoS(const DDS::Policy::TopicData&              topic_data,
                   const DDS::Policy::Durability&             durability,

#ifdef  OMG_DDS_PERSISTENCE_SUPPORT
                   const DDS::Policy::DurabilityService&      durability_service,
#endif  // OMG_DDS_PERSISTENCE_SUPPORT

                   const DDS::Policy::Deadline&               deadline,
                   const DDS::Policy::LatencyBudget&          budget,
                   const DDS::Policy::Liveliness&             liveliness,
                   const DDS::Policy::Reliability&            reliability,
                   const DDS::Policy::DestinationOrder&       order,
                   const DDS::Policy::History&                history,
                   const DDS::Policy::ResourceLimits&         resources,
                   const DDS::Policy::TransportPriority&      priority,
                   const DDS::Policy::Lifespan&               lifespan,
                   const DDS::Policy::Ownership&              ownership)
      : topic_data_(topic_data)
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
{

}

TopicQoS::~TopicQoS()
{

}

void TopicQoS::policy(const DDS::Policy::TopicData& topic_data)
{
    topic_data_ = topic_data;
}

void TopicQoS::policy(const DDS::Policy::Durability& durability)
{
    durability_ = durability;
}

#ifdef  OMG_DDS_PERSISTENCE_SUPPORT
void TopicQoS::policy(const DDS::Policy::DurabilityService& durability_service)
{
    durability_service_ = durability_service;
}
#endif  // OMG_DDS_PERSISTENCE_SUPPORT

void TopicQoS::policy(const DDS::Policy::Deadline& deadline)
{
    deadline_ = deadline;
}

void TopicQoS::policy(const DDS::Policy::LatencyBudget&  budget)
{
    budget_ = budget;
}

void TopicQoS::policy(const DDS::Policy::Liveliness& liveliness)
{
    liveliness_ = liveliness;
}

void TopicQoS::policy(const DDS::Policy::Reliability& reliability)
{
    reliability_ = reliability;
}

void TopicQoS::policy(const DDS::Policy::DestinationOrder& order)
{
    order_ = order;
}

void TopicQoS::policy(const DDS::Policy::History& history)
{
    history_ = history;
}

void TopicQoS::policy(const DDS::Policy::ResourceLimits& resources)
{
    resources_ = resources;
}

void TopicQoS::policy(const DDS::Policy::TransportPriority& priority)
{
    priority_ = priority;
}

void TopicQoS::policy(const DDS::Policy::Lifespan& lifespan)
{
    lifespan_ = lifespan;
}

void TopicQoS::policy(const DDS::Policy::Ownership& ownership)
{
    ownership_ = ownership;
}


template<>
const DDS::Policy::TopicData&
TopicQoS::policy<DDS::Policy::TopicData>() const
{
    return topic_data_;
}

template<>
const DDS::Policy::Durability&
TopicQoS::policy<DDS::Policy::Durability>() const
{
    return durability_;
}

#ifdef  OMG_DDS_PERSISTENCE_SUPPORT
template<> const DDS::Policy::DurabilityService&
TopicQoS::policy<DDS::Policy::DurabilityService>() const
{
    return durability_service_;
}
#endif  // OMG_DDS_PERSISTENCE_SUPPORT

template<>
const DDS::Policy::Deadline&
TopicQoS::policy<DDS::Policy::Deadline>() const
{
    return deadline_;
}

template<>
const DDS::Policy::LatencyBudget&
TopicQoS::policy<DDS::Policy::LatencyBudget>() const
{
    return budget_;
}

template<>
const DDS::Policy::Liveliness&
TopicQoS::policy<DDS::Policy::Liveliness>() const
{
    return liveliness_;
}

template<>
const DDS::Policy::Reliability&
TopicQoS::policy<DDS::Policy::Reliability>() const
{
    return reliability_;
}

template<>
const DDS::Policy::DestinationOrder&
TopicQoS::policy<DDS::Policy::DestinationOrder>() const
{
    return order_;
}

template<>
const DDS::Policy::History&
TopicQoS::policy<DDS::Policy::History>() const
{
    return history_;
}

template<>
const DDS::Policy::ResourceLimits&
TopicQoS::policy<DDS::Policy::ResourceLimits>() const
{
    return resources_;
}

template<>
const DDS::Policy::TransportPriority&
TopicQoS::policy<DDS::Policy::TransportPriority>() const
{
    return priority_;
}

template<>
const DDS::Policy::Lifespan&
TopicQoS::policy<DDS::Policy::Lifespan>() const
{
    return lifespan_;
}

template<>
const DDS::Policy::Ownership&
TopicQoS::policy<DDS::Policy::Ownership>() const
{
    return ownership_;
}

}
