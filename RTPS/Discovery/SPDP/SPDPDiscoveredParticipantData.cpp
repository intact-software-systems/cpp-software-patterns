#include "RTPS/Discovery/SPDP/SPDPDiscoveredParticipantData.h"

namespace RTPS { namespace SPDP
{

SPDPDiscoveredParticipantData::SPDPDiscoveredParticipantData(ParticipantProxy participantProxy, Duration duration)
    : participantProxy_(participantProxy)
    , leaseDuration_(duration)
{
}

SPDPDiscoveredParticipantData::~SPDPDiscoveredParticipantData()
{

}

Duration SPDPDiscoveredParticipantData::leaseDuration() const
{
    return leaseDuration_;
}

void SPDPDiscoveredParticipantData::setLeaseDuration(const Duration &leaseDuration)
{
    leaseDuration_ = leaseDuration;
}

ParticipantProxy SPDPDiscoveredParticipantData::participantProxy() const
{
    return participantProxy_;
}

void SPDPDiscoveredParticipantData::setParticipantProxy(const ParticipantProxy &participantProxy)
{
    participantProxy_ = participantProxy;
}

}}

