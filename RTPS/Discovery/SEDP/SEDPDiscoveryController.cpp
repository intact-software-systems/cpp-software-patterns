#include "RTPS/Discovery/SEDP/SEDPDiscoveryController.h"

namespace RTPS { namespace SEDP
{
// -----------------------------------------------------
//  Discovery controller state implementation
// -----------------------------------------------------
SEDPDiscoveryControllerState::SEDPDiscoveryControllerState()
{ }

SEDPDiscoveryControllerState::~SEDPDiscoveryControllerState()
{ }

// -----------------------------------------------------
//  Discovery controller config implementation
// -----------------------------------------------------

SEDPDiscoveryControllerConfig::SEDPDiscoveryControllerConfig()
{ }

SEDPDiscoveryControllerConfig::~SEDPDiscoveryControllerConfig()
{ }


// -----------------------------------------------------
//  Discovery controller implementation
// -----------------------------------------------------

SEDPDiscoveryController::SEDPDiscoveryController()
{ }

SEDPDiscoveryController::~SEDPDiscoveryController()
{ }

// -----------------------------------------------------------
// Interface KeyValueObserver<GUID, DDS::Discovery::PublicationBuiltinTopicData>
// -----------------------------------------------------------

void SEDPDiscoveryController::OnObjectCreated(DDS::Discovery::PublicationBuiltinTopicData value)
{
    // Write DiscoveredWriterData to participant history-cache
}

void SEDPDiscoveryController::OnObjectDeleted(DDS::Discovery::PublicationBuiltinTopicData value)
{
    // Delete DiscoveredWriterData from participant history-cache
}

void SEDPDiscoveryController::OnObjectModified(DDS::Discovery::PublicationBuiltinTopicData value)
{
    // Update DiscoveredWriterData in participant history-cache
}

// -----------------------------------------------------------
// Interface KeyValueObserver<GUID, DDS::Discovery::SubscriptionBuiltinTopicData>
// -----------------------------------------------------------

void SEDPDiscoveryController::OnObjectCreated(DDS::Discovery::SubscriptionBuiltinTopicData value)
{
}

void SEDPDiscoveryController::OnObjectDeleted(DDS::Discovery::SubscriptionBuiltinTopicData value)
{
}

void SEDPDiscoveryController::OnObjectModified(DDS::Discovery::SubscriptionBuiltinTopicData value)
{
}

// -----------------------------------------------------------
// Interface KeyValueObserver<GUID, DDS::Discovery::TopicBuiltinTopicData>
// -----------------------------------------------------------

void SEDPDiscoveryController::OnObjectCreated(DDS::Discovery::TopicBuiltinTopicData value)
{
}

void SEDPDiscoveryController::OnObjectDeleted(DDS::Discovery::TopicBuiltinTopicData value)
{
}

void SEDPDiscoveryController::OnObjectModified(DDS::Discovery::TopicBuiltinTopicData value)
{
}

}}
