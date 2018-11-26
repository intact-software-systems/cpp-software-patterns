#include"DCPS/Factory/DCPSInitStrategies.h"
#include"DCPS/Domain/DomainParticipant.h"
#include"DCPS/Pub/Publisher.h"
#include"DCPS/Sub/Subscriber.h"

namespace DCPS { namespace Factory
{

// ------------------------------------------------------------------------------
// Example implementations of configuration of RTPS into DCPS entities
// ------------------------------------------------------------------------------

/**
 * @brief RTPSDomainParticipantInit::RTPSDomainParticipantInit
 */
RTPSDomainParticipantInit::RTPSDomainParticipantInit()
{ }

RTPSDomainParticipantInit::~RTPSDomainParticipantInit()
{ }

bool RTPSDomainParticipantInit::Init(Domain::DomainParticipantPtr &participant)
{
    //participant->GetTransientState();

    return true;
}

/**
 * @brief RTPSPublisherInit::RTPSPublisherInit
 */
RTPSPublisherInit::RTPSPublisherInit()
{ }

RTPSPublisherInit::~RTPSPublisherInit()
{ }

bool RTPSPublisherInit::Init(Publication::PublisherPtr &publisher)
{
    return true;
}

/**
 * @brief RTPSSubscriberInit::RTPSSubscriberInit
 */
RTPSSubscriberInit::RTPSSubscriberInit()
{ }

RTPSSubscriberInit::~RTPSSubscriberInit()
{ }

bool RTPSSubscriberInit::Init(Subscription::SubscriberPtr &subscriber)
{
    return true;
}

}}

