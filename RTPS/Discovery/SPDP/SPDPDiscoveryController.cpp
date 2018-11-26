#include "RTPS/Discovery/SPDP/SPDPDiscoveryController.h"

namespace RTPS { namespace SPDP
{

// -----------------------------------------------------
//  Discovery controller state implementation
// -----------------------------------------------------

SPDPDiscoveryControllerState::SPDPDiscoveryControllerState()
{ }

SPDPDiscoveryControllerState::~SPDPDiscoveryControllerState()
{ }

RtpsParticipant *SPDPDiscoveryControllerState::participant() const
{
    return participant_;
}

void SPDPDiscoveryControllerState::setParticipant(RTPS::RtpsParticipant *participant)
{
    participant_ = participant;
}

// -----------------------------------------------------
// Discovery controller config holder
// -----------------------------------------------------

SPDPDiscoveryControllerConfig::SPDPDiscoveryControllerConfig()
{ }

SPDPDiscoveryControllerConfig::~SPDPDiscoveryControllerConfig()
{ }

// -----------------------------------------------------
// Discovery controller implementation
// -----------------------------------------------------

SPDPDiscoveryController::SPDPDiscoveryController()
    : Templates::ContextObject<SPDPDiscoveryControllerConfig, SPDPDiscoveryControllerState>()
{ }

SPDPDiscoveryController::~SPDPDiscoveryController()
{ }

// -----------------------------------------------------------
// Interface KeyValueObserver
// -----------------------------------------------------------

void SPDPDiscoveryController::OnObjectCreated(GUID key, DDS::Discovery::ParticipantBuiltinTopicData value)
{
    // Write SPDPDiscoveredParticipantData to participant history-cache
}

void SPDPDiscoveryController::OnObjectDeleted(GUID key)
{
    // Remove SPDPDiscoveredParticipantData from history-cache
}

void SPDPDiscoveryController::OnObjectModified(GUID key, DDS::Discovery::ParticipantBuiltinTopicData value)
{
    // Update SPDPDiscoveredParticipantData in history-cache
}


// -----------------------------------------------------------
// Interface DiscoveryController
// -----------------------------------------------------------

bool SPDPDiscoveryController::AddParticipant(const GUID &guid)
{
    return false;
}

bool SPDPDiscoveryController::RemoveParticipant(const GUID &guid)
{
    return false;
}

void SPDPDiscoveryController::initialize(const GUID &guid)
{
//    RTPS::RtpsParticipant::Ptr participant = RTPS::RtpsFactory::CreateSimpleParticipantDiscovery(guid);
//    this->GetTransientState().setParticipant(participant);
}


}}
