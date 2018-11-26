#include"DCPS/Domain/DomainParticipantState.h"

#include"DCPS/Pub/IncludeLibs.h"
#include"DCPS/Sub/IncludeLibs.h"

using namespace RxData;
using namespace DCPS::Subscription;
using namespace DCPS::Publication;
using namespace DCPS::Topic;

namespace DCPS { namespace Domain
{

DomainParticipantState::DomainParticipantState()
    : cache_()
{ }

DomainParticipantState::DomainParticipantState(CacheDescription description)
    : cache_(CacheFactory::Instance().getOrCreateCache(description))
{ }

DomainParticipantState::~DomainParticipantState()
{ }

DCPSConfig::PublisherAccess DomainParticipantState::publishers()
{
    return DCPSConfig::PublisherAccess(cache_, DCPSConfig::PublisherCacheTypeName());
}

DCPSConfig::SubscriberAccess DomainParticipantState::subscribers()
{
    return DCPSConfig::SubscriberAccess(cache_, DCPSConfig::SubscriberCacheTypeName());
}

DCPSConfig::TopicAccess DomainParticipantState::topics()
{
    return DCPSConfig::TopicAccess(cache_, DCPSConfig::TopicCacheTypeName());
}

}}
