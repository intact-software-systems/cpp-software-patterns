#include"DDS/PublisherQoS.h"

namespace DDS
{

PublisherQoS::PublisherQoS(const DDS::Policy::Presentation& presentation,
                           const DDS::Policy::Partition& partition,
                           const DDS::Policy::GroupData& groupData,
                           const DDS::Policy::EntityFactory& factoryPolicy)
    : presentation_(presentation)
    , partition_(partition)
    , groupData_(groupData)
    , factoryPolicy_(factoryPolicy)
{

}

//PublisherQoS::PublisherQoS(const PublisherQoS& other)
//	: presentation_(other.presentation_)
//	, partition_(other.partition_)
//	, groupData_(other.groupData_)
//	, factoryPolicy_(other.factoryPolicy_)
//{

//}

template<>
const DDS::Policy::Presentation&
PublisherQoS::policy<DDS::Policy::Presentation>() const
{
    return presentation_;
}

template<>
const DDS::Policy::Partition&
PublisherQoS::policy<DDS::Policy::Partition>() const
{
    return partition_;
}

template<>
const DDS::Policy::GroupData&
PublisherQoS::policy<DDS::Policy::GroupData>() const
{
    return groupData_;
}

template<>
const DDS::Policy::EntityFactory&
PublisherQoS::policy<DDS::Policy::EntityFactory>() const
{
    return factoryPolicy_;
}

void PublisherQoS::policy(const DDS::Policy::Presentation& presentation)
{
    presentation_ = presentation;
}

void PublisherQoS::policy(const DDS::Policy::Partition& partition)
{
    partition_ = partition;
}

void PublisherQoS::policy(const DDS::Policy::GroupData& groupData)
{
    groupData_ = groupData;
}

void PublisherQoS::policy(const DDS::Policy::EntityFactory& factoryPolicy)
{
    factoryPolicy_ = factoryPolicy;
}

}
